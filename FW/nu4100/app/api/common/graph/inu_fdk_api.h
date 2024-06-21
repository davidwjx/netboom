#ifndef _INU_FDK_API_H_
#define _INU_FDK_API_H_

#include "inu2.h"

/**** Common API definitions ****/
/* the total number of FDK nodes allowed in one device */
#define INU_FDK__MAX_NODES_NUMBER (10)

typedef void* inu_fdkH;


/**** Target API definitions ****/

#define INU_FDK__AUTO_DONE_DATA_FLAG    (1<<0)
#define INU_FDK__AUTO_CACHE_INV_FLAG    (1<<1)
#define INU_FDK__AUTO_OPERATE_DONE_FLAG (1<<2)
#define INU_FDK__AUTO_CACHE_WB_FLAG     (1<<3)

/* max size for sending general params between GP to DSP cores */
#define INU_FDK__GENERAL_MSG_MAX_SIZE   (256)


typedef struct
{
   void                 *msgP;
   UINT32               msgSize;
} inu_fdk__dspMsgParamsT;

/* The callback is called when the node is created */
typedef void (*inu_fdk__ctor_func_t)           (inu_fdkH meH);
/* The callback is called when the node is removed */
typedef void (*inu_fdk__dtor_func_t)           (inu_fdkH meH);
/* The callback is called when the node is state is changed to start */
typedef void (*inu_fdk__start_func_t)      (inu_fdkH meH, inu_function__startParamsT *startParamsP);
/* The callback is called when the node is state is changed to stop */
typedef void (*inu_fdk__stop_func_t)      (inu_fdkH meH, inu_function__stopParamsT *stopParamsP);
/* The callback is called when all the node data inputs are valid. */
typedef void (*inu_fdk__operate_func_t)       (inu_fdkH meH, inu_function__operateParamsT *paramsP);
/* The callback is executed when data is received for this node */
typedef void (*inu_fdk__rxData_func_t)        (inu_fdkH meH, unsigned char *bufP, unsigned int len);
/* The callback is executed when transmition of data has completed */
typedef void (*inu_fdk__txDataDone_func_t)   (inu_fdkH meH, unsigned char *bufP);
/* The callback is executed when DSP sends a message to this node */
typedef void (*inu_fdk__dspMsg_func_t)        (inu_fdkH meH, inu_fdk__dspMsgParamsT *msgP, inu_function__operateParamsT *paramsP, inu_function__coreE dspSource);
/* The callback is executed when user data is no longer in use */
typedef void (*inu_fdk__userDataFree_func_t) (inu_fdkH meH, void *bufP);


typedef struct
{
   inu_fdk__ctor_func_t           ctorCb;
   inu_fdk__dtor_func_t           dtorCb;
   inu_fdk__start_func_t          startCb;
   inu_fdk__stop_func_t           stopCb;
   inu_fdk__operate_func_t        operateCb;
   inu_fdk__rxData_func_t         rxDataCb;
   inu_fdk__txDataDone_func_t     txDataDoneCb;
   inu_fdk__dspMsg_func_t         dspMsgCb;
   inu_fdk__userDataFree_func_t   userDataFreeCb;
   int                            nodeId;
   UINT32                         flags;
}inu_fdk__initNodeParamsT;

/* 
   Callbacks provided before the node is created. 
   When the node is created, the functions will be binded to it 
   The nodeId is used to identify to which node the functions 
   will be binded 
*/
ERRG_codeE inu_fdk__initNodeCbFunctions( inu_fdk__initNodeParamsT cbsRegister);

typedef void (*inu_fdk__cevaEventCbT)  (inu_fdk__dspMsgParamsT *dspMsgParamsP);

typedef struct
{
   inu_fdk__cevaEventCbT       cevaEventCb;
} inu_fdk__cevaCbsT;

typedef struct
{
   inu_fdk__cevaCbsT    cevaMsgCbsFunc;
} inu_fdk__gpCallBackFunctionsT;


/* 
   Init the target library
*/
   ERRG_codeE inu_fdk__gpInit(    int       wdTimeout,
                                  UINT32    clientDdrBufSize,
                                  UINT32    *clientDdrBufPhyAddrP,
                                  UINT32    *clientDdrBufVirtAddrP,
                                  inu_device_standalone_mode_e standAloneMode,
                                  inu_device_interface_0_e ctrlInterface,
                                  inu_deviceH *devicePtr);

/* 
   Deinit the target library
*/
ERRG_codeE inu_fdk__deinit(inu_deviceH device);


typedef struct
{
   inu_device__hwVersionIdU  hwVerId;
   inu_device__fwVersionIdU  fwVerId;
} inu_fdk__getVersionT;

ERRG_codeE inu_fdk__getVersion(inu_fdk__getVersionT *versionP);
ERRG_codeE inu_fdk__gpCacheWr(void *ptr, UINT32 size);
ERRG_codeE inu_fdk__gpCacheInv(void *ptr, UINT32 size);
ERRG_codeE inu_fdk__convertPhysicalToVirtual(UINT32 physicalAddr, UINT32 *oVirtualAddrP);
ERRG_codeE inu_fdk__convertVirtualToPhysical(UINT32 virtualAddr, UINT32 *oPhysicalAddrP);
ERRG_codeE inu_fdk__getPhysicalAddr(inu_dataH dataH, UINT32 *oPhysicalAddrP);
ERRG_codeE inu_fdk__gpToDspMsgSend(inu_fdkH fdkH, inu_fdk__dspMsgParamsT *msgParamsP, inu_function__operateParamsT *paramsP, inu_function__coreE coreTarget);
ERRG_codeE inu_fdk__gpToDspMsgSyncSend(inu_fdkH fdkH, inu_fdk__dspMsgParamsT *msgParamsP, inu_function__coreE coreTarget);

void inu_fdk__gpSendLog(INU_DEFSG_logLevelE level, ERRG_codeE errCode, const char *fileP, const char *funcP, UINT32 lineNum, const char * fmt, ...);

#define inu_fdk__gp_log(level, errCode, ...) inu_fdk__gpSendLog(level, (ERRG_codeE)errCode, __FILE__, __func__, __LINE__, __VA_ARGS__)
ERRG_codeE inu_fdk__complete(inu_fdkH meH);
ERRG_codeE inu_fdk__doneData(inu_fdkH meH, inu_dataH dataH);
ERRG_codeE inu_fdk__newUserData(inu_fdkH meH, inu_dataH dataH, UINT32 len, inu_dataH *cloneH, inu_data__hdr_t *hdrP);


//connection IOCTL commands
typedef enum
{
   /* 
        Config:
        Config is called on group, and then per each sensor.
        The config is called before MIPI configuration of the nu4000, and requires the sensor to
        enable the MIPI for stopstate
        argP @ NULL
     */
   FDK_SENSOR_DRVG_CONFIG_SENSOR_IOCTL_E           ,
   /* 
        Start:
        Start is called on group only. This should start the streaming of frames from the sensor
        to the NU4000 on normal mode. On trigger mode (low power) it should do the last preparations
        before the trigger funcion will be called.
        argP @ NULL
     */  
   FDK_SENSOR_DRVG_START_SENSOR_IOCTL_E            ,
   /* 
        Stop:
        Stop is called on group only. This should stop the streaming of frames from the sensor
        to the NU4000 on normal mode. 
        argP @ NULL
     */  
   FDK_SENSOR_DRVG_STOP_SENSOR_IOCTL_E             ,
   /* 
        SetFrameRate:
        The function is called when the sensor is required to change the FPS configuration.
        argP @ inu_fdk__setFrameRateParamsT
     */  
   FDK_SENSOR_DRVG_SET_FRAME_RATE_IOCTL_E          ,
   /* 
        SetExposureTime:
        The function is called when the sensor is required to change the exposure configuration.
        argP @ inu_fdk__exposureTimeCfgT
     */  
   FDK_SENSOR_DRVG_SET_EXPOSURE_TIME_IOCTL_E       ,
   /* 
        GetExposureTime:
        The function is called when the sensor is required to return the current exposure configuration.
        argP @ inu_fdk__exposureTimeCfgT
     */  
   FDK_SENSOR_DRVG_GET_EXPOSURE_TIME_IOCTL_E       ,
   /* 
        SetGain:
        The function is called when the sensor is required to change the gain configuration.
        argP @ inu_fdk__gainCfgT
     */  
   FDK_SENSOR_DRVG_SET_GAIN_IOCTL_E                ,
   /*
        GetGain:
        The function is called when the sensor is required to return the gain configuration.
        argP @ inu_fdk__gainCfgT
     */
   FDK_SENSOR_DRVG_GET_GAIN_IOCTL_E                ,
   /*
        TriggerSensor:
        The function is called in low power mode (trigger). It will be called at the required FPS,
        and the implementation should perform the operation which requires in order for the 
        sensor to produce a frame
        argP @ NULL
     */
   FDK_SENSOR_DRVG_TRIGGER_SENSOR_IOCTL_E          ,
   /*
        Orientation:
        The function is called in to modify the sensor orientation (mirror/flip or both).
        argP @ inu_fdk__orientationParamsT
     */
   FDK_SENSOR_DRVG_ORIENTATION_IOCTL_E             ,
   /*
        PowerUp:
        The function is called to power up the sensor. Its called before any other operation,
        and may be called multiple of times to allow the I2C address change procedure
        argP @ NULL
     */
   FDK_SENSOR_DRVG_POWER_UP_SENSOR_IOCTL_E         ,
   /*
        PowerDown:
        The function is called to power down the sensor. Its called before any other operation,
        and may be called multiple of times to allow the I2C address change procedure
        argP @ NULL
     */
   FDK_SENSOR_DRVG_POWER_DOWN_SENSOR_IOCTL_E       ,
   /*
        ChangeAddress:
        The function is called to modify the sensor I2C address. Its required to allow
        controlling cases of multiple sensors in one group (like stereo)
        argP @ UINT8
     */
   FDK_SENSOR_DRVG_CHANGE_ADDRES_IOCTL_E           ,
   FDK_SENSOR_DRVG_NUM_OF_IOCTLS_E
} FDK_SENSOR_DRVG_ioctlListE;


/*
     inu_fdk__sensorIoctlFuncListT:
     One generic function definition for all types of ioctls. The function will pass the groupId and sensorId on which to run
     the ioctl. For cases the ioctl should be run for the entire group, the sensorId will be set to 0xFFFF.
     The argP param should be casted according to the ioctl
*/
typedef ERRG_codeE (*inu_fdk__sensorIoctlFuncListT)(UINT16 groupId, UINT16 sensorId, void *argP);

/*
     inu_fdk__setSensorIoctlFunctionTbl:
     Setup the ioctl functions. Must be called before inu_fdk__gpInit
  */
ERRG_codeE inu_fdk__setSensorIoctlFunctionTbl( FDK_SENSOR_DRVG_ioctlListE ioctlNum, inu_fdk__sensorIoctlFuncListT ioctl );


typedef struct
{
   UINT32 frameRate;
} inu_fdk__setFrameRateParamsT;

typedef struct
{
   UINT32 exposureTime; //[us]
} inu_fdk__exposureTimeCfgT;

typedef struct
{
   UINT32 orientation;
} inu_fdk__orientationParamsT;

typedef struct
{
   UINT32 analogGain; //Q^4
   UINT32 digitalGain; //Q^4
} inu_fdk__gainCfgT;



/**** Host API definitions ****/
ERRG_codeE inu_fdk__send_data_async(inu_fdkH meH, char *bufP, unsigned int len);

#endif
