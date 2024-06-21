/****************************************************************************
 *
 *   FileName: stereo_demo_gp.c
 *
 *   Author: 
 *
 *   Date: 
 *
 *   Description: Receive frame from DDR and process it using vdsp
 *
 ****************************************************************************/

/****************************************************************************
 ***************               I N C L U D E   F I L E S        *************
 ****************************************************************************/ 

#include "inu2.h"
#include <assert.h>
#include <unistd.h>
#include <string.h>
#include "stereo_demo.h"
#include <mem_pool.h>
#include <inu_data.h>
#include <sys/time.h>

/****************************************************************************
 ***************         L O C A L       D E F N I T I O N S  ***************
 ****************************************************************************/
#define STEREO_DEMO_NODE_ID               (1)
#define STEREO_DEMO_DDR_SIZE              (0)

/// @brief Call Inuitive API with error logging
#define CALL_INU_API(FUNC, MSG) {               \
    ret = FUNC;                                 \
    if (ERRG_FAILED(ret)) {                     \
        inu_fdk__gp_log(LOG_ERROR_E, ret, MSG); \
    }                                           \
}

/****************************************************************************
 ***************            L O C A L    T Y P E D E F S      ***************
 ****************************************************************************/



/****************************************************************************
 ***************       L O C A L         D A T A              ***************
 ****************************************************************************/
static   UINT32                           STEREO_DEMO_ddrPhysAddr = 0;
static   UINT32                           STEREO_DEMO_ddrVirtAddr = 0;

/****************************************************************************
 ***************     P R E    D E F I N I T I O N     OF      ***************
 ***************     L O C A L         F U N C T I O N S      ***************
 ****************************************************************************/

/****************************************************************************
 ***************      L O C A L       F U N C T I O N S       ***************
 ****************************************************************************/

/****************************************************************************
*
*  Function Name: STEREO_DEMO_invokeDsp
*
*  Description:   This routine sends a message to the DSP to start processing a frame, the message contains all required parameters such as 
*              frame resolution, frame address and algorithm type
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context:
*
****************************************************************************/
static void STEREO_DEMO_invokeDsp(inu_fdkH meH, inu_function__operateParamsT *paramsP, inu_function__coreE targetDsp)
{
   ERRG_codeE                       ret;
   inu_fdk__dspMsgParamsT           generalMsgForDsp;
   STEREO_DEMOG_gpDspMsgProtocolT    gpToCevaMsgContent;

   gpToCevaMsgContent.opcode = STEREO_DEMOG_FRAME_ALG_E;

   //the basic demo msg protocol will be sent as the general message buffer   
   generalMsgForDsp.msgP     = &gpToCevaMsgContent;
   generalMsgForDsp.msgSize  = sizeof(STEREO_DEMOG_gpDspMsgProtocolT);

   CALL_INU_API(inu_fdk__gpToDspMsgSend(meH, &generalMsgForDsp, paramsP, targetDsp), "Failed to send msg to EV72\n");
}


/****************************************************************************
*
*  Function Name: STEREO_DEMO_freeData
*
*  Description: This is the callback when our created data is no longer in use by any other node 
*                   in the graph
*
*  Inputs: 
*
*  Outputs:
*
*  Returns:
*
*  Context:
*
****************************************************************************/
static void STEREO_DEMO_freeData(inu_fdkH meH, void *bufP)
{
   inu_fdk__gp_log(LOG_INFO_E, NULL, "bufP is now free\n");
}


/****************************************************************************
*
*  Function Name: STEREO_DEMO_createNewData
*
*  Description: create new data
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context:
*
****************************************************************************/
static ERRG_codeE STEREO_DEMO_createNewData(inu_fdkH meH, inu_dataH *newDataH, inu_function__operateParamsT* paramsP)
{
    inu_nodeH  currentH = NULL;
    ERRG_codeE ret;
    inu_data__hdr_t* hdrP;
    inu_cdnn_data__hdr_t cdnnHdr;

    //search for the data node we created for the new data.

    while ((currentH = inu_node__getNextOutputNode(meH, currentH)) > 0) {
        if (inu_ref__instanceOf(currentH, INU_CDNN_DATA_REF_TYPE)) { break; }
    }
    if (!currentH) { assert(0); }//error building the graph

    //hdrP = inu_data__getHdr((inu_dataH)currentH);
    hdrP = inu_data__getHdr((inu_dataH)paramsP->dataInputs[0]);

    cdnnHdr.dataHdr = *hdrP;
    cdnnHdr.network_id = 0;

    //request data obj. use it to get access to the actual memory
    CALL_INU_API(inu_fdk__newUserData(meH, (inu_dataH)currentH, STEREO_OUTPUT_SIZE, newDataH, (inu_data__hdr_t*)&cdnnHdr),
                 "failed to allocate a new data block\n");

    return ret;
}


/****************************************************************************
*
*  Function Name: STEREO_DEMO_release_data
*
*  Description: release data, notify graph manager
*
*  Inputs: chanParamsP - channel parameters
*
*  Outputs:
*
*  Returns:
*
*  Context:
*
****************************************************************************/
static void STEREO_DEMO_finish_operate(inu_fdkH meH, inu_function__operateParamsT *paramsP)
{
    ERRG_codeE ret;
    int i = 0;

    inu_fdk__gp_log(LOG_DEBUG_E, NULL, "finishOperating %d\n", paramsP->dataInputsNum);
    
    //notify the graph system that the data is ready for dispatching
    for (i = 0; i < paramsP->dataInputsNum; i++) {
        CALL_INU_API(inu_fdk__doneData(meH, paramsP->dataInputs[i]), "failed to dispatch data to graph\n");
    }

    CALL_INU_API(inu_fdk__complete(meH), "failed to inform function is complete to graph\n");
}


/****************************************************************************
*
*  Function Name: STEREO_DEMO_operate
*
*  Description: this routine trigers the GP and EV to process a frame
*
*  Inputs: chanParamsP - channel parameters
*
*  Outputs:
*
*  Returns:
*
*  Context:
*
****************************************************************************/
static void STEREO_DEMO_operate(inu_fdkH meH, inu_function__operateParamsT *paramsP)
{
   int i = 0;
   inu_image__hdr_t *imgHdrP;
   inu_imageH        imageH;
   inu_dataH         newDataH;
   void             *dataP;
   ERRG_codeE        ret;
   UINT64            startProcess, endProcess;

   ret = STEREO_DEMO_createNewData(meH, &newDataH, paramsP);
   if (ERRG_SUCCEEDED(ret))
   {
      // TODO
      // Remove this hack which adjusts the image size
      // The image was increased in height to include fake keypoints
      // hence the height adjustment here. The buffer pointer is also moved accordingly
      // to process at the start of correct image data

      inu_imageH imageInH = paramsP->dataInputs[0];
      inu_image__hdr_t* inputImgHdrP = (inu_image__hdr_t*)inu_data__getHdr(imageInH);
      inputImgHdrP->imgDescriptor.bufferHeight = 800;
      MEM_POOLG_bufDescT *bufDescP;
      inu_data__bufDescPtrGet((inu_data *)paramsP->dataInputs[0], &bufDescP);
      bufDescP->dataP += (800*100);

      imageInH = paramsP->dataInputs[1];
      inputImgHdrP = (inu_image__hdr_t*)inu_data__getHdr(imageInH);
      inputImgHdrP->imgDescriptor.bufferHeight = 800;
      inu_data__bufDescPtrGet((inu_data *)paramsP->dataInputs[1], &bufDescP);
      bufDescP->dataP += (800*100);

      //Add new data to the operate params
      paramsP->dataInputs[paramsP->dataInputsNum] = newDataH;
      paramsP->dataInputsNum++;
      STEREO_DEMO_invokeDsp(meH, paramsP, INU__EV62_CORE_E);
   }
   else
   {
      STEREO_DEMO_finish_operate(meH, paramsP);
   }
}

/****************************************************************************
*
*  Function Name: STEREO_DEMO_handleHostCtrlMsgCb
*
*  Description: this routine gets parameters from host and perform a simple processing
*
*  Inputs: hostMsgParamsP - message parameters
*
*  Outputs:
*
*  Returns:
*
*  Context:
*
****************************************************************************/
static void STEREO_DEMO_handleHostCtrlMsgCb(inu_fdkH meH, inu_function__startParamsT* paramsP)
{
   STEREO_DEMOG_gpDspMsgProtocolT      gpToEvMsgContent;
   inu_fdk__dspMsgParamsT           generalMsgForDsp;

   inu_fdk__gp_log(LOG_INFO_E, NULL, "STEREO_DEMO_handleHostCtrlMsgCb\n");
   gpToEvMsgContent.opcode               = STEREO_DEMOG_START_ALG_E;
   generalMsgForDsp.msgP                 = &gpToEvMsgContent;
   generalMsgForDsp.msgSize              = sizeof(STEREO_DEMOG_gpDspMsgProtocolT);
   inu_fdk__gpToDspMsgSyncSend(meH, &generalMsgForDsp, INU__EV62_CORE_E);

}


/****************************************************************************
*
*  Function Name: STEREO_DEMO_handleCevaCtrlMsgCb
*
*  Description: This is the EV done callback
*
*  Inputs: cevaMsgParamsP
*
*  Outputs:
*
*  Returns:
*
*  Context:
*
****************************************************************************/
static void STEREO_DEMO_handleDspMsgCb(inu_fdkH meH, inu_fdk__dspMsgParamsT *msgP, inu_function__operateParamsT *paramsP, inu_function__coreE dspSource)
{
   inu_fdk__gp_log(LOG_DEBUG_E, NULL, "STEREO_DEMO_handleDspMsgCb\n");
   STEREO_DEMO_finish_operate(meH, paramsP);
}


/****************************************************************************
 ***************     G L O B A L         F U N C T I O N S     **************
 ****************************************************************************/

/****************************************************************************
 ***************     G L O B A L         F U N C T I O N S     **************
 ****************************************************************************/
/****************************************************************************
*
*  Function Name: main
*
*  Description: 
*
*  Inputs: 
*
*  Outputs:
*
*  Returns:
*
*  Context:
*
****************************************************************************/
int main(int argc, char *argv[])
{
   ERRG_codeE               errCode = RET_SUCCESS;
   int                      wdTimeout = 3; // give 3 seconds until reset
   inu_fdk__getVersionT     version;
   inu_deviceH              mydevice;
   inu_fdk__initNodeParamsT nodeCbsRegister = {0};
   
   nodeCbsRegister.nodeId              = STEREO_DEMO_NODE_ID;
   nodeCbsRegister.flags               = INU_FDK__AUTO_CACHE_INV_FLAG;//(INU_FDK__AUTO_DONE_DATA_FLAG) | (INU_FDK__AUTO_OPERATE_DONE_FLAG);
   nodeCbsRegister.startCb             = STEREO_DEMO_handleHostCtrlMsgCb;
   nodeCbsRegister.operateCb           = STEREO_DEMO_operate;
   nodeCbsRegister.dspMsgCb            = STEREO_DEMO_handleDspMsgCb;
   //nodeCbsRegister.userDataFreeCb      = STEREO_DEMO_freeData;

   errCode = inu_fdk__initNodeCbFunctions(nodeCbsRegister);

   // This routine initiates the ARM and requests a buffer on DDR during run time. it gets its physical and virtual address
   errCode = inu_fdk__gpInit( wdTimeout, STEREO_DEMO_DDR_SIZE, &STEREO_DEMO_ddrPhysAddr, &STEREO_DEMO_ddrVirtAddr,INU_DEVICE__NORMAL_MODE, INU_DEVICE_INTERFACE_0_USB,&mydevice);

   if(ERRG_SUCCEEDED(errCode))
   {
      // get Inuitive version
      inu_fdk__getVersion(&version);
      inu_fdk__gp_log(LOG_INFO_E, NULL, "FW version=%d.%d.%d.%d, HW version: 0x%X.\n", version.fwVerId.fields.major, version.fwVerId.fields.minor, version.fwVerId.fields.build, version.fwVerId.fields.subBuild, version.hwVerId.val);
      do
      {
         // go to sleep for 10ms
         usleep(10000);
      } while (1);
   }

   inu_fdk__deinit(mydevice);

   return 0;
}




