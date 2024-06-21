/****************************************************************************
 *
 *   FileName: stereo_demo_ev.c
 *
 *   Author:  Eldad M.
 *
 *   Date: 
 *
 *   Description: 
 *   
 ****************************************************************************/
#include <stdio.h>
#include <string.h>
#include "client_api.h"
#include "stereo_demo.h"
#include "evthreads.h"
#include "ocl_kernel_requirements.h"

#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <stdarg.h>
#include <assert.h>

#include <ev_ocl.h>

//Declaration of OpenCL kernel
EVOCL_KERNEL_DECL(ocl_gaussian3x3);
/****************************************************************************
***************     L O C A L        D E F I N I T I O N S    ***************
****************************************************************************/
#define  GAUSSIAN_OCL 1

typedef struct
{
   UINT32                             outputImageAddr;
   UINT32                             inputImageAddr;
   inu_image__hdr_t*                  inputImgDescP;
   STEREO_DEMOG_algConfigT*           algConfigP;
   UINT16                             jobDescriptor;
   inu_function__operateParamsT*      funcOperateParamsP;
   STEREO_DEMOG_gpDspMsgProtocolT*    gpDspMsgP;
}  STEREO_DEMO_EVP_threadParamsStructT;

typedef struct
{
   inu_function__operateParamsT*      funcOperateParamsP;
   STEREO_DEMOG_gpDspMsgProtocolT*    gpDspMsgP;
   inu_cdnn_data__hdr_t*              outputCnnHdr;
   UINT8*                             outputCnnBlob;
   UINT16                             jobDescriptor;
}  STEREO_DEMO_EVP_cnnThreadParamsStructT;

typedef struct
{
    STEREO_DEMO_cnnMessageHdrT msg_header;
    int net_id;
    unsigned int frame_id;
    uint8_t* blob_output_address;
    uint8_t* blob_output_header;
    unsigned int jobDescriptor;
    EvIntCondType* intCondSignal;
    int number_of_images_in_a_set; // Number of input images that the CNN expects
    int number_of_injected_sets; // Number of injected sets. (How many times to run the CNN)
    STEREO_DEMO_injectImageParamsT input_sets[STEREO_DEMO_MAX_INPUT_IMAGES_PER_CNN];
}STEREO_DEMO_injectImagesT;

/****************************************************************************
***************                     D A T A                  ***************
****************************************************************************/
static STEREO_DEMOG_algConfigT STEREO_DEMO_EVP_algConfig;

#pragma data(".stereo_demo")
CLIENT_APIG_msgQueT                  STEREO_DEMO_EVP_msgQue;
CLIENT_APIG_msgQueT                  STEREO_DEMO_EVP_msgQueVdsp2;
CLIENT_APIG_msgQueT                  STEREO_DEMO_EVP_msgQueCnn;
#pragma data()

extern EvIntCondType                 SCHEDG_Condition;
EvIntCondType                        STEREO_DEMO_EVG_vdsp2Condition;
EvIntCondType                        STEREO_DEMO_EVG_cnnCondition;
UINT8*                               STEREO_DEMO_EVG_cnnOutput[2];
inu_cdnn_data__hdr_t*                STEREO_DEMO_EVG_cnnOutputHdr[2];
UINT32                               STEREO_DEMO_EVG_cnnCurrentBuf;
UINT32                               STEREO_DEMO_EVG_cnnMutex;


/****************************************************************************
***************      L O C A L         D E C L A R A T I O N S  *************
****************************************************************************/
static void       STEREO_DEMO_EVP_processFrame(UINT32 outImageAddr, UINT32 inImageAddr, inu_image__hdr_t* inImageDescP, STEREO_DEMOG_algConfigT* algConfigP);
static void       STEREO_DEMO_EVP_startAlg(STEREO_DEMOG_gpDspMsgProtocolT *msgParamsP, inu_function__operateParamsT *paramsP);
static void*      STEREO_DEMO_EVP_processThread(void* arg);
static void*      STEREO_DEMO_EVP_vdspThread(void* arg);
static void*      STEREO_DEMO_EVP_cnnThread(void* arg);
static void       STEREO_DEMO_EVP_handleGpCtrlMsgCb(CLIENT_APIG_cevaMsgParamsT *generalMsgParamsP, inu_function__operateParamsT *paramsP, UINT16 jobDescriptor);
static void       STEREO_DEMO_EVP_initCb();


/****************************************************************************
**************   L O C A L      F U N C T I O N S                                        ***************
****************************************************************************/

/****************************************************************************
*
*  Function Name: STEREO_DEMO_EVP_processFrame
*
*  Description: This routine process the frame 
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
static void STEREO_DEMO_EVP_processFrame(UINT32                       outImageAddr,
                                         UINT32                       inImageAddr,
                                         inu_image__hdr_t             *inImageDescP,
                                         STEREO_DEMOG_algConfigT      *algConfigP)
{

   demo_ctl_t ctl;   
   void *args[] = { (UINT8 *)inImageAddr, (UINT8 *)outImageAddr, &ctl};

   ctl.width = inImageDescP->imgDescriptor.width;
   ctl.height = inImageDescP->imgDescriptor.height;
   ctl.offset = 1;
   
   CLIENT_APIG_evSendLog(LOG_DEBUG_E, "STEREO_DEMO_EVP_processFrame -------------------- 0x%X\n", inImageAddr);
    uint64_t start, end;
   start = getRTC();
   #if GAUSSIAN_OCL
    callOclManualKernel(OCL_KERNEL(ocl_gaussian3x3), args);
   #endif
   end = getRTC();
   CLIENT_APIG_evSendLog(LOG_DEBUG_E, "STEREO_DEMO TIME %llu --------------------\n", end-start);

}

/****************************************************************************
*
*  Function Name: STEREO_DEMO_EVP_startAlg
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
static void STEREO_DEMO_EVP_startAlg(STEREO_DEMOG_gpDspMsgProtocolT *msgParamsP, inu_function__operateParamsT *paramsP)
{

   //enable algorithm 
   STEREO_DEMO_EVP_algConfig.enableAlg = TRUE;
   //Initialize DMA to enable async_work_group_copy, used in OCL kernel
   //evStuInit();
   //print debug information
   CLIENT_APIG_evSendLog(LOG_INFO_E, "start: stereo demo\n");
   
   EvThreadType ThreadType;

   STEREO_DEMO_EVP_msgQue.maxMsgs = CLIENT_APIG_QUE_MAX_ELEMENTS;
   STEREO_DEMO_EVP_msgQue.msgSize = sizeof(STEREO_DEMO_EVP_threadParamsStructT);
   if(CLIENT_APIG_createMsgQue(&STEREO_DEMO_EVP_msgQue, 0) != SUCCESS_E)
   {
      CLIENT_APIG_evSendLog(LOG_ERROR_E, "STEREO demo msg queue create failed\n");
   }
   
   //Create a thread on core 0 for the demo frame processing
   ThreadType = evNewThread(STEREO_DEMO_EVP_processThread, EVTH_INIT_CPU, EV_CORE_ID_ZERO, EVTH_INIT_ARGPTR, NULL, EVTH_INIT_LAST);         
   if(ThreadType == NULL)
   {
      CLIENT_APIG_evSendLog(LOG_ERROR_E, "Failed to open a new thread\n");
   }

   STEREO_DEMO_EVP_msgQueVdsp2.maxMsgs = CLIENT_APIG_QUE_MAX_ELEMENTS;
   STEREO_DEMO_EVP_msgQueVdsp2.msgSize = sizeof(STEREO_DEMO_EVP_threadParamsStructT);
   if (CLIENT_APIG_createMsgQue(&STEREO_DEMO_EVP_msgQueVdsp2, 0) != SUCCESS_E)
   {
       CLIENT_APIG_evSendLog(LOG_ERROR_E, "STEREO demo Vdsp2 msg queue create failed\n");
   }

   //Create a thread on core 1 for the demo 2nd frame processing
   ThreadType = evNewThread(STEREO_DEMO_EVP_vdspThread, EVTH_INIT_CPU, EV_CORE_ID_ONE, EVTH_INIT_ARGPTR, NULL, EVTH_INIT_LAST);
   if (ThreadType == NULL)
   {
       CLIENT_APIG_evSendLog(LOG_ERROR_E, "Failed to open a new vdsp thread\n");
   }

   STEREO_DEMO_EVP_msgQueCnn.maxMsgs = CLIENT_APIG_QUE_MAX_ELEMENTS;
   STEREO_DEMO_EVP_msgQueCnn.msgSize = sizeof(STEREO_DEMO_EVP_cnnThreadParamsStructT);
   if (CLIENT_APIG_createMsgQue(&STEREO_DEMO_EVP_msgQueCnn, 0) != SUCCESS_E)
   {
      CLIENT_APIG_evSendLog(LOG_ERROR_E, "cnn msg queue create failed\n");
   }

   //Create a thread on core 1 for the cnn
   ThreadType = evNewThread(STEREO_DEMO_EVP_cnnThread, EVTH_INIT_CPU, EV_CORE_ID_ONE, EVTH_INIT_ARGPTR, NULL, EVTH_INIT_LAST);
   if (ThreadType == NULL)
   {
      CLIENT_APIG_evSendLog(LOG_ERROR_E, "Failed to open a new cnn thread\n");
   }

   int ret = evIntCondInit(&STEREO_DEMO_EVG_cnnCondition);
   if (ret != EVTH_ERROR_OK)
   {
       CLIENT_APIG_evSendLog(LOG_ERROR_E, "Failed to create cnnCondSignal\n");
      return;
   }

   //init cnn (CNN_HAL_THREAD_CPU is 1)
   CLIENT_APIG_evCnnInit(EV_CORE_ID_ONE);

   //init internal duoble buffer for cnn output
   STEREO_DEMO_EVG_cnnOutput[0] = (UINT8*)evMemAlloc(STEREO_OUTPUT_CNN_BLOB_MAX_SIZE, EV_MR_GLOBAL_HEAP, -1);
   STEREO_DEMO_EVG_cnnOutput[1] = (UINT8*)evMemAlloc(STEREO_OUTPUT_CNN_BLOB_MAX_SIZE, EV_MR_GLOBAL_HEAP, -1);
   memset(STEREO_DEMO_EVG_cnnOutput[0], 0, STEREO_OUTPUT_CNN_BLOB_MAX_SIZE);
   memset(STEREO_DEMO_EVG_cnnOutput[1], 0, STEREO_OUTPUT_CNN_BLOB_MAX_SIZE);
   STEREO_DEMO_EVG_cnnOutputHdr[0] = (inu_cdnn_data__hdr_t*)evMemAlloc(sizeof(inu_cdnn_data__hdr_t), EV_MR_GLOBAL_HEAP, -1);
   STEREO_DEMO_EVG_cnnOutputHdr[1] = (inu_cdnn_data__hdr_t*)evMemAlloc(sizeof(inu_cdnn_data__hdr_t), EV_MR_GLOBAL_HEAP, -1);
   memset(STEREO_DEMO_EVG_cnnOutputHdr[0], 0, sizeof(inu_cdnn_data__hdr_t));
   memset(STEREO_DEMO_EVG_cnnOutputHdr[1], 0, sizeof(inu_cdnn_data__hdr_t));
   STEREO_DEMO_EVG_cnnCurrentBuf = 0;
   STEREO_DEMO_EVG_cnnMutex = 1;
   CLIENT_APIG_evSendLog(LOG_INFO_E, "STEREO_DEMO_EVG_cnnOutput 0x%X 0x%X\n", STEREO_DEMO_EVG_cnnOutput[0], STEREO_DEMO_EVG_cnnOutput[1]);
   CLIENT_APIG_evSendLog(LOG_INFO_E, "STEREO_DEMO_EVG_cnnOutputHdr 0x%X 0x%X\n", STEREO_DEMO_EVG_cnnOutputHdr[0], STEREO_DEMO_EVG_cnnOutputHdr[1]);

}

/****************************************************************************
*
*  Function Name: STEREO_DEMO_EVP_processThread
*
*  Description: STEREO demo frame processing is performed in this thread's context
*               triggering cnn engine and second vdsp
*
*  Inputs:  pointer to struct
*
*  Outputs:
*
*  Returns:
*
*  Context:
*
****************************************************************************/

static void * STEREO_DEMO_EVP_processThread(void *arg)
{
    STEREO_DEMO_EVP_threadParamsStructT localThreadParams;
    STEREO_DEMO_EVP_threadParamsStructT vdsp2ThreadParams;
    UINT32  localThreadParamsSize;
   INT32  status;
   CLIENT_APIG_evSendLog(LOG_INFO_E, "STEREO demo thread started on core %d\n", evGetCpu());
   while(1)
   {
      localThreadParamsSize = sizeof(STEREO_DEMO_EVP_threadParamsStructT);
      status = CLIENT_APIG_recvMsg(&STEREO_DEMO_EVP_msgQue,  (UINT8*)&localThreadParams, &localThreadParamsSize, 0 /*not used*/);      
      if(status == SUCCESS_E) 
      {
         STEREO_DEMO_EVP_cnnThreadParamsStructT cnnThreadParams;

         CLIENT_APIG_evSendLog(LOG_DEBUG_E, "STEREO_DEMO_EVP_processThread: STEREO_DEMO_EVG_cnnMutex %d STEREO_DEMO_EVG_cnnCurrentBuf %d\n", STEREO_DEMO_EVG_cnnMutex, STEREO_DEMO_EVG_cnnCurrentBuf);
         if (STEREO_DEMO_EVG_cnnMutex)
         {
            cnnThreadParams.gpDspMsgP = localThreadParams.gpDspMsgP;
            cnnThreadParams.funcOperateParamsP = localThreadParams.funcOperateParamsP;
            cnnThreadParams.jobDescriptor = localThreadParams.jobDescriptor;
            cnnThreadParams.outputCnnBlob = STEREO_DEMO_EVG_cnnOutput[STEREO_DEMO_EVG_cnnCurrentBuf];
            cnnThreadParams.outputCnnHdr = STEREO_DEMO_EVG_cnnOutputHdr[STEREO_DEMO_EVG_cnnCurrentBuf];
            //Trigger cnn thread (by sending msg to its msg que)
            CLIENT_APIG_evSendLog(LOG_DEBUG_E, "send msg to cnn thread\n");
            if (CLIENT_APIG_sendMsg(&STEREO_DEMO_EVP_msgQueCnn, (UINT8*)&cnnThreadParams, sizeof(STEREO_DEMO_EVP_cnnThreadParamsStructT)) != SUCCESS_E)
            {
                CLIENT_APIG_evSendLog(LOG_ERROR_E, "Failed to send msg to STEREO demo cnn thread\n");
            }
         }

         memcpy((UINT8*)&vdsp2ThreadParams, (UINT8*)&localThreadParams, localThreadParamsSize);
         inu_imageH imageOutH = vdsp2ThreadParams.funcOperateParamsP->dataInputs[RIGHT_CHANNEL];//output is image
         inu_imageH imageInH = vdsp2ThreadParams.funcOperateParamsP->dataInputs[RIGHT_CHANNEL];//image input
         UINT32 outputImageAddr = (UINT32)inu_data__getMemPtr(imageOutH);
         UINT32 inputImageAddr = (UINT32)inu_data__getMemPtr(imageInH);
         inu_image__hdr_t* inputImgHdrP = (inu_image__hdr_t*)inu_data__getHdr(imageInH);;
         vdsp2ThreadParams.inputImageAddr = inputImageAddr;
         vdsp2ThreadParams.inputImgDescP = inputImgHdrP;
         vdsp2ThreadParams.outputImageAddr = outputImageAddr;
         //Trigger vdsp2 thread (by sending msg to its msg que)
         CLIENT_APIG_evSendLog(LOG_DEBUG_E, "send msg to STEREO demo vdsp2 thread\n");
         if (CLIENT_APIG_sendMsg(&STEREO_DEMO_EVP_msgQueVdsp2, (UINT8*)&vdsp2ThreadParams, sizeof(STEREO_DEMO_EVP_threadParamsStructT)) != SUCCESS_E)
         {
             CLIENT_APIG_evSendLog(LOG_ERROR_E, "Failed to send msg to STEREO demo vdsp2 thread\n");
         }

         CLIENT_APIG_evSendLog(LOG_DEBUG_E, "VDSP 1 -------------------- 0x%X\n", localThreadParams.inputImageAddr);
         STEREO_DEMO_EVP_processFrame(localThreadParams.outputImageAddr, localThreadParams.inputImageAddr, localThreadParams.inputImgDescP, localThreadParams.algConfigP);
         CLIENT_APIG_evSendLog(LOG_DEBUG_E, "VDSP 1 -------------------- END\n");
         //copy current results to common blob
         UINT32 databaseListNum = localThreadParams.funcOperateParamsP->dataInputsNum;
         inu_dataH  outputH = localThreadParams.funcOperateParamsP->dataInputs[databaseListNum - 1];//cnn data output
         UINT8 *output_blob_addr = (UINT8*)inu_data__getMemPtr(outputH);
         inu_cdnn_data__hdr_t* outputHdrP = (inu_cdnn_data__hdr_t*)inu_data__getHdr(outputH);
         /*
         CLIENT_APIG_evSendLog(LOG_INFO_E, "outputHdrP: %p size: %d\n---\n", outputHdrP, 
            sizeof(inu_cdnn_data__hdr_t));*/
         //cnn blob
         memcpy(output_blob_addr, STEREO_DEMO_EVG_cnnOutput[STEREO_DEMO_EVG_cnnCurrentBuf], STEREO_OUTPUT_CNN_BLOB_MAX_SIZE);
         memcpy(outputHdrP, STEREO_DEMO_EVG_cnnOutputHdr[STEREO_DEMO_EVG_cnnCurrentBuf], sizeof(inu_cdnn_data__hdr_t));
         //process blob
         UINT32* output_process_blob = (UINT32*)(output_blob_addr + STEREO_OUTPUT_CNN_BLOB_MAX_SIZE);
         static UINT32 processIndex = 0;
         *output_process_blob = 0xBABA0000 + processIndex;
         processIndex ++;
         if (processIndex > 0xffff)
            processIndex = 0;

         //Waiting for a signal from vdsp2 thread
         evIntCondWait(&STEREO_DEMO_EVG_vdsp2Condition);

         evDataCacheFlushLines(output_blob_addr, output_blob_addr + STEREO_OUTPUT_SIZE);
         //Push jobDescriptor to scheduler finished jobs que (so scheduler will update the host)
         CLIENT_APIG_schedPushFinishedwJob(localThreadParams.jobDescriptor);
         //Trigger the scheduler thread (it will wake up and deque this finished job, and update the host)
         evIntCondSignal(&SCHEDG_Condition);
      }
   }
}

/****************************************************************************
*
*  Function Name: STEREO_DEMO_EVP_vdspThread
*
*  Description: STEREO demo frame processing is performed in this thread's context
*
*  Inputs:  pointer to struct
*
*  Outputs:
*
*  Returns:
*
*  Context:
*
****************************************************************************/

static void* STEREO_DEMO_EVP_vdspThread(void* arg)
{
    STEREO_DEMO_EVP_threadParamsStructT localThreadParams;
    UINT32  localThreadParamsSize;
    INT32  status;
    CLIENT_APIG_evSendLog(LOG_INFO_E, "STEREO demo vdsp2 thread started on core %d\n", evGetCpu());
    while (1)
    {
        localThreadParamsSize = sizeof(STEREO_DEMO_EVP_threadParamsStructT);
        status = CLIENT_APIG_recvMsg(&STEREO_DEMO_EVP_msgQueVdsp2, (UINT8*)&localThreadParams, &localThreadParamsSize, 0 /*not used*/);
        if (status == SUCCESS_E)
        {
            CLIENT_APIG_evSendLog(LOG_DEBUG_E, "VDSP 2 -------------------- 0x%X\n", localThreadParams.inputImageAddr);
            STEREO_DEMO_EVP_processFrame(localThreadParams.outputImageAddr, localThreadParams.inputImageAddr, localThreadParams.inputImgDescP, localThreadParams.algConfigP);
            //copy current results to common blob
            UINT32 databaseListNum = localThreadParams.funcOperateParamsP->dataInputsNum;
            inu_dataH  outputH = localThreadParams.funcOperateParamsP->dataInputs[databaseListNum - 1];//cnn data output
            UINT8* output_blob_addr = (UINT8*)inu_data__getMemPtr(outputH);
            //process blob
            UINT32* output_process_blob = (UINT32*)(output_blob_addr + STEREO_OUTPUT_CNN_BLOB_MAX_SIZE + STEREO_OUTPUT_PROCESS_BLOB);
            static UINT32 process2Index = 0;
            *output_process_blob = 0xABBA0000 + process2Index;
            process2Index++;
            if (process2Index > 0xffff)
                process2Index = 0;

            CLIENT_APIG_evSendLog(LOG_DEBUG_E, "VDSP 2 -------------------- END\n");
            //Trigger the process thread
            evIntCondSignal(&STEREO_DEMO_EVG_vdsp2Condition);
        }
    }
}

/****************************************************************************
*
*  Function Name: STEREO_DEMO_EVP_runCnn
*
*  Description: run cnn
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

static void STEREO_DEMO_EVP_runCnn
(inu_function__operateParamsT* funcOperateParamsP,
   STEREO_DEMOG_gpDspMsgProtocolT* gpDspMsgP,
   inu_cdnn_data__hdr_t* outputCnnHdr,
   UINT8* outputCnnBlob,
   INT32 channel_num,
   UINT16 jobDescriptor,
   EvIntCondType* threadCond)
{

   CLIENT_APIG_evSendLog(LOG_DEBUG_E, "runCnn %d \n", jobDescriptor);

   STEREO_DEMO_EVP_algConfig.startX = gpDspMsgP->algConfig.startX;
   STEREO_DEMO_EVP_algConfig.startY = gpDspMsgP->algConfig.startY;
   STEREO_DEMO_EVP_algConfig.width = gpDspMsgP->algConfig.width;
   STEREO_DEMO_EVP_algConfig.height = gpDspMsgP->algConfig.height;

   //UINT32 databaseListNum = funcOperateParamsP->dataInputsNum;
   //inu_dataH  outputH = funcOperateParamsP->dataInputs[databaseListNum - 1];//cnn data output
   //UINT8 *output_blob_addr = (UINT8*)inu_data__getMemPtr(outputH);

   if (STEREO_DEMO_EVP_algConfig.enableAlg == TRUE)
   {
      // Run the SSD CNN 
      {
         //inu_cdnn_data__hdr_t* outputHdrP = (inu_cdnn_data__hdr_t*)inu_data__getHdr(outputH);
         inu_cdnn_data__hdr_t* outputHdrP = (inu_cdnn_data__hdr_t*)outputCnnHdr;
         UINT8 *output_blob_addr = outputCnnBlob;
         STEREO_DEMO_injectImagesT* process_image;
         UINT32 imageWidth;
         UINT32 imageHeight;

         process_image = (STEREO_DEMO_injectImagesT*)malloc(sizeof(STEREO_DEMO_injectImagesT));
         process_image->msg_header.msg_code = STEREO_DEMO_MSG_INJECT_IMAGES;
         process_image->msg_header.msg_len = sizeof(STEREO_DEMO_injectImagesT);
         process_image->net_id = 0;
         process_image->number_of_images_in_a_set = 1;
         process_image->jobDescriptor = jobDescriptor;
         process_image->intCondSignal = threadCond;
         process_image->blob_output_address = (UINT8*)(output_blob_addr);
         process_image->blob_output_header = (UINT8*)outputHdrP;

         for (int image_num = 0; image_num < process_image->number_of_images_in_a_set; image_num++)
         {
            inu_imageH imageInH = funcOperateParamsP->dataInputs[channel_num];
            UINT32 inputImageAddr = (UINT32)inu_data__getMemPtr(imageInH);
            inu_image__hdr_t* inputImgHdrP = (inu_image__hdr_t*)inu_data__getHdr(imageInH);
            UINT32 isRGB;
            UINT32 numImages;
            UINT32 pixelSize;
            UINT32 numChannels = 1;
            STEREO_DEMO_ElementTypesE elementType = STEREO_DEMO_ELEMENT_TYPE_UINT8;

            imageWidth = inputImgHdrP->imgDescriptor.stride;
            imageHeight = inputImgHdrP->imgDescriptor.bufferHeight;

            numImages = inputImgHdrP->imgDescriptor.numInterleaveImages;
            isRGB = 1;
            pixelSize = inputImgHdrP->imgDescriptor.bitsPerPixel / 8;

            process_image->number_of_injected_sets = numImages;
            process_image->frame_id = inputImgHdrP->dataHdr.dataIndex;
            process_image->input_sets[image_num].channels = numChannels;
            process_image->input_sets[image_num].width = imageWidth;
            process_image->input_sets[image_num].height = imageHeight;
            process_image->input_sets[image_num].images_ptr = (void*)inputImageAddr;
            process_image->input_sets[image_num].channel_swap = CLIENT_APIG_netInfoGetChannelSwap(process_image->net_id);
            process_image->input_sets[image_num].resize = true;
            process_image->input_sets[image_num].element_type = elementType;
            process_image->input_sets[image_num].yuv_format = !(isRGB);
            process_image->input_sets[image_num].tensor_input = false;
            process_image->input_sets[image_num].num_of_tensor_pointers = 0;
            CLIENT_APIG_evSendLog(LOG_DEBUG_E, "CLIENT_APIG_cnnHandleMessage outputCnnBlob 0x%X outputHdrP 0x%X %d %d\n", outputCnnBlob, outputHdrP, numImages, imageWidth);
         }

         int ret = CLIENT_APIG_cnnHandleMessage((STEREO_DEMO_cnnMessageHdrT*)process_image);
         if (ret)
         {
            CLIENT_APIG_evSendLog(LOG_ERROR_E, "ev_inu_new_request: failed to add a new request for ssd\n");
         }
      }

   }
}

/****************************************************************************
*
*  Function Name: STEREO_DEMO_EVP_cnnThread
*
*  Description: cnn is performed in this thread's context
*
*  Inputs:  pointer to struct
*
*  Outputs:
*
*  Returns:
*
*  Context:
*
****************************************************************************/

static void* STEREO_DEMO_EVP_cnnThread(void* arg)
{
    STEREO_DEMO_EVP_cnnThreadParamsStructT localThreadParams;
    UINT32  localThreadParamsSize;
    INT32  status;
    CLIENT_APIG_evSendLog(LOG_INFO_E, "cnn thread started on core %d\n", evGetCpu());
    while (1)
    {
        localThreadParamsSize = sizeof(STEREO_DEMO_EVP_cnnThreadParamsStructT);
        status = CLIENT_APIG_recvMsg(&STEREO_DEMO_EVP_msgQueCnn, (UINT8*)&localThreadParams, &localThreadParamsSize, 0 /*not used*/);
        if (status == SUCCESS_E)
        {
            CLIENT_APIG_evSendLog(LOG_DEBUG_E, "STEREO_DEMO_EVP_cnnThread -------------------- %d\n", localThreadParams.jobDescriptor);
            STEREO_DEMO_EVG_cnnMutex = 0;//lock
            uint64_t start, end;
            start = getRTC();
#if 1            
            if (localThreadParams.funcOperateParamsP->dataInputsNum > 0 ) {
               int i;
               for (i = 0; i < localThreadParams.funcOperateParamsP->dataInputsNum -1; i++ ) {
                  STEREO_DEMO_EVP_runCnn(localThreadParams.funcOperateParamsP, localThreadParams.gpDspMsgP, localThreadParams.outputCnnHdr, localThreadParams.outputCnnBlob + i * STEREO_OUTPUT_CNN_BLOB, i, SCHED_NOT_FINISHED_JOB, &STEREO_DEMO_EVG_cnnCondition);
                  //Waiting for a signal from the cnn hw
                  evIntCondWait(&STEREO_DEMO_EVG_cnnCondition);
               }
            }
#else
            //run cnn on left image
            STEREO_DEMO_EVP_runCnn(localThreadParams.funcOperateParamsP, localThreadParams.gpDspMsgP, localThreadParams.outputCnnHdr, localThreadParams.outputCnnBlob, LEFT_CHANNEL, SCHED_NOT_FINISHED_JOB, &STEREO_DEMO_EVG_cnnCondition);
            //Waiting for a signal from the cnn hw
            evIntCondWait(&STEREO_DEMO_EVG_cnnCondition);
            //run cnn on right image
            STEREO_DEMO_EVP_runCnn(localThreadParams.funcOperateParamsP, localThreadParams.gpDspMsgP, localThreadParams.outputCnnHdr, localThreadParams.outputCnnBlob + STEREO_OUTPUT_CNN_BLOB, RIGHT_CHANNEL, SCHED_NOT_FINISHED_JOB, &STEREO_DEMO_EVG_cnnCondition);
            //Waiting for a signal from the cnn hw
            evIntCondWait(&STEREO_DEMO_EVG_cnnCondition);
#endif
            end = getRTC();
            CLIENT_APIG_evSendLog(LOG_DEBUG_E, "STEREO_DEMO CNN TIME %llu --------------------\n", end - start);
            CLIENT_APIG_evSendLog(LOG_DEBUG_E, "STEREO_DEMO_EVP_cnnThread -------------------- END\n");
            STEREO_DEMO_EVG_cnnCurrentBuf = 1 - STEREO_DEMO_EVG_cnnCurrentBuf;//switch output buffers
            STEREO_DEMO_EVG_cnnMutex = 1;//unlock

            //Push jobDescriptor to scheduler finished jobs que (so scheduler will update the host)
            //CLIENT_APIG_schedPushFinishedwJob(localThreadParams.jobDescriptor);
            //Trigger the scheduler thread (it will wake up and deque this finished job, and update the host)
            //evIntCondSignal(&SCHEDG_Condition);
        }
    }
}



/****************************************************************************
*
*  Function Name: BASIC_DEMO_EVP_handleGpCtrlMsgCb
*
*  Description: This routine starts processing a frame or get general mesage from the GP
*
*  Inputs: ctrlMsgOpcode - Message type
*        generalMsgParamsP - The message
*
*  Outputs:
*
*  Returns:
*
*  Context:
*
****************************************************************************/
static void STEREO_DEMO_EVP_handleGpCtrlMsgCb( CLIENT_APIG_cevaMsgParamsT *generalMsgParamsP, inu_function__operateParamsT *paramsP, UINT16 jobDescriptor )
{
   STEREO_DEMOG_gpDspMsgProtocolT *msgParamsP = (STEREO_DEMOG_gpDspMsgProtocolT *)generalMsgParamsP->msg;
   STEREO_DEMO_EVP_threadParamsStructT threadParams;
   //STEREO_DEMO_EVP_cnnThreadParamsStructT cnnThreadParams;

   CLIENT_APIG_evSendLog(LOG_DEBUG_E, "handleGpCtrlMsgCb %d \n", msgParamsP->opcode);
   
   switch(msgParamsP->opcode)
   {
      //Processing a frame
      case STEREO_DEMOG_FRAME_ALG_E:
      {
         inu_imageH imageOutH       = paramsP->dataInputs[LEFT_CHANNEL];//output is image
         inu_imageH imageInH        = paramsP->dataInputs[LEFT_CHANNEL];//image input
         UINT32 outputImageAddr     = (UINT32)inu_data__getMemPtr(imageOutH);
         UINT32 inputImageAddr      = (UINT32)inu_data__getMemPtr(imageInH);
         inu_image__hdr_t *inputImgHdrP   = (inu_image__hdr_t*)inu_data__getHdr(imageInH);;

      if (STEREO_DEMO_EVP_algConfig.enableAlg == TRUE)
      {
         threadParams.inputImageAddr = inputImageAddr;
         threadParams.inputImgDescP  = inputImgHdrP;
         threadParams.outputImageAddr    = outputImageAddr;
         threadParams.algConfigP     = &STEREO_DEMO_EVP_algConfig;
         threadParams.jobDescriptor  = jobDescriptor;
         threadParams.gpDspMsgP = msgParamsP;
         threadParams.funcOperateParamsP = paramsP;
         //Trigger process thread (by sending msg to its msg que)
         CLIENT_APIG_evSendLog(LOG_DEBUG_E, "send msg to STEREO demo thread\n");
         if (CLIENT_APIG_sendMsg(&STEREO_DEMO_EVP_msgQue, (UINT8*)&threadParams, sizeof(STEREO_DEMO_EVP_threadParamsStructT)) != SUCCESS_E)
         {
             CLIENT_APIG_evSendLog(LOG_ERROR_E, "Failed to send msg to STEREO demo process thread\n");
         }

         //cnnThreadParams.gpDspMsgP = msgParamsP;
         //cnnThreadParams.funcOperateParamsP = paramsP;
         //cnnThreadParams.outBaseAddrOffset = 0;
         //cnnThreadParams.jobDescriptor = jobDescriptor;
         ////Trigger cnn thread (by sending msg to its msg que)
         //CLIENT_APIG_evSendLog(LOG_INFO_E, "send msg to cnn thread\n");
         //if (CLIENT_APIG_sendMsg(&STEREO_DEMO_EVP_msgQueCnn, (UINT8*)&cnnThreadParams, sizeof(STEREO_DEMO_EVP_cnnThreadParamsStructT)) != SUCCESS_E)
         //{
         //    CLIENT_APIG_evSendLog(LOG_ERROR_E, "Failed to send msg to STEREO demo cnn thread\n");
         //}
      }
      else
      {
         //Push jobDescriptor to scheduler finished jobs que (so scheduler will update the host)
         CLIENT_APIG_schedPushFinishedwJob(jobDescriptor);
         //Trigger the scheduler thread (it will wake up and deque this finished job, and update the host)
         evIntCondSignal(&SCHEDG_Condition);
      }
         
      }
      break;

      //GP sent general message (enable/disable)
      case STEREO_DEMOG_START_ALG_E:
      {
         CLIENT_APIG_evSendLog(LOG_INFO_E, "START FDK ----------------\n");
         STEREO_DEMO_EVP_startAlg(msgParamsP, paramsP);
         //Push jobDescriptor to scheduler finished jobs que (so scheduler will update the host)
         CLIENT_APIG_schedPushFinishedwJob(jobDescriptor);
         //Trigger the scheduler thread (it will wake up and deque this finished job, and update the host)
         evIntCondSignal(&SCHEDG_Condition);
      }
      break;
      
      case STEREO_DEMOG_STOP_ALG_E:
      {
         //disable the algorithm
         STEREO_DEMO_EVP_algConfig.enableAlg = FALSE;
         //print some debug information
         CLIENT_APIG_evSendLog(LOG_DEBUG_E, "stop: startX=%d, startY=%d, width=%d, height=%d\n", STEREO_DEMO_EVP_algConfig.startX, STEREO_DEMO_EVP_algConfig.startY, STEREO_DEMO_EVP_algConfig.width, STEREO_DEMO_EVP_algConfig.height);
         CLIENT_APIG_deleteMsgQue(&STEREO_DEMO_EVP_msgQue);
         CLIENT_APIG_deleteMsgQue(&STEREO_DEMO_EVP_msgQueVdsp2);
         CLIENT_APIG_deleteMsgQue(&STEREO_DEMO_EVP_msgQueCnn);
         //Push jobDescriptor to scheduler finished jobs que (so scheduler will update the host)
         CLIENT_APIG_schedPushFinishedwJob(jobDescriptor);
         //Trigger the scheduler thread (it will wake up and deque this finished job, and update the host)
         evIntCondSignal(&SCHEDG_Condition);
      }
      break;

      default:
      {
         CLIENT_APIG_evSendLog(LOG_ERROR_E, "ERR!!! receive unknown control message: opcode=%d\n", msgParamsP->opcode);
      }
      break;

   }

}

/****************************************************************************
*
*  Function Name: STEREO_DEMO_EVP_initCb
*
*  Description: This routine is called only once when the demo is intialized
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
static void STEREO_DEMO_EVP_initCb()
{

   //reset the data struct for algorithm 
   STEREO_DEMO_EVP_algConfig.enableAlg   = 0;
   STEREO_DEMO_EVP_algConfig.startX      = 0;
   STEREO_DEMO_EVP_algConfig.startY      = 0;
   STEREO_DEMO_EVP_algConfig.width       = 0;
   STEREO_DEMO_EVP_algConfig.height      = 0;

   CLIENT_APIG_evSendLog(LOG_INFO_E, "EV72 initialized \n");
}

/****************************************************************************
***************                M A I N        F U N C T I O N                              ***************
****************************************************************************/

/****************************************************************************
*
*  Function Name: main
*
*  Description: This routine registers 2 callback functions, one for initialization and another to handle GP message. GP message could be
*              (1)General message to enable/disable algorithm (2)processing of a frame
*  Inputs:
*
*  Outputs:
*
*  Returns: This routine never returns
*
*  Context:
*
****************************************************************************/
int main(void)
{
   CLIENT_APIG_evInit(STEREO_DEMO_EVP_handleGpCtrlMsgCb, STEREO_DEMO_EVP_initCb);
}



