/****************************************************************************
 *
 *   FileName: basic_demo_gp.c
 *
 *   Author: Ola Boaron
 *
 *   Date: 
 *
 *   Description: Receive frame from DDR and process 
 *
 ****************************************************************************/

/****************************************************************************
 ***************               I N C L U D E   F I L E S        *************
 ****************************************************************************/ 

#include "inu2.h"
#include <assert.h>
#include <unistd.h>
#include <string.h>
#include "basic_demo.h"
#include <sys/time.h>

/****************************************************************************
 ***************         L O C A L       D E F N I T I O N S  ***************
 ****************************************************************************/
#define BASIC_DEMO_NODE_ID               1
#define BASIC_DEMO_DDR_SIZE              0

/****************************************************************************
 ***************            L O C A L    T Y P E D E F S      ***************
 ****************************************************************************/



/****************************************************************************
 ***************       L O C A L         D A T A              ***************
 ****************************************************************************/
static   UINT32                           BASIC_DEMO_ddrPhysAddr;
static   UINT32                           BASIC_DEMO_ddrVirtAddr;
static   BASIC_DEMO_hostGpMsgStructT      BASIC_DEMO_hostMsg;
static   BASIC_DEMO_gpHostMsgStructT      BASIC_DEMO_msgStruct;
static   UINT32                           BASIC_DEMO_counter = 0;
static   UINT32                           BASIC_DEMO_gpProcessTime = 0;

/****************************************************************************
 ***************      L O C A L       F U N C T I O N S       ***************
 ****************************************************************************/


/****************************************************************************
*
*  Function Name: BASIC_DEMO_freeData
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
static void BASIC_DEMO_freeData(inu_fdkH meH, void *bufP)
{
   inu_fdk__gp_log(LOG_DEBUG_E, NULL, "bufP %p is now free\n", bufP);
}


/****************************************************************************
*
*  Function Name: BASIC_DEMO_createRawData
*
*  Description: This function allocates new data instance of the type "RAW_DATA". It will be used
*           for holding the data that is sent to host, and contains clock and counter
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
static ERRG_codeE BASIC_DEMO_createRawData(inu_fdkH meH, inu_dataH *clone )
{
   inu_nodeH  currentH = NULL;
   ERRG_codeE ret;
   BASIC_DEMO_gpHostMsgStructT *msgP;
   inu_data__hdr_t *hdrP;

   //search for the data node we created for the new data.
   currentH = inu_node__getNextOutputNode(meH,currentH);
   while (currentH)
   {
      if (inu_ref__instanceOf(currentH, INU_RAW_DATA_REF_TYPE))
      {
         break;
      }
      currentH = inu_node__getNextOutputNode(meH,currentH);
   }
   if (!currentH)
      assert(0); //error building the graph
      
   hdrP = inu_data__getHdr((inu_dataH)currentH);

   //request data obj. use it to get access to the actual memory
   ret = inu_fdk__newUserData(meH,(inu_dataH)currentH,sizeof(BASIC_DEMO_msgStruct),clone,hdrP);
   if (ERRG_FAILED(ret))
   {
      inu_fdk__gp_log(LOG_ERROR_E, ret, "failed to allocate new data\n");
   }
   return ret;
}

/****************************************************************************
*
*  Function Name: BASIC_DEMO_release_data
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
static ERRG_codeE BASIC_DEMO_release_data(inu_fdkH meH, inu_function__operateParamsT *paramsP)
{
   ERRG_codeE ret;
   UINT32      i;

   //notify the graph system that the data is ready for dispatching
   for (i = 0; i < paramsP->dataInputsNum; i++)
   {
      ret = inu_fdk__doneData(meH, paramsP->dataInputs[i]);
      if (ERRG_FAILED(ret))
      {
       inu_fdk__gp_log(LOG_WARN_E, NULL, "failed to dispatch data to graph\n");
      }
   }

   ret = inu_fdk__complete(meH);
   if (ERRG_FAILED(ret))
   {
      inu_fdk__gp_log(LOG_WARN_E, NULL, "failed to inform function is complete to graph\n");
   }
   return ret;
}


/****************************************************************************
*
*  Function Name: BASIC_DEMO_finish_operate
*
*  Description: release data
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
static void BASIC_DEMO_finish_operate(inu_fdkH meH, inu_function__operateParamsT *paramsP)
{
   BASIC_DEMO_gpHostMsgStructT *basicDemoMsgP;

   //use API to access memory
   basicDemoMsgP = (BASIC_DEMO_gpHostMsgStructT*)inu_data__getMemPtr(paramsP->dataInputs[DATA_RAW_OUTPUT]);
   if (basicDemoMsgP)
   {
       //basicDemoMsgP is sent to host as FDK_RAW_data
     inu_fdk__gpCacheInv(basicDemoMsgP, sizeof(BASIC_DEMO_gpHostMsgStructT));
     basicDemoMsgP->counter = BASIC_DEMO_counter++;
     basicDemoMsgP->gpUsec  = BASIC_DEMO_gpProcessTime; 
     inu_fdk__gpCacheWr(basicDemoMsgP, sizeof(BASIC_DEMO_gpHostMsgStructT));      
   }
   BASIC_DEMO_release_data(meH, paramsP);
}

/****************************************************************************
*
*  Function Name: BASIC_DEMO_getUsecTime
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
void BASIC_DEMO_getUsecTime(UINT64 *usecP)
{
   struct timeval time;
   if (gettimeofday(&time, NULL) == -1)
   {
      inu_fdk__gp_log(LOG_ERROR_E, NULL,  "gettimeofday failed\n");
   }  
   *usecP = (time.tv_sec*1000000) + time.tv_usec;
}

/****************************************************************************
*
*  Function Name: BASIC_DEMO_gpColorFrame16bits
*
*  Description:   This routine is the algorithm running in the GP, it gets a ROI (region of interest) and process its pixels
*              This routine changes the pixels values. every frame it increses the value of the chroma component in 10
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
static void BASIC_DEMO_gpColorFrame16bits(UINT16 *ptr, UINT32 bufWidth, UINT32 bufHeight, UINT32 startX, UINT32 startY, UINT32 picWidth, UINT32 picHeight)
{
   static UINT8   pixelColor = 0x1;
   UINT32         i, j;
   UINT16         *pixelPtr, *linePtr = (ptr + (bufWidth*startY) + startX);

   if((bufWidth >= startX + picWidth) && (bufHeight >= startY + picHeight))
   {
      for(i=0; i < picHeight; i++)
      {
         inu_fdk__gpCacheInv((void *)linePtr, picWidth*3);
         memset(linePtr,pixelColor,picWidth*3);
         inu_fdk__gpCacheWr((void *)linePtr, picWidth*3);
         linePtr += bufWidth*3;
      }
     
      pixelColor = (pixelColor + 10) & 0xFF;
     
   }
   else
   {
      inu_fdk__gp_log(LOG_ERROR_E, NULL, "wrong configuration params!!!, bufWidth=%d, bufHeight=%d, startX=%d, startY=%d, picWidth=%d, picHeight=%d\n", bufWidth, bufHeight, startX, startY, picWidth, picHeight);
   }

}


/****************************************************************************
*
*  Function Name: BASIC_DEMO_operate
*
*  Description: this routine trigers the GP and CEVA to process a frame
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
static void BASIC_DEMO_operate(inu_fdkH meH, inu_function__operateParamsT *paramsP)
{
   int i = 0;
   inu_image__hdr_t *imgHdrP;
   inu_dataH         newRawDataH, RGBDataH;
   void             *dataP;
   ERRG_codeE        ret;
   UINT64          startProcess, endProcess;

   BASIC_DEMO_gpHostMsgStructT *msgP;
   
   
   if (BASIC_DEMO_hostMsg.algInfo.enable)
   {
      // update of the data array [DATA_RGB_INPUT][DATA_RAW_OUTPUT]
      // this data is from type RAW, will be used for transfer of any data to host
      ret = BASIC_DEMO_createRawData(meH, &newRawDataH);
      
      if (ERRG_SUCCEEDED(ret))
      {
         //Add new data to the operate params
         paramsP->dataInputs[DATA_RAW_OUTPUT] = newRawDataH;
         paramsP->dataInputsNum++;
      }
      
      // Access RGB input data
      RGBDataH = paramsP->dataInputs[DATA_RGB_INPUT];
      imgHdrP = (inu_image__hdr_t *)inu_data__getHdr(RGBDataH);
 
      dataP = inu_data__getMemPtr(RGBDataH);

      BASIC_DEMO_getUsecTime(&startProcess);
      BASIC_DEMO_gpColorFrame16bits((UINT16 *)dataP, imgHdrP->imgDescriptor.width, imgHdrP->imgDescriptor.height, BASIC_DEMO_hostMsg.algInfo.startX ,BASIC_DEMO_hostMsg.algInfo.startY, BASIC_DEMO_hostMsg.algInfo.picWidth, BASIC_DEMO_hostMsg.algInfo.picHeight);
      BASIC_DEMO_getUsecTime(&endProcess);

      BASIC_DEMO_gpProcessTime = (UINT32)(endProcess - startProcess);     

      inu_fdk__gp_log(LOG_INFO_E, NULL, "OPERATE index  %d\n", imgHdrP->dataHdr.dataIndex);

      inu_fdk__gp_log(LOG_DEBUG_E, NULL, "%p start color: bufWidth=%d, bufHeight=%d (startX=%d startY=%d  width=%d  height=%d)\n", meH, 
                                             imgHdrP->imgDescriptor.width, imgHdrP->imgDescriptor.height, BASIC_DEMO_hostMsg.algInfo.startX , 
                                             BASIC_DEMO_hostMsg.algInfo.startY,BASIC_DEMO_hostMsg.algInfo.picWidth,BASIC_DEMO_hostMsg.algInfo.picHeight);
      // Finish the operation on current frame
      BASIC_DEMO_finish_operate(meH, paramsP);
   }
   else
   {
      //No free memory for new data, release everything
      BASIC_DEMO_release_data(meH, paramsP);
   }      
}


/****************************************************************************
*
*  Function Name: BASIC_DEMO_handleHostCtrlMsgCb
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
static void BASIC_DEMO_handleHostCtrlMsgCb(inu_fdkH meH, unsigned char *bufP, unsigned int len)
{
   //Copy host message 
   memcpy (&BASIC_DEMO_hostMsg, bufP, len);

   inu_fdk__gp_log(LOG_INFO_E, NULL, "Handle Ctrl msg\n");
}

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
   
   nodeCbsRegister.nodeId              = BASIC_DEMO_NODE_ID;
   nodeCbsRegister.flags               = 0;//INU_FDK__AUTO_CACHE_INV_FLAG;//(INU_FDK__AUTO_DONE_DATA_FLAG) | (INU_FDK__AUTO_OPERATE_DONE_FLAG);
   nodeCbsRegister.operateCb           = BASIC_DEMO_operate; //function that will be called when all input data is ready 
   nodeCbsRegister.rxDataCb            = BASIC_DEMO_handleHostCtrlMsgCb; // will be called when Host send message
   nodeCbsRegister.userDataFreeCb      = BASIC_DEMO_freeData;        //  will be called when data is no longer in use

   errCode = inu_fdk__initNodeCbFunctions(nodeCbsRegister);

   // This routine initiates the ARM and requests a buffer on DDR during run time. it gets its physical and virtual address
   errCode = inu_fdk__gpInit( wdTimeout, BASIC_DEMO_DDR_SIZE, &BASIC_DEMO_ddrPhysAddr, &BASIC_DEMO_ddrVirtAddr,INU_DEVICE__NORMAL_MODE, INU_DEVICE_INTERFACE_0_USB,&mydevice);

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




