/* umdevxs_interrupt.c
 *
 * Interrupt support for the Linux UMDevXS driver.
 */

/*****************************************************************************
* Copyright (c) 2009-2018 INSIDE Secure B.V. All Rights Reserved.
*
* This confidential and proprietary software may be used only as authorized
* by a licensing agreement from INSIDE Secure.
*
* The entire notice above must be reproduced on all authorized copies that
* may only be made to the extent permitted by a licensing agreement from
* INSIDE Secure.
*
* For more information or support, please go to our online support system at
* https://customersupport.insidesecure.com.
* In case you do not have an account for this system, please send an e-mail
* to ESSEmbeddedHW-Support@insidesecure.com.
*****************************************************************************/

#include "c_umdevxs.h"              // config options
#include "umdevxs_internal.h"

#include <linux/delay.h>            // msleep

#include "basic_defs.h"             // uint8_t, etc.

#ifndef UMDEVXS_REMOVE_INTERRUPT

#include <linux/interrupt.h>
#include <linux/irqreturn.h>
#include <linux/semaphore.h>
#include <linux/mutex.h>        // mutex_*

// signalling int handler -> app thread
static struct semaphore UMDevXS_Interrupt_sem;
static struct mutex UMDevXS_Interrupt_mutex;    // concurrency protection

#endif /* UMDEVXS_REMOVE_INTERRUPT */

static int UMDevXS_Interrupt_InstalledIRQ = -1;


/*----------------------------------------------------------------------------
 * UMDevXS_Interrupt_WaitWithTimeout
 *
 * Returns 0 on interrupt, 1 on timeout, <0 in case of error.
 */
int
UMDevXS_Interrupt_WaitWithTimeout(
    const unsigned int Timeout_ms)
{
#ifdef UMDEVXS_REMOVE_INTERRUPT
    // simulate the timeout
    msleep(Timeout_ms);
    return 1;
#else
    // convert the timeout to jiffies
    unsigned long jiffies = msecs_to_jiffies(Timeout_ms);
    int res;

    if (UMDevXS_Interrupt_InstalledIRQ == -1)
    {
        // simulate the timeout
        msleep(Timeout_ms);
        return 1;
    }

    // warn for high semaphore count
    res = UMDevXS_Interrupt_sem.count;
    if (res > 1)
    {
        LOG_WARN(
            UMDEVXS_LOG_PREFIX
            "UMDevXS_Interrupt_WaitWithTimeout: "
            "sem.count = %d\n",
            res);
    }

    // concurrency protection for the following section
    mutex_lock(&UMDevXS_Interrupt_mutex);

    // when we get here, the interrupt can be enabled or disabled
    // we wait for the semahore
    // when we get it, we know the interrupt occured, incremented the
    // semaphore and disabled the interrupt.
    // we can then safely enable the interrupt again

    // wait on the semaphore, with timeout
    res = down_timeout(&UMDevXS_Interrupt_sem, (long)jiffies);

    if (res == 0)
    {
        // managed to decrement the semaphore

        // allow the semaphore to be incremented by the interrupt handler
        enable_irq(UMDevXS_Interrupt_InstalledIRQ);
    }

    // end of concurrency protection
    mutex_unlock(&UMDevXS_Interrupt_mutex);

    if (res == 0)
    {
        return 0;
    }

    // handle timeout
    // interrupt is still enabled, so do not touch it
    if (res == -ETIME)
    {
        return 1;
    }

    LOG_WARN(
        UMDEVXS_LOG_PREFIX
        "UMDevXS_Interrupt_WaitWithTimeout: "
        "down_timeout returned %d\n",
        res);

#endif /* UMDEVXS_REMOVE_INTERRUPT */

    return -2;
}


/*----------------------------------------------------------------------------
 * UMDevXS_Interrupt_TopHalfHandler
 *
 * This is the interrupt handler function call by the kernel when our hooked
 * interrupt is active, which means the interrupt from the PCI card.
 * We ask the interrupt controller which sources are active.
 * For all  interrupts we invoke the handler directly in this context.
 */
#ifndef UMDEVXS_REMOVE_INTERRUPT
static irqreturn_t
UMDevXS_Interrupt_TopHalfHandler(
    int irq,
    void * dev_id)
{
    const uint32_t IntSource = 1 << irq;

    IDENTIFIER_NOT_USED(dev_id);

    if (irq != UMDevXS_Interrupt_InstalledIRQ)
    {
        return IRQ_NONE;
    }

    if (IntSource & UMDEVXS_INTERRUPT_TRACE_FILTER)
    {
        Log_FormattedMessage(
            UMDEVXS_LOG_PREFIX
            "UMDevXS_Interrupt_TopHalfHandler: "
            "irq = %d\n",
            irq);
    }

    // increase the semaphore
    up(&UMDevXS_Interrupt_sem);

    // disable the interrupt to avoid spinning
    // will be enabled when "event" has been propagated to user mode
    disable_irq_nosync(UMDevXS_Interrupt_InstalledIRQ);

    return IRQ_HANDLED;
}
#endif /* UMDEVXS_REMOVE_INTERRUPT */


/*----------------------------------------------------------------------------
 * UMDevXS_Interrupt_Init
 */
void
UMDevXS_Interrupt_Init(
    const int nIRQ)
{
    if (nIRQ != -1)
    {
#ifndef UMDEVXS_REMOVE_INTERRUPT
        int res;
        void * dev;
        unsigned long flags;

#ifndef UMDEVXS_REMOVE_DEVICE_OF
        dev = UMDevXS_OFDev_GetReference();
        flags = IRQF_DISABLED | IRQ_TYPE_EDGE_RISING;
#else
        dev = NULL;
        flags = 0;
#endif

        // semaphore is used in top-half, so make it ready
        sema_init(&UMDevXS_Interrupt_sem, /*initial value:*/0);

        mutex_init(&UMDevXS_Interrupt_mutex);

        // must set prior to hooking
        // when interrupt happens immediately, top-half check against this
        UMDevXS_Interrupt_InstalledIRQ = nIRQ;

        // install the top-half interrupt handler for the given IRQ
        // any reason not to allow sharing?
        res = request_irq(
                  nIRQ,
                  UMDevXS_Interrupt_TopHalfHandler,
                  flags,
                  UMDEVXS_MODULENAME,
                  dev);

        if (res)
        {
            // not hooked after all, so clear global to avoid irq_free
            UMDevXS_Interrupt_InstalledIRQ = -1;

            LOG_CRIT(
                UMDEVXS_LOG_PREFIX
                "UMDevXS_Interrupt_Init: "
                "request_irq returned %d\n",
                res);
        }
        else
        {
            LOG_CRIT(
                UMDEVXS_LOG_PREFIX
                "UMDevXS_Interrupt_Init: "
                "Successfully hooked IRQ %d\n",
                nIRQ);
        }
#endif /* UMDEVXS_REMOVE_INTERRUPT */
    }
}


/*----------------------------------------------------------------------------
 * UMDevXS_Interrupt_UnInit
 */
void
UMDevXS_Interrupt_UnInit(void)
{
    if (UMDevXS_Interrupt_InstalledIRQ != -1)
    {
#ifndef UMDEVXS_REMOVE_INTERRUPT
        void * dev;

#ifndef UMDEVXS_REMOVE_DEVICE_OF
        dev = UMDevXS_OFDev_GetReference();
#else
        dev = NULL;
#endif

        // unhook the interrupt
        free_irq(UMDevXS_Interrupt_InstalledIRQ, dev);

        UMDevXS_Interrupt_InstalledIRQ = -1;
#endif /* UMDEVXS_REMOVE_INTERRUPT */
    }
}


/* end of file umdevxs_interrupt.c */
