/****************************************************************************
 *
 *   FileName: pwr_mngr.c
 *
 *   Author: Oshri A
 *
 *   Date:
 *
 *   Description: Power Manager
 *
 ****************************************************************************/

/****************************************************************************
 ***************          I N C L U D E  F I L E S                ***************
 ****************************************************************************/
#include <stdio.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/syscall.h>
#include <poll.h>
#include <pthread.h>
#include "assert.h"

#include "pwr_mngr.h"
#include "cnn_boot.h"
#include "gme_mngr.h"
#include "gme_drv.h"
#include "ve_mngr.h"

#include "inu_types.h"
#include "err_defs.h"
#include "log.h"

/****************************************************************************
 ***************         L O C A L       D E F N I T I O N S  ***************
 ****************************************************************************/

#define PWR_MNGRP_GPIO_26_BIT           (1 << 26)    // GPIO 26 (pin 26 in GPIO0)

/* PM modes */
#define PM_MODE0                        "0"
#define PM_MODE1                        "1"
#define PM_MODE2                        "2"

/****************************************************************************
 ***************            L O C A L    T Y P E D E F S      ***************
 ****************************************************************************/

/****************************************************************************
 ***************       L O C A L         D A T A              ***************
 ****************************************************************************/
static OS_LYRG_threadHandle PWR_MNGR_threadHandle = NULL;


/****************************************************************************
 ***************       G L O B A L       D A T A              ***************
 ****************************************************************************/

/****************************************************************************
 ***************      L O C A L       F U N C T I O N S       ***************
 ****************************************************************************/
static ERRG_codeE PWR_MNGRP_w_wakeup(char *mode)
{
    int f;

    LOGG_PRINT(LOG_DEBUG_E, NULL, "write %s to pm_host_usb_u3\n", mode);

    f = open("/sys/power/pm_host_usb_u3", O_RDWR | O_SYNC);
    if (f < 0)
    {
        printf("error opening pm_host_usb_u3\n");
        return PWR_MNGR__ERR_OUT_OF_RSRCS;
    }
    write (f, mode, strlen(mode));
    close (f);

    return PWR_MNGR__RET_SUCCESS;
}

static ERRG_codeE PWR_MNGRP_w_state()
{
    int f;

    LOGG_PRINT(LOG_DEBUG_E, NULL, "Call PWR_MNGRP_w_state\n");

    f = open("/sys/power/state", O_RDWR | O_SYNC);
    if (f < 0)
    {
        printf("Error opening /sys/power/state\n");
        return PWR_MNGR__ERR_OUT_OF_RSRCS;
    }
    write(f, "mem", strlen("mem"));
    close(f);

    return PWR_MNGR__RET_SUCCESS;
}

static ERRG_codeE PWR_MNGRG_read_pm_host(PWR_MNGRG_SLEEP_MODE_E *mode)
{
    int f;

    f = open("/sys/power/pm_host_usb_u3", O_RDWR | O_SYNC);
    if (f < 0)
    {
        printf("error opening pm_host_usb_u3\n");
        return PWR_MNGR__ERR_OUT_OF_RSRCS;
    }

    read(f, mode, sizeof(PWR_MNGRG_SLEEP_MODE_E));
    *mode = (*mode - 0x30) & 0xff;
    close(f);

    return PWR_MNGR__RET_SUCCESS;
}

static ERRG_codeE PWR_MNGRP_store_pm_values(char *c)
{
    int ret;

    ret = PWR_MNGRP_w_wakeup(c);
    if (ERRG_FAILED(ret))
    {
        LOGG_PRINT(LOG_ERROR_E, NULL, "Writing pm_host failed\n");
        return PWR_MNGR__ERR_ILLEGAL_STATE;
    }

    ret = PWR_MNGRP_w_state();
    if (ERRG_FAILED(ret))
    {
        LOGG_PRINT(LOG_ERROR_E, NULL, "Writing 'mem' failed\n");
        return PWR_MNGR__ERR_ILLEGAL_STATE;
    }

    return PWR_MNGR__RET_SUCCESS;
}

static ERRG_codeE PWR_MNGRP_notifyKernel(PWR_MNGRG_SLEEP_MODE_E mode)
{
    int ret;
    char *c;

    switch(mode)
    {
        case PWR_MNGRG_INTERNAL_SUSPEND_E:
            c = PM_MODE0;
            break;
        case PWR_MNGRG_USB_U3_SUSPEND_E:
            c = PM_MODE1;
            break;
        case PWR_MNGRG_POWER_DOWN_E:
            c = PM_MODE2;
            break;
        default:
        LOGG_PRINT(LOG_ERROR_E, NULL, "Unknown PM mode\n");
        return PWR_MNGR__ERR_ILLEGAL_STATE;
    }

    // delay to allow flushing log to user (currently power down is done manually)
    OS_LYRG_usleep(40000);

    // the following write will enter the kernel to suspend mode - the cpu will be halted, on wake-up the pc will
    // continue from the instruction that it halted at
    if (PWR_MNGRP_store_pm_values(c) != PWR_MNGR__RET_SUCCESS)
    {
        LOGG_PRINT(LOG_ERROR_E, NULL, "Failure storing parameters\n");
        return PWR_MNGR__ERR_ILLEGAL_STATE;
    }

    // added delay to avoid exit before entering sleep (sometimes there is a delay between writing to kernel and the actual suspending)
    OS_LYRG_usleep(10000);

    return PWR_MNGR__RET_SUCCESS;
}

static ERRG_codeE PWR_MNGRG_enterPowerDown(PWR_MNGRG_SLEEP_MODE_E mode)
{
    return PWR_MNGRP_notifyKernel(mode);
}

static void PWR_MNGRG_waitForGPIO(PWR_MNGRG_SLEEP_MODE_E *mode)
{
    int fd_gpio;
    char buf_gpio[3];

    fd_gpio = open("/sys/class/gpio/gpio250/value", O_RDONLY);

    lseek(fd_gpio, 0, 0);
    read(fd_gpio, buf_gpio, 2);

    LOGG_PRINT(LOG_INFO_E, NULL, "Waiting for GPIO to indicate PD event\n");

    while (buf_gpio[0] == '1')
    {
        lseek(fd_gpio, 0, 0);
        read(fd_gpio, buf_gpio, 2);
    }

    close(fd_gpio);

    *mode = PWR_MNGRG_POWER_DOWN_E;
}

static ERRG_codeE PWR_MNGRG_waitForU3(PWR_MNGRG_SLEEP_MODE_E *mode)
{
    ERRG_codeE retCode = PWR_MNGR__RET_SUCCESS;

    LOGG_PRINT(LOG_INFO_E, NULL, "Waiting for U3 suspend event\n");

    /* Detect U3 */
    while (TRUE)
    {
        retCode = PWR_MNGRG_read_pm_host(mode);
        if (ERRG_SUCCEEDED(retCode))
        {
            if(*mode == PWR_MNGRG_USB_U3_SUSPEND_E)
            {
                LOGG_PRINT(LOG_INFO_E, NULL, "U3 detected\n");
                break;
            }
        }
        else
        {
            LOGG_PRINT(LOG_WARN_E, NULL, "failed reading pm host\n");
            break;
        }
        OS_LYRG_usleep(10000);
    }
    return retCode;
}

static void* PWR_MNGRG_waitForEventThread(void *argP)
{
    ERRG_codeE                  retCode = PWR_MNGR__RET_SUCCESS;
    inu_deviceH                 deviceH = (inu_deviceH)argP;
    PWR_MNGRG_SLEEP_MODE_E      mode;
    inu_device__powerDownCmdE   state;

    while (TRUE)
    {
        // Wait until getting 'wait for u3' event
        do
        {
            inu_device__getPowerDownStatus(&state);
            OS_LYRG_usleep(10000);

        } while (state != INU_DEVICE_POWER_DOWN_WAIT_FOR_EVENT_E);

        retCode = PWR_MNGRG_waitForU3(&mode);   // blocking function, return when moving to u3
        if (ERRG_SUCCEEDED(retCode))
        {
            PWR_MNGRG_prepareToPowerDown(deviceH, mode);

            PWR_MNGRG_enterPowerDown(mode);     // go to suspend, exit this function on wake-up

            PWR_MNGRG_wakeUpRecovery(deviceH);
        }
    }
    PWR_MNGR_threadHandle = NULL;

    return PWR_MNGR_threadHandle;
}

/****************************************************************************
 ***************        G L O B A L           F U N C T I O N S     **************
 ****************************************************************************/

/****************************************************************************
*
*   Function Name: PWR_MNGRG_prepareToPowerDown
*
*   Description:
*
*   Inputs: none
*
*   Outputs: none
*
*   Returns: none
*
*   Context: pheripherals control
*
****************************************************************************/
ERRG_codeE PWR_MNGRG_prepareToPowerDown(inu_deviceH deviceH, PWR_MNGRG_SLEEP_MODE_E mode)
{
    ERRG_codeE                  retCode = PWR_MNGR__RET_SUCCESS;
    inu_device__powerDownCmdE   state;
    MEM_MAPG_addrT              src, dest;
    UINT32                      size;

    LOGG_PRINT(LOG_INFO_E, retCode, "Enter FW power down sequence\n");
#ifdef ENABLE_VISION_EFFECT
    // wait for all streams to stop (supported only in cases where the streams run over UVC)
    while (VE_MNGRP_anyStreamIsRunning() != NULL)
    {
        OS_LYRG_usleep(10000);
    }
#endif

    // halt EV and prepare cnn code in DDR
    CNN_BOOTG_reloadApp();

    // backup LRAM to DDR (will be restore in SPL when resuming from power down)
    MEM_MAPG_getVirtAddr(MEM_MAPG_DDR_LRAM_BACKUP_E, &dest);
    MEM_MAPG_getVirtAddr(MEM_MAPG_LRAM_CNN_BOOT_E, &src);
    MEM_MAPG_getSize(MEM_MAPG_DDR_LRAM_BACKUP_E, &size);
    memcpy(dest, src, size);

    inu_device__updatePowerDownStatus(INU_DEVICE_POWER_DOWN_PREPARE_E);

    do
    {
        inu_device__getPowerDownStatus(&state);
        OS_LYRG_usleep(10000);

    } while (state != INU_DEVICE_POWER_DOWN_ENTER_E);

    // store pin mux setup (in DDR)
    GME_DRVG_savePinMuxSetup();

    LOGG_PRINT(LOG_INFO_E, retCode, "Ready for power down\n");

    return retCode;
}

/****************************************************************************
*
*   Function Name: PWR_MNGRG_wakeUpRecovery
*
*   Description:
*
*   Inputs:
*
*   Outputs: none
*
*   Returns:
*
*   Context: pheripherals control
*
****************************************************************************/
ERRG_codeE PWR_MNGRG_wakeUpRecovery(inu_deviceH deviceH)
{
    ERRG_codeE retCode = PWR_MNGR__RET_SUCCESS;

    LOGG_PRINT(LOG_INFO_E, NULL, "Start wakeup sequence\n");

    // restore pin mux setup from DDR
    GME_DRVG_restorePinMuxSetup();

    inu_device__updatePowerDownStatus(INU_DEVICE_POWER_DOWN_EXIT_E);

    return(retCode);
}

/****************************************************************************
*
*   Function Name: PWR_MNGRG_waitForEvent
*
*   Description:
*
*   Inputs: none
*
*   Outputs: none
*
*   Returns: none
*
*   Context: pheripherals control
*
****************************************************************************/
ERRG_codeE PWR_MNGRG_waitForEvent(inu_deviceH deviceH)
{
    ERRG_codeE              retCode = PWR_MNGR__RET_SUCCESS;
    OS_LYRG_threadParams threadParams;

#ifdef PWR_MNGRG_ENABLE_PD
    if (PWR_MNGR_threadHandle)
    {
        LOGG_PRINT(LOG_ERROR_E, NULL, "Power down thread is already running\n");
        return PWR_MNGR__ERR_THREAD_ERROR;
    }

    threadParams.func = (OS_LYRG_threadFunction)PWR_MNGRG_waitForEventThread;
    threadParams.id = OS_LYRG_PWR_MNGR_THREAD_ID_E;
    threadParams.event = NULL;
    threadParams.param = deviceH;

    //create thread
    PWR_MNGR_threadHandle = OS_LYRG_createThread(&threadParams);
    if (PWR_MNGR_threadHandle == NULL)
    {
        LOGG_PRINT(LOG_ERROR_E, NULL, "Failed to create thread\n");
        return PWR_MNGR__ERR_THREAD_ERROR;
    }
#endif

    return retCode;
}

