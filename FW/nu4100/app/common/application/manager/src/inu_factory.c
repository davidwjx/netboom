#include "inu_factory.h"
#include "internal_cmd.h"
#include "inu2_internal.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "log.h"

#include "conn_lyr.h"

#include "usb_ctrl_com.h"
#include "uart_com.h"
#include "stub_com.h"
#include "tcp_com.h"
#include "udp_com.h"
#include "cdc_com.h"
#include "uvc_com.h"
#include "assert.h"
#include "helsinki.h"
#if DEFSG_IS_GP
#include "gme_mngr.h"
#endif
unsigned int UVC_VIDEO_findStreamNum(char *name);
void uvc_com_Setsrc(void *inu_ref,void *ifaceCfg);

typedef void (*inu_vtable_init_t)( void );
typedef const inu_ref__VTable* (*inu_vtable_get_t)( void );

#define INU_FACTORY__USB_INTERFACE_NUM (11)
#define INU_FACTORY__CONN_USB_IF0_STR  "usb_if_0"
#define INU_FACTORY__CONN_USB_IF1_STR  "usb_if_1"
#define INU_FACTORY__CONN_USB_IF2_STR  "usb_if_2"
#define INU_FACTORY__CONN_USB_IF3_STR  "usb_if_3"
#define INU_FACTORY__CONN_USB_IF4_STR  "usb_if_4"
#define INU_FACTORY__CONN_USB_IF5_STR  "usb_if_5"
#define INU_FACTORY__CONN_UART_STR     "uart"
#define INU_FACTORY__CONN_TCP_STR      "tcp"
#define INU_FACTORY__CONN_UDP_STR      "udp"
#define INU_FACTORY__CONN_CDC_STR      "cdc"
#define INU_FACTORY__CONN_UVC1_STR     "uvc1"
#define INU_FACTORY__CONN_UVC2_STR     "uvc2"
#define INU_FACTORY__CONN_UVC3_STR     "uvc3"
#define INU_FACTORY__CONN_UVC4_STR     "uvc4"
#define INU_FACTORY__CONN_STUB_STR     "stub"

#define INU_FACTORY__CONN_QUE_SIZE_DEFAULT      (1024)

typedef struct
{
   inu_factory__connIdE       id;
   int                        enabled;
   UINT32                     chanMap;
   CONN_LYRG_openCfgT         connConfig;
   inu_comm                   *comm;
   int                        refCtr;
}inu_factory__connConfigT;

typedef struct
{
   UINT32                  memSize;
   inu_vtable_init_t       vTableInit_cb;
   inu_vtable_get_t        vTableGet_cb;
   UINT32                  remoteNewCmd;
   inu_ref__statsEnable_e  statsEnable;
   BOOLEAN                 dataDirectFlag;
}inu_factory__refParams;

typedef struct
{
   inu_ref__types refType;
   int              enabled;
   inu_factory__connIdE ctrlConn;
   inu_factory__connIdE dataConn;
} inu_factory__chanConfigT;


static inu_factory__refParams       refParamsTable[INU_FACTORY__REF_NUM];
static inu_factory__connConfigT     connConfigTbl[INU_FACTORY__NUM_CONNECTIONS_E];
static inu_factory__chanConfigT     chanConfigTbl[INU_FACTORY__REF_NUM];


static USB_CTRL_COMG_paramsT        usbIfParams[INU_FACTORY__USB_INTERFACE_NUM];
static UART_COMG_interfaceParamsT   uartIfParams;
static TCP_COMG_interfaceParamsT    tcpIfParams;
static CDC_COMG_interfaceParamsT    cdcIfParams;
#if DEFSG_IS_GP
static UVC_COMG_paramsT             uvcIfParams[INU_FACTORY__CONN_NUM_UVC];
#endif
static char *connStrTbl[INU_FACTORY__NUM_CONNECTIONS_E];


static void initConnConfigEntry(inu_factory__connIdE id, char *str, void *ifCfg, CLS_COMG_getIfOps ifOps, UINT16 queSize)
{
   connConfigTbl[id].id = id;
   connConfigTbl[id].connConfig.ifCfg = ifCfg;
   connConfigTbl[id].connConfig.queueSize = queSize;
   connConfigTbl[id].connConfig.getIfOps = ifOps;
   connConfigTbl[id].enabled = 0;
   connConfigTbl[id].comm    = NULL;
   connConfigTbl[id].refCtr  = 0;
   connStrTbl[id] = str;
}

static void initUsbIfParams(void)
{
   enum { USB_IF0 = 0, USB_IF1 = 1, USB_IF2 = 2, USB_IF3 =3, USB_IF4 = 4, USB_IF5 =5, NUM_ENDPOINTS=6};

   usbIfParams[USB_IF0].usbInterfaceIdx = 0;
   usbIfParams[USB_IF0].fixedBuffSize=0;
   usbIfParams[USB_IF0].fixedSizeCap=FALSE;
   usbIfParams[USB_IF0].txCap=TRUE;
   usbIfParams[USB_IF0].rxCap=TRUE;

   usbIfParams[USB_IF1].usbInterfaceIdx = 1;
   usbIfParams[USB_IF1].fixedBuffSize = 0;
   usbIfParams[USB_IF1].fixedSizeCap = FALSE;
   usbIfParams[USB_IF1].txCap = TRUE;
   usbIfParams[USB_IF1].rxCap = TRUE;

   usbIfParams[USB_IF2].usbInterfaceIdx = 2;
   usbIfParams[USB_IF2].fixedBuffSize = 0;
   usbIfParams[USB_IF2].fixedSizeCap = FALSE;
   usbIfParams[USB_IF2].txCap = TRUE;
   usbIfParams[USB_IF2].rxCap = TRUE;

   usbIfParams[USB_IF3].usbInterfaceIdx = 3;
   usbIfParams[USB_IF3].fixedBuffSize = 0;
   usbIfParams[USB_IF3].fixedSizeCap = FALSE;
   usbIfParams[USB_IF3].txCap = TRUE;
   usbIfParams[USB_IF3].rxCap = TRUE;

   usbIfParams[USB_IF4].usbInterfaceIdx = 4;
   usbIfParams[USB_IF4].fixedBuffSize = 0;
   usbIfParams[USB_IF4].fixedSizeCap = FALSE;
   usbIfParams[USB_IF4].txCap = TRUE;
   usbIfParams[USB_IF4].rxCap = TRUE;
   usbIfParams[USB_IF5].usbInterfaceIdx = 5;
   usbIfParams[USB_IF5].fixedBuffSize = 0;
   usbIfParams[USB_IF5].fixedSizeCap = FALSE;
   usbIfParams[USB_IF5].txCap = TRUE;
   usbIfParams[USB_IF5].rxCap = TRUE;
   /*Helsinki specific change below that allows us to send/receive fixed size packets to reduce the amount of small URBs (USB request buffer)*/
   for(int i =0 ; i < NUM_ENDPOINTS; i ++)
   {
      UINT32 fixedSizeURB = HELSINKI_getURBSize(i);
      if(fixedSizeURB)
      {
         usbIfParams[i].unBufferedReadSize = fixedSizeURB;
         usbIfParams[i].unBufferedReadCap = TRUE;
      }
      else
      {  
         usbIfParams[i].unBufferedReadSize = 0;
         usbIfParams[i].unBufferedReadCap = FALSE;
      }
   }
}
#if DEFSG_IS_HOST
static void inu_factory__setUartIfParams(interfaceUnion* interfaceParamsP)
{
   uartIfParams.portNumber = interfaceParamsP->uart.portNumber;
   uartIfParams.baudrate   = interfaceParamsP->uart.baudrate;
}

static void inu_factory__setCdcIfParams(interfaceUnion* interfaceParamsP)
{
   cdcIfParams.portNumber = interfaceParamsP->cdc.portNumber;
   cdcIfParams.baudrate   = interfaceParamsP->cdc.baudrate;
}
#else
static void initUvcIfParams()
{
  unsigned int ind,entry;

  for (ind=INU_FACTORY__CONN_UVC1_E;ind<=INU_FACTORY__CONN_UVC_MAX_E;ind++)
  {
     entry = ind - INU_FACTORY__CONN_UVC1_E;
     uvcIfParams[entry].uvcInterfaceIdx = entry;
     uvcIfParams[entry].fixedBuffSize = 0;
     uvcIfParams[entry].fixedSizeCap = FALSE;
     uvcIfParams[entry].txCap = TRUE;
     uvcIfParams[entry].rxCap = TRUE;
     uvcIfParams[entry].uvc_service = NULL;
  }
}
#endif //DEFSG_IS_HOST
static void inu_factory__setTcpIfParams(interfaceUnion *interfaceParamsP)
{
   tcpIfParams.port = interfaceParamsP->tcp.port;
#if DEFSG_IS_GP
   tcpIfParams.isServer = TRUE;
   strcpy(tcpIfParams.ipAddrPeerP, interfaceParamsP->tcp.hostIp);
#else
   tcpIfParams.isServer = FALSE;
   strcpy(tcpIfParams.ipAddrPeerP, interfaceParamsP->tcp.targetIp);
#endif
}

static void inu_factory__initConnConfigTbl()
{

   enum { USB_IF0 = 0, USB_IF1 = 1, USB_IF2 = 2, USB_IF3 = 3, USB_IF4 = 4, USB_IF5 = 5};
   initUsbIfParams();
#if DEFSG_IS_GP
   initUvcIfParams();
#endif
   initConnConfigEntry(INU_FACTORY__CONN_USB_IF0_E, INU_FACTORY__CONN_USB_IF0_STR, &(usbIfParams[USB_IF0]), USB_CTRL_COMG_getOps, INU_FACTORY__CONN_QUE_SIZE_DEFAULT);
   initConnConfigEntry(INU_FACTORY__CONN_USB_IF1_E, INU_FACTORY__CONN_USB_IF1_STR, &(usbIfParams[USB_IF1]), USB_CTRL_COMG_getOps, INU_FACTORY__CONN_QUE_SIZE_DEFAULT);
   initConnConfigEntry(INU_FACTORY__CONN_USB_IF2_E, INU_FACTORY__CONN_USB_IF2_STR, &(usbIfParams[USB_IF2]), USB_CTRL_COMG_getOps, INU_FACTORY__CONN_QUE_SIZE_DEFAULT);
   initConnConfigEntry(INU_FACTORY__CONN_USB_IF3_E, INU_FACTORY__CONN_USB_IF3_STR, &(usbIfParams[USB_IF3]), USB_CTRL_COMG_getOps, INU_FACTORY__CONN_QUE_SIZE_DEFAULT);
   initConnConfigEntry(INU_FACTORY__CONN_USB_IF4_E, INU_FACTORY__CONN_USB_IF4_STR, &(usbIfParams[USB_IF4]), USB_CTRL_COMG_getOps, INU_FACTORY__CONN_QUE_SIZE_DEFAULT);
   initConnConfigEntry(INU_FACTORY__CONN_USB_IF5_E, INU_FACTORY__CONN_USB_IF5_STR, &(usbIfParams[USB_IF5]), USB_CTRL_COMG_getOps, INU_FACTORY__CONN_QUE_SIZE_DEFAULT);
   initConnConfigEntry(INU_FACTORY__CONN_UART_E   , INU_FACTORY__CONN_UART_STR   , &uartIfParams          , UART_COMG_getOps    , INU_FACTORY__CONN_QUE_SIZE_DEFAULT);
   initConnConfigEntry(INU_FACTORY__CONN_TCP_E    , INU_FACTORY__CONN_TCP_STR    , &tcpIfParams           , TCP_COMG_getOps     , INU_FACTORY__CONN_QUE_SIZE_DEFAULT);
   initConnConfigEntry(INU_FACTORY__CONN_CDC_E    , INU_FACTORY__CONN_CDC_STR    , &cdcIfParams           , CDC_COMG_getOps     , INU_FACTORY__CONN_QUE_SIZE_DEFAULT);
#if DEFSG_IS_GP
#ifdef ENABLE_UVC
   initConnConfigEntry(INU_FACTORY__CONN_UVC1_E   , INU_FACTORY__CONN_UVC1_STR    , &uvcIfParams[0]           , UVC_COMG_getOps     , INU_FACTORY__CONN_QUE_SIZE_DEFAULT);
   initConnConfigEntry(INU_FACTORY__CONN_UVC2_E   , INU_FACTORY__CONN_UVC2_STR    , &uvcIfParams[1]           , UVC_COMG_getOps     , INU_FACTORY__CONN_QUE_SIZE_DEFAULT);
   initConnConfigEntry(INU_FACTORY__CONN_UVC3_E   , INU_FACTORY__CONN_UVC3_STR    , &uvcIfParams[2]           , UVC_COMG_getOps     , INU_FACTORY__CONN_QUE_SIZE_DEFAULT);
   initConnConfigEntry(INU_FACTORY__CONN_UVC4_E   , INU_FACTORY__CONN_UVC4_STR    , &uvcIfParams[3]           , UVC_COMG_getOps     , INU_FACTORY__CONN_QUE_SIZE_DEFAULT);
#endif
#endif
 
   initConnConfigEntry(INU_FACTORY__CONN_STUB_E   , INU_FACTORY__CONN_STUB_STR   , NULL                   , STUB_COMG_getOps    , INU_FACTORY__CONN_QUE_SIZE_DEFAULT);
}


static void initChConfigEntry(inu_ref__types refType, inu_factory__connIdE ctrlConn, inu_factory__connIdE dataConn)
{
   chanConfigTbl[refType].refType  = refType;
   chanConfigTbl[refType].ctrlConn = ctrlConn;
   chanConfigTbl[refType].dataConn = dataConn;
}

static inu_factory__connIdE convertInterfaceEnum(inu_device_interface_0_e deviceInterface)
{
   inu_factory__connIdE ret = INU_FACTORY__CONN_USB_IF0_E;
   switch(deviceInterface)
   {
      case (INU_DEVICE_INTERFACE_0_USB):
         ret = INU_FACTORY__CONN_USB_IF0_E;
      break;
      case (INU_DEVICE_INTERFACE_0_UART):
         ret = INU_FACTORY__CONN_UART_E;
      break;
      case (INU_DEVICE_INTERFACE_0_TCP):
         ret = INU_FACTORY__CONN_TCP_E;
      break;
      case (INU_DEVICE_INTERFACE_0_STUB):
         ret = INU_FACTORY__CONN_STUB_E;
      break;
      case (INU_DEVICE_INTERFACE_0_CDC):
         ret = INU_FACTORY__CONN_CDC_E;
      break;
      default:
         assert(0);
   }
   return ret;
}

void inu_factory__initChanConfigTbl(inu_device_interface_0_t *deviceIf0)
{
   inu_device_interface_0_e deviceInterface = deviceIf0->select;

   if (deviceInterface == INU_DEVICE_INTERFACE_0_TCP)
   {
      inu_factory__setTcpIfParams(&deviceIf0->interfaceParams);
   }
#if DEFSG_IS_HOST
   else if (deviceInterface == INU_DEVICE_INTERFACE_0_UART)
   {
      inu_factory__setUartIfParams(&deviceIf0->interfaceParams);
   }
   else if (deviceInterface == INU_DEVICE_INTERFACE_0_CDC)
   {
      inu_factory__setCdcIfParams(&deviceIf0->interfaceParams);
   }
#endif //DEFSG_IS_HOST
   inu_factory__connIdE ctrlConn   = convertInterfaceEnum(deviceInterface);
   inu_factory__connIdE streamConn = convertInterfaceEnum(deviceInterface);

   //if (streamConn == INU_FACTORY__CONN_USB_IF0_E)
   if (0) // for Varjo
   {
      // Since the networks are too big to pass on USB0 in parallel to the stream, use USF1 for them
      initChConfigEntry(INU_LOAD_NETWORK_REF_TYPE, ctrlConn, INU_FACTORY__CONN_USB_IF1_E);
      initChConfigEntry(INU_LOAD_BACKGROUND_REF_TYPE, ctrlConn, INU_FACTORY__CONN_USB_IF1_E);
   }
   else
   {
      initChConfigEntry(INU_LOAD_NETWORK_REF_TYPE, ctrlConn, streamConn);
      initChConfigEntry(INU_LOAD_BACKGROUND_REF_TYPE, ctrlConn, streamConn);
   }

   initChConfigEntry(INU_DEVICE_REF_TYPE, ctrlConn, streamConn);
   initChConfigEntry(INU_SENSOR_REF_TYPE, ctrlConn, streamConn);
   initChConfigEntry(INU_SENSORS_GROUP_REF_TYPE, ctrlConn, streamConn);
   initChConfigEntry(INU_SOC_CHANNEL_REF_TYPE, ctrlConn, streamConn);
   initChConfigEntry(INU_HISTOGRAM_REF_TYPE, ctrlConn, streamConn);
   initChConfigEntry(INU_HISTOGRAM_DATA_REF_TYPE, ctrlConn, streamConn);
   initChConfigEntry(INU_SOC_WRITER_REF_TYPE, ctrlConn, streamConn);
   initChConfigEntry(INU_IMAGE_REF_TYPE, ctrlConn, streamConn);
   initChConfigEntry(INU_FDK_REF_TYPE, ctrlConn, streamConn);
   initChConfigEntry(INU_FDK_DSO_REF_TYPE, ctrlConn, streamConn);
   initChConfigEntry(INU_IMU_REF_TYPE, ctrlConn, streamConn);
   initChConfigEntry(INU_IMU_DATA_REF_TYPE, ctrlConn, streamConn);
   initChConfigEntry(INU_TEMPERATURE_REF_TYPE, ctrlConn, streamConn);
   initChConfigEntry(INU_TEMPERATURE_DATA_REF_TYPE, ctrlConn, streamConn);
   initChConfigEntry(INU_GRAPH_REF_TYPE, ctrlConn, streamConn);
   initChConfigEntry(INU_STREAMER_REF_TYPE, ctrlConn, streamConn);
   initChConfigEntry(INU_RAW_DATA_REF_TYPE, ctrlConn, streamConn);
   initChConfigEntry(INU_CVA_CHANNEL_REF_TYPE, ctrlConn, streamConn);
   initChConfigEntry(INU_CVA_DATA_REF_TYPE, ctrlConn, streamConn);
   initChConfigEntry(INU_LOGGER_REF_TYPE,  ctrlConn, streamConn);
   initChConfigEntry(INU_CDNN_REF_TYPE,  ctrlConn, streamConn);
   initChConfigEntry(INU_CDNN_DATA_REF_TYPE,  ctrlConn, streamConn);
   initChConfigEntry(INU_VISION_PROC_REF_TYPE,  ctrlConn, streamConn);
   initChConfigEntry(INU_SLAM_REF_TYPE,  ctrlConn, streamConn);
   initChConfigEntry(INU_SLAM_DATA_REF_TYPE,  ctrlConn, streamConn);
   initChConfigEntry(INU_TSNR_CALC_REF_TYPE, ctrlConn, streamConn);
   initChConfigEntry(INU_TSNR_DATA_REF_TYPE,  ctrlConn, streamConn);
   initChConfigEntry(INU_SENSOR_CONTROL_REF_TYPE,  ctrlConn, streamConn);
   initChConfigEntry(INU_FAST_ORB_REF_TYPE,  ctrlConn, streamConn);
   initChConfigEntry(INU_FAST_ORB_DATA_REF_TYPE,  ctrlConn, streamConn);
   initChConfigEntry(INU_COMPRESS_REF_TYPE,  ctrlConn, streamConn);
   initChConfigEntry(INU_DPE_PP_REF_TYPE,  ctrlConn, streamConn);
   initChConfigEntry(INU_PP_REF_TYPE,  ctrlConn, streamConn);
   initChConfigEntry(INU_MIPI_CHANNEL_REF_TYPE,  ctrlConn, streamConn);
   initChConfigEntry(INU_ISP_CHANNEL_REF_TYPE,  ctrlConn, streamConn);
   initChConfigEntry(INU_METADATA_INJECT_REF_TYPE,  ctrlConn, streamConn);
}

static void inu_factory__initRefTblEntry(inu_ref__types refType, UINT32 memSize,inu_vtable_init_t init_cb, inu_vtable_get_t get_cb, UINT32 remoteNewCmd, inu_ref__statsEnable_e enableStats, BOOLEAN dataDirectFlag)
{
   refParamsTable[refType].memSize       = memSize;
   refParamsTable[refType].vTableInit_cb = init_cb;
   refParamsTable[refType].vTableGet_cb  = get_cb;
   refParamsTable[refType].remoteNewCmd  = remoteNewCmd;
   refParamsTable[refType].statsEnable   = enableStats;
   refParamsTable[refType].dataDirectFlag= dataDirectFlag;
}

static void inu_factory__initRefTbl()
{
   inu_ref__statsEnable_e statsEnable = INU_REF__STATS_ENABLED;
#if DEFSG_IS_HOST
   statsEnable = INU_REF__STATS_DISABLED;
#endif
   inu_factory__initRefTblEntry(INU_DEVICE_REF_TYPE,        sizeof(inu_device),        inu_device__vtable_init,         (inu_vtable_get_t)inu_device__vtable_get,          0,                                  statsEnable,             FALSE);
   inu_factory__initRefTblEntry(INU_SENSOR_REF_TYPE,        sizeof(inu_sensor),        inu_sensor__vtable_init,         (inu_vtable_get_t)inu_sensor__vtable_get,          INTERNAL_CMDG_NEW_SENSOR_E,         INU_REF__STATS_DISABLED, FALSE);
   inu_factory__initRefTblEntry(INU_SENSORS_GROUP_REF_TYPE, sizeof(inu_sensors_group), inu_sensors_group__vtable_init,  (inu_vtable_get_t)inu_sensors_group__vtable_get,   INTERNAL_CMDG_NEW_SENSORS_GROUP_E,  INU_REF__STATS_DISABLED, FALSE);
   inu_factory__initRefTblEntry(INU_SOC_CHANNEL_REF_TYPE,   sizeof(inu_soc_channel),   inu_soc_channel__vtable_init,    (inu_vtable_get_t)inu_soc_channel__vtable_get,     INTERNAL_CMDG_NEW_SOC_CH_E,         statsEnable,             FALSE);
   inu_factory__initRefTblEntry(INU_HISTOGRAM_REF_TYPE,     sizeof(inu_histogram),     inu_histogram__vtable_init,      (inu_vtable_get_t)inu_histogram__vtable_get,       INTERNAL_CMDG_NEW_HISTOGRAM_E,      statsEnable,             FALSE);
   inu_factory__initRefTblEntry(INU_HISTOGRAM_DATA_REF_TYPE,sizeof(inu_histogram_data),inu_histogram_data__vtable_init, (inu_vtable_get_t)inu_histogram_data__vtable_get,  INTERNAL_CMDG_NEW_HISTOGRAM_DATA_E, INU_REF__STATS_DISABLED, FALSE);
   inu_factory__initRefTblEntry(INU_SOC_WRITER_REF_TYPE,    sizeof(inu_soc_writer),    inu_soc_writer__vtable_init,     (inu_vtable_get_t)inu_soc_writer__vtable_get,      INTERNAL_CMDG_NEW_SOC_WRITER_E,     INU_REF__STATS_DISABLED, FALSE);
   inu_factory__initRefTblEntry(INU_IMAGE_REF_TYPE,         sizeof(inu_image),         inu_image__vtable_init,          (inu_vtable_get_t)inu_image__vtable_get,           INTERNAL_CMDG_NEW_IMAGE_E,          INU_REF__STATS_DISABLED, FALSE);
   inu_factory__initRefTblEntry(INU_FDK_REF_TYPE,           sizeof(inu_fdk),           inu_fdk__vtable_init,            (inu_vtable_get_t)inu_fdk__vtable_get,             INTERNAL_CMDG_NEW_FDK_E,            INU_REF__STATS_ENABLED,  FALSE);
   inu_factory__initRefTblEntry(INU_ISP_CHANNEL_REF_TYPE,   sizeof(inu_isp_channel),   inu_isp_channel__vtable_init,    (inu_vtable_get_t)inu_isp_channel__vtable_get,     INTERNAL_CMDG_NEW_ISP_CH_E,         statsEnable,             FALSE);
// inu_factory__initTblEntry(INU_FDK_DSO_REF_TYPE, sizeof(inu_dso_fdk), inu_dso_fdk__vtable_init, INTERNAL_CMDG_NEW_FDK_E);
   inu_factory__initRefTblEntry(INU_IMU_REF_TYPE,           sizeof(inu_imu),           inu_imu__vtable_init,            (inu_vtable_get_t)inu_imu__vtable_get,             INTERNAL_CMDG_NEW_IMU_E,            statsEnable,             FALSE);
   inu_factory__initRefTblEntry(INU_IMU_DATA_REF_TYPE,      sizeof(inu_imu_data),      inu_imu_data__vtable_init,       (inu_vtable_get_t)inu_imu_data__vtable_get,        INTERNAL_CMDG_NEW_IMU_DATA_E,       INU_REF__STATS_DISABLED, FALSE);
   inu_factory__initRefTblEntry(INU_TEMPERATURE_REF_TYPE,   sizeof(inu_temperature),   inu_temperature__vtable_init,    (inu_vtable_get_t)inu_temperature__vtable_get,     INTERNAL_CMDG_NEW_TEMPERATURE_E,    INU_REF__STATS_DISABLED, FALSE);
   inu_factory__initRefTblEntry(INU_TEMPERATURE_DATA_REF_TYPE, sizeof(inu_temperature_data), inu_temperature_data__vtable_init, (inu_vtable_get_t)inu_temperature_data__vtable_get, INTERNAL_CMDG_NEW_TEMPERATURE_DATA_E, INU_REF__STATS_DISABLED, FALSE);
   inu_factory__initRefTblEntry(INU_GRAPH_REF_TYPE,         sizeof(inu_graph),         inu_graph__vtable_init,          (inu_vtable_get_t)inu_graph__vtable_get,           INTERNAL_CMDG_NEW_GRAPH_E,          INU_REF__STATS_DISABLED, FALSE);
   inu_factory__initRefTblEntry(INU_STREAMER_REF_TYPE,      sizeof(inu_streamer),      inu_streamer__vtable_init,       (inu_vtable_get_t)inu_streamer__vtable_get,        INTERNAL_CMDG_NEW_STREAMER_E,       INU_REF__STATS_ENABLED,  TRUE);
   inu_factory__initRefTblEntry(INU_RAW_DATA_REF_TYPE,      sizeof(inu_data),          inu_data__vtable_init,           (inu_vtable_get_t)inu_data__vtable_get,            INTERNAL_CMDG_NEW_DATA_E,           INU_REF__STATS_DISABLED, FALSE);
   inu_factory__initRefTblEntry(INU_LOGGER_REF_TYPE,        sizeof(inu_logger),        inu_logger__vtable_init,         (inu_vtable_get_t)inu_logger__vtable_get,          INTERNAL_CMDG_NEW_LOGGER_E,         INU_REF__STATS_DISABLED, FALSE);
#ifdef ENABLE_SENSOR_CONTROL
   inu_factory__initRefTblEntry(INU_SENSOR_CONTROL_REF_TYPE,sizeof(inu_sensor_control),inu_sensor_control__vtable_init, (inu_vtable_get_t)inu_sensor_control__vtable_get,  INTERNAL_CMDG_NEW_SENSOR_CONTROL_E, statsEnable,             FALSE);
#endif
   inu_factory__initRefTblEntry(INU_FAST_ORB_REF_TYPE,      sizeof(inu_fast_orb),      inu_fast_orb__vtable_init,       (inu_vtable_get_t)inu_fast_orb__vtable_get,        INTERNAL_CMDG_NEW_FAST_ORB_E,       statsEnable,             FALSE);
   inu_factory__initRefTblEntry(INU_FAST_ORB_DATA_REF_TYPE, sizeof(inu_fast_orb_data), inu_fast_orb_data__vtable_init,  (inu_vtable_get_t)inu_fast_orb_data__vtable_get,   INTERNAL_CMDG_NEW_FAST_ORB_DATA_E,  INU_REF__STATS_DISABLED, FALSE);
   inu_factory__initRefTblEntry(INU_CVA_CHANNEL_REF_TYPE,   sizeof(inu_cva_channel),   inu_cva_channel__vtable_init,    (inu_vtable_get_t)inu_cva_channel__vtable_get,     INTERNAL_CMDG_NEW_CVA_CH_E,         statsEnable,             FALSE);
   inu_factory__initRefTblEntry(INU_CVA_DATA_REF_TYPE,      sizeof(inu_cva_data),      inu_cva_data__vtable_init,       (inu_vtable_get_t)inu_cva_data__vtable_get,        INTERNAL_CMDG_NEW_CVA_DATA_E,       INU_REF__STATS_DISABLED, FALSE);
   inu_factory__initRefTblEntry(INU_CDNN_REF_TYPE,          sizeof(inu_cdnn),          inu_cdnn__vtable_init,           (inu_vtable_get_t)inu_cdnn__vtable_get,            INTERNAL_CMDG_NEW_CDNN_E,           statsEnable,             FALSE);
   inu_factory__initRefTblEntry(INU_CDNN_DATA_REF_TYPE,     sizeof(inu_cdnn_data),     inu_cdnn_data__vtable_init,      (inu_vtable_get_t)inu_cdnn_data__vtable_get,       INTERNAL_CMDG_NEW_CDNN_DATA_E,      INU_REF__STATS_DISABLED, FALSE);
#ifdef VISION_PROC
   inu_factory__initRefTblEntry(INU_VISION_PROC_REF_TYPE,   sizeof(inu_vision_proc),   inu_vision_proc__vtable_init,    (inu_vtable_get_t)inu_vision_proc__vtable_get,     INTERNAL_CMDG_NEW_VISION_PROC_E,    statsEnable,             FALSE);
#endif
   inu_factory__initRefTblEntry(INU_SLAM_REF_TYPE,          sizeof(inu_slam),          inu_slam__vtable_init,           (inu_vtable_get_t)inu_slam__vtable_get,            INTERNAL_CMDG_NEW_SLAM_E,           statsEnable,             FALSE);
   inu_factory__initRefTblEntry(INU_SLAM_DATA_REF_TYPE,     sizeof(inu_slam_data),     inu_slam_data__vtable_init,      (inu_vtable_get_t)inu_slam_data__vtable_get,       INTERNAL_CMDG_NEW_SLAM_DATA_E,      INU_REF__STATS_DISABLED, FALSE);
   inu_factory__initRefTblEntry(INU_TSNR_CALC_REF_TYPE,     sizeof(inu_tsnr_calc),     inu_tsnr_calc__vtable_init,      (inu_vtable_get_t)inu_tsnr_calc__vtable_get,       INTERNAL_CMDG_NEW_TSNR_CALC_E,      statsEnable,             FALSE);
   inu_factory__initRefTblEntry(INU_TSNR_DATA_REF_TYPE,     sizeof(inu_tsnr_data),     inu_tsnr_data__vtable_init,      (inu_vtable_get_t)inu_tsnr_data__vtable_get,       INTERNAL_CMDG_NEW_TSNR_DATA_E,      INU_REF__STATS_DISABLED, FALSE);
   inu_factory__initRefTblEntry(INU_POINT_CLOUD_DATA_REF_TYPE, sizeof(inu_point_cloud_data), inu_point_cloud_data__vtable_init, (inu_vtable_get_t)inu_point_cloud_data__vtable_get, INTERNAL_CMDG_NEW_POINT_CLOUD_DATA_E, INU_REF__STATS_DISABLED,FALSE);
   inu_factory__initRefTblEntry(INU_LOAD_NETWORK_REF_TYPE,  sizeof(inu_load_network),  inu_load_network__vtable_init,   (inu_vtable_get_t)inu_load_network__vtable_get,    INTERNAL_CMDG_NEW_LOAD_NETWORK_E,   statsEnable,             TRUE);
   inu_factory__initRefTblEntry(INU_LOAD_BACKGROUND_REF_TYPE, sizeof(inu_load_background), inu_load_background__vtable_init,   (inu_vtable_get_t)inu_load_background__vtable_get, INTERNAL_CMDG_NEW_LOAD_BACKGROUND_E, statsEnable,     TRUE);
   inu_factory__initRefTblEntry(INU_COMPRESS_REF_TYPE,      sizeof(inu_compress),      inu_compress__vtable_init,       (inu_vtable_get_t)inu_compress__vtable_get,        INTERNAL_CMDG_NEW_COMPRESS_E,       INU_REF__STATS_DISABLED, FALSE);
   inu_factory__initRefTblEntry(INU_DPE_PP_REF_TYPE,        sizeof(inu_dpe_pp),        inu_dpe_pp__vtable_init,         (inu_vtable_get_t)inu_dpe_pp__vtable_get,          INTERNAL_CMDG_NEW_DPE_PP_E,         statsEnable,             FALSE);
   inu_factory__initRefTblEntry(INU_PP_REF_TYPE,            sizeof(inu_pp),            inu_pp__vtable_init,             (inu_vtable_get_t)inu_pp__vtable_get,              INTERNAL_CMDG_NEW_PP_E,             statsEnable,             FALSE);
   inu_factory__initRefTblEntry(INU_MIPI_CHANNEL_REF_TYPE,  sizeof(inu_mipi_channel),  inu_mipi_channel__vtable_init,   (inu_vtable_get_t)inu_mipi_channel__vtable_get,    INTERNAL_CMDG_NEW_MIPI_CH_E,        statsEnable,             FALSE);
   inu_factory__initRefTblEntry(INU_METADATA_INJECT_REF_TYPE,  sizeof(inu_metadata_injector),  inu_metadata_injector__vtable_init,   (inu_vtable_get_t)inu_metadata_injector__vtable_get,    INTERNAL_CMDG_NEW_METADATA_INJECTOR_E,        statsEnable,             FALSE);
}

void inu_factory__init()
{
   inu_factory__initRefTbl();
   inu_factory__initConnConfigTbl();
}

inu_ref__types inu_factory__remoteNewCmdToType(UINT32 remoteNewCmd)
{
   inu_ref__types ret;

   switch(remoteNewCmd)
   {
      case (INTERNAL_CMDG_NEW_GRAPH_E):
      {
         ret = INU_GRAPH_REF_TYPE;
         break;
      }

      case (INTERNAL_CMDG_NEW_IMAGE_E):
      {
         ret = INU_IMAGE_REF_TYPE;
         break;
      }

      case (INTERNAL_CMDG_NEW_SOC_CH_E):
      {
         ret = INU_SOC_CHANNEL_REF_TYPE;
         break;
      }

      case (INTERNAL_CMDG_NEW_CVA_CH_E):
      {
         ret = INU_CVA_CHANNEL_REF_TYPE;
         break;
      }

      case (INTERNAL_CMDG_NEW_HISTOGRAM_E):
      {
         ret = INU_HISTOGRAM_REF_TYPE;
         break;
      }

      case (INTERNAL_CMDG_NEW_HISTOGRAM_DATA_E):
      {
         ret = INU_HISTOGRAM_DATA_REF_TYPE;
         break;
      }

      case (INTERNAL_CMDG_NEW_SOC_WRITER_E):
      {
         ret = INU_SOC_WRITER_REF_TYPE;
         break;
      }

      case (INTERNAL_CMDG_NEW_STREAMER_E):
      {
         ret = INU_STREAMER_REF_TYPE;
         break;
      }

      case (INTERNAL_CMDG_NEW_METADATA_INJECTOR_E):
      {
         ret = INU_METADATA_INJECT_REF_TYPE;
         break;
      }

      case (INTERNAL_CMDG_NEW_SENSOR_E):
      {
         ret = INU_SENSOR_REF_TYPE;
         break;
      }

      case (INTERNAL_CMDG_NEW_SENSORS_GROUP_E):
      {
         ret = INU_SENSORS_GROUP_REF_TYPE;
         break;
      }

      case (INTERNAL_CMDG_NEW_IMU_E):
      {
         ret = INU_IMU_REF_TYPE;
         break;
      }

      case (INTERNAL_CMDG_NEW_IMU_DATA_E):
      {
         ret = INU_IMU_DATA_REF_TYPE;
         break;
      }

      case (INTERNAL_CMDG_NEW_TEMPERATURE_E):
      {
         ret = INU_TEMPERATURE_REF_TYPE;
         break;
      }

      case (INTERNAL_CMDG_NEW_TEMPERATURE_DATA_E):
      {
         ret = INU_TEMPERATURE_DATA_REF_TYPE;
         break;
      }

      case(INTERNAL_CMDG_NEW_FDK_E):
      {
         ret = INU_FDK_REF_TYPE;
         break;
      }

      case(INTERNAL_CMDG_NEW_DATA_E):
      {
         ret = INU_RAW_DATA_REF_TYPE;
         break;
      }

      case(INTERNAL_CMDG_NEW_CVA_DATA_E):
      {
         ret = INU_CVA_DATA_REF_TYPE;
         break;
      }

      case(INTERNAL_CMDG_NEW_LOGGER_E):
      {
         ret = INU_LOGGER_REF_TYPE;
         break;
      }

      case(INTERNAL_CMDG_NEW_SENSOR_CONTROL_E):
      {
         ret = INU_SENSOR_CONTROL_REF_TYPE;
         break;
      }

      case(INTERNAL_CMDG_NEW_FAST_ORB_E):
      {
         ret = INU_FAST_ORB_REF_TYPE;
         break;
      }

      case(INTERNAL_CMDG_NEW_FAST_ORB_DATA_E):
      {
         ret = INU_FAST_ORB_DATA_REF_TYPE;
         break;
      }

      case(INTERNAL_CMDG_NEW_CDNN_E):
      {
         ret = INU_CDNN_REF_TYPE;
         break;
      }

      case(INTERNAL_CMDG_NEW_CDNN_DATA_E):
      {
         ret = INU_CDNN_DATA_REF_TYPE;
         break;
      }

      case(INTERNAL_CMDG_NEW_VISION_PROC_E):
      {
         ret = INU_VISION_PROC_REF_TYPE;
         break;
      }

      case(INTERNAL_CMDG_NEW_SLAM_E):
      {
         ret = INU_SLAM_REF_TYPE;
         break;
      }

      case(INTERNAL_CMDG_NEW_SLAM_DATA_E):
      {
         ret = INU_SLAM_DATA_REF_TYPE;
         break;
      }

     case(INTERNAL_CMDG_NEW_TSNR_DATA_E):
     {
        ret = INU_TSNR_DATA_REF_TYPE;
        break;
     }

     case(INTERNAL_CMDG_NEW_TSNR_CALC_E):
     {
        ret = INU_TSNR_CALC_REF_TYPE;
        break;
     }

     case(INTERNAL_CMDG_NEW_POINT_CLOUD_DATA_E):
     {
        ret = INU_POINT_CLOUD_DATA_REF_TYPE;
        break;
     }

     case(INTERNAL_CMDG_NEW_LOAD_NETWORK_E):
     {
        ret = INU_LOAD_NETWORK_REF_TYPE;
        break;
     }

     case(INTERNAL_CMDG_NEW_LOAD_BACKGROUND_E):
     {
         ret = INU_LOAD_BACKGROUND_REF_TYPE;
         break;
     }

     case(INTERNAL_CMDG_NEW_COMPRESS_E):
     {
        ret = INU_COMPRESS_REF_TYPE;
        break;
     }

     case(INTERNAL_CMDG_NEW_DPE_PP_E):
     {
        ret = INU_DPE_PP_REF_TYPE;
        break;
     }

     case(INTERNAL_CMDG_NEW_PP_E):
     {
        ret = INU_PP_REF_TYPE;
        break;
     }

     case(INTERNAL_CMDG_NEW_MIPI_CH_E):
     {
        ret = INU_MIPI_CHANNEL_REF_TYPE;
        break;
     }

      case (INTERNAL_CMDG_NEW_ISP_CH_E):
      {
         ret = INU_ISP_CHANNEL_REF_TYPE;
         break;
      }

     default:
     {
        ret = INU_FACTORY__REF_NUM;
        break;
     }
   }
   return ret;
}


static ERRG_codeE inu_factory__acquireComm(inu_factory__connIdE connId)
{
   ERRG_codeE ret = INU_FACTORY__RET_SUCCESS;
   inu_comm__CtorParams     ctorParams;
   inu_factory__connConfigT *connCfgP  = &connConfigTbl[connId];

   if(!connCfgP->enabled)
   {
      ctorParams.id = connCfgP->id;
      ret = inu_comm__new(&connCfgP->comm, &ctorParams);
      if (ERRG_SUCCEEDED(ret))
      {
         connCfgP->enabled = 1;
      }
      connCfgP->refCtr++;
   }
   else
   {
      connCfgP->refCtr++;
   }
   return ret;
}


int inu_factory__releaseComm(void *commP)
{
   inu_comm                 *comm = (inu_comm*)commP;
   inu_factory__connConfigT *connCfgP  = &connConfigTbl[comm->id];

   if(connCfgP->enabled)
   {
      connCfgP->refCtr--;
      if(connCfgP->refCtr == 0)
      {
         //inu_comm__delete(comm); --should be called here, but due to limitation of conn_lyr and inu_usb implementation, workaround is required
         connCfgP->enabled = 0;
      }
   }
   else
   {
      //trying to release comm not used
      assert(0);
   }
   return connCfgP->refCtr;
}


ERRG_codeE inu_factory__delete(inu_ref *me, int force)
{
   ERRG_codeE ret = INU_FACTORY__RET_SUCCESS;
   inu_ref__DtorParams dtorParams;
   //reuse
   dtorParams.ref.id = me->id;

   if (!force && dtorParams.ref.id > 0)
   {
      ret = inu_ref__remoteDelete(me, INTERNAL_CMDG_DELETE_REF_E, &dtorParams);
   }

   // in case that remote gp crashes, such remoteDelete is certain to fail, but these resource still need to be released.
   // by david @ 2023.11.01
   //if (ERRG_SUCCEEDED(ret) || (ret == INU_REF__ERR_CONN_DOWN))
   {
      INU_REF__LOGG_PRINT(me,LOG_INFO_E, NULL, "\n",me);
      inu_comm__unregister((inu_comm*)inu_ref__getCtrlComm(me), me);
      inu_comm__unregister((inu_comm*)inu_ref__getDataComm(me), me);
      me->p_vtable->p_dtor(me);
      free(me);
   }
   return ret;
}


ERRG_codeE inu_factory__new(inu_ref **me, void *ctorParamsP, inu_ref__types ref_type)
{
   ERRG_codeE ret = INU_FACTORY__RET_SUCCESS;
   inu_ref__CtorParams *refCtorParamsP = (inu_ref__CtorParams*)ctorParamsP;
   inu_ref *ref;
   inu_factory__refParams   *entryP        = &refParamsTable[ref_type];
   inu_factory__chanConfigT *chanCfgP      = &chanConfigTbl[ref_type];

   //first we make sure the comm for control for this ref is opened and ready
   ret = inu_factory__acquireComm(chanCfgP->ctrlConn);
   if (ERRG_SUCCEEDED(ret))
   {
      refCtorParamsP->ctrlCommH = connConfigTbl[chanCfgP->ctrlConn].comm;
      ret = inu_factory__acquireComm(chanCfgP->dataConn);
      if (ERRG_SUCCEEDED(ret))
      {
         refCtorParamsP->dataCommH = connConfigTbl[chanCfgP->dataConn].comm;
      }
   }

   if (ERRG_FAILED(ret))
   {
      //comm not ready, exit
      return ret;
   }

   //create the new ref
   ref = (inu_ref*)malloc(entryP->memSize);
   if (ref)
   {
      memset(ref, 0x00, entryP->memSize);
      entryP->vTableInit_cb();
      inu_ref__setVTable(ref, entryP->vTableGet_cb());
      refCtorParamsP->statsEnable        = entryP->statsEnable;
      refCtorParamsP->dataChanDirectFlag = entryP->dataDirectFlag;
      ret = ref->p_vtable->p_ctor(ref, refCtorParamsP);
      ref->refType = ref_type;
      ref->refSize = inu_factory__getRefSize(ref_type);
      if (ERRG_FAILED(ret))
      {
         free(ref);
         ref = NULL;
      }
      else
      {
         INU_REF__LOGG_PRINT(ref,LOG_INFO_E, NULL, "Ctor %s (address = %p) id %d type %d\n",refCtorParamsP->userName,ref,ref->id,ref->refType);
         //device created during boot
         if (ref_type != INU_DEVICE_REF_TYPE)
         {
            ret = inu_ref__remoteNew(ref,entryP->remoteNewCmd,ctorParamsP);
         }

         if (ERRG_FAILED(ret))
         {
            INU_REF__LOGG_PRINT(ref,LOG_ERROR_E, ret, "Ctor failed, release\n",ref);
            inu_factory__releaseComm((inu_comm*)inu_ref__getCtrlComm(ref));
            inu_factory__releaseComm((inu_comm*)inu_ref__getDataComm(ref));
            ref->p_vtable->p_dtor(ref);
            free(ref);
         }
         else
         {
            *me = ref;
         }
      }
   }
   else
   {
      ret = INU_FACTORY__ERR_OUT_OF_MEM;
   }

   return ret;
}


void *inu_factory__getConnConfig(inu_factory__connIdE id)
{
   return &connConfigTbl[id].connConfig;
}

void *inu_factory__getConnHandle(inu_factory__connIdE id)
{
   if (!connConfigTbl[id].comm)
      return NULL;
   else
      return connConfigTbl[id].comm->conn;
}

void *inu_factory__getCommHandle(inu_factory__connIdE id)
{
   return connConfigTbl[id].comm;
}

void inu_factory__restoreAllComm(inu_deviceH deviceH)
{
   ERRG_codeE ret = INU_FACTORY__RET_SUCCESS;
   inu_factory__connConfigT *connCfgP;
   UINT32 connId;

   for (connId = 0; connId < INU_FACTORY__NUM_CONNECTIONS_E; connId++)
   {
      int refIndex = 0;

      connCfgP  = &connConfigTbl[connId];

      if (connCfgP->enabled)
      {
         connCfgP->enabled = 0;

         ret = inu_factory__acquireComm((inu_factory__connIdE)connId);
         if (ERRG_SUCCEEDED(ret))
         {
            inu_ref__changeDataComm((inu_ref*)deviceH, connConfigTbl[connId].comm);
         }
      }
   }
}

void inu_factory__releaseAllComm()
{
   inu_factory__connConfigT *connCfgP;
   UINT32 connId;

   for (connId = 0; connId < INU_FACTORY__NUM_CONNECTIONS_E; connId++)
   {
      connCfgP  = &connConfigTbl[connId];
      if (connCfgP->enabled)
      {
         inu_comm__delete(connCfgP->comm);
         connCfgP->enabled = 0;
         connCfgP->refCtr = 0;
      }
   }
}

ERRG_codeE inu_factory__changeDataComm(inu_ref *me, inu_factory__connIdE connId)
{
   ERRG_codeE ret = INU_FACTORY__RET_SUCCESS;
   inu_comm *oldComm;
   //first we make sure the comm for control for this ref is opened and ready
   ret = inu_factory__acquireComm(connId);
   if (ERRG_SUCCEEDED(ret))
   {
      oldComm = (inu_comm*)inu_ref__changeDataComm(me, connConfigTbl[connId].comm);
      if (oldComm)
         inu_factory__releaseComm(oldComm);
   }
   return ret;
}

UINT32 inu_factory__getRefSize(inu_ref__types ref_type)
{
   return refParamsTable[ref_type].memSize;
}


