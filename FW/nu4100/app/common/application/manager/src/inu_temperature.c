#include "inu2_types.h"
#include "inu2_internal.h"
#include "inu_types.h"
#include "err_defs.h"
#include "internal_cmd.h"
#include "mem_pool.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "inu_temperature.h"
#include "inu_imu.h"
#include "nucfg.h"

#if DEFSG_IS_GP
#if DEFSG_IS_OS_LINUX
   #include "os_lyr.h"
   #include <unistd.h>
   #include <inttypes.h>
   #include <poll.h>
   #include "cmem.h"
   #include "inu_common.h"
   #include "pvt_drv.h"
   #include "i2c_hl_drv.h"
   #include "inu_charger.h"
   #include "gme_mngr.h"
   #include "i2c_drv.h"
#endif
#endif

#ifdef __KERNEL_GPIO__
#include <errno.h>
#include <unistd.h>
#endif

#define I2C_TEMP_MUX_ADDR (0x48)

typedef struct inu_temperature__privData
{
   BOOLEAN                    active;
   BOOLEAN                    thrActive;
   inu_temperature_data       *temp_data;
   MEM_POOLG_handleT          stubPoolH;
   OS_LYRG_timerT             inu_temperature_Timer;
   int                        tempFPS;
#if DEFSG_IS_GP
   UINT64                     ctrIndex;
   int                        fdHwmon0;
   int                        fdHwmon1;
#endif
}inu_temperature__privData;

static inu_function__VTable _vtable;
static bool _bool_vtable_initialized = 0;
static const char* name = "INU_TEMPERATURE";


#if DEFSG_IS_GP
int is_valid_fd(int fd)
{
   return fcntl(fd, F_GETFL) != -1 || errno != EBADF;
}

/****************************************************************************
*
*  Function Name: cal_temp
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*    Success or specific error code.
*
*  Context:
*
****************************************************************************/
static float temp_tbl[40] = {
    7, 10, 14, 19, 26, 35, 47, 61, 79, 102, 128, 159, 196, 236,
    281, 330, 381, 433, 486, 538, 588, 636, 680, 721, 758, 791, 820,
    846, 869, 888, 905, 920, 933, 945, 955, 963, 970, 977, 983, 4096};

static int inu_temperature__calc_temp(uint16_t val)
{
   int i = 0;
   while (val > temp_tbl[i])
      i++;

   float temp_start = -40.0 + 5.0 * (i - 1);
   float temp_end = temp_start + 5.0;

   float k = 5.0 / (temp_tbl[i] - temp_tbl[i - 1]);
   float temp = (temp_start + k * (val - temp_tbl[i - 1])) * 1000.0;

   return (int)temp;
}

/****************************************************************************
*
*  Function Name: CHARGERP_readNtc
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*    Success or specific error code.
*
*  Context:
*
****************************************************************************/
ERRG_codeE inu_temperature__readNtc(int *temp)
{
   UINT8 reg31, reg30;
   ERRG_codeE ret = (ERRG_codeE)RET_SUCCESS;
   //CHARGERP_infoT *paramsP = &CHARGERP_params;

   ret = CHARGERG_readReg(0x31, &reg31);
   ret = CHARGERG_readReg(0x30, &reg30);

   *temp = inu_temperature__calc_temp((reg31 << 8) + reg30);

   LOGG_PRINT(LOG_DEBUG_E, NULL, "reg31= %x, reg30 = %x, temp=0x%x\n", reg31, reg30, *temp);

   return ret;
}

/****************************************************************************
*
*  Function Name: inu_temperature__selectTempSensor
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*    Success or specific error code.
*
*  Context:
*
****************************************************************************/
ERRG_codeE inu_temperature__selectTempSensor(unsigned int sensorNum)
{
      unsigned char data=0xff;

      I2C_DRVG_direct_write( 0,  1,  sensorNum, I2C_TEMP_MUX_ADDR, 1000);
      I2C_DRVG_direct_read( 0, 1, &data,  I2C_TEMP_MUX_ADDR,  1000);
      LOGG_PRINT(LOG_INFO_E, NULL, "Set sensor %d for temperature test\n", data &0xf);
}

/****************************************************************************
*
*  Function Name: inu_temperature__gpTimerInt
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: interrupt controller service (gp)
*
****************************************************************************/
void inu_temperature__gpTimerInt(void *argP)
{
   ERRG_codeE ret;
   inu_temperature_data__hdr_t   tempHdr;
   inu_imu__temperatureParams    imuTempParams;
   inu_temperature_data          *temp_data;
   inu_temperature               *me      = (inu_temperature*)argP;
   inu_temperature__privData     *privP   = (inu_temperature__privData*)me->privP;
   MEM_POOLG_bufDescT            *bufP;
   INU_DEFSG_moduleTypeE modelType = GME_MNGRG_getModelType();
   UINT64 myTime;

   memset(&tempHdr,0,sizeof(inu_temperature_data__hdr_t));

   //read imu temp, sensor temp, pvt details
   ret               = inu_imu__gpGetTemperatoreIoctl(NULL,&imuTempParams);
   tempHdr.imuTemp   = imuTempParams.temperature;

   // M4.5S reads Temperature vis ATTiny
   if ((modelType == INU_DEFSG_BOOT310_E) || (modelType == INU_DEFSG_BOOT330_E) || (modelType == INU_DEFSG_BOOT340_E) || (modelType == INU_DEFSG_BOOT311_E))
   {
      int temp = 0;
      inu_temperature__readNtc(&temp);

      tempHdr.sensorTempCvr = temp;
      tempHdr.sensorTempCvl = temp;
   }
   else
   {
      char buf[6];
      if (is_valid_fd(privP->fdHwmon0))
      {
         memset(buf,0,6);
         pread(privP->fdHwmon0, buf, 6, 0);
         tempHdr.sensorTempCvr= atoi(buf);
      }

      if (is_valid_fd(privP->fdHwmon1))
      {
         memset(buf,0,6);
         pread(privP->fdHwmon1, buf, 6, 0);
         tempHdr.sensorTempCvl= atoi(buf);
      }
   }

   ret   = PVT_DRVG_readTemperatureData(&tempHdr.chipTemp);
   ret   = PVT_DRVG_readVoltageData(&tempHdr.chipVoltage);
   ret   = PVT_DRVG_readProcessData(tempHdr.chipProcess);

   //fill timestamp and frame index
   OS_LYRG_getTimeNsec(&myTime);
   tempHdr.dataHdr.timestamp = myTime;
   tempHdr.dataHdr.dataIndex = privP->ctrIndex++;

   MEM_POOLG_alloc(privP->stubPoolH,4,&bufP);
   bufP->dataLen = 4;
   //create data
   ret = inu_function__newData((inu_function*)me,(inu_data*)privP->temp_data, bufP, &tempHdr, (inu_data**)&temp_data);
   if (ERRG_SUCCEEDED(ret))
   {
      inu_function__doneData((inu_function*)me,(inu_data*)temp_data);
   }
}
#endif

static const char* inu_temperature__name(inu_ref *me)
{
   FIX_UNUSED_PARAM_WARN(me);
   return name;
}

static void inu_temperature__dtor(inu_ref *me)
{
   inu_temperature__privData         *privP   = (inu_temperature__privData*)((inu_temperature*)me)->privP;
   inu_function__vtable_get()->node_vtable.ref_vtable.p_dtor(me);
#if DEFSG_IS_GP
   //deinit timer  to fps, and init pvt
   PVT_DRVG_deinit();

   //deinit timer  to fps, and deinit pvt
   MEM_POOLG_closePool(privP->stubPoolH);

   close(privP->fdHwmon0);
   close(privP->fdHwmon1);
#endif
   free(privP);
}

/* Constructor */
ERRG_codeE inu_temperature__ctor(inu_temperature *me, inu_temperature__CtorParams *ctorParamsP)
{
   ERRG_codeE ret;
   inu_temperature__privData *privP;

   ret = inu_function__ctor(&me->function, &ctorParamsP->functionCtorParams);
   if (ERRG_SUCCEEDED(ret))
   {
      privP = (inu_temperature__privData*)malloc(sizeof(inu_temperature__privData));
      if(privP)
      {
         memset(privP, 0, sizeof(inu_temperature__privData));
         me->privP = privP;
#if DEFSG_IS_GP
         MEM_POOLG_cfgT cfg;
         cfg.bufferSize          = 4; //stub buffer
         cfg.numBuffers          = 8;
         cfg.resetBufPtrInAlloc  = 0;
         cfg.freeCb              = NULL;
         cfg.freeArg             = NULL;
         cfg.type                = MEM_POOLG_TYPE_ALLOC_CMEM_E;
         cfg.memP                = NULL;
         MEM_POOLG_initPool(&privP->stubPoolH, &cfg);

         //init timer  to fps, and init pvt
         if (ERRG_FAILED(PVT_DRVG_init()))
            return INU_TEMPERATURE__ERR_UNEXPECTED;

         privP->tempFPS=ctorParamsP->fps;

         privP->fdHwmon0 = open("/sys/class/hwmon/hwmon0/temp1_input", O_RDONLY);
         if(privP->fdHwmon0 >= 0)
         {
            LOGG_PRINT(LOG_DEBUG_E, NULL, "hwmon0 open succeeded\n");
         }

         privP->fdHwmon1 = open("/sys/class/hwmon/hwmon1/temp1_input", O_RDONLY);
         if(privP->fdHwmon1 >= 0)
         {
            LOGG_PRINT(LOG_DEBUG_E, NULL, "hwmon1 open succeeded\n");
         }

#endif
      }
      else
      {
         ret = INU_TEMPERATURE__ERR_OUT_OF_MEM;
      }
   }
   return ret;
}

static void inu_temperature__newOutput(inu_node *me, inu_node *output)
{
   inu_temperature *temp = (inu_temperature*)me;
   inu_temperature__privData *privP = (inu_temperature__privData*)temp->privP;

   inu_function__vtable_get()->node_vtable.p_newOutput(me, output);
#if DEFSG_IS_GP
   //temp creates data, allocate internal data pool for dispatching data
   inu_data__allocInternalDuplicatePool((inu_data*)output, 8); //TODO: set properly from calculating "path len time / time per frame"
#endif
   //save data to ease access
   privP->temp_data = (inu_temperature_data*)output;
}

static ERRG_codeE inu_temperature__start(inu_function *me, inu_function__startParamsT *startParamP)
{
   ERRG_codeE ret;
#if DEFSG_IS_GP
   inu_temperature *temp = (inu_temperature*)me;
   inu_temperature__privData *privP = (inu_temperature__privData*)temp->privP;
   privP->ctrIndex = 0;
#endif
   ret = inu_function__vtable_get()->p_start(me, startParamP);
#if DEFSG_IS_GP
   //trigger timer
   //interrupt registeration
   UINT32      sec,nsec;
   if (privP->tempFPS != 0)
   {
      //create the temperature timer
      privP->inu_temperature_Timer.funcCb = inu_temperature__gpTimerInt;
      privP->inu_temperature_Timer.argP   = (void*)me;
      ret = OS_LYRG_createTimer(&privP->inu_temperature_Timer);
      if(ERRG_FAILED(ret))
      {
         LOGG_PRINT(LOG_ERROR_E, NULL, "failed to create trigger timer\n");
      }
      else
      {
         LOGG_PRINT(LOG_DEBUG_E, NULL, "create trigger timer.\n");
      }

      if (privP->tempFPS== 1)
      {
         sec   = 1;
         nsec  = 0;
      }
      else
      {
         sec   = 0;
         nsec  = 1000000000 / privP->tempFPS;
      }

      //start the timer
      privP->inu_temperature_Timer.firstExpiryNsec       = 500000000;//half a sec
      privP->inu_temperature_Timer.firstExpirySec        = 0;
      privP->inu_temperature_Timer.intervalExpiryNsec    = nsec;
      privP->inu_temperature_Timer.intervalExpirySec     = sec;
      ret = OS_LYRG_setTimer(&privP->inu_temperature_Timer);
   }
#endif
   return ret;
}

static ERRG_codeE inu_temperature__stop(inu_function *me, inu_function__stopParamsT *stopParamP)
{
   ERRG_codeE ret;
#if DEFSG_IS_GP
   inu_temperature *temp = (inu_temperature*)me;
   inu_temperature__privData *privP = (inu_temperature__privData*)temp->privP;
#endif
   ret = inu_function__vtable_get()->p_stop(me, stopParamP);
#if DEFSG_IS_GP
   //stop timer
   OS_LYRG_deleteTimer(&privP->inu_temperature_Timer);
#endif
   return ret;
}


void inu_temperature__vtable_init(void)
{
   if (!_bool_vtable_initialized) {
      inu_function__vtableInitDefaults(&_vtable);
      _vtable.node_vtable.ref_vtable.p_name = inu_temperature__name;
      _vtable.node_vtable.ref_vtable.p_dtor = inu_temperature__dtor;
      _vtable.node_vtable.ref_vtable.p_ctor = (inu_ref__Ctor*)inu_temperature__ctor;

      _vtable.node_vtable.p_newOutput = inu_temperature__newOutput;

      _vtable.p_start = inu_temperature__start;
      _vtable.p_stop  = inu_temperature__stop;

      _bool_vtable_initialized = true;
   }
}

const inu_function__VTable *inu_temperature__vtable_get(void)
{
   inu_temperature__vtable_init();
   return (const inu_function__VTable*)&_vtable;
}
