#include "helsinki_isp_synchronizer.h"
#include "helsinki.h"
#include "metadata_updater.h"
#include "gen_sensor_drv.h"
#include "ppe_mngr.h"
#include "focus_ctrl.h"
#include "inu_sensor_api.h"
#include <signal.h>
#include "isp_mngr.hpp"
#include "helsinki.h"
#include "inu_metadata_serializer.h"
#include <stdbool.h> 
static _Atomic int enable_awb_sync = true;
static _Atomic int enable_ae_sync = true; /*Atomic so no mutex is needed*/
#define ISP_SYNC_QUEUE_SIZE 10
static OS_LYRG_msgQueT notificationQ; /*Queue to store ISP callback events*/
static OS_LYRG_msgQueT isp_expoQ; /*Queue to store ISP callback events*/
static OS_LYRG_msgQueT isp_wbQ; /*Queue to store ISP callback events*/
OS_LYRG_threadHandle threadHandle;
static int frameCounter = 0;
/*This module synchonizes the WB and Exposure of multiple ISPs using the metadata infrastructure*/

void HELSINKI_notifyISPSyncThread(UINT32 frameID, UINT64 timestamp_ns)
{
    if(!HELSINKI_getMaster())
    {
        METADATA_SENSOR_UPDATER_interruptNotification notification = {0,0,0,0,0,0,0};
        notification.frameID = frameID;
        notification.timestamp = timestamp_ns;
        OS_LYRG_sendMsg(&notificationQ,(UINT8 *) &notification,sizeof(notification));
    }
}
inu_isp_channel__ispWbParamT previous_wbParams;
inu_isp_channel__exposureParamT previous_expoParams;

bool matching_expo_params(inu_isp_channel__exposureParamT * in, inu_isp_channel__exposureParamT * out)
{
    if(in->analogGain != out->analogGain)
        return false;
    if(in->integrationTime != out->integrationTime)
        return false;
    if(in->ispGain != out->ispGain)
        return false;
    return true;
}
bool matching_wb_params(inu_isp_channel__ispWbParamT * in, inu_isp_channel__ispWbParamT * out)
{
    for(unsigned int i =0; i < ISP_CHANNEL_CCMATRIX_SIZE; i++)
    {
        if(in->ccMatrix.coeff[i] != out->ccMatrix.coeff[i])
        {
            LOGG_PRINT(LOG_DEBUG_E,NULL,"Difference found in CC matrix at %lu, %.3f!=%.3f \n",i,in->ccMatrix.coeff[i],out->ccMatrix.coeff[i]);
            return false;
        }
    }
    if(in->ccOffset.blue != out->ccOffset.blue)
        return false;
    if(in->ccOffset.green != out->ccOffset.green)
        return false;
    if(in->ccOffset.red != out->ccOffset.red)
        return false;
    if(in->wbGain.gainB != out->wbGain.gainB)
        return false;
    if(in->wbGain.gainGb != out->wbGain.gainGb)
        return false;
    if(in->wbGain.gainGr != out->wbGain.gainGr)
        return false;
    if(in->wbGain.gainR != out->wbGain.gainR)
        return false;
    return true;
}
void HELSINKI_ISP_SYNCHRONIZER_set_AWBUartSyncState(bool awbSync)
{
    LOGG_PRINT(LOG_DEBUG_E,NULL,"Setting UART sync state to be %lu,%lu \n", awbSync);
    enable_awb_sync = awbSync;   /*Enable UART Sync is atomic*/
}
void HELSINKI_ISP_SYNCHRONIZER_set_AEUartSyncState(bool aeSync)
{
    LOGG_PRINT(LOG_DEBUG_E,NULL,"Setting UART sync state to be %lu,%lu \n", aeSync);
    enable_ae_sync = aeSync;   /*Enable UART Sync is atomic*/
}
ERRG_codeE HELSINKI_ISP_Synchronizer()
{
    inu_isp_channel__ispWbParamT wbParams;
    inu_isp_channel__exposureParamT expoParams;
    INU_Metadata_T snsrUpdate = ZEROED_OUT_METADATA;
    bool expoChanged = false;
    bool wbChanged = false;
    char buffer[512] = {0};
    ERRG_codeE ret;
    if(HELSINKI_getMaster())
    {
        LOGG_PRINT(LOG_DEBUG_E,NULL,"The ISP synchornizer should only run on the slave \n");
        return INU_METADATA__ERR_INVALID_ARGS;
    }
    if(!enable_awb_sync && !enable_ae_sync)
    {
        /*Uart Sync off*/
        return  INU_METADATA__ERR_INVALID_ARGS;
    }
    if(enable_awb_sync)
    {
        ret = ISP_MNGRG_getWbParams(ISP0_READER,&wbParams);
        if(ERRG_FAILED(ret))
        {
            LOGG_PRINT(LOG_ERROR_E,NULL,"Failed to read the white balance params\n");
            return ret;
        }
        wbChanged = !matching_wb_params(&previous_wbParams,&wbParams);
        previous_wbParams = wbParams;
    }
    if(enable_ae_sync)
    {
        ret = ISP_MNGRG_getExposureParams(ISP0_READER,&expoParams);
        if(ERRG_FAILED(ret))
        {
            LOGG_PRINT(LOG_ERROR_E,NULL,"Failed to read the exposure params\n");
            return ret;
        }
        expoChanged = !matching_expo_params(&previous_expoParams,&expoParams);
        previous_expoParams = expoParams;
    }
    if(!expoChanged && !wbChanged)
    {
        LOGG_PRINT(LOG_DEBUG_E,NULL,"Skipping sending, WB and Exposure is the same \n");
        return INU_METADATA__RET_SUCCESS;
    }
    LOGG_PRINT(LOG_DEBUG_E,NULL,"%lu,%lu \n", expoChanged,wbChanged);
    
    /*We have now read the WB gains */
    snsrUpdate.framebuffer_offset = 0;
    snsrUpdate.frameID = 0;
    snsrUpdate.serialization_counter = 1;
    snsrUpdate.timestamp = 0;
    /*Used by the target*/
    snsrUpdate.protobuf_packet.state[0].timestamp_ns.nanoseconds = 0;
    snsrUpdate.protobuf_packet.metadatacounter = 0;	/*Unused by the target*/
    snsrUpdate.protobuf_packet.state[0].sender = INU_metadata_HardwareType_HOST;
    snsrUpdate.protobuf_packet.state[0].target = INU_metadata_HardwareType_SLAVE;
    snsrUpdate.protobuf_packet.state[0].sensor.sensorType = INU_metadata_SensorType_AF_VST;
    snsrUpdate.protobuf_packet.state[0].sensor.side = INU_metadata_Channel_Side_Left;
    snsrUpdate.protobuf_packet.state[0].sensor.channelScale = INU_metadata_Channel_Scale_Cropped;
    snsrUpdate.protobuf_packet.state[0].has_sensor = true;
    snsrUpdate.protobuf_packet.state[0].has_sensor_metadata = true;
    snsrUpdate.protobuf_packet.state[0].has_timestamp_ns = true;
    snsrUpdate.protobuf_packet.state[0].context = INU_metadata_Context_Context_A;
    /*Master downscaled stream*/
    snsrUpdate.protobuf_packet.state[1].timestamp_ns.nanoseconds = 0;
    snsrUpdate.protobuf_packet.metadatacounter = 0;	/*Unused by the target*/
    snsrUpdate.protobuf_packet.state[1].sender = INU_metadata_HardwareType_HOST;
    snsrUpdate.protobuf_packet.state[1].target = INU_metadata_HardwareType_MASTER;
    snsrUpdate.protobuf_packet.state[1].sensor.sensorType = INU_metadata_SensorType_AF_VST;
    snsrUpdate.protobuf_packet.state[1].sensor.side = INU_metadata_Channel_Side_Left;
    snsrUpdate.protobuf_packet.state[1].sensor.channelScale = INU_metadata_Channel_Scale_Downscaled;
    snsrUpdate.protobuf_packet.state[1].has_sensor = true;
    snsrUpdate.protobuf_packet.state[1].has_sensor_metadata = true;
    snsrUpdate.protobuf_packet.state[1].has_timestamp_ns = true;
    snsrUpdate.protobuf_packet.state[1].context = INU_metadata_Context_Context_A;
    /*Master cropped stream*/
    snsrUpdate.protobuf_packet.state[2].timestamp_ns.nanoseconds = 0;
    snsrUpdate.protobuf_packet.metadatacounter = 0;	/*Unused by the target*/
    snsrUpdate.protobuf_packet.state[2].sender = INU_metadata_HardwareType_HOST;
    snsrUpdate.protobuf_packet.state[2].target = INU_metadata_HardwareType_MASTER;
    snsrUpdate.protobuf_packet.state[2].sensor.sensorType = INU_metadata_SensorType_AF_VST;
    snsrUpdate.protobuf_packet.state[2].sensor.side = INU_metadata_Channel_Side_Left;
    snsrUpdate.protobuf_packet.state[2].sensor.channelScale = INU_metadata_Channel_Scale_Cropped;
    snsrUpdate.protobuf_packet.state[2].has_sensor = true;
    snsrUpdate.protobuf_packet.state[2].has_sensor_metadata = true;
    snsrUpdate.protobuf_packet.state[2].has_timestamp_ns = true;
    snsrUpdate.protobuf_packet.state[2].context = INU_metadata_Context_Context_A;
    /*This is where the colour correction coeffcients are copied from */
    int stateCount  = 3;
    /*Send over uart every HELSINKI_UART_PERIOD frames to reduce CPU load on the Master NU4K */
    if((frameCounter % HELSINKI_UART_PERIOD) == 0)
        stateCount= 3;
    else
         stateCount = 1;

    frameCounter++;
    for(int i=0; i < stateCount; i ++)
    {
        if(enable_awb_sync)
        {
            for(int j=0; j< 9; j++)
            {	/*Set the colour correction matrix*/
                snsrUpdate.protobuf_packet.state[i].sensor_metadata.wb.colourcorrectinCoefficients.coeff[j] = wbParams.ccMatrix.coeff[j];
                snsrUpdate.protobuf_packet.state[i].sensor_metadata.wb.colourcorrectinCoefficients.coeff_count++;
            }
            /*Set the colour correction offsets to be the same as downscaled*/
            snsrUpdate.protobuf_packet.state[i].sensor_metadata.wb.colourcorrection.blue = wbParams.ccOffset.blue;
            snsrUpdate.protobuf_packet.state[i].sensor_metadata.wb.colourcorrection.green =wbParams.ccOffset.green;
            snsrUpdate.protobuf_packet.state[i].sensor_metadata.wb.colourcorrection.red =wbParams.ccOffset.red;
            /*Set the gains to be gain*/
            snsrUpdate.protobuf_packet.state[i].sensor_metadata.wb.wbGains.gainR =wbParams.wbGain.gainR;
            snsrUpdate.protobuf_packet.state[i].sensor_metadata.wb.wbGains.gainGr =wbParams.wbGain.gainGr;
            snsrUpdate.protobuf_packet.state[i].sensor_metadata.wb.wbGains.gainGb =wbParams.wbGain.gainGb;
            snsrUpdate.protobuf_packet.state[i].sensor_metadata.wb.wbGains.gainB =wbParams.wbGain.gainB;
            /*Set the has fields so that this data is serialzied*/
            snsrUpdate.protobuf_packet.state[i].sensor_metadata.wb.has_wbGains = true;
            snsrUpdate.protobuf_packet.state[i].sensor_metadata.wb.has_colourcorrection = true;
            snsrUpdate.protobuf_packet.state[i].sensor_metadata.wb.has_colourcorrectinCoefficients = true;
        }
        if(enable_ae_sync)
        {
            /*Copy the exposure parameters*/
            snsrUpdate.protobuf_packet.state[i].sensor_metadata.isp_expo.analogGain = expoParams.analogGain;
            snsrUpdate.protobuf_packet.state[i].sensor_metadata.isp_expo.integrationTime = expoParams.integrationTime;
            snsrUpdate.protobuf_packet.state[i].sensor_metadata.isp_expo.ispGain = expoParams.ispGain;
        }
        /*Only send the white balance and expo if it has changed!!*/
        snsrUpdate.protobuf_packet.state[i].sensor_metadata.has_wb = wbChanged && enable_awb_sync;
        snsrUpdate.protobuf_packet.state[i].sensor_metadata.has_isp_expo = expoChanged && enable_ae_sync;
    }
    snsrUpdate.protobuf_packet.state_count = stateCount;
    /*Serialize the message*/
    ret = inu_metadata__serialize(buffer,sizeof(buffer),&snsrUpdate);
    if(ERRG_FAILED(ret))
    {
        LOGG_PRINT(LOG_ERROR_E,NULL,"Failed to serialize protobuf command\n");
        return ret;
    }
    /*Handle the message*/
    ret = METADATA_UPDATER_processIncomingSensorUpdateRequest(buffer,sizeof(buffer),true,false);
    if(ERRG_FAILED(ret))
    {
        LOGG_PRINT(LOG_ERROR_E,NULL,"Failed to handle incoming sensor update request\n");
        return ret;
    }
    return INU_METADATA__RET_SUCCESS;
}
int HELSINKI_ISP_Sync_thread(void * arg)
{
    while(1)
    {
        METADATA_SENSOR_UPDATER_interruptNotification notification = {0,0,0,0,0,0,0};
        UINT32 size = sizeof(notification);
        int status = OS_LYRG_recvMsg(&notificationQ,(UINT8 *)&notification,&size,1000);
        if ((status == SUCCESS_E) && (size == sizeof(METADATA_SENSOR_UPDATER_interruptNotification)))
        {
            LOGG_PRINT(LOG_DEBUG_E,NULL,"Running ISP synchronizer for frame ID %lu, TS:%llu ms  \n", notification.frameID,(notification.timestamp)/(1000*1000));
            HELSINKI_ISP_Synchronizer();
        }
    }
}
/**
 * @brief Creates the metadata updater thread
 * 
 *
 * @return Returns an error code
 */
static ERRG_codeE HELSINKI_ISP_SYNC_createThread()
{
   ERRG_codeE           retCode = INU_METADATA__RET_SUCCESS;
   OS_LYRG_threadParams threadParams;
   /*Thread parameters*/
   threadParams.func = HELSINKI_ISP_Sync_thread;
   threadParams.id = OS_LYR_HELSINKI_ISP_SYNC_THREAD_ID_E;
   threadParams.event = NULL;
   threadParams.param = NULL;

   //create thread
   threadHandle = OS_LYRG_createThread(&threadParams);
   if (threadHandle == NULL)
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, "Failed to create thread\n");
      return INU_METADATA__ERR_ILLEGAL_STATE;
   }
   return retCode;
}
void HELSINKI_ISP_Sync_Init()
{
    memset(&previous_wbParams,0,sizeof(previous_wbParams));
    memset(&previous_expoParams,0,sizeof(previous_expoParams));
    notificationQ.maxMsgs = ISP_SYNC_QUEUE_SIZE;
    notificationQ.msgSize = sizeof(METADATA_SENSOR_UPDATER_interruptNotification);  /*This is a useful type for storing timing related info*/
    snprintf(notificationQ.name, OS_LYRG_MAX_MQUE_NAME_LEN, "/ISPSync_%lu_%lu", 0,0);
            
    OS_LYRG_createMsgQue(&notificationQ, OS_LYRG_MULTUPLE_EVENTS_NUM_E);

    isp_expoQ.maxMsgs = METADATA_ISP_PARAM_QUEUE_SIZE;
    isp_expoQ.msgSize = sizeof(METADATA_ISP_Expo_Queue_Entry);
    snprintf(isp_expoQ.name, OS_LYRG_MAX_MQUE_NAME_LEN, "/ISPExpoQSync_%lu", 0);
    ERRG_codeE ret = OS_LYRG_createMsgQue(&isp_expoQ, OS_LYRG_MULTUPLE_EVENTS_NUM_E);
    if (ret != SUCCESS_E)
    {
        LOGG_PRINT(LOG_ERROR_E, NULL, "Could not ISP Expo Queue \n");
    }
    isp_wbQ.maxMsgs = METADATA_ISP_PARAM_QUEUE_SIZE;
    isp_wbQ.msgSize = sizeof(METADATA_ISP_Wb_Queue_Entry);
    snprintf(isp_wbQ.name, OS_LYRG_MAX_MQUE_NAME_LEN, "/ISPWBQSync_%lu", 0);
    ret = OS_LYRG_createMsgQue(&isp_wbQ, OS_LYRG_MULTUPLE_EVENTS_NUM_E);
    if (ret != SUCCESS_E)
    {
        LOGG_PRINT(LOG_ERROR_E, NULL, "Could not create ISP WB Queue \n");
    }


    HELSINKI_ISP_SYNC_createThread();
}

 int HELSINKI_ISP_SYNCHRONIZER_AE_callback(struct Vsi3AMetaExp *expo_ptr)
 {
    METADATA_ISP_Expo_Queue_Entry queueEntry;
    memset(&queueEntry,0,sizeof(queueEntry));
    OS_LYRG_getTimeNsec(&queueEntry.notificationInfo.timestamp);
    queueEntry.value = *expo_ptr;
    if(OS_LYRG_sendMsg(&isp_expoQ,(UINT8 *)&queueEntry,sizeof(queueEntry))!=0)
      LOGG_PRINT(LOG_ERROR_E,NULL,"Failed to write to AWB Q \n");
    return 0;
 }
 int HELSINKI_ISP_SYNCHRONIZER_AWB_callback(struct Vsi3AMetaWb *wb_ptr)
 {
    METADATA_ISP_Wb_Queue_Entry queueEntry;
    memset(&queueEntry,0,sizeof(queueEntry));
    OS_LYRG_getTimeNsec(&queueEntry.notificationInfo.timestamp);
    queueEntry.value = *wb_ptr;
    if(OS_LYRG_sendMsg(&isp_wbQ,(UINT8 *)&queueEntry,sizeof(queueEntry))!=0)
      LOGG_PRINT(LOG_ERROR_E,NULL,"Failed to write to AWB Q \n");
    return 0;
 }