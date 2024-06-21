// inu_host.cpp : Defines the entry point for the console application.
//
/****************************************************************************
 *
 *   FileName: cmd.c
 *
 *   Author:
 *
 *   Date:
 *
 *   Description: tool for sending commands from host to nu3000 inu_target
 *
 ****************************************************************************/
#if 0
//#define TEST_PERF_APP
//#define INU_SDK

#ifndef INU_SDK
/****************************************************************************
 ***************       I N C L U D E    F I L E S             ***************
 ****************************************************************************/
#include "inu_types.h"
#include "err_defs.h"
#include "inu.h"
#include "os_lyr.h"
#include <stdint.h>

#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <errno.h>
#include <time.h>

#if (defined _WIN32) || (defined _WIN64)
  #include <conio.h>
  #define SLESH "\\"
  #include "getopt.h"
#elif defined(__linux__)
#include <inttypes.h>
  #define SLESH "//"
#ifdef TEST_PERF_APP
#include <signal.h>
#endif
  #include <unistd.h>
  #include <getopt.h>
#endif


#ifdef TEST_PERF_APP
#include "usb_ctrl_com.h"
#include "os_lyr.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************************
 ***************       L O C A L    D E F N I T I O N S       ***************
 ****************************************************************************/
#define CMDP_ASSERT(x) (void)(x)
#define CMDP_CONFIG_REGS_TBL_SIZE  (512)
enum { TIMEOUT_MSEC_E = 1000 * 2 };

/****************************************************************************
 ***************       L O C A L    T Y P E D E F S           ***************
 ****************************************************************************/
typedef struct
{
   INUG_serviceIdE        sid;
} SERVICEP_infoT;

typedef struct
{
   char               *bufP;
   UINT32             bufsize;
} SERVICEP_bufT;

typedef struct
{
   UINT32 CMDG_sensorResid;
   UINT32 CMDG_sensorFps;
   UINT32 iamChannel;
   UINT32 iamChanSelect;
   char *filename;
   clock_t Test_dur;
   UINT32 dpeChan;
   UINT32 dpeChanSel;
   UINT32 colorChannel;
   UINT32 colorChanSelect;
   UINT32 numSamples;
}TestParamsT;
/****************************************************************************
 ***************      G L O B A L         T Y P E D E F S     ***************
 ****************************************************************************/
//Define services here
typedef enum
{
   INU_host_videoE          = 0,
   INU_host_depthE          = 1,
   INU_host_webcamE         = 2,
   INU_host_clientE         = 3,
   INU_host_positionSensorE = 4,
   INU_host_injectionE      = 5,
   INU_host_audioE          = 6
} INU_host_testIdE;

/****************************************************************************
 ***************       L O C A L         D A T A              ***************
 ****************************************************************************/

static inu_device__writeRegT CMDP_configRegTbl[CMDP_CONFIG_REGS_TBL_SIZE];
static unsigned int CMDP_configRegCnt = 0;
char *configfolder=NULL;
char *XMLfile=NULL;
int loadXML=0;
int generalPurpose=0;
char configfile[300];
char *Bootfolder=NULL;
char errStr[100];
bool isStereoSensorStarted = false;
int requiredStreamsCnt = 0;
int openedStreamsCnt = 0;

INUG_handleT CMDP_loggerHandle;
INUG_handleT CMDP_sysHandle;
INUG_handleT CMDP_videoHandle;
INUG_handleT CMDP_injectHandle;
INUG_handleT CMDP_depthHandle;
INUG_handleT CMDP_WebcamHandle;
INUG_handleT CMDP_ClientHandle;
INUG_handleT CMDP_AlgHandle;
INUG_handleT CMDP_PosHandle;
INUG_handleT CMDP_audioHandle;


UINT32 CMDG_iaeWidth = 0;
UINT32 CMDG_iaeHeight = 0;
UINT32 CMDG_iaeWidthWebcam = 0;
UINT32 CMDG_iaeHeightWebcam = 0;
UINT32 testInProgress = 0;
UINT32 CMDG_isGeneratorMode=0;
UINT32 CMDG_sensorResid, CMDG_sensorFps, Channel=0, ChanSelect=0,test=0, first_word;
OS_LYRG_mutexT mutex;//to avoid closing streams while open other stream, in multi-thread mode

clock_t Test_dur;
ERRG_codeE ret;

//For saving read frames
#define FRAME_STORE_MEM_SIZE (32*1024*1024)

#define NUMBER_TRIES (10000)
#define NUMBER_BUFFER_A (1)
#define BUFFER_SIZE_A (40960)
#define NUMBER_BUFFER_B (11)
#define BUFFER_SIZE_B (256)

#define DATA_CHECK

UINT32 results[NUMBER_TRIES];

/****************************************************************************
 ***************     P R E    D E F I N I T I O N     OF           ***************
 ***************     L O C A L         F U N C T I O N S      ***************
 ****************************************************************************/
static void CMDP_setITbl2cReg(unsigned int *indexP, UINT32 i2cNum, UINT8 tarAddress, UINT8 accessSize, UINT16 regAddress,  UINT32 regWidth, UINT32 i2cData,UINT32 phase);
static void CMDP_setTblSocReg(unsigned int *indexP, UINT32 addr, UINT32 val, UINT32 phase);
static void CMDP_setITbWaitReg(unsigned int *indexP, UINT32 waitReg, UINT32 phase);
       void CMDP_LoadCeva(INUG_dataTypeE cevaId);

/****************************************************************************
 ***************     L O C A L         F U N C T I O N S      ***************
 ****************************************************************************/

//add log holder functionality for OS layer
#ifndef LINUX_MAKE_BUILD
    void LOGG_outputStrLog(INU_DEFSG_logLevelE level, ERRG_codeE errCode, const char *fileP, const char *funcP, UINT32 lineNum, const char * fmt, ...)
    {
    }
#endif

int millisleep_host(unsigned ms)
{
#if (defined _WIN32) || (defined _WIN64)
  SetLastError(0);
  Sleep(ms);
  return GetLastError() ?-1 :0;
#elif defined(__linux__)
  usleep(1000 * ms);
  return 0;
#else
#error ("no milli sleep available for platform")
  return -1;
#endif
}
#if 1

/****************************************************************************
*
*  Function Name: CMDP_parseConfigIReg
*
*  Description: Set registers entry in the config table
*
*  Inputs:
*
*  Returns:
*
****************************************************************************/
int CMDP_parseConfigIReg (char *fileName, UINT32 phase)
{
    char line[1000];
    FILE *fIn;
    char *str;
   unsigned int *ip = &CMDP_configRegCnt;
   UINT32 val,reg;
   UINT32 i2cNum, tarAddress, accessSize, regAddress, regWidth, i2cData, waitReg;
   char i2cMode;

    // Open input file and process line by line.
    if ((fIn = fopen (fileName, "r")) == NULL) {
        fprintf (stderr, "Cannot open %s, errno = %d\n", fileName, errno);
        return 1;
    }

    while (fgets (line, sizeof (line), fIn) != NULL) {
        // Check if line was too long.

        if (line[strlen (line) - 1] != '\n') {
            fprintf (stderr, "Line too long: [%s...]\n", line);
            fclose (fIn);
            return 1;
        }

         // Output the line and start processing it.
         //printf ("%s   ", line);
        str = line;

        // Skip white space and scan first inetegr.
        while (*str == ' ') str++;
      // Check for the comment section
      if (*str == '#') {
         if (strncmp(str,"#*# Video Size",14) == 0) {
               str+=14;
               while (*str == ' ') str++;
               if (sscanf (str, "%d %d", &CMDG_iaeWidth, &CMDG_iaeHeight) != 2)
               {
                  printf("Can't parse meta line\n");
                  break;
               }
               //printf("Video Size: width=0x%x, height=0x%x\n",CMDG_iaeWidth, CMDG_iaeHeight);
         }else {
            //printf("Skipping comments\n");
            continue;
         }
      }
      else if (strncmp(str,"REG",3) == 0)
      {
         str +=3;
         while (*str == ' ') str++;
         if (sscanf (str, "%x = %x", &reg,&val) != 2)
         {
            printf("Can't parse REG line\n");
            break;
         }
         //printf("REG: address=0%x, value=0%x\n",reg, val);
         CMDP_setTblSocReg(ip, reg ,val, phase);
      }
      else if (strncmp(str,"I2C",3) == 0)
      {
         str +=3;
         while (*str == ' ') str++;
         if (sscanf (str, "%d %c %x %d %x %d %x", &i2cNum, &i2cMode, &tarAddress, &accessSize, &regAddress, &regWidth, &i2cData) != 7)
         {
            printf("Can't parse I2C line\n");
            break;
         }

         //printf("I2C: i2cNum=0x%x, tarAddress=0x%x, accessSize=0x%x, regAddress=0x%x, regWidth=0x%x, i2cData=0x%x\n",i2cNum, tarAddress, accessSize, regAddress, regWidth, i2cData);
         CMDP_setITbl2cReg(ip, i2cNum, tarAddress, accessSize, regAddress, regWidth, i2cData, phase);
      }
        else if (strncmp(str,"WAIT",4) == 0)
        {
            str +=4;
            while (*str == ' ') str++;
            if (sscanf (str, "%d ", &waitReg) != 1)
         {
            printf("Can't parse WAIT line\n");
            break;
         }
            //printf("WAIT: waitReg= %d\n", waitReg);
            CMDP_setITbWaitReg(ip, waitReg, phase);
        }

    }
    // Close input file and exit.
    fclose (fIn);
    return 0;
}

/****************************************************************************
*
*  Function Name: CMDP_setITbl2cReg
*
*  Description: Set an I2C register entry in the config table
*
*  Inputs:
*
*  Returns:
*
****************************************************************************/
static void CMDP_setITbl2cReg(unsigned int *indexP, UINT32 i2cNum, UINT8 tarAddress, UINT8 accessSize, UINT16 regAddress,  UINT32 regWidth, UINT32 i2cData, UINT32 phase)
{
   unsigned int i = *indexP;
   CMDP_configRegTbl[i].phase = (inu_device__dbPhaseE)phase;
   CMDP_configRegTbl[i].regType = INU_DEVICE__REG_I2C_E;
   CMDP_configRegTbl[i].i2cReg.i2cNum = (inu_device__i2cNumE)i2cNum;
   CMDP_configRegTbl[i].i2cReg.tarAddress = tarAddress;
   CMDP_configRegTbl[i].i2cReg.accessSize = accessSize;
   CMDP_configRegTbl[i].i2cReg.regAddress = regAddress;
   CMDP_configRegTbl[i].i2cReg.regWidth =regWidth;
   CMDP_configRegTbl[i].i2cReg.i2cData = i2cData;

   *indexP = *indexP+1;
   CMDP_ASSERT(*indexP<CMDP_CONFIG_REGS_TBL_SIZE);
}



/****************************************************************************
*
*  Function Name: CMDP_setITbl2cReg
*
*  Description: Set an I2C register entry in the config table
*
*  Inputs:
*
*  Returns:
*
****************************************************************************/
static void CMDP_setITbWaitReg(unsigned int *indexP, UINT32 waitReg, UINT32 phase)
{
   unsigned int i = *indexP;
   CMDP_configRegTbl[i].phase = (inu_device__dbPhaseE)phase;
   CMDP_configRegTbl[i].regType = INU_DEVICE__REG_WAIT_E;
   CMDP_configRegTbl[i].waitReg.usecWait = waitReg;

   *indexP = *indexP+1;
   CMDP_ASSERT(*indexP<CMDP_CONFIG_REGS_TBL_SIZE);
}

/****************************************************************************
*
*  Function Name: CMDP_setTblSocReg
*
*  Description: Set an SOC register entry in the config table
*
*  Inputs:
*
*  Returns:
*
****************************************************************************/
static void CMDP_setTblSocReg(unsigned int *indexP, UINT32 addr, UINT32 val, UINT32 phase)
{
   unsigned int i = *indexP;

   CMDP_configRegTbl[i].phase = (inu_device__dbPhaseE)phase;
   CMDP_configRegTbl[i].regType = INU_DEVICE__REG_SOC_E;
   CMDP_configRegTbl[i].socReg.addr= addr;
   CMDP_configRegTbl[i].socReg.val= val;

   *indexP = *indexP+1;
   CMDP_ASSERT(*indexP<CMDP_CONFIG_REGS_TBL_SIZE);
}

/****************************************************************************
*
*  Function Name: CMDP_writeConfigRegTbl
*
*  Description: Write config reg table to target using INU api.
*
*  Inputs:
*
*  Returns:
*
****************************************************************************/
void CMDP_writeConfigRegTbl(void)
{
   unsigned int i;
   unsigned int status;
   for(i = 0 ; i < CMDP_configRegCnt; i++)
   {
      status = INUG_ioctl(CMDP_sysHandle, INUG_IOCTL_SYSTEM_WR_REG_E, &CMDP_configRegTbl[i]);
      CMDP_ASSERT(status);
   }
}

/****************************************************************************
*
*  Function Name: CMDP_basicInitTarget
*
*  Description: Do basic initializations to target:
*     Init library
*     Open Logger service
*     Open system service
*     Read version
*     Clear database
*
*  Inputs:
*
*  Returns:
*
****************************************************************************/
void CMDP_targetBasicInit(void)
{
    ERRG_codeE status=(ERRG_codeE)RET_SUCCESS;
    INUG_ioctlLoggerSetParamsT loggerParams;
    INUG_ioctlDbControlT db;
    INUG_ioctlGetVersionT version;

    status = INUG_init();
    CMDP_ASSERT(status);

    printf("INUG_open logger\n");
    status = INUG_open(&CMDP_loggerHandle,INUG_SERVICE_LOGGER_E);
    CMDP_ASSERT(status);

    loggerParams.hostLogSeverity = (INU_DEFSG_logLevelE)2;
    loggerParams.hostPrintLogs = 1;
    loggerParams.hostSendLogs = 0;
    loggerParams.hostSendGpLogs = 0;
    loggerParams.hostPrintGpLogs = 1;
    loggerParams.gpLogSeverity = (INU_DEFSG_logLevelE)2;
    loggerParams.gpPrintLogs = 1;
    loggerParams.gpSendLogs = 1;
    loggerParams.gpSendCdcLogs = 0;
    status = INUG_ioctl(CMDP_loggerHandle, INUG_IOCTL_LOGGER_SET_PARAMS_E, &loggerParams);
    CMDP_ASSERT(status);

    printf("INUG_ioctl start logger\n");
    status = INUG_ioctl(CMDP_loggerHandle, INUG_IOCTL_LOGGER_START_E, NULL);
    CMDP_ASSERT(status);

    printf("INUG_open system \n");
    status = INUG_open(&CMDP_sysHandle, INUG_SERVICE_SYSTEM_E);
    CMDP_ASSERT(status);
    printf("INUG_open system -> done \n");

    //printf("Load Ceva\n");
    //CMDP_LoadCeva(INUG_DATA_TYPE_SYS_CEVA_A_APP_E);

    status = INUG_ioctl(CMDP_sysHandle,INUG_IOCTL_SYSTEM_GET_VERSION_E, &version);
    CMDP_ASSERT(status);

    db.dbControl = INU_DEFSG_DB_CONTROL_CLEAR_E;
    status = INUG_ioctl(CMDP_sysHandle,INUG_IOCTL_SYSTEM_DB_CONTROL_E, &db);
    CMDP_ASSERT(status);

}

/****************************************************************************
*
*  Function Name: CMDP_targetClose
*
*  Description:
*     Close system service
*     Stop logger service
*     Close logger service
*
*  Inputs:
*
*  Returns:
*
****************************************************************************/
void CMDP_targetClose(void)
{
   ERRG_codeE status=(ERRG_codeE)RET_SUCCESS;

   printf("INUG_close system \n");
   status = INUG_close(CMDP_sysHandle, 0);
   CMDP_ASSERT(status);

   printf("INUG_ioctl stop logger\n");
   status = INUG_ioctl(CMDP_loggerHandle, INUG_IOCTL_LOGGER_STOP_E, NULL);
   CMDP_ASSERT(status);

   printf("INUG_close logger \n");
   status = INUG_close(CMDP_loggerHandle, 0);
   CMDP_ASSERT(status);

   //printf("INUG_ioctl stop system\n");
   //status = INUG_ioctl(CMDP_sysHandle, INU_CMDG_STOP_E, NULL);
   //CMDP_ASSERT(status);

   printf("INUG_deinit \n");
   status = INUG_deinit();
   CMDP_ASSERT(status);
}

/****************************************************************************
*
*  Function Name: CMDP_targetConfigSensors
*
*  Description: Configure all sensors
*
*  Inputs: resolution, frequency, which sensor to turn on
*
*  Returns:
*
****************************************************************************/
void CMDP_targetConfigSensors(UINT32 CMDG_sensorResid, UINT32 CMDG_sensorFps, UINT32 CMDG_sensorOn)
{
   ERRG_codeE status=(ERRG_codeE)RET_SUCCESS;
   INUG_ioctlSensorConfigT sensorConfig;

   switch(CMDG_sensorOn)
   {
   case 0:
       sensorConfig.sensorSelect = INU_DEFSG_SENSOR_0_E;
       status = INUG_ioctl(CMDP_sysHandle,INUG_IOCTL_SYSTEM_SENSOR_GET_CFG, &sensorConfig);
       CMDP_ASSERT(status);
       sensorConfig.resolutionId                    = (INU_DEFSG_resolutionIdE)CMDG_sensorResid;
       sensorConfig.fps                             = CMDG_sensorFps;
       sensorConfig.format                          = (INU_DEFSG_sensorOutFormatE)INU_DEFSG_SENSOR_BAYER_RAW_10_E;
       sensorConfig.sensorFunc                      = INU_DEFSG_SEN_MASTER_E;
       sensorConfig.sensorCmd                       = INU_DEFSG_SENSOR_CONFIG_E;
       status = INUG_ioctl(CMDP_sysHandle, INUG_IOCTL_SYSTEM_SENSOR_SET_CFG, &sensorConfig);
       CMDP_ASSERT(status);

       sensorConfig.sensorSelect = INU_DEFSG_SENSOR_1_E;
       status = INUG_ioctl(CMDP_sysHandle,INUG_IOCTL_SYSTEM_SENSOR_GET_CFG, &sensorConfig);
       CMDP_ASSERT(status);
       sensorConfig.resolutionId                    = (INU_DEFSG_resolutionIdE)CMDG_sensorResid;
       sensorConfig.fps                             = CMDG_sensorFps;
       sensorConfig.format                          = (INU_DEFSG_sensorOutFormatE)INU_DEFSG_SENSOR_BAYER_RAW_10_E;
       sensorConfig.sensorFunc                      = INU_DEFSG_SEN_SLAVE_E;
       sensorConfig.sensorCmd                       = INU_DEFSG_SENSOR_CONFIG_E;
       status = INUG_ioctl(CMDP_sysHandle, INUG_IOCTL_SYSTEM_SENSOR_SET_CFG, &sensorConfig);
       CMDP_ASSERT(status);
       break;

    case 1:
       sensorConfig.sensorSelect = INU_DEFSG_SENSOR_2_E;
       status = INUG_ioctl(CMDP_sysHandle,INUG_IOCTL_SYSTEM_SENSOR_GET_CFG, &sensorConfig);
       CMDP_ASSERT(status);
       sensorConfig.resolutionId                    = (INU_DEFSG_resolutionIdE)CMDG_sensorResid;
       sensorConfig.fps                             = CMDG_sensorFps;
       sensorConfig.format                          = (INU_DEFSG_sensorOutFormatE)INU_DEFSG_SENSOR_RGB_565_E;
       sensorConfig.sensorFunc                      = INU_DEFSG_SEN_SINGLE_E;
       sensorConfig.sensorCmd                       = INU_DEFSG_SENSOR_CONFIG_E;
       status = INUG_ioctl(CMDP_sysHandle, INUG_IOCTL_SYSTEM_SENSOR_SET_CFG, &sensorConfig);
       CMDP_ASSERT(status);
       break;
   }
}


/****************************************************************************
*
*  Function Name: CMDP_targetActivateSensor
*
*  Description: Activate the requested sensor
*
*  Inputs:
*
*  Returns:
*
****************************************************************************/
void CMDP_targetActivateSensor(INU_DEFSG_senSelectE select, INU_DEFSG_sensorOutFormatE sensorOutFormat)
{
   INUG_ioctlSensorConfigT sensorConfig;
   ERRG_codeE status=(ERRG_codeE)RET_SUCCESS;

   //activate sensor
   sensorConfig.sensorSelect = select;
   status = INUG_ioctl(CMDP_sysHandle,INUG_IOCTL_SYSTEM_SENSOR_GET_CFG, &sensorConfig);
   CMDP_ASSERT(status);
   sensorConfig.resolutionId                    = (INU_DEFSG_resolutionIdE)CMDG_sensorResid;
   sensorConfig.fps                             = CMDG_sensorFps;
   sensorConfig.format                          = sensorOutFormat;
   sensorConfig.sensorFunc                      = INU_DEFSG_SEN_MASTER_E;
   sensorConfig.sensorCmd                       = INU_DEFSG_SENSOR_ACTIVATE_E;
   status = INUG_ioctl(CMDP_sysHandle, INUG_IOCTL_SYSTEM_SENSOR_SET_CFG, &sensorConfig);
   CMDP_ASSERT(status);
}

/****************************************************************************
*
*  Function Name: CMDP_targetConfigGenerator
*
*  Description: Activate the requested sensor
*
*  Inputs:
*
*  Returns:
*
****************************************************************************/
void CMDP_targetConfigGenerator(INU_DEFSG_resolutionIdE resId, int fps, int mode)
{
   unsigned int status;
   inu_device__writeRegT reg;
   int width,height;

   switch (resId)
   {
   case INU_DEFSG_RES_VGA_E:
      width  = 640;
      height = 480;
      break;
   case INU_DEFSG_RES_HD_E:
      width  = 1280;
      height = 960;
      break;
   default:
      width  = 640;
      height = 480;
   }

   reg.phase = INU_DEFSG_DB_PHASE_1_E;
   reg.regType = INU_DEVICE__REG_SOC_E;
   reg.socReg.addr = 0x08000020;
   // use generator 0 for all inputs
   reg.socReg.val = 0x00008333;
   status = INUG_ioctl(CMDP_sysHandle,INUG_IOCTL_SYSTEM_WR_REG_E, &reg);
   CMDP_ASSERT(status);
   reg.socReg.addr = 0x08000024;
   reg.socReg.val = 0x10111;
   status = INUG_ioctl(CMDP_sysHandle,INUG_IOCTL_SYSTEM_WR_REG_E, &reg);
   CMDP_ASSERT(status);

   // Lane configure
   reg.socReg.addr = 0x08000008;
   reg.socReg.val = (0x41000000) | (width-1) | ((height-1) <<12);
   status = INUG_ioctl(CMDP_sysHandle,INUG_IOCTL_SYSTEM_WR_REG_E, &reg);
   CMDP_ASSERT(status);

   reg.socReg.addr = 0x0800000C;
   reg.socReg.val = (0x41000000) | (width-1) | ((height-1) <<12);
   status = INUG_ioctl(CMDP_sysHandle,INUG_IOCTL_SYSTEM_WR_REG_E, &reg);
   CMDP_ASSERT(status);

   reg.socReg.addr = 0x08000010;
   reg.socReg.val = (0x51000000) | (width-1) | ((height-1) <<12);
   status = INUG_ioctl(CMDP_sysHandle,INUG_IOCTL_SYSTEM_WR_REG_E, &reg);
   CMDP_ASSERT(status);

   //Bypass all
   reg.socReg.addr = 0x08000004;
   reg.socReg.val = 0xAAAAAAAA;
   status = INUG_ioctl(CMDP_sysHandle,INUG_IOCTL_SYSTEM_WR_REG_E, &reg);
   CMDP_ASSERT(status);
   reg.socReg.addr = 0x08000014;
   reg.socReg.val = 0xFFAAAAAA;
   status = INUG_ioctl(CMDP_sysHandle,INUG_IOCTL_SYSTEM_WR_REG_E, &reg);
   CMDP_ASSERT(status);
   reg.socReg.addr = 0x08000040;
   reg.socReg.val = 0x00000003;
   status = INUG_ioctl(CMDP_sysHandle,INUG_IOCTL_SYSTEM_WR_REG_E, &reg);
   CMDP_ASSERT(status);

   // use this values to set dsr in CMDP_targetConfigIae
   CMDG_iaeWidth = width;
   CMDG_iaeHeight = height;
}

/****************************************************************************
*
*  Function Name: CMDP_targetConfigIae
*
*  Description: Configure IAE
*
*  Inputs:
*
*  Returns:
*
****************************************************************************/
void CMDP_targetConfigIae(void)
{
   ERRG_codeE status=(ERRG_codeE)RET_SUCCESS;
   INUG_ioctlIaeConfigT iaeConfig;

   status = INUG_ioctl(CMDP_sysHandle, INUG_IOCTL_SYSTEM_IAE_GET_CONFIG, &iaeConfig);
   CMDP_ASSERT(status);

//   iaeConfig.iau[INU_DEFSG_IAU_0_E].dsr.width =  (UINT16)CMDG_iaeWidth;
//  iaeConfig.iau[INU_DEFSG_IAU_0_E].dsr.height = (UINT16)CMDG_iaeHeight;

//   iaeConfig.iau[INU_DEFSG_IAU_1_E].dsr.width =  (UINT16)CMDG_iaeWidth;
//  iaeConfig.iau[INU_DEFSG_IAU_1_E].dsr.height = (UINT16)CMDG_iaeHeight;

   status = INUG_ioctl(CMDP_sysHandle, INUG_IOCTL_SYSTEM_IAE_SET_CONFIG, &iaeConfig);
   CMDP_ASSERT(status);
}


/****************************************************************************
*
*  Function Name: CMDP_targetConfigDpe
*
*  Description: Configure DPE
*
*  Inputs: channel, channelselect
*
*  Returns:
*
****************************************************************************/
void CMDP_targetConfigDpe(UINT32 dpeChannel,UINT32 dpeChanSelect)
{
   ERRG_codeE status=(ERRG_codeE)RET_SUCCESS;
   INUG_ioctlDpeConfigT dpeDepth;

   status = INUG_ioctl(CMDP_sysHandle, INUG_IOCTL_SYSTEM_DPE_GET_CONFIG_E, &dpeDepth);
   CMDP_ASSERT(status);
/*
   dpeDepth.minDisparity      =17;
   dpeDepth.maxDisparity      =143;
   dpeDepth.zMin              =0;
   dpeDepth.disparityOffset   =10;
   dpeDepth.dpeOpticalFactor  =35747;
*/
   //selecting depth as output
   dpeDepth.ctrl = (INU_DEFSG_dpeOutSelectE)dpeChanSelect;

   //activating depth channel
//   dpeDepth.outChan[(INU_DEFSG_dpeChanIdE)dpeChannel].active= TRUE;
//   dpeDepth.outChan[(INU_DEFSG_dpeChanIdE)dpeChannel].outWidth=  (UINT32)CMDG_iaeWidth;
//   dpeDepth.outChan[(INU_DEFSG_dpeChanIdE)dpeChannel].outHeight= (UINT32)CMDG_iaeHeight;
//   dpeDepth.outChan[(INU_DEFSG_dpeChanIdE)dpeChannel].bufferWidth  =(UINT32) CMDG_iaeWidth;
//   dpeDepth.outChan[(INU_DEFSG_dpeChanIdE)dpeChannel].bufferHeight =(UINT32) CMDG_iaeHeight;

   status = INUG_ioctl(CMDP_sysHandle, INUG_IOCTL_SYSTEM_DPE_SET_CONFIG_E, &dpeDepth);
   CMDP_ASSERT(status);
}

/****************************************************************************
*
*  Function Name: CMDP_targetStartChan
*
*  Description: Start IAE channel
*
*  Inputs:
*
*  Returns:
*
****************************************************************************/
void CMDP_targetStartChan(UINT32 Channel,UINT32 ChanSelect)
{
   ERRG_codeE status=(ERRG_codeE)RET_SUCCESS;
   INUG_ioctlIaeConfigT iaeConfig;
  int width, height;
   if (Channel == INU_DEFSG_IAE_CH_WEBCAM_E)
   {
      width = CMDG_iaeWidthWebcam;
      height = CMDG_iaeHeightWebcam;
   }
   else
   {
      width = CMDG_iaeWidth;
      height = CMDG_iaeHeight;
   }

   status = INUG_ioctl(CMDP_sysHandle, INUG_IOCTL_SYSTEM_IAE_GET_CONFIG, &iaeConfig);
   CMDP_ASSERT(status);

   iaeConfig.outChan[(INU_DEFSG_iaeChanIdE)Channel].active = TRUE;
   iaeConfig.outChan[(INU_DEFSG_iaeChanIdE)Channel].outWidth = CMDG_iaeWidth;
   iaeConfig.outChan[(INU_DEFSG_iaeChanIdE)Channel].outHeight = CMDG_iaeHeight;
//   iaeConfig.outChan[(INU_DEFSG_iaeChanIdE)Channel].outChanSelect = (INU_DEFSG_iamChanSelectE)ChanSelect;
   iaeConfig.outChan[(INU_DEFSG_iaeChanIdE)Channel].bufferWidth = CMDG_iaeWidth;
   iaeConfig.outChan[(INU_DEFSG_iaeChanIdE)Channel].bufferHeight = CMDG_iaeHeight;
//   iaeConfig.outChan[(INU_DEFSG_iaeChanIdE)Channel].frameStartX = 0;
//   iaeConfig.outChan[(INU_DEFSG_iaeChanIdE)Channel].frameStartY = 0;
   //iaeConfig.slu[INU_DEFSG_SENSOR_0_E].interfaceType = INU_DEFSG_SLU_MIPI_INTERFACE_E;
   //iaeConfig.slu[INU_DEFSG_SENSOR_1_E].interfaceType = INU_DEFSG_SLU_MIPI_INTERFACE_E;
   //iaeConfig.slu[INU_DEFSG_SENSOR_2_E].interfaceType = INU_DEFSG_SLU_MIPI_INTERFACE_E;

   status = INUG_ioctl(CMDP_sysHandle, INUG_IOCTL_SYSTEM_IAE_SET_CONFIG, &iaeConfig);
   CMDP_ASSERT(status);
}

/****************************************************************************
*
*  Function Name: CMDP_readFrames
*
*  Description: read frames
*
*  Inputs:
*
*  Returns:
*
****************************************************************************/
void CMDP_readFrames(UINT32 channel, UINT32 ChanSelect, INUG_handleT CMDP_Handle, clock_t Test_dur, UINT32 streamid, void *streamSpecificDataP)
{
   int width = 0;
   int height = 0;
   int          frame_cnt_err=0;
   int          numbytes_cnt_err=0;
   int          read_frame_cnt_err=0;
   int          multiply=2;
   UINT32       frame_cnt=1;
   INU_CMDG_streamReadT INU_CMDG_streamRead;
//   INU_CMDG_streamRead.mode = 0;
   FILE *fileP = NULL;
   char         filename[32];

   #if (defined _WIN32) || (defined _WIN64)
   clock_t      start_t, end_t, total_t=0;
   unsigned int frameSize;
   unsigned int fps;
   unsigned int mbs;
   #endif

   INU_CMDG_streamRead.specificStreamDataP = streamSpecificDataP;

#if defined(__linux__)
   struct timeval start, end;
   long secs_used=0,micros_used=0;
#endif

   printf("read frames\n");
   if (ChanSelect==2) //*2 for each pixels *2 for interleaving
        multiply=4;

   if (channel == INU_DEFSG_IAE_CH_WEBCAM_E)
   {
      width = CMDG_iaeWidthWebcam;
      height = CMDG_iaeHeightWebcam;
   }
   else
   {
      width = CMDG_iaeWidth;
      height = CMDG_iaeHeight;
   }

#if (defined _WIN32) || (defined _WIN64)
   start_t = clock();
   while(total_t<Test_dur) //count missed frames and number of bytes per frame
#elif defined(__linux__)
   gettimeofday(&start, NULL);
   while(micros_used<Test_dur) //count missed frames and number of bytes per frame
#endif
   {

        INU_CMDG_streamRead.streamId=streamid;
        ret = INUG_read(CMDP_Handle, &INU_CMDG_streamRead, TIMEOUT_MSEC_E);
//         printf("INU_CMDG_streamRead.mode=%d \n",INU_CMDG_streamRead.mode);
//         printf("INU_CMDG_streamRead.bufSize=%d \n",INU_CMDG_streamRead.bufSize);
        CMDP_ASSERT(ret);
        if(ERRG_FAILED(ret))
        {
            INUG_err2Str(ret, errStr);
            printf("--->recieve error %s\n", errStr);
            read_frame_cnt_err++;
            //break;
        }
        else
        {
         printf("Frame received size %d, frame count = %d \n",INU_CMDG_streamRead.bufSize, INU_CMDG_streamRead.frameCntr);

            //check for right num of bytes in frame
//            if (INU_CMDG_streamRead.bufSize!=(width*height*multiply))
//            {
//                printf("Error number of bytes not right %d!=%d \n",INU_CMDG_streamRead.bufSize, (width*height*multiply));
//                numbytes_cnt_err++;
                //break;
            //}
      /*
            else
            {
               int i =0;
               for (i = 0; i < INU_CMDG_streamRead.bufSize; i++)
               {
                  printf("%x",((UINT8*)INU_CMDG_streamRead.bufP)[i]);
               }
            }*/

            // check for frame counter
            /*if (INU_CMDG_streamRead.frameCntr!=frame_cnt)
            {
                printf("Error: frame number %d, should be %d\n", INU_CMDG_streamRead.frameCntr,frame_cnt);
                frame_cnt_err+=(INU_CMDG_streamRead.frameCntr-frame_cnt);
                frame_cnt=(INU_CMDG_streamRead.frameCntr);
            }*/

            // frame store to file
            sprintf(filename,"frame%u.raw", frame_cnt);
            fileP = fopen(filename, "wb");
            if (fileP)
            {
                if(fwrite(INU_CMDG_streamRead.bufP, INU_CMDG_streamRead.bufSize, 1, fileP) == 1)
                    printf("wrote frame %u at %s\n", frame_cnt, filename);
                else
                    printf("failed to write frame\n");
                fflush(fileP);
                fclose(fileP);
            }
            else
                printf("failed to open frame %s\n", filename);


    //        printf("frame num %u, streamID %d\n", frame_cnt, streamid);

            ret = INUG_ioctl(CMDP_Handle, INU_CMDG_FREE_BUF_E, &INU_CMDG_streamRead.freeBufP);
            CMDP_ASSERT(ret);
            if(ERRG_FAILED(ret))
            {
                INUG_err2Str(ret, errStr);
                printf("receive error %s\n", errStr);
            }
            frame_cnt++;
        }
#if (defined _WIN32) || (defined _WIN64)
     end_t = clock();
      total_t = (clock_t)((double)(end_t - start_t) / CLOCKS_PER_SEC);
#elif defined(__linux__)
      gettimeofday(&end, NULL);
      secs_used=(end.tv_sec - start.tv_sec); //avoid overflow by subtracting first
      micros_used= ((secs_used*1000000) + end.tv_usec) - (start.tv_usec);
      micros_used=micros_used/CLOCKS_PER_SEC;
#endif
   }

   #if (defined _WIN32) || (defined _WIN64)
   fps = frame_cnt/total_t;
   frameSize = width*height*multiply;
   mbs= (fps*frameSize)/1024/1024;

   printf("Summary of StreamID: %d\n", streamid);
   printf("Total seconds taken for %d frames by CPU: %d\n",frame_cnt ,total_t);
   printf("Frame size %dx%dx%d(%d bytes), fps=%d, throughput: %dMB/s(%dMbit/s)\n",width, height,multiply, frameSize,fps,mbs,mbs*8);

   #elif defined(__linux__)
   printf("Total seconds taken for %u frames by CPU: %ld\n",frame_cnt ,micros_used);
   #endif
   printf("frame_cnt_err %d\n", frame_cnt_err);
   printf("numbytes_cnt_err %d\n", numbytes_cnt_err);
   printf("read_frame_cnt_err %d\n", read_frame_cnt_err);
}

/****************************************************************************
*
*  Function Name: CMDP_writeFrames
*
*  Description: read frames
*
*  Inputs:
*
*  Returns:
*
****************************************************************************/
void CMDP_writeFrames(clock_t Test_dur, UINT8 *buf, UINT32 bufSize, UINT32 CMDG_sensorFps, UINT32 loopBackFlag, INU_CMDG_streamReadT *videoStreamReadParamsP)
{
   UINT64   framePeriodUsec;
   UINT64   sleepDurMsec;
   UINT32   frameCntr = 0;
   INT64    leftUsec;
   UINT64   startUsec;
   UINT64   endUsec;
   UINT64   sleepDurUsec;
   UINT32   failCnt = 0;
   UINT32   tooLateCnt = 0;

   framePeriodUsec = 1000*1000/CMDG_sensorFps + ((1000*1000 % CMDG_sensorFps) ? 1 : 0);
   leftUsec = 1000*1000*Test_dur;
   INUG_ioctl(CMDP_sysHandle, INUG_IOCTL_SYSTEM_GET_STATS_E, NULL);
#if defined(__linux__)
   printf("bufSize=%d frame period=%" PRId64 " usec fps:requested:%d->actual:%" PRId64 "\n",bufSize, framePeriodUsec, CMDG_sensorFps,(1000*1000)/framePeriodUsec);
#else
   printf("bufSize=%d frame period=%llu usec fps:requested:%d->actual:%llu\n",bufSize, framePeriodUsec, CMDG_sensorFps,(1000*1000)/framePeriodUsec);
#endif

   while(leftUsec > 0)
   {
      OS_LYRG_getUsecTime(&startUsec);

      ret = INUG_write(CMDP_injectHandle, buf, bufSize, INU_CMDG_DATA_TYPE_INJECT_E);
      if(ERRG_FAILED(ret))
      {
         failCnt++;
      }

      if(ERRG_SUCCEEDED(ret))
      {
         if(loopBackFlag)
         {
            ret = INUG_read(CMDP_videoHandle, videoStreamReadParamsP, TIMEOUT_MSEC_E);
            if(ERRG_FAILED(ret))
            {
               INUG_ioctl(CMDP_sysHandle, INUG_IOCTL_SYSTEM_GET_STATS_E, NULL);
               printf("read injection data fail\n");
            }
         }
      }

      if(ERRG_SUCCEEDED(ret))
      {
         frameCntr++;
      }

      OS_LYRG_getUsecTime(&endUsec);
      if(endUsec-startUsec > framePeriodUsec)
      {
         sleepDurUsec = 0;
         tooLateCnt++;
         leftUsec -= (endUsec-startUsec);
      }
      else
      {
         //sleep what is left
         sleepDurUsec = framePeriodUsec-(endUsec-startUsec+1);
         sleepDurMsec = sleepDurUsec/1000 + ((sleepDurUsec % 1000)? 1: 0);
         millisleep_host((unsigned)sleepDurMsec);
         leftUsec -= (sleepDurMsec*1000 + (endUsec-startUsec));
      }
   }

   INUG_ioctl(CMDP_sysHandle, INUG_IOCTL_SYSTEM_GET_STATS_E, NULL);
   printf("send %d frames in %d sec. expected FR=%d, actual FR=%d failed=%d too_late=%d \n",
      frameCntr, (int)Test_dur, CMDG_sensorFps, frameCntr/((int)Test_dur),failCnt, tooLateCnt);

}

/****************************************************************************
*
*  Function Name: CMDP_targetOpenStartStream
*
*  Description: CMDP_writeConfigRegTbl
*               open streaming service
*               start streaming service
*  Inputs:
*
*  Returns:
*
****************************************************************************/
void CMDP_targetOpenStartStream(void *streamStartParams, INUG_serviceIdE serviceId, INUG_handleT *handleP)
 {
    ERRG_codeE status=(ERRG_codeE)RET_SUCCESS;
    status = INUG_open(handleP, serviceId);
    CMDP_ASSERT(status);

    switch (serviceId)
    {
        case INUG_SERVICE_VIDEO_STREAM_E:
        {
            status = INUG_ioctl(CMDP_videoHandle, INUG_IOCTL_VIDEO_STREAM_START_E, streamStartParams);
            break;
        }
        case INUG_SERVICE_DEPTH_STREAM_E:
        {
            status = INUG_ioctl(CMDP_depthHandle, INUG_IOCTL_DEPTH_STREAM_START_E, streamStartParams);
            break;
        }
        case INUG_SERVICE_WEBCAM_STREAM_E:
        {
            status = INUG_ioctl(CMDP_WebcamHandle, INUG_IOCTL_WEBCAM_STREAM_START_E, streamStartParams);
            break;
        }
        case INUG_SERVICE_CLIENT_STREAM_E:
        {
            status = INUG_ioctl(CMDP_ClientHandle, INUG_IOCTL_CLIENT_STREAM_START_E, streamStartParams);
            break;
        }
        case INUG_SERVICE_ALG_STREAM_E:
        {
            status = INUG_ioctl(CMDP_AlgHandle, INUG_IOCTL_ALG_STREAM_START_E, streamStartParams);
            break;
        }
        case INUG_SERVICE_POS_SENSORS_E:
        {
            status = INUG_ioctl(CMDP_PosHandle, INUG_IOCTL_POS_SENSORS_START_E, streamStartParams);
            break;
        }
        case INUG_SERVICE_INJECT_STREAM_E:
        {
            status = INUG_ioctl(CMDP_injectHandle, INUG_IOCTL_INJECT_STREAM_START_E, streamStartParams);
            break;
        }
        case INUG_SERVICE_AUDIO_STREAM_E:
        {
            status = INUG_ioctl(CMDP_audioHandle, INUG_IOCTL_AUDIO_STREAM_START_E, streamStartParams);
            break;
        }

        default:
        break;
    }
 }


/****************************************************************************
*
*  Function Name: CMDP_targetCloseVideo
*
*  Description: deactivate channel 0 stop & close video
*
*  Inputs:
*
*  Returns:
*
****************************************************************************/
 void CMDP_targetCloseVideo(UINT32 streamid)
 {
    ERRG_codeE                  status=(ERRG_codeE)RET_SUCCESS;
    INUG_ioctlIaeConfigT        iaeConfig;
    UINT32                      Channel=0;
    INU_CMDG_videoStreamStopT   videoStreamStopParams;

    videoStreamStopParams.streamId=streamid;
    status = INUG_ioctl(CMDP_sysHandle, INUG_IOCTL_SYSTEM_IAE_GET_CONFIG, &iaeConfig);
    CMDP_ASSERT(status);

//    iaeConfig.outChan[(INU_DEFSG_iaeChanIdE)Channel].active = FALSE;

    status = INUG_ioctl(CMDP_sysHandle, INUG_IOCTL_SYSTEM_IAE_SET_CONFIG, &iaeConfig);
    CMDP_ASSERT(status);

    printf("stop video streaming service\n");
    status = INUG_ioctl(CMDP_videoHandle, INUG_IOCTL_VIDEO_STREAM_STOP_E, &videoStreamStopParams);
    CMDP_ASSERT(status);

    //TODO: add closing sensor and IAE channel
    printf("close video streaming service\n");
    status = INUG_close(CMDP_videoHandle, 0);
    CMDP_ASSERT(status);
 }


/****************************************************************************
*
*  Function Name: CMDP_targetCloseDepth
*
*  Description: stop & close depth
*
*  Inputs:
*
*  Returns:
*
****************************************************************************/
 void CMDP_targetCloseDepth(UINT32 streamid)
 {
    ERRG_codeE status=(ERRG_codeE)RET_SUCCESS;
    INU_CMDG_depthStreamStopT depthStreamStopParams;

    depthStreamStopParams.streamId=streamid;
    printf("stop depth streaming service\n");

    status = INUG_ioctl(CMDP_depthHandle, INUG_IOCTL_DEPTH_STREAM_STOP_E, &depthStreamStopParams);
    CMDP_ASSERT(status);

    status = INUG_close(CMDP_depthHandle, 0);
    CMDP_ASSERT(status);
 }

 /****************************************************************************
*
*  Function Name: CMDP_targetCloseWebcam
*
*  Description: stop & close webcam
*
*  Inputs:
*
*  Returns:
*
****************************************************************************/
void CMDP_targetCloseWebcam(UINT32 streamid)
 {
    ERRG_codeE status=(ERRG_codeE)RET_SUCCESS;
    INU_CMDG_webcamStreamStopT INU_CMDG_webcamStreamStopParams;

    printf("stop webcam streaming service\n");
    status = INUG_ioctl(CMDP_WebcamHandle, INUG_IOCTL_WEBCAM_STREAM_STOP_E, &INU_CMDG_webcamStreamStopParams);

    status = INUG_close(CMDP_WebcamHandle, 0);
    CMDP_ASSERT(status);
 }

/****************************************************************************
*
*  Function Name: CMDP_targetCloseClient
*
*  Description: stop & close client
*
*  Inputs:
*
*  Returns:
*
****************************************************************************/
 void CMDP_targetCloseClient(UINT32 streamid)
 {
    ERRG_codeE status=(ERRG_codeE)RET_SUCCESS;
    INU_CMDG_streamStopT clientStreamStopParams;

    printf("stop Client streaming service\n");
    status = INUG_ioctl(CMDP_ClientHandle, INUG_IOCTL_CLIENT_STREAM_STOP_E, &clientStreamStopParams);

    status = INUG_close(CMDP_ClientHandle, 0);
    CMDP_ASSERT(status);
 }


/****************************************************************************
*
*  Function Name: CMDP_targetClosePos
*
*  Description: stop & close position sensor
*
*  Inputs:
*
*  Returns:
*
****************************************************************************/
void CMDP_targetClosePos( void )
{
  ERRG_codeE status=(ERRG_codeE)RET_SUCCESS;
  INU_CMDG_streamStopT posStreamStopParams;

  printf("stop Position sensors streaming service\n");
  status = INUG_ioctl(CMDP_PosHandle, INUG_IOCTL_POS_SENSORS_STOP_E, &posStreamStopParams);

  status = INUG_close(CMDP_PosHandle, 0);
  CMDP_ASSERT(status);
}


 /****************************************************************************
*
*  Function Name: CMDP_targetCloseAlg
*
*  Description: stop & close alg
*
*  Inputs:
*
*  Returns:
*
****************************************************************************/
 void CMDP_targetCloseAlg(UINT32 streamid)
 {
    ERRG_codeE status=(ERRG_codeE)RET_SUCCESS;
    INU_CMDG_streamStopT algStreamStopParams;

    printf("stop Alg streaming service\n");
    status = INUG_ioctl(CMDP_AlgHandle, INUG_IOCTL_ALG_STREAM_STOP_E, &algStreamStopParams);

    status = INUG_close(CMDP_AlgHandle, 0);
    CMDP_ASSERT(status);
 }


/****************************************************************************
*
*  Function Name: CMDP_targetCloseAudio
*
*  Description:
*
*  Inputs:
*
*  Returns:
*
****************************************************************************/
 void CMDP_targetCloseAudio(UINT32 streamid)
 {
    ERRG_codeE                  status=(ERRG_codeE)RET_SUCCESS;
    UINT32                      Channel=0;
    INU_CMDG_audioStreamStopT   audioStreamStopParams;

    audioStreamStopParams.streamId=streamid;

    printf("stop audio streaming service\n");
    status = INUG_ioctl(CMDP_audioHandle, INUG_IOCTL_AUDIO_STREAM_STOP_E, &audioStreamStopParams);
    CMDP_ASSERT(status);

    printf("close audio streaming service\n");
    status = INUG_close(CMDP_audioHandle, 0);
    CMDP_ASSERT(status);
 }


/****************************************************************************
*
*  Function Name: readBinFile
*
*  Description: read Bin File for luts
*
*  Inputs:
*
*  Returns:
*
****************************************************************************/
void readBinFile(const char *name, SERVICEP_bufT *buffer)
{
    FILE *file;
    file = fopen(name,"rb");  // r for read, b for binary
    if (file)
    {
        fseek(file, 0, SEEK_END);
        buffer->bufsize=ftell(file);
        fseek(file, 0, SEEK_SET);
    }
    else
    {
        printf("ERROR fopen");
    }

    buffer->bufP=(char *)malloc((buffer->bufsize)+1);
    fread(buffer->bufP,buffer->bufsize,1,file);
    fclose(file);
}

void loadLut(char filebin[])
{
    char *LutPtr=NULL;
    char binfile[500];
    SERVICEP_bufT lutfile;
    INU_DEFSG_lutHeaderT lutHdr;
    UINT32 size2malloc;
    readBinFile(filebin,&lutfile);
    size2malloc=sizeof(INU_DEFSG_lutHeaderT) + lutfile.bufsize;
    LutPtr=(char* )malloc(size2malloc);
#if 0
    strcpy(binfile, configfolder);
    strcat(binfile, SLESH"iae_ib_left.bin");
    if (!strcmp (filebin,binfile))
    {
        lutHdr.lutId=setLutModeAB(INU_DEFSG_IAE_LUT_CVL_IB_E,0);
    }
    else
    {
        memset(binfile, 0, sizeof binfile);
        strcpy(binfile, configfolder);
        strcat(binfile, SLESH"iae_ib_right.bin");
        if (!strcmp (filebin,binfile))
        {
            lutHdr.lutId=setLutModeAB(INU_DEFSG_IAE_LUT_CVR_IB_E,0);
        }
        else
        {
            memset(binfile, 0, sizeof binfile);
            strcpy(binfile, configfolder);
            strcat(binfile, SLESH"iae_dsr_left.bin");
            if (!strcmp (filebin,binfile))
            {
                lutHdr.lutId=setLutModeAB(INU_DEFSG_IAE_LUT_CVL_DSR_E,0);
            }
            else
            {
                    memset(binfile, 0, sizeof binfile);
                    strcpy(binfile, configfolder);
                    strcat(binfile, SLESH"iae_dsr_right.bin");
                    if (!strcmp (filebin,binfile))
                        lutHdr.lutId=setLutModeAB(INU_DEFSG_IAE_LUT_CVR_DSR_E,0);
            }
        }
    }
#endif
    memcpy(LutPtr, &lutHdr,sizeof(lutHdr));
    memcpy(LutPtr+sizeof(lutHdr),lutfile.bufP,lutfile.bufsize);
    INUG_write(CMDP_sysHandle, LutPtr, size2malloc, INUG_DATA_TYPE_SYS_LUT_E);
    free(LutPtr);
    free(lutfile.bufP);
    memset(filebin, 0, sizeof binfile);
}


void CMDP_readLUT_from_file()
{
    char filebin[500];
    strcpy(filebin, configfolder);
    strcat(filebin,SLESH"iae_ib_left.bin");
    loadLut(filebin);
    strcpy(filebin, configfolder);
    strcat(filebin,SLESH"iae_ib_right.bin");
    loadLut(filebin);
    strcpy(filebin, configfolder);
    strcat(filebin,SLESH"iae_dsr_left.bin");
    loadLut(filebin);
    strcpy(filebin, configfolder);
    strcat(filebin,SLESH"iae_dsr_right.bin");
    loadLut(filebin);
}

/****************************************************************************
*
*  Function Name: CMDP_writeConfigRegTbl
*
*  Description: Write config reg table to target using INU api.
*
*  Inputs:
*
*  Returns:
*
****************************************************************************/
void CMDP_LoadCeva(INUG_dataTypeE cevaId)
{
    char cevafile[500];
    SERVICEP_bufT cevaPtr;
    strcpy(cevafile, Bootfolder);
    if (cevaId==INUG_DATA_TYPE_SYS_CEVA_A_APP_E)
        strcat(cevafile,SLESH"inu_app_A.cva");
    else if (cevaId==INUG_DATA_TYPE_SYS_CEVA_B_APP_E)
        strcat(cevafile,SLESH"inu_app_B.cva");

    readBinFile(cevafile, &cevaPtr);
    if (cevaId==INUG_DATA_TYPE_SYS_CEVA_A_APP_E)
        INUG_write(CMDP_sysHandle, cevaPtr.bufP, cevaPtr.bufsize, INUG_DATA_TYPE_SYS_CEVA_A_APP_E);
    if (cevaId==INUG_DATA_TYPE_SYS_CEVA_B_APP_E)
        INUG_write(CMDP_sysHandle, cevaPtr.bufP, cevaPtr.bufsize, INUG_DATA_TYPE_SYS_CEVA_B_APP_E);
    free(cevaPtr.bufP);
}


/****************************************************************************
*
*  Function Name: start_sensor
*
*  Description: start sensor.
*
*  Inputs: resolution , frequency, configuration_file
*
*  Returns:
*
****************************************************************************/
void start_sensor(UINT32 CMDG_sensorResid,UINT32 CMDG_sensorFps, char *filename,INU_DEFSG_senSelectE CMDG_sensSelect,INU_DEFSG_sensorOutFormatE CMDG_sensFormat, UINT32 CMDG_sensorOn)
{
    printf("start sensor!!!\n");

   if (!CMDG_isGeneratorMode)
      CMDP_targetConfigSensors(CMDG_sensorResid, CMDG_sensorFps, CMDG_sensorOn);
   if (CMDG_isGeneratorMode)
      CMDP_targetConfigGenerator((INU_DEFSG_resolutionIdE)CMDG_sensorResid, CMDG_sensorFps, 1);
   CMDP_targetConfigIae();
    //activate sensors
   if (!CMDG_isGeneratorMode)
      CMDP_targetActivateSensor(CMDG_sensSelect, CMDG_sensFormat);
    printf("end sensor!!!\n");
}

/****************************************************************************
*
*  Function Name: start client
*
*  Description: start client
*
*  Inputs:
*
*  Returns:
*
****************************************************************************/
void start_client(INU_CMDG_clientStreamStartT *INU_CMDG_clientStreamStartParams)
{
    CMDP_targetOpenStartStream(INU_CMDG_clientStreamStartParams, INUG_SERVICE_CLIENT_STREAM_E, &CMDP_ClientHandle);
}

/****************************************************************************
*
*  Function Name: start alg
*
*  Description: start alg
*
*  Inputs:
*
*  Returns:
*
****************************************************************************/
void start_alg(INU_CMDG_algStreamStartT *INU_CMDG_algStreamStartParams)
{
    ERRG_codeE status=(ERRG_codeE)RET_SUCCESS;
    INUG_ioctlAlgorithmT Algorithm;
    Algorithm.algorithmType=ALGORITHM_TYPE_SENSOR_CONTROL_E;
    Algorithm.algorithmAction=ALGORITHM_ACTION_START_E;
    Algorithm.numOfParams=0; //fixme

    CMDP_targetOpenStartStream(INU_CMDG_algStreamStartParams, INUG_SERVICE_ALG_STREAM_E, &CMDP_AlgHandle);
    status = INUG_ioctl(CMDP_AlgHandle, INUG_IOCTL_ALG_COMMAND_E, &Algorithm);
    CMDP_ASSERT(status);
}

/****************************************************************************
*
*  Function Name: read_production
*
*  Description: read production & calibrarion data
*
*  Inputs:
*
*  Returns:
*
****************************************************************************/
void read_production()
{
   ERRG_codeE status=(ERRG_codeE)RET_SUCCESS;
   INUG_ioctlGetVersionT        *version = (INUG_ioctlGetVersionT *)malloc(sizeof(INUG_ioctlGetVersionT));
   INUG_ioctlCalibrationHdrRdT  *calibration    = (INUG_ioctlCalibrationHdrRdT *)malloc(sizeof(INUG_ioctlCalibrationHdrRdT));
   INUG_ioctlCalibrationRdT     *calibrationRd  = (INUG_ioctlCalibrationRdT *)malloc(sizeof(INUG_ioctlCalibrationRdT));

   status               = INUG_ioctl(CMDP_sysHandle, INUG_IOCTL_SYSTEM_GET_VERSION_E, version);
   status               = INUG_ioctl(CMDP_sysHandle, INUG_IOCTL_SYSTEM_CALIBRATION_HDR_RD_E, calibration);

   calibrationRd->calibrationRdSize    = calibration->calibrationDataSize;
   calibrationRd->calibrationRdBufP    = (UINT8 *)malloc(calibration->calibrationDataSize);
   status                              = INUG_ioctl(CMDP_sysHandle, INUG_IOCTL_SYSTEM_CALIBRATION_RD_E, calibrationRd);
   CMDP_ASSERT(status);
}

/****************************************************************************
*
*  Function Name: start alg
*
*  Description: start alg
*
*  Inputs:
*
*  Returns:
*
****************************************************************************/
void start_ab()
{
    ERRG_codeE status=(ERRG_codeE)RET_SUCCESS;
    INUG_ioctlAlternateConfigT Alternate;

    Alternate.alternateModeEnable=1;
    Alternate.modeANumFrames     =5;
    Alternate.modeBNumFrames     =15;
    status = INUG_ioctl(CMDP_sysHandle, INUG_IOCTL_SYSTEM_ALTERNATE_SET_CONFIG_E, &Alternate);
    CMDP_ASSERT(status);
}

/****************************************************************************
*
*  Function Name: load_XML
*
*  Description: load XML.
*
*  Inputs:
*
*  Returns:
*
****************************************************************************/
void load_XML()
{
    memset(CMDP_configRegTbl,0,sizeof(CMDP_configRegTbl));
    CMDP_configRegCnt=0;
    CMDP_parseConfigIReg(XMLfile, INU_DEFSG_DB_PHASE_0_E);
    printf("writing config reg table %d regs...\n",CMDP_configRegCnt);
    CMDP_writeConfigRegTbl();
}


/****************************************************************************
*
*  Function Name: start_video
*
*  Description: start video.
*
*  Inputs:
*
*  Returns:
*
****************************************************************************/
void start_video(UINT32 iamChannel,UINT32 iamChanSelect, INU_CMDG_videoStreamStartT *videoStreamStartParams)
{
    //activate video channel
    CMDP_targetStartChan(iamChannel, iamChanSelect);
    //Start video
    CMDP_targetOpenStartStream(videoStreamStartParams, INUG_SERVICE_VIDEO_STREAM_E, &CMDP_videoHandle); //interleaving from channel 0
}

/****************************************************************************
*
*  Function Name: start_audio
*
*  Description: start audio.
*
*  Inputs:
*
*  Returns:
*
****************************************************************************/
void start_audio(INU_CMDG_audioStreamStartT *audioStreamStartParams)
{
    //Start audio
    CMDP_targetOpenStartStream(audioStreamStartParams, INUG_SERVICE_AUDIO_STREAM_E, &CMDP_audioHandle);
}


/****************************************************************************
*
*  Function Name: start_injection
*
*  Description: start injection channel.
*
*  Inputs:
*
*  Returns:
*
****************************************************************************/
void start_injection(INU_CMDG_injectStreamStartT *injectStartParamsP, UINT32 CMDG_sensorFps)
{
   ERRG_codeE           status = (ERRG_codeE)RET_SUCCESS;
   INUG_ioctlIaeConfigT iaeConfig;
   INU_DEFSG_iaeChanIdE Channel = INU_DEFSG_IAE_CH_INJECT_E;

   status = INUG_ioctl(CMDP_sysHandle, INUG_IOCTL_SYSTEM_IAE_GET_CONFIG, &iaeConfig);
   CMDP_ASSERT(status);

   status = INUG_open(&CMDP_injectHandle, INUG_SERVICE_INJECT_STREAM_E);
   CMDP_ASSERT(status);

   // activate inject mechanism
//   iaeConfig.iim.injector.enable = TRUE;
//   iaeConfig.iim.injector.fps    = CMDG_sensorFps;
//   iaeConfig.iim.injector.width  = CMDG_iaeWidth;
//   iaeConfig.iim.injector.height = CMDG_iaeHeight;
//   iaeConfig.iim.injector.mode   = INU_DEFSG_INJECT_MODE_STEREO_SENSORS_E;

   // dsr config
//   iaeConfig.iau[INU_DEFSG_IAU_0_E].dsr.width   = (UINT16)CMDG_iaeWidth;
//   iaeConfig.iau[INU_DEFSG_IAU_0_E].dsr.height  = (UINT16)CMDG_iaeHeight;
//   iaeConfig.iau[INU_DEFSG_IAU_1_E].dsr.width   = (UINT16)CMDG_iaeWidth;
//   iaeConfig.iau[INU_DEFSG_IAU_1_E].dsr.height  = (UINT16)CMDG_iaeHeight;

   // activate injection channel
//   iaeConfig.outChan[(INU_DEFSG_iaeChanIdE)Channel].active        = TRUE;
//   iaeConfig.outChan[(INU_DEFSG_iaeChanIdE)Channel].outWidth      = CMDG_iaeWidth;
//   iaeConfig.outChan[(INU_DEFSG_iaeChanIdE)Channel].outHeight     = CMDG_iaeHeight;
//   iaeConfig.outChan[(INU_DEFSG_iaeChanIdE)Channel].bufferWidth   = CMDG_iaeWidth;
//   iaeConfig.outChan[(INU_DEFSG_iaeChanIdE)Channel].bufferHeight  = CMDG_iaeHeight;
//   iaeConfig.outChan[(INU_DEFSG_iaeChanIdE)Channel].outChanSelect = (INU_DEFSG_iamChanSelectE)ChanSelect;

   status = INUG_ioctl(CMDP_sysHandle, INUG_IOCTL_SYSTEM_IAE_SET_CONFIG, &iaeConfig);
   CMDP_ASSERT(status);

   // Start injection service
   status = INUG_ioctl(CMDP_injectHandle, INUG_IOCTL_INJECT_STREAM_START_E, injectStartParamsP);
}

/****************************************************************************
*
*  Function Name: stop_injection
*


*  Description: start injection channel.
*
*  Inputs:
*
*  Returns:
*
****************************************************************************/
void stop_injection(INU_CMDG_injectStreamStopT *injectStopParamsP)
{
   ERRG_codeE           status = (ERRG_codeE)RET_SUCCESS;
   INUG_ioctlIaeConfigT iaeConfig;
   INU_DEFSG_iaeChanIdE Channel = INU_DEFSG_IAE_CH_INJECT_E;

   printf("stop injection streaming service\n");
   status = INUG_ioctl(CMDP_injectHandle, INUG_IOCTL_INJECT_STREAM_STOP_E, injectStopParamsP);
   CMDP_ASSERT(status);

   status = INUG_ioctl(CMDP_sysHandle, INUG_IOCTL_SYSTEM_IAE_GET_CONFIG, &iaeConfig);
   CMDP_ASSERT(status);

//   iaeConfig.outChan[INU_DEFSG_IAE_CH_INJECT_E].active = FALSE;

   status = INUG_ioctl(CMDP_sysHandle, INUG_IOCTL_SYSTEM_IAE_SET_CONFIG, &iaeConfig);
   CMDP_ASSERT(status);

   //TODO: add closing sensor and IAE channel
   printf("close injection streaming service\n");
   status = INUG_close(CMDP_injectHandle, 0);
   CMDP_ASSERT(status);
}

/****************************************************************************
*
*  Function Name: start_depth
*
*  Description: start depth.
*
*  Inputs:
*
*  Returns:
*
****************************************************************************/
void start_depth(UINT32 dpeChannel,UINT32 dpeChanSelect, INU_CMDG_depthStreamStartT *depthStreamStartParams)
{
   printf("Depth stream needs to be implemented! exit\n");
   //CMDP_targetStartChan(INU_DEFSG_IAE_CH_VID_0_E, INU_DEFSG_IAM_CH_SELECT_LEFT_E);
    //activate depth channel
    //CMDP_targetConfigDpe(dpeChannel, dpeChannel);
    //Start depth
    //CMDP_targetOpenStartStream(depthStreamStartParams, INUG_SERVICE_DEPTH_STREAM_E, &CMDP_depthHandle);
}

/****************************************************************************
*
*  Function Name: start_depth
*
*  Description: start depth.
*
*  Inputs:
*
*  Returns:
*
****************************************************************************/
void start_webcam(UINT32 colorChannel,UINT32 colorChanSelect, INU_CMDG_webcamStreamStartT *INU_CMDG_webcamStreamStartParams)
{
    CMDP_targetStartChan(colorChannel, colorChanSelect);
    CMDP_targetOpenStartStream(INU_CMDG_webcamStreamStartParams, INUG_SERVICE_WEBCAM_STREAM_E, &CMDP_WebcamHandle);
}

/****************************************************************************
*
*  Function Name: CMDP_targetTestAudio
*
*  Description: runs audio test
*
*  Inputs: test duration
*
*  Returns:
*
****************************************************************************/
void CMDP_targetTestAudio(UINT32 CMDG_sensorResid, clock_t Test_dur)
{
   INU_CMDG_audioStreamStartT *audioStreamStartParams   =NULL;
   audioStreamStartParams=(INU_CMDG_videoStreamStartT* )malloc(sizeof(INU_CMDG_videoStreamStartT));
   audioStreamStartParams->dataType = INUG_DATA_TYPE_AUDIO_STREAM_E;

   CMDG_iaeWidth = 256; //just to make it 512
   CMDG_iaeHeight = 1;

   //start_ab();
   read_production();
//   start_sensor(CMDG_sensorResid, CMDG_sensorFps, filename, INU_DEFSG_SENSOR_0_E, INU_DEFSG_SENSOR_BAYER_RAW_10_E, 0);
   start_audio(audioStreamStartParams);
//  NOT SUPPORTED - need to adapt to new threads functionality
//   CMDP_readFrames(0, CMDP_audioHandle, Test_dur, audioStreamStartParams->streamId, NULL);
   CMDP_targetCloseAudio(audioStreamStartParams->streamId); //closing channel
   free(audioStreamStartParams);
}


/****************************************************************************
*
*  Function Name: CMDP_targetTestVideo
*
*  Description: runs video test- reads configuration from configuration_file, configures sensors & iae, starts channels & video stream, checks frames output.
*
*  Inputs: resolution , frequency, channel, channel select, config_file
*
*  Returns:
*
****************************************************************************/
int CMDP_targetTestVideo(void* params)
 {
    INU_CMDG_videoStreamStartT videoStreamStartParams;
    INU_CMDG_videoSpecificDataT videoSpecificData;
    videoStreamStartParams.dataType = INUG_DATA_TYPE_VIDEO_STREAM_CHAN0_E;

    TestParamsT *f = (TestParamsT*)params;
    char *configfile = f->filename;
    UINT32 iamChannel = f->iamChannel;
    UINT32 iamChanSelect = f->iamChanSelect;
    UINT32 resId = f->CMDG_sensorResid;
    UINT32 fps = f->CMDG_sensorFps;

   OS_LYRG_lockMutex(&mutex);
//   if (!isStereoSensorStarted)
//   {
//      start_sensor(resId, fps, configfile, INU_DEFSG_SENSOR_0_E, INU_DEFSG_SENSOR_BAYER_RAW_10_E, 0);
//      isStereoSensorStarted = true;
//   }
   start_video(iamChannel, iamChanSelect, &videoStreamStartParams);
   openedStreamsCnt++;
   OS_LYRG_unlockMutex(&mutex);

   while (openedStreamsCnt != requiredStreamsCnt)
   {
      //do nothing. wait until all streams will start
   }
   // We assume that readFrames func, takes "long" time to be done, and therefore there is no chance for deadlock since 'openedStreamsCnt' changes.
    CMDP_readFrames(iamChannel, iamChanSelect, CMDP_videoHandle, Test_dur, (&videoStreamStartParams)->streamId, &videoSpecificData);

   OS_LYRG_lockMutex(&mutex);
      openedStreamsCnt--;
   OS_LYRG_unlockMutex(&mutex);

   while (openedStreamsCnt != 0)
   {
      //do nothing. wait until all streams will finish to read
   }

    CMDP_targetCloseVideo((&videoStreamStartParams)->streamId); //closing channel      [Dima] - calls the IOCTL

   return 0;
 }

/****************************************************************************
*
*  Function Name: CMDP_targetTestInjection
*
*  Description: runs video test- reads configuration from configuration_file, configures sensors & iae, starts channels & video stream, checks frames output.
*
*  Inputs: resolution , frequency, channel, channel select, config_file
*
*  Returns:
*
****************************************************************************/
void CMDP_targetTestInjection(UINT32 CMDG_sensorResid, UINT32 CMDG_sensorFps, clock_t Test_dur, UINT32 loopBackFlag)
{
   INU_CMDG_injectStreamStartT   *injectStartParamsP = NULL;
   INU_CMDG_injectStreamStopT    *injectStopParamsP = NULL;
   INU_CMDG_videoStreamStartT    *videoStreamStartParamsP = NULL;
   INU_CMDG_videoStreamStopT     *videoStreamStopParamsP = NULL;
   INU_CMDG_streamReadT          *videoStreamReadParamsP = NULL;
   UINT8                         *bufP;
   UINT32                        bufSize;

   if(CMDG_sensorResid == INU_DEFSG_RES_HD_E)
   {
      CMDG_iaeWidth  = 1280;
      CMDG_iaeHeight = 960;
   }
   else if(CMDG_sensorResid == INU_DEFSG_RES_VGA_E)
   {
      CMDG_iaeWidth  = 640;
      CMDG_iaeHeight = 480;
   }

   bufSize = 4*(CMDG_iaeWidth * CMDG_iaeHeight);

   bufP = (UINT8 *)malloc(bufSize);

   injectStartParamsP = (INU_CMDG_injectStreamStartT* )malloc(sizeof(INU_CMDG_injectStreamStartT));
   injectStartParamsP->dataType = INU_CMDG_DATA_TYPE_INJECT_E;

   injectStopParamsP = (INU_CMDG_injectStreamStopT* )malloc(sizeof(INU_CMDG_injectStreamStopT));
   injectStopParamsP->streamId = injectStartParamsP->streamId;

   start_injection(injectStartParamsP, CMDG_sensorFps);
   if(loopBackFlag)
   {
      videoStreamStartParamsP = (INU_CMDG_videoStreamStartT* )malloc(sizeof(INU_CMDG_videoStreamStartT));
      videoStreamStartParamsP->dataType   = INUG_DATA_TYPE_VIDEO_STREAM_CHAN0_E;
//      start_video(INU_DEFSG_IAE_CH_VID_0_E, INU_DEFSG_IAM_CH_SELECT_INTERLEAVE_E, videoStreamStartParamsP);
      videoStreamReadParamsP = (INU_CMDG_streamReadT* )malloc(sizeof(INU_CMDG_streamReadT));
      videoStreamReadParamsP->streamId    = videoStreamStartParamsP->streamId;
   }
   CMDP_writeFrames(Test_dur, bufP, bufSize, CMDG_sensorFps, loopBackFlag, videoStreamReadParamsP);
   stop_injection(injectStopParamsP);
   if(loopBackFlag)
   {
      videoStreamStopParamsP = (INU_CMDG_videoStreamStopT* )malloc(sizeof(INU_CMDG_videoStreamStopT));
      videoStreamStopParamsP->streamId = videoStreamStartParamsP->streamId;
      CMDP_targetCloseVideo(videoStreamStopParamsP->streamId); //closing channel
      free(videoStreamStartParamsP);
      free(videoStreamStopParamsP);
      free(videoStreamReadParamsP);
   }

   free(bufP);
   free(injectStartParamsP);
   free(injectStopParamsP);

}

/****************************************************************************
*
*  Function Name: CMDP_targetTestDepth
*
*  Description: runs depth test- reads configuration from configuration_file, configures sensors iae dpe , starts depth stream, checks frames output.
*
*  Inputs: resolution , frequency, channel, channel select, configuration_file
*
*  Returns:
*
****************************************************************************/
int CMDP_targetTestDepth(void* params)
 {
    INU_CMDG_depthSpecificDataT depthSpecificData;
    INU_CMDG_depthStreamStartT depthStreamStartParams;
    depthStreamStartParams.dataType = INUG_DATA_TYPE_DEPTH_STREAM_E;

   TestParamsT *f = (TestParamsT*)params;
   char *configfile = f->filename;
   UINT32 resId = f->CMDG_sensorResid;
   UINT32 fps = f->CMDG_sensorFps;
   UINT32 dpeChannel = f->dpeChan;
   UINT32 dpeChanSelect = f->dpeChanSel;

   OS_LYRG_lockMutex(&mutex);//to avoid closing streams while open other stream, in multi-thread mode
   if (!isStereoSensorStarted)
   {
      start_sensor(resId, fps, configfile, INU_DEFSG_SENSOR_0_E, INU_DEFSG_SENSOR_BAYER_RAW_10_E, 0);
      isStereoSensorStarted = true;
   }
   start_depth(dpeChannel, dpeChannel, &depthStreamStartParams);
   openedStreamsCnt++;
   OS_LYRG_unlockMutex(&mutex);

   while (openedStreamsCnt != requiredStreamsCnt)
   {
      //do nothing. wait until all streams will start
   }

    if (loadXML)
        load_XML();
   // We assume that readFrames func, takes "long" time to be done, and therefore there is no chance for deadlock since 'openedStreamsCnt' changes.
    CMDP_readFrames(INU_DEFSG_IAE_CH_VID_0_E, dpeChanSelect, CMDP_depthHandle, Test_dur, (&depthStreamStartParams)->streamId, &depthSpecificData);

   OS_LYRG_lockMutex(&mutex);
   openedStreamsCnt--;
   OS_LYRG_unlockMutex(&mutex);

   while (openedStreamsCnt != 0)
   {
      //do nothing. wait until all streams will finish to read
   }
    CMDP_targetCloseDepth((&depthStreamStartParams)->streamId);

   return 0;
 }


/****************************************************************************
*
*  Function Name: CMDP_targetTestWebcam
*
*  Description: runs webcam test- reads configuration from configuration_file, configures sensors , starts webcam stream, checks frames output.
*
*  Inputs: resolution , frequency, channel, channel select, configuration_file
*
*  Returns:
*
****************************************************************************/
//void CMDP_targetTestWebcam(UINT32 CMDG_sensorResid,UINT32 CMDG_sensorFps ,UINT32 colorChannel,UINT32 colorChanSelect, char *filename, clock_t Test_dur)
int CMDP_targetTestWebcam(void* params)
 {
    INU_CMDG_webcamStreamStartT webcamStreamStartParams;
    webcamStreamStartParams.dataType = INUG_DATA_TYPE_WEBCAM_STREAM_E;

   TestParamsT *f = (TestParamsT*)params;
   char *configfile = f->filename;
   UINT32 colorChannel = f->colorChannel;
   UINT32 colorChanSelect = f->colorChanSelect;
   UINT32 resId = f->CMDG_sensorResid;
   UINT32 fps = f->CMDG_sensorFps;


//     if (CMDG_sensorResid==INU_DEFSG_RES_HD_E)
//     {
//         CMDG_iaeWidth=1280;
//         CMDG_iaeHeight=960;
//     }
    if(CMDG_sensorResid== INU_DEFSG_RES_HD_E)
    {
      CMDG_iaeWidthWebcam = 1280;// 640;
      CMDG_iaeHeightWebcam = 960;// 480;
    }

   OS_LYRG_lockMutex(&mutex);//to avoid closing streams while open other stream, in multi-thread mode
      start_sensor(resId, fps, configfile, INU_DEFSG_SENSOR_2_E, INU_DEFSG_SENSOR_RGB_565_E, 1);
      start_webcam(colorChannel, colorChanSelect, &webcamStreamStartParams);
      openedStreamsCnt++;
   OS_LYRG_unlockMutex(&mutex);

   while (openedStreamsCnt != requiredStreamsCnt)
   {
      //do nothing. wait until all streams will start
   }

   // We assume that readFrames func, takes "long" time to be done, and therefore there is no chance for deadlock since 'openedStreamsCnt' changes.
    CMDP_readFrames(colorChannel, colorChanSelect, CMDP_WebcamHandle, Test_dur, (&webcamStreamStartParams)->streamId, NULL);

   OS_LYRG_lockMutex(&mutex);
      openedStreamsCnt--;
   OS_LYRG_unlockMutex(&mutex);

   while (openedStreamsCnt != 0)
   {
      //do nothing. wait until all streams will finish to read
   }

    CMDP_targetCloseWebcam((&webcamStreamStartParams)->streamId);

   return 0;
 }


/****************************************************************************
*
*  Function Name: CMDP_targetStartClient
*
*  Description: send buffer ioctl
*  Inputs:
*
*  Returns:
*
****************************************************************************/
void CMDP_targetSendBufClient(INU_CMDG_streamStartT *clientStreamStartParams)
{
    ERRG_codeE status=(ERRG_codeE)RET_SUCCESS;
    INUG_ioctlClientDataInT client_dataA,client_dataB;
    INU_CMDG_streamReadT streamReadParamsP;
    UINT32 StartingTime[NUMBER_TRIES], EndingTime[NUMBER_TRIES];
    UINT32 TestStartingTime, TestEndingTime, TestDeltaTime;
    UINT32 sum=0, i=0, j=0, max=0, min=0xFFFFFFFF;
    float pps;
    UINT8 bufA[BUFFER_SIZE_A];
    UINT8 bufB[BUFFER_SIZE_B];

   streamReadParamsP.streamId =INUG_SERVICE_CLIENT_STREAM_E;

    printf("sending %d buffers with size of %d,%d, and toggle of %d,%d to client\n",NUMBER_TRIES,BUFFER_SIZE_A,BUFFER_SIZE_B,NUMBER_BUFFER_A,NUMBER_BUFFER_B);

    for (i=0; i<BUFFER_SIZE_A; i++)
           bufA[i]=i;

    for (i=0; i<BUFFER_SIZE_B; i++)
                bufB[i]=i;

    client_dataA.length=BUFFER_SIZE_A;
    client_dataA.dataP=bufA;
    client_dataB.length=BUFFER_SIZE_B;
    client_dataB.dataP=bufB;

   OS_LYRG_getMsecTime(&TestStartingTime);

    i=0;
    while ((i < NUMBER_TRIES) && (ERRG_SUCCEEDED(status)))
    {
              for(j = 0; (j < NUMBER_BUFFER_A) && (ERRG_SUCCEEDED(status)) && (i < NUMBER_TRIES); j++)
              {
                     //start counter
                     OS_LYRG_getMsecTime(&StartingTime[i]);

                     //send packet A
                     if (ERRG_SUCCEEDED(status))
                     {
                           status = INUG_ioctl(CMDP_ClientHandle, INUG_IOCTL_CLIENT_STREAM_SEND_BUF_E, &client_dataA);
                     }

                     //read packet A
                     if (ERRG_SUCCEEDED(status))
                     {
                           status = INUG_read(CMDP_ClientHandle, &streamReadParamsP, TIMEOUT_MSEC_E);
                     }

                     //end counter
                     OS_LYRG_getMsecTime(&EndingTime[i]);

                     if (ERRG_SUCCEEDED(status))
                     {
       #ifdef DATA_CHECK
                           if (memcmp(bufA,streamReadParamsP.bufP,BUFFER_SIZE_A) != 0)
                           {
                                  printf("DATA IS NOT THE SAME! pckt = %d\n",streamReadParamsP.frameCntr);
                            }
       #endif
                           status = INUG_ioctl(CMDP_ClientHandle, INU_CMDG_FREE_BUF_E, &streamReadParamsP.freeBufP);
                     }

                     i++;
              }



              for(j = 0; (j < NUMBER_BUFFER_B) && (ERRG_SUCCEEDED(status)) && (i < NUMBER_TRIES); j++)
              {
                     //start counter
                     OS_LYRG_getMsecTime(&StartingTime[i]);

                     //send packet B
                     if (ERRG_SUCCEEDED(status))
                     {
                           status = INUG_ioctl(CMDP_ClientHandle, INUG_IOCTL_CLIENT_STREAM_SEND_BUF_E, &client_dataB);
                     }

                     //read packet B
                     if (ERRG_SUCCEEDED(status))
                     {
                           status = INUG_read(CMDP_ClientHandle, &streamReadParamsP, TIMEOUT_MSEC_E);
                     }

                     //end counter
                     OS_LYRG_getMsecTime(&EndingTime[i]);

                     if (ERRG_SUCCEEDED(status))
                     {
       #ifdef DATA_CHECK
                           if (memcmp(bufB,streamReadParamsP.bufP,BUFFER_SIZE_B) != 0)
                           {
                                  printf("DATA IS NOT THE SAME! pckt = %d\n",streamReadParamsP.frameCntr);
                           }
       #endif
                           status = INUG_ioctl(CMDP_ClientHandle, INU_CMDG_FREE_BUF_E, &streamReadParamsP.freeBufP);
                     }

                     i++;
              }
    }

    OS_LYRG_getMsecTime(&TestEndingTime);

    for (j = 0; j < i; j++)
    {
      //start calculation for B
      results[j] = (EndingTime[j] - StartingTime[j]);

        //printf("%d\n",results[j]);
        sum+=results[j];

              if (results[j] > max)
                     max = results[j];

              if (results[j] < min)
                     min = results[j];
    }

   TestDeltaTime = TestEndingTime - TestStartingTime;

   pps = (float)((i*1000) / TestDeltaTime);

   printf("total samples = %d, PPS = %f, average RTT = %f, max RTT = %d, min RTT = %d\n", i, pps, (float)sum/NUMBER_TRIES,max,min);
}


/****************************************************************************
*
*  Function Name: CMDP_targetTestClient
*
*  Description: runs client test- reads configuration from configuration_file, configures sensors , starts client stream, checks frames output.
*
*  Inputs: resolution, frequency, configuration_file
*
*  Returns:
*
****************************************************************************/
void CMDP_targetTestClient(UINT32 CMDG_sensorResid,UINT32 CMDG_sensorFps, char *filename)
{
   INU_CMDG_clientStreamStartT *clientStreamStartParams=NULL;
   clientStreamStartParams=(INU_CMDG_clientStreamStartT* )malloc(sizeof(INU_CMDG_clientStreamStartT));
   clientStreamStartParams->dataType = INU_CMDG_DATA_TYPE_CLIENT_E;

   start_sensor(CMDG_sensorResid, CMDG_sensorFps, filename, INU_DEFSG_SENSOR_0_E, INU_DEFSG_SENSOR_BAYER_RAW_10_E, 0);
   CMDP_targetOpenStartStream(clientStreamStartParams, INUG_SERVICE_CLIENT_STREAM_E, &CMDP_ClientHandle);
   CMDP_targetSendBufClient(clientStreamStartParams);
   CMDP_targetCloseClient(clientStreamStartParams->streamId);
   free(clientStreamStartParams);
}


/**
 * struct iio_channel_info - information about a given channel
 * @name: channel name
 * @generic_name: general name for channel type
 * @scale: scale factor to be applied for conversion to si units
 * @offset: offset to be applied for conversion to si units
 * @index: the channel index in the buffer output
 * @bytes: number of bytes occupied in buffer output
 * @mask: a bit mask for the raw output
 * @is_signed: is the raw value stored signed
 * @enabled: is this channel enabled
 **/
struct iio_channel_info {
   char *name;
   char *generic_name;
   float scale;
   float offset;
   unsigned index;
   unsigned bytes;
   unsigned bits_used;
   unsigned shift;
   uint64_t mask;
   unsigned be;
   unsigned is_signed;
   unsigned enabled;
   unsigned location;
};

#define TEST_POS_SENSOR_LENGTH (10000)
#define TEST_POS_SENSOR_SAMPLE_PERIOD_TIME_NS(__hz) (1000000000 / __hz)

typedef struct
{
    UINT32 channels[3];
   UINT64 timestamp;
} IMU_sensorDataT;

typedef struct
{
   IMU_sensorDataT sensorsData[INU_DEFSG_POS_SENSOR_NUM_TYPES_E][TEST_POS_SENSOR_LENGTH];
   UINT32 index[INU_DEFSG_POS_SENSOR_NUM_TYPES_E];
} IMU_testDataT;

IMU_testDataT imuTestData;


void print2byte(int input, struct iio_channel_info *info,IMU_sensorDataT *sampleData,
        int channel)
{
   /* First swap if incorrect endian */
//   if (info->be)
//      input = be16toh((uint16_t)input);
//   else
//      input = le16toh((uint16_t)input);

   /*
    * Shift before conversion to avoid sign extension
    * of left aligned data
    */
   input = input >> info->shift;
   if (info->is_signed) {
      int16_t val = input;
      val &= (1 << info->bits_used) - 1;
      val = (int16_t)(val << (16 - info->bits_used)) >>
         (16 - info->bits_used);
      sampleData->channels[channel] = (UINT32)(((float)val + info->offset)*info->scale);
      //printf("%05f ", ((float)val + info->offset)*info->scale);
   } else {
      uint16_t val = input;
      val &= (1 << info->bits_used) - 1;
      //printf("%05f ", ((float)val + info->offset)*info->scale);
      sampleData->channels[channel] = (UINT32)(((float)val + info->offset)*info->scale);
   }
}


/**
 * process_scan() - print out the values in SI units
 * @data:      pointer to the start of the scan
 * @channels:      information about the channels. Note
 *  size_from_channelarray must have been called first to fill the
 *  location offsets.
 * @num_channels:   number of channels
 **/
void process_scan(char *data,
        struct iio_channel_info *channels,
        int num_channels,
        IMU_sensorDataT *sampleData)
{
   int k;
   for (k = 0; k < num_channels; k++)
      switch (channels[k].bytes) {
         /* only a few cases implemented so far */
      case 2:
         print2byte(*(uint16_t *)(data + channels[k].location),
               &channels[k],sampleData,k);
         break;
      case 4:
         if (!channels[k].is_signed) {
            uint32_t val = *(uint32_t *)
               (data + channels[k].location);
            sampleData->channels[k] = (UINT32)(((float)val + channels[k].offset)*channels[k].scale);

            //printf("%05f ", ((float)val +
            //       channels[k].offset)*
            //       channels[k].scale);

         }
         break;
      case 8:
         if (channels[k].is_signed) {
            int64_t val = *(int64_t *)(data + channels[k].location);
            if ((val >> channels[k].bits_used) & 1)
               val = (val & channels[k].mask) |
                  ~channels[k].mask;
            /* special case for timestamp */
            if (channels[k].scale == 1.0f &&
                channels[k].offset == 0.0f)
            {
#if (defined _WIN32) || (defined _WIN64)
            //printf("%I64d ", val);
            sampleData->timestamp = val;
#else
            //printf("%" PRId64, val);
            sampleData->timestamp = val;
#endif
            }
            else
            {
                  //printf("%05f ", ((float)val + channels[k].offset)*channels[k].scale);

               sampleData->timestamp = (UINT64)(((float)val + channels[k].offset)*channels[k].scale);
            }
         }
         break;
      default:
         break;
      }
        //printf("\n");
}


/****************************************************************************
*
*  Function Name: CMDP_targetTestPos
*
*  Description: runs IMU test- starts the position service, recevies the data and prints to screen.
*
*  Inputs:
*
*  Returns:
*
****************************************************************************/
#define ABS(a)    ((a)>(0) ? (a) : (-(a)))
#define MAX_JITTER_ALLOWED_NS (50000)

#define ANALYZE_STATS

char *channelTypeToString(INU_DEFSG_posSensorChannelTypeE type)
{
   switch(type)
   {
      case(INU_DEFSG_POS_SENSOR_ACCELAROMETER_X_E):
      return "accel_x ";
      case(INU_DEFSG_POS_SENSOR_ACCELAROMETER_Y_E):
      return "accel_y ";
      case(INU_DEFSG_POS_SENSOR_ACCELAROMETER_Z_E):
      return "accel_z ";
      case(INU_DEFSG_POS_SENSOR_GYROSCOPE_X_E):
      return "gyro_x ";
      case(INU_DEFSG_POS_SENSOR_GYROSCOPE_Y_E):
      return "gyro_y ";
      case(INU_DEFSG_POS_SENSOR_GYROSCOPE_Z_E):
      return "gyro_z ";
      case(INU_DEFSG_POS_SENSOR_MAGNETOMETER_X_E):
      return "magn_x ";
      case(INU_DEFSG_POS_SENSOR_MAGNETOMETER_Y_E):
      return "magn_y ";
      case(INU_DEFSG_POS_SENSOR_MAGNETOMETER_Z_E):
      return "magn_z ";
      case(INU_DEFSG_POS_SENSOR_TIMESTAMP_E):
      return "ts ";
      default:
         return "";
   }
}

int CMDP_targetTestPos(void* params)
{
    int i,j;
   ERRG_codeE status=(ERRG_codeE)RET_SUCCESS;
   INU_CMDG_posStartT posStartParams;
   INU_CMDG_streamReadT streamReadParamsP;
   INU_CMDG_sendPosDataT posSensorData;
    UINT64 deltas[INU_DEFSG_POS_SENSOR_NUM_TYPES_E][TEST_POS_SENSOR_LENGTH];
    UINT64 deltasAve[INU_DEFSG_POS_SENSOR_NUM_TYPES_E];
   UINT64 deltasSum[INU_DEFSG_POS_SENSOR_NUM_TYPES_E];
   UINT64 maxJitter;
   UINT64 minJitter;
   UINT64 aveJitter;
   UINT64 jitter;
   UINT64 jitterSamples;
   UINT64 loseCount;
   UINT32 sensor,sample;
   clock_t start_t, end_t, total_t = 0;
   struct iio_channel_info channels[INU_DEFSG_POS_SENSOR_NUM_TYPES_E][POS_DATA_MAX_NUM_OF_CHANNELS];
   INU_DEFSG_posSensorChannelTypeE type[INU_DEFSG_POS_SENSOR_NUM_TYPES_E][POS_DATA_MAX_NUM_OF_CHANNELS];

   TestParamsT *f = (TestParamsT*)params;
   UINT32 frameRate[INU_DEFSG_POS_SENSOR_NUM_TYPES_E];
   UINT32 totalFrameRate = 0;

#if defined(__linux__)
   struct timeval start, end;
   long secs_used = 0, micros_used = 0;
#endif

   OS_LYRG_lockMutex(&mutex);
   openedStreamsCnt++;
   OS_LYRG_unlockMutex(&mutex);

   while (openedStreamsCnt != requiredStreamsCnt)
   {
      //do nothing. wait until all streams will start
   }

   memset(&imuTestData,0,sizeof(imuTestData));
   memset(&posStartParams,0,sizeof(posStartParams));
   CMDP_targetOpenStartStream(&posStartParams, INUG_SERVICE_POS_SENSORS_E, &CMDP_PosHandle);

   for (j = 0; j < INU_DEFSG_POS_SENSOR_NUM_TYPES_E; j++)
   {
      totalFrameRate += (int)posStartParams.odr[j];
      frameRate[j] = (int)posStartParams.odr[j];
      for (i = 0; i < posStartParams.num_channels[j]; i++)
      {
         channels[j][i].bits_used = posStartParams.hdr[j][i].bits_used;
         channels[j][i].bytes = posStartParams.hdr[j][i].bytes;
         channels[j][i].is_signed = posStartParams.hdr[j][i].is_signed;
         channels[j][i].location = posStartParams.hdr[j][i].location;
         channels[j][i].mask = posStartParams.hdr[j][i].mask;
         channels[j][i].offset = posStartParams.hdr[j][i].offset;
         channels[j][i].scale = posStartParams.hdr[j][i].scale;
         channels[j][i].be = posStartParams.hdr[j][i].be;
         channels[j][i].shift = posStartParams.hdr[j][i].shift;
         type[j][i] = posStartParams.hdr[j][i].channelType;
         printf("channel:");
         printf(channelTypeToString(posStartParams.hdr[j][i].channelType));
         printf("scale = %f, bits_used=%d, is_signed=%d\n", channels[j][i].scale, channels[j][i].bits_used, channels[j][i].is_signed);
      }
   }


   j = 0;

   streamReadParamsP.specificStreamDataP = (void*)&posSensorData;

#ifdef ANALYZE_STATS
   printf("********************* Starting to collect samples of IMU. IMU freq %d [hz] *****************************\n", totalFrameRate);
#endif

#if (defined _WIN32) || (defined _WIN64)
   start_t = clock();
   while (total_t < Test_dur)
#elif defined(__linux__)
   gettimeofday(&start, NULL);
   while (micros_used < Test_dur)
#endif
   {

      //start with reading few temperatures
      INU_CMDG_posTemperatureT posTemperature;
      for (j = 0; j < 20; j++)
      {
         status = INUG_ioctl(CMDP_PosHandle, INUG_IOCTL_POS_SENSORS_TEMPERATURE_E, &posTemperature);
         printf("temperature = %f\n", posTemperature.temperature);
      }

      //read packet A
      if (ERRG_SUCCEEDED(status))
      {
         status = INUG_read(CMDP_PosHandle, &streamReadParamsP, TIMEOUT_MSEC_E);
      }

      if (ERRG_SUCCEEDED(status))
      {
#ifdef ANALYZE_STATS
        if (imuTestData.index[posSensorData.sensorType] < TEST_POS_SENSOR_LENGTH)
        {
           process_scan((char*)streamReadParamsP.bufP,
              channels[posSensorData.sensorType] /*channels*/,
              posStartParams.num_channels[posSensorData.sensorType] /* num_channels */,
              &imuTestData.sensorsData[posSensorData.sensorType][imuTestData.index[posSensorData.sensorType]]);

           imuTestData.index[posSensorData.sensorType]++;
        }
#endif
         status = INUG_ioctl(CMDP_PosHandle, INU_CMDG_FREE_BUF_E, &streamReadParamsP.freeBufP);
      }
      else
      {
         printf("TIMEOUT READING IMU! j = %d\n",j);
         status = (ERRG_codeE)RET_SUCCESS;
      }
#if (defined _WIN32) || (defined _WIN64)
     end_t = clock();
     total_t = (clock_t)((double)(end_t - start_t) / CLOCKS_PER_SEC);
#elif defined(__linux__)
     gettimeofday(&end, NULL);
     secs_used = (end.tv_sec - start.tv_sec); //avoid overflow by subtracting first
     micros_used = ((secs_used * 1000000) + end.tv_usec) - (start.tv_usec);
     micros_used = micros_used / CLOCKS_PER_SEC;
#endif
   }

   printf("Done collecting samples, Analyze samples:\n");
#ifdef ANALYZE_STATS
   for (sensor = 0; sensor < INU_DEFSG_POS_SENSOR_NUM_TYPES_E; sensor++)
   {
      printf("sensor %d, num of samples = %d\n",sensor,imuTestData.index[sensor]);
      /* if we have samples in DB */
      if (imuTestData.index[sensor] > 0)
      {
         loseCount = 0;
         deltasAve[sensor] = 0;
         deltasSum[sensor] = 0;

         for (sample = 0; sample < (imuTestData.index[sensor] - 1); sample++)
         {
            deltas[sensor][sample] = imuTestData.sensorsData[sensor][sample+1].timestamp - imuTestData.sensorsData[sensor][sample].timestamp;
            deltasSum[sensor] += deltas[sensor][sample];

            //printf("sensor %d, sample=%d, ts=%lld, delta=%lld\n",sensor,sample,imuTestData.sensorsData[sensor][sample].timestamp,deltas[sensor][sample]);
         }

         deltasAve[sensor] = deltasSum[sensor] / sample;

         if (deltasAve[sensor] == 0)
         {
            printf("sensor = %d error! calc Hz is 0!! skip stats, continue to next sensor\n",sensor);
            continue;
         }

         aveJitter = 0;
         maxJitter = 0;
         minJitter = 9999999999;
         jitterSamples = 0;

         printf("Average Hz achieved = %lld, (ave=%lld,sum=%lld) \n",1000000000 / deltasAve[sensor],deltasAve[sensor],deltasSum[sensor]);

         /* should base lost count on calulcated Hz, TODO when higher bitrate.  */
         //deltasAve[sensor] = TEST_POS_SENSOR_SAMPLE_PERIOD_TIME_NS(frameRate[sensor]);

         for(sample = 0; sample < (imuTestData.index[sensor] - 1); sample++)
         {
            if (deltas[sensor][sample] > (deltasAve[sensor] + deltasAve[sensor]/2)) /* sample which is larger then 1.5*calc Hz is considered a drop */
            {
               loseCount += deltas[sensor][sample] / deltasAve[sensor];
               printf("Lost packet: num lost = %lld, at sample %d\n",(deltas[sensor][sample] / deltasAve[sensor]),sample);
            }
            else
            {
               jitter = ABS((INT64)deltas[sensor][sample] - (INT64)deltasAve[sensor]);
               if (jitter < minJitter)
                  minJitter = jitter;

               if (jitter > maxJitter)
                  maxJitter = jitter;

               if (jitter > MAX_JITTER_ALLOWED_NS)
                  printf("High jitter %lld, found at sample %ld, delta = %lld, deltasAve = %lld\n",jitter,sample,(INT64)deltas[sensor][sample],(INT64)deltasAve[sensor]);

               aveJitter += jitter;
               jitterSamples++;
            }
         }

         if (jitterSamples > 0)
            aveJitter = aveJitter / jitterSamples;

         printf("Total lostCount = %lld, maxJitter = %lld, minJitter = %lld, aveJitter = %lld, jitterSamples = %lld\n",
                loseCount,
                maxJitter,
                minJitter,
                aveJitter,
                jitterSamples);

      }
   }
#endif

   CMDP_targetClosePos();

   OS_LYRG_lockMutex(&mutex);
      openedStreamsCnt--;
   OS_LYRG_unlockMutex(&mutex);

   while (openedStreamsCnt != 0)
   {
      //do nothing. wait until all streams will finish to read
   }
   return 0;
}

int hotplug_callback_func(INU_DEFSG_usbStatusE usbEvent, INU_DEFSG_hwVersionIdU hwVersion)
{
   if (usbEvent == INU_DEFSG_USB_EVT_BOOT_COMPLETE)
   {
      printf("hotplug_callback: USB_BOOT_COMPLETE received, rev = 0x%x, ver = 0x%x, val: 0x%x\n",hwVersion.fields.revisionId,hwVersion.fields.versionId,hwVersion.val);
      testInProgress=1;
   }
   else if (usbEvent == INU_DEFSG_USB_EVT_REMOVAL)
   {
      printf("hotplug_callback: USB_REMOVED received, rev = 0x%x, ver = 0x%x, val: 0x%x\n",hwVersion.fields.revisionId,hwVersion.fields.versionId,hwVersion.val);
      testInProgress=0;
//      INUG_deinit();
//      INUG_boot_deinit();
      // close ServiceManager
   }
   else if (usbEvent == INU_DEFSG_USB_EVT_BOOT_FAILED)
   {
      printf("hotplug_callback: USB_BOOT_FAILED received, rev = 0x%x, ver = 0x%x, val: 0x%x\n",hwVersion.fields.revisionId,hwVersion.fields.versionId,hwVersion.val);
   }
   else
   {
      printf("hotplug_callback: unsupported USB event received, rev = 0x%x, ver = 0x%x, val: 0x%x\n",hwVersion.fields.revisionId,hwVersion.fields.versionId,hwVersion.val);
   }
   return 0;
}

int test_invoke_func()
{
   UINT32 index = 0; //Equivalent to INU_host_testIdE
   TestParamsT f;
   INU_host_testIdE Test = INU_host_videoE;
   // start ServiceManager
   CMDP_targetBasicInit(); //start logger, start system
   switch (Test)
   {
   case (INU_host_videoE) :
      f.filename = configfile;
      f.iamChannel = Channel;
      f.iamChanSelect = ChanSelect;
      f.CMDG_sensorResid = CMDG_sensorResid;
      f.CMDG_sensorFps = CMDG_sensorFps;



      CMDP_targetTestVideo(/*CMDG_sensorResid, CMDG_sensorFps , Channel, ChanSelect, configfile, Test_dur*/ &f); //Video test, configuration- resolution, fps, channel number, channel select, argv address
      break;
      case (INU_host_depthE):
//         CMDP_targetTestDepth(CMDG_sensorResid, CMDG_sensorFps , INU_DEFSG_DPE_CH_DEPTH_E, INU_DEFSG_DPE_OUT_DISP_E, configfile, Test_dur); //Depth test, configuration- resolution, fps, channel number, channel select, argv address
      break;
      case (INU_host_webcamE):
//         CMDP_targetTestWebcam(CMDG_sensorResid, CMDG_sensorFps , INU_DEFSG_IAE_CH_WEBCAM_E, 3, configfile, Test_dur); //Webcam test, configuration- resolution, fps, channel number, channel select, argv address
      break;
      case (INU_host_clientE):
         CMDP_targetTestClient(CMDG_sensorResid, CMDG_sensorFps , configfile); //Client test, configuration- resolution, fps, channel number, channel select, argv address
      break;
      case (INU_host_positionSensorE):
//         CMDP_targetTestPos(generalPurpose, CMDG_sensorFps);
      break;
      case (INU_host_injectionE):
 //        CMDP_targetTestInjection(CMDG_sensorResid, CMDG_sensorFps, Test_dur, 0); //   Injection test, configuration- resolution, fps, test duration in sec,
      break;
      case (INU_host_audioE):
 //        CMDP_targetTestAudio(CMDG_sensorResid, Test_dur); //   Audio test, configuration- test duration in sec,
      break;
   }

   CMDP_targetClose();

   INU_CMDG_clientStreamStartT clientStreamStartParams;
   clientStreamStartParams.dataType = INU_CMDG_DATA_TYPE_CLIENT_E;
   INU_CMDG_algStreamStartT algStreamStartParams;
   algStreamStartParams.dataType = INU_CMDG_DATA_TYPE_ALG_E;

   OS_LYRG_aquireMutex(&mutex);

   //LUT's loading
   if ((!CMDG_isGeneratorMode) && (configfolder != NULL))
      CMDP_readLUT_from_file();
   //Local initializations & configurations
   CMDP_parseConfigIReg(configfile, INU_DEFSG_DB_PHASE_1_E);
   CMDP_writeConfigRegTbl();

  if (generalPurpose)
      start_client(&clientStreamStartParams);

  start_alg(&algStreamStartParams);
#if 0

  TestParamsT testParams;
  testParams.CMDG_sensorResid = CMDG_sensorResid;
  testParams.CMDG_sensorFps = CMDG_sensorFps;
  testParams.filename = configfile;
  testParams.Test_dur = Test_dur;

  while (test)
  {
     if (test & 0x1)
     {
        switch (Test)
        {
         case (INU_host_videoE):
         {
              testParams.iamChannel = Channel;
              testParams.iamChanSelect = ChanSelect;
              testParams.dpeChan = NULL;
              testParams.dpeChanSel = NULL;
              testParams.colorChannel = NULL;
              testParams.colorChanSelect = NULL;

              OS_LYRG_threadParams thrParams = { NULL, CMDP_targetTestVideo, NULL, &testParams, OS_LYRG_SERVICE_VIDEO_THREAD_ID_E };
              thrdHndlrD = OS_LYRG_createThread(&thrParams);
              test = 0;
              break;
         }
         case (INU_host_depthE):
         {
              testParams.iamChannel = NULL;
              testParams.iamChanSelect = NULL;
              testParams.dpeChan = INU_DEFSG_DPE_CH_DEPTH_E;
              testParams.dpeChanSel = INU_DEFSG_DPE_OUT_DISP_E;
              testParams.colorChannel = NULL;
              testParams.colorChanSelect = NULL;

           OS_LYRG_threadParams thrParams = { NULL, CMDP_targetTestDepth, NULL, &testParams, OS_LYRG_SERVICE_DEPTH_THREAD_ID_E };
           thrdHndlrV = OS_LYRG_createThread(&thrParams);

           break;
         }
         case (INU_host_webcamE):
         {
               testParams.iamChannel = NULL;
               testParams.iamChanSelect = NULL;
               testParams.dpeChan = NULL;
               testParams.dpeChanSel = NULL;
               testParams.colorChannel = INU_DEFSG_IAE_CH_WEBCAM_E;
               testParams.colorChanSelect = 0;

               OS_LYRG_threadParams thrParams = { NULL, CMDP_targetTestWebcam, NULL, &testParams, OS_LYRG_SERVICE_DEPTH_THREAD_ID_E };
               thrdHndlrW = OS_LYRG_createThread(&thrParams);

               break;
         }

         case (INU_host_clientE):
               //CMDP_targetTestClient(CMDG_sensorResid, CMDG_sensorFps , configfile); //Client test, configuration- resolution, fps, channel number, channel select, argv address
               break;
         case (INU_host_positionSensorE):
         {
               TestParamsT testParams;

               OS_LYRG_threadParams thrParams = { NULL, CMDP_targetTestPos, NULL, &testParams, OS_LYRG_SERVICE_POS_SENSORS_THREAD_ID_E };
               thrdHndlrP = OS_LYRG_createThread(&thrParams);
               break;
         }
         case (INU_host_injectionE):
               // CMDP_targetTestInjection(CMDG_sensorResid, CMDG_sensorFps, Test_dur, 0); //   Injection test, configuration- resolution, fps, test duration in sec,
               break;
         case (INU_host_audioE):
               // CMDP_targetTestAudio(CMDG_sensorResid, Test_dur); //   Audio test, configuration- test duration in sec,
               break;
         }
      }
      //test = test >> 1;
      //index++;
   }
   OS_LYRG_waitForThread(thrdHndlrW,100000 /*Test_dur * 2000*/);
   OS_LYRG_waitForThread(thrdHndlrD,100000 /*Test_dur * 2000*/);
   OS_LYRG_waitForThread(thrdHndlrV, 100000 /*Test_dur * 2000*/);
   OS_LYRG_waitForThread(thrdHndlrP, 100000 /*Test_dur * 2000*/);

//   if (generalPurpose)
//      CMDP_targetCloseClient(clientStreamStartParams.streamId);
//   CMDP_targetCloseAlg(algStreamStartParams.streamId);

   CMDP_targetClose();
#endif
   return 0;
}

#ifdef TEST_PERF_APP
#include <assert.h>

CLS_COMG_ifOperations ops;
void *h;
USB_CTRL_COMG_paramsT params;
int test_stop;
unsigned long long total_sent = 0;
#define PRINT_FREQUENCY (1)

#if defined(__linux__)
static void ctrlc_cb(int s)
{
   test_stop = 1;
}
#else
static BOOL WINAPI ctrlc_cb(DWORD dwCtrlType)
{
   test_stop = 1;
   return TRUE;
}
#endif

#define TEST_PERF_BUFSZ (1024 * 1024 * 4)
#define TEST_PERF_HDR1SZ (4)
#define TEST_PERF_HDR2SZ (12)
#define TEST_PERF_HDRSZ  (TEST_PERF_HDR1SZ + TEST_PERF_HDR2SZ)

int my_send_func(void *bufr)
{
   unsigned char *buf = (unsigned char *)bufr;
   unsigned char mult = buf[0];
   unsigned int bsent;
   unsigned int print_cnt = 0;
   UINT32 frame_cnt = 0;
   UINT32 frame_cnt2 = (UINT32)~0;

   while(!test_stop)
   {
#if 0
      memcpy(buf,&frame_cnt,4);
      ops.send(h, buf, (1 << mult) * 1024 + TEST_PERF_HDRSZ, &bsent);
      total_sent += bsent;
      frame_cnt++;
#else
      memcpy(buf,&frame_cnt,4);
      memcpy(buf+TEST_PERF_HDR1SZ,&frame_cnt2,4);

      ops.send(h, buf, TEST_PERF_HDRSZ, &bsent);
      total_sent += bsent;
      ops.send(h, buf+TEST_PERF_HDRSZ, (1 << mult) * 1024, &bsent);
      total_sent += bsent;

      frame_cnt++;
      frame_cnt2--;
#endif
      if((++print_cnt % PRINT_FREQUENCY) == 0)
         printf("S");
   }
   return 0;
}

void run_test_perf(void)
{
   static unsigned char buf[TEST_PERF_BUFSZ + TEST_PERF_HDRSZ];
   static unsigned char bufr[TEST_PERF_BUFSZ + TEST_PERF_HDRSZ];
   unsigned long long total = 0;
   unsigned int len;
   unsigned int print_cnt = 0;
   UINT32 frame_cnt = 0;
#if defined(__linux__)
   struct timeval tst,ted;
#else
   UINT32 stsec,edsec;
   UINT16 stmsec,edmsec;
#endif
   int ms;
   unsigned long long mms;
#if defined(__linux__)
   struct sigaction sigact;
#endif
   unsigned char mult = 9;
   OS_LYRG_threadParams tp = {
      NULL,
      my_send_func,
      NULL,
      bufr,
      OS_LYRG_DEFAULT_THREAD_ID_E,
   };
   OS_LYRG_threadHandle th;

#if defined(__linux__)
   sigact.sa_handler = ctrlc_cb;
   sigemptyset(&sigact.sa_mask);
   sigact.sa_flags = 0;
   sigaction(SIGINT, &sigact, NULL);
#else
   SetConsoleCtrlHandler(ctrlc_cb, TRUE);
#endif

   USB_CTRL_COMG_getOps(&ops);
   ops.open(&h, &params);
   ops.send(h, &mult, 1, &len);
   bufr[0] = mult;
   th = OS_LYRG_createThread(&tp);

#if defined(__linux__)
   gettimeofday(&tst, NULL);
#else
   OS_LYRG_getTime(&stsec,&stmsec);
#endif

   while(!test_stop)
   {
      UINT32 cnt = 0;
      ops.recv(h, buf, (1 << mult) * 1024 + TEST_PERF_HDRSZ, &len);
      total += len;

      memcpy(&cnt, buf,4);
      if(cnt != frame_cnt)
      {
         printf("frame cnt error got %u expected %u\n",cnt,frame_cnt);
         frame_cnt = cnt;
      }
      frame_cnt++;

      //printf("received %d bytes - %02x\n", len, buf[0]);
      if((++print_cnt % PRINT_FREQUENCY) == 0)
         printf("R");
   }

   OS_LYRG_closeThread(th);
   ops.close(&h);
#if defined(__linux__)
   gettimeofday(&ted, NULL);
   ms = (ted.tv_sec - tst.tv_sec) * 1000 + (ted.tv_usec - tst.tv_usec) / 1000;
   mms = total * 8 / ms;
   printf("RECEIVED %llu bytes in %d ms - %llu,%03llu Kbps\n", total, ms, mms / 1000, mms % 1000);
   mms = total_sent * 8 / ms;
   printf("SENT %llu bytes in %d ms - %llu,%03llu Kbps\n", total_sent, ms, mms / 1000, mms % 1000);
#else
   OS_LYRG_getTime(&edsec,&edmsec);
   ms = (edsec - stsec) * 1000 + (edmsec - stmsec);
   mms = total * 8 / ms;
   printf("RECEIVED %llu bytes in %d ms - %llu,%03lu Kbps\n", total, ms, mms / 1000, mms % 1000);
   mms = total_sent * 8 / ms;
   printf("SENT %llu bytes in %d ms - %llu,%03lu Kbps\n", total_sent, ms, mms / 1000, mms % 1000);
#endif
}
#endif

void test_usb_perfomance()
{
    ERRG_codeE status=(ERRG_codeE)RET_SUCCESS;
    INU_CMDG_clientStreamStartT *clientStreamStartParams=NULL;
    INU_CMDG_streamReadT INU_CMDG_streamRead;

    status = INUG_init();
    CMDP_ASSERT(status);

    clientStreamStartParams=(INU_CMDG_clientStreamStartT* )malloc(sizeof(INU_CMDG_clientStreamStartT));
    clientStreamStartParams->dataType = INU_CMDG_DATA_TYPE_CLIENT_E;

    start_client(clientStreamStartParams);

   while(1)
   {
      status = INUG_read(CMDP_ClientHandle, &INU_CMDG_streamRead, TIMEOUT_MSEC_E);
      CMDP_ASSERT(status);
      if(ERRG_FAILED(status))
      {
         printf("recieve error\n");
      }
      else
      {
         printf("recieve %d bytes\n", INU_CMDG_streamRead.bufSize);
         status = INUG_ioctl(CMDP_ClientHandle, INU_CMDG_FREE_BUF_E, &INU_CMDG_streamRead.freeBufP);
         CMDP_ASSERT(status);
         if(ERRG_FAILED(status))
         {
            printf("buff free error\n");
         }
      }
   }

    free(clientStreamStartParams);

}


/****************************************************************************
 ***************     G L O B A L         F U N C T I O N S    ***************
 ****************************************************************************/
int main(int argc, char* argv[])
{
   ERRG_codeE status=(ERRG_codeE)RET_SUCCESS;
   testInProgress =0;
   int readflash=-1;
   printf("main: start host app...\n");
   if (argc <= 1)
   {
        printf ("Usage: inu_host.exe \n"
         "\t -R resolution \n"
         "\t -F FPS \n"
         "\t -B Boot directory location \n"
         "\t -f config.ireg location \n"
         "\t -D Test duration \n"
         "\t -T Which kind of test to run \n"
         "\t -C Channel \n"
         "\t -G General purpose \n"
         "\t -X Load XML \n"
         "\t -S Channel Select \n"
         "\t -E Generator mode \n"
         "\t example: -R H -F 20 -B ../boot -f ../configfile -G 1 -D 10 -T D \n"
         );
       return 1;
   }

    //Target configuration
   while ((first_word= getopt(argc, argv, ("R:F:B:f:D:T:X:G:S:C:E"))) != EOF)
   {
        switch (first_word)
      {
            case ('T'):
            requiredStreamsCnt++;
                switch (*optarg)
                {
                    case ('V'):
                  test |= 1 << (int)INU_host_videoE;
                        break;
                    case ('D'):
                  test |= 1 << (int)INU_host_depthE;
                        break;
                    case ('W'):
                  test |= 1 << (int)INU_host_webcamE;
                        break;
                    case ('C'):
                        test|=INU_host_clientE;
                        break;
                    case ('P'):
                        test|= 1 << (int)INU_host_positionSensorE;
                        break;
                    case ('I'):
                        test|=INU_host_injectionE;
                        break;
               case ('A'):
                  test|=INU_host_audioE;
                  break;
                }
                break;
            case ('R'):
                switch (*optarg)
                {
                    case ('H'):
                        CMDG_sensorResid=INU_DEFSG_RES_HD_E;
                        break;
                    case ('V'):
                        CMDG_sensorResid=INU_DEFSG_RES_VGA_E;
                        break;
                    case '?':
                        printf ("Unknown option character \n");
                        break;
                }
                break;
            case ('F'):
                CMDG_sensorFps=atoi(optarg);
                break;
            case ('G'):
                generalPurpose=atoi(optarg);
                break;
            case ('f'):
                configfolder=optarg;
                strcpy(configfile, configfolder);
                strcat(configfile, SLESH"config.ireg");
                break;
            case ('X'):
                XMLfile=optarg;
                loadXML=1;
                break;
            case ('B'):
                Bootfolder=optarg;
                break;
            case ('C'):
                Channel=atoi(optarg);
                break;
            case ('D'):
                Test_dur=atoi(optarg);
                break;
            case ('S'):
                ChanSelect=atoi(optarg);
                break;
         case ('E'):
                CMDG_isGeneratorMode=1;
                break;
            case '?':
                printf ("Unknown option character \n");
                break;
        }
    }

   if (Bootfolder != NULL)
   {
       status=INUG_boot_init(hotplug_callback_func, Bootfolder, readflash,0,0,100000);
       if(status != BOOT__RET_SUCCESS) {
         printf("INUG_boot_init: failed (err=%d)\n", status);
         return status;
      }

      printf("INUG_boot_init: success...\n");
      printf("configfolder= %s\n",configfolder);
      printf("Press 'CTL^C' to exit...\n");
      while(!testInProgress)
         millisleep_host(10000);
   }

   //test_usb_perfomance();
#ifdef TEST_PERF_APP
   run_test_perf();
#else
   test_invoke_func();
#endif
   if (testInProgress)
      INUG_boot_deinit();
    printf("Exiting test app ...\n");
    return 0;
}
#endif

#ifdef __cplusplus
}
#endif

// Code for INU SDK
#else

/// EXTERNAL FUNCTIONALITY (without inu.h, inu_defs.h and so on)

//#ifndef LINUX_MAKE_BUILD
//    void LOGG_outputStrLog(INU_DEFSG_logLevelE level, ERRG_codeE errCode, const char *fileP, const char *funcP, UINT32 lineNum, const char * fmt, ...)
    //{
    //}
//#endif

#include "inu_defs/inu_types.h"
#include "inu_defs/err_defs.h"
#include "inu.h"
#include "os_lyr.h"


#include <stdint.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#if (defined _WIN32) || (defined _WIN64)
  #include "getopt.h"
#elif defined(__linux__)
  #include <unistd.h>
  #include <getopt.h>
#endif


#ifdef __cplusplus
extern "C" {
#endif

#include "inu_sdk.h"

//add log holder functionality for OS layer
#ifndef LINUX_MAKE_BUILD
    void LOGG_outputStrLog(INU_DEFSG_logLevelE level, ERRG_codeE errCode, const char *fileP, const char *funcP, UINT32 lineNum, const char * fmt, ...)
    {
    }
#endif

int FWP_writeToFile(unsigned char *dataP, unsigned int len, int frame_cnt, const char *frame_type)
{
   FILE *fileP;
   char filename[100];
   size_t ret = 0;
    sprintf(filename,"%s%04d.raw", frame_type, frame_cnt);
    fileP = fopen(filename, "wb");
    if (fileP)
    {
      ret = fwrite(dataP, len, 1, fileP);
        if(ret != 1) {
            printf("inu_host_test: failed to write frame to file %s\n",filename);
         ret = 1;
      }
        fflush(fileP);
        fclose(fileP);
    }
   return 0;
}

int FWP_videoDataCb(unsigned char *dataP, unsigned int len, void *messageParamsP, void *cbParams)
{
   FWG_messageExtendedMetadataT *msgMetadata = (FWG_messageExtendedMetadataT *)FWG_getStreamMetadata(cbParams, FWG_VIDEO_CHAN0_STREAM_E, messageParamsP);
   printf("received video frame size=%d, mode=%d, cnt=%d, res=%dx%d\n",len, msgMetadata->mode, msgMetadata->frameCntr, msgMetadata->width, msgMetadata->height);
   //FWP_writeToFile(dataP, len, msgMetadata->frameCntr, "video");
   return 0;
}

int FWP_depthDataCb(unsigned char *dataP, unsigned int len, void *messageParamsP, void *cbParams)
{
   FWG_messageExtendedMetadataT *msgMetadata = (FWG_messageExtendedMetadataT *)FWG_getStreamMetadata(cbParams, FWG_DEPTH_CHAN0_STREAM_E, messageParamsP);
   printf("received depth frame size=%d, mode=%d, cnt=%d, res=%dx%d\n",len, msgMetadata->mode, msgMetadata->frameCntr, msgMetadata->width, msgMetadata->height);
   //FWP_writeToFile(dataP, len, msgMetadata->frameCntr, "depth");
   return 0;
}

void print_usage() {
    printf("Usage: rectangle [ap] -l num -b num\n");
}


FWG_initParamsT fwInitParams;
FWG_sensorInfoT sensorInfo;
FWG_streamChannelInfoT streamChannelInfo;

int main(int argc, char* argv[])
{
   int ret;
   void *fwHandle;
   static int interleave_flag=0, full_flag=0, binning_flag=0, alternate_flag=0, color_hd_flag=0, color_vga_flag=0;
   static int video0_flag=0, video1_flag=0, depth_flag=0, webcam_flag=0, client_flag=0, position_flag=0, alg_flag=0, inject_flag;
   char *bootFolder;
   char *configFolder;
   char *snapshotFolder;
   char *injectFile;
   int fps=0, color_fps=0, time=0;
   int opt, long_index=0;

   static struct option long_options[] =
    {
          /* These options set a flag. */
          {"interleave", no_argument, &interleave_flag, 1},
        {"full", no_argument, &full_flag, 1},
        {"binning", no_argument, &binning_flag, 1},
        {"alternate", no_argument, &alternate_flag, 1},
        {"color_hd", no_argument, &color_hd_flag, 1},
        {"color_vga", no_argument, &color_vga_flag, 1},
        {"video0", no_argument, &video0_flag, 1},
        {"video1", no_argument, &video1_flag, 1},
        {"depth", no_argument, &depth_flag, 1},
        {"webcam", no_argument, &webcam_flag, 1},
        {"client", no_argument, &client_flag, 1},
        {"position", no_argument, &position_flag, 1},
        {"alg", no_argument, &alg_flag, 1},
        {"inject", no_argument, &inject_flag, 1},
          {"boot",   required_argument, 0, 'b'},
        {"config",   required_argument, 0, 'c'},
        {"snapshot",   required_argument, 0, 's'},
        {"inject_file",   required_argument, 0, 'i'},
        {"fps", required_argument, 0, 'f'},
        {"color_fps", required_argument, 0, 'p'},
        {"time", required_argument, 0, 't'},
          {0, 0, 0, 0}
    };

   // default  paarameters setup
   fps = 30;
   binning_flag = 1;
   interleave_flag = 1;
   color_fps = 30;
   color_vga_flag =1;
   time = 20;
   video0_flag = 1;
   depth_flag = 1;
   bootFolder = "C:\\Program Files\\Inuitive\\InuDev\\bin\\boot0";
   configFolder = "C:\\Program Files\\Inuitive\\InuDev\\config";

    while ((opt = getopt_long(argc, argv,"b:c:s:i:f:p:t:",
                   long_options, &long_index )) != -1) {
        switch (opt) {
             case 'f' : fps = atoi(optarg);
                 break;
          case 'p' : color_fps = atoi(optarg);
                 break;
          case 't' : time = atoi(optarg);
                 break;
             case 'b' : bootFolder = optarg;
                 break;
          case 'c' : configFolder = optarg;
             break;
          case 's' : snapshotFolder = optarg;
             break;
          case 'i' : injectFile = optarg;
                 break;
//             default: print_usage();
//                 exit(EXIT_FAILURE);
        }
    }

   memset(&fwInitParams,0,sizeof(FWG_initParamsT));
   memset(&sensorInfo,0,sizeof(FWG_sensorInfoT));
   memset(&streamChannelInfo,0,sizeof(FWG_streamChannelInfoT));


   fwInitParams.bootFolder = bootFolder;
   fwInitParams.configFolder = configFolder;

   // open media transport monitor logic
   ret = FWG_init(&fwHandle, &fwInitParams);
   if (ret) {
      printf("FWG_init failed(ret=%d)\n", ret);
      return 1;
   }
   // connect and prepare target for work
   ret = FWG_open(fwHandle);
   if (ret) {
      printf("FWG_init failed(ret=%d)\n", ret);
      return 1;
   }

   // config stereo sensor
   sensorInfo.sensorId = FWG_SENSOR_SELECT_STEREO;
   sensorInfo.senorResolution = FWG_SENSOR_RES_BINNNG;
   if (binning_flag)
      sensorInfo.senorResolution = FWG_SENSOR_RES_BINNNG;
   else if (full_flag)
      sensorInfo.senorResolution = FWG_SENSOR_RES_FULL;

   sensorInfo.senorFps = fps;
   sensorInfo.senorOutFormat = FWG_SENSOR_BAYER_RAW_10_E;
   ret = FWG_configSensor(fwHandle, &sensorInfo);
   if (ret) {
      printf("FWG_configSensor failed, sensorId =%d(ret=%d)\n", sensorInfo.sensorId, ret);
      return 1;
   }

   //  start video channel + video streaming
   if (video0_flag)
   {
      if (interleave_flag)
         streamChannelInfo.streamSourceId = FWG_STREAM_SOURCE_VIDEO_INTERLEAVE_E;
      else
         streamChannelInfo.streamSourceId = FWG_STREAM_SOURCE_VIDEO_LEFT_E;
      streamChannelInfo.streamOutWidth = 0;
      streamChannelInfo.streamOutHeight = 0;
      ret = FWG_startStreamChannel(fwHandle, FWG_VIDEO_CHAN0_STREAM_E, &streamChannelInfo);
      if (ret) {
         printf("can't start video channel, (ret=%d)\n", ret);
         return 1;
      }
   }

   //  start depth channel + depth streaming
   if (depth_flag)
   {
      streamChannelInfo.streamSourceId = FWG_STREAM_SOURCE_DEPTH_DISP_E;

      ret = FWG_startStreamChannel(fwHandle, FWG_DEPTH_CHAN0_STREAM_E, &streamChannelInfo);
      if (ret) {
         printf("can't start depth channel, (ret=%d)\n", ret);
         return 1;
      }
   }
   // start video streaming
   if (video0_flag)
   {
      ret = FWG_startStream(fwHandle, FWG_VIDEO_CHAN0_STREAM_E, FWP_videoDataCb);
   }

   // stop video streaming
   if (depth_flag)
   {
      ret = FWG_startStream(fwHandle, FWG_DEPTH_CHAN0_STREAM_E, FWP_depthDataCb);
   }
   // main loop processing
   FWG_utilSleepMs(time * 1000);

   // close video
   if (video0_flag)
   {
      ret = FWG_stopStream(fwHandle, FWG_VIDEO_CHAN0_STREAM_E);
   }

   // close depth
   if (depth_flag)
   {
      ret = FWG_stopStream(fwHandle, FWG_DEPTH_CHAN0_STREAM_E);
   }

   return 0;
}

#ifdef __cplusplus
}
#endif

#endif
#else
int main()
{
   int i = 5;
   return i;
}

#endif