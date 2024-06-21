/****************************************************************************
 *
 *   FileName: PP_ev.c
 *
 *   Author:  
 *
 *   Date: 
 *
 *   Description: 
 *   
 ****************************************************************************/
#include "inu2_internal.h"
#include <stdio.h>
#include "data_base.h"
#include "err_defs.h"
#include "log.h"
#include "math.h"
#include "client_api.h"
#include "sched.h"
#include "ictl_drv.h"

#include "ev_inu_voxel_grid_filtering.h"
#include "ev_ocl_vgf_interface.h"
#include <cnn_dev.h>


/****************************************************************************
***************     L O C A L        D E F I N I T I O N S    ***************
****************************************************************************/

typedef enum
{
   PP_EV_OPERATE,
   PP_EV_START,
   PP_EV_STOP,
}PP_appStateE;

typedef struct
{
   UINT16          jobDescriptor;
   unsigned short  * dataIn;   // pointer to the input depth image
   short       * dataOut;  // pointer for the point cloud outputs, each point is x y and z
   inu_point_cloud_data__hdr_t *dataHdr; // pointer for the point cloud data header
   unsigned int    * overlapping_rows_buffer;  // pointer for the buffer that holds the voxel indexes of the overlapping rows between w strips
   int            input_width;    // input depth image width  
   int            input_height;   // input depth image height 
   int            vgf_flag;      // 0- for pcl output, 1-for pcl_output with VGF
}  PP_EVP_threadParamsStructT;


/****************************************************************************
***************                     D A T A                  ***************
****************************************************************************/
DATA_BASEG_ppDataDataBaseT PP_EVG_msgParamsP;   // 

float ppBaseline;
float ppFx;
float ppFy;
float ppCxL;
float ppCxR;
float ppCy;
int ppMaxDepthMm;

static vgf_init_params_t      vgf_init_params;
static vgf_dynamic_params_t      vgf_dynamic_params;


int evInuVoxelGridFiltering_init(vgf_init_params_t *vgf_init_p);
int evInuVoxelGridFiltering(vgf_dynamic_params_t *vgf_info);

static CLIENT_APIG_msgQueT PP_EVP_msgQue;

/****************************************************************************
***************      L O C A L         D E C L A R A T I O N S  *************
****************************************************************************/
int PP_frameIdx = 0;

/****************************************************************************
**************   L O C A L      F U N C T I O N S                                        ***************
****************************************************************************/

/****************************************************************************
*
*  Function Name: PP_EVP_init
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
void PP_EVP_init()
{
}

/****************************************************************************
*
*  Function Name: PP_EVP_processThread
*
*  Description: PP frame processing is performed in this thread's context
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

static void * PP_EVP_processThread(void *arg)
{
    PP_EVP_threadParamsStructT localThreadParams;
    UINT32  localThreadParamsSize;
    INT32  status;
    UINT64 tick, tock;
   
    LOGG_PRINT(LOG_INFO_E, (ERRG_codeE)NULL, "PP thread started on core %d\n", evGetCpu());
   
   
    while(1)
    {
      localThreadParamsSize = sizeof(PP_EVP_threadParamsStructT);
      status = CLIENT_APIG_recvMsg(&PP_EVP_msgQue,  (UINT8*)&localThreadParams, &localThreadParamsSize, 0 /*not used*/);   
      if(status == SUCCESS_E) 
      {
         tick = getRTC();
         //--- VoxelGridFiltering inputs -----------------------
         vgf_dynamic_params.dataIn  = localThreadParams.dataIn;   // pointer to the input depth image
         vgf_dynamic_params.dataOut = localThreadParams.dataOut;     // pointer for the point cloud outputs, each point is x y and z
         vgf_dynamic_params.overlapping_rows_buffer = localThreadParams.overlapping_rows_buffer;         // pointer for the buffer that holds the voxel indexes of the overlapping rows between w strips
         vgf_dynamic_params.input_width   = localThreadParams.input_width;             // input depth image width 
         vgf_dynamic_params.input_height = localThreadParams.input_height;             // input depth image height 
         vgf_dynamic_params.vgf_flag   = localThreadParams.vgf_flag;
         
      
         // --- call VoxelGridFiltering function -----------
         localThreadParams.dataHdr->pointCloudListSize = evInuVoxelGridFiltering(&vgf_dynamic_params)*6;
         evDataCacheFlushLines(localThreadParams.dataHdr, localThreadParams.dataHdr + 1); 
               
         tock = getRTC();
         LOGG_PRINT(LOG_INFO_E,(ERRG_codeE)NULL,"evInuVoxelGridFiltering,  num_outputs=%d,process frame cycles: %llu dataIndex %d\n",localThreadParams.dataHdr->pointCloudListSize/6,(tock - tick), localThreadParams.dataHdr->dataHdr.dataIndex);
            
         //Push jobDescriptor to scheduler finished jobs que 
         SCHEDG_pushFinishedJob(localThreadParams.jobDescriptor);
         //Trigger the scheduler thread (it will wake up and deque this finished job)
         evIntCondSignal(&SCHEDG_Condition);
      }
      else 
      {
         LOGG_PRINT(LOG_INFO_E,(ERRG_codeE)NULL,"VGF read fail\n");
      }

    }
}




/****************************************************************************
***************                M A I N        F U N C T I O N                              ***************
****************************************************************************/

/****************************************************************************
*
*  Function Name: PP_EVG_app
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
void PP_EVG_app(void *jobDescriptorP, UINT16 jobDescriptor, inu_function__operateParamsT *paramsP)
{
   static int isInit = 0;
   static DATA_BASEG_ppDataDataBaseT *msgParamsP = &PP_EVG_msgParamsP;
    DATA_BASEG_jobDescriptorT *jobDb = (DATA_BASEG_jobDescriptorT*)jobDescriptorP;
    UINT64 tick, tock;
    EvThreadType ThreadType;
    PP_EVP_threadParamsStructT threadParams;

   switch (jobDb->data)
   {
      case PP_EV_OPERATE:
      {        
         inu_image *image = (inu_image *)jobDb->inputList.inu_dataPhy[0];
         INT32 inWidth = image->imageHdr.imgDescriptor.width * image->imageHdr.imgDescriptor.numInterleaveImages;       
         INT32 inStride = image->imageHdr.imgDescriptor.stride;         
         INT32 inHeight = image->imageHdr.imgDescriptor.height;
         UINT8 *imageInP = (UINT8 *)(image->data.dataPhyAddr);
         
         INT32 outputNum = jobDb->inputList.databaseListNum;
         
         inu_image *imageOut = (inu_image *)jobDb->inputList.inu_dataPhy[1];
         INT32 outWidth = imageOut->imageHdr.imgDescriptor.width * image->imageHdr.imgDescriptor.numInterleaveImages;         
         INT32 outStride = imageOut->imageHdr.imgDescriptor.stride;        
         INT32 outHeight = imageOut->imageHdr.imgDescriptor.height;
         UINT8 *imageOutP = (UINT8 *)(imageOut->data.dataPhyAddr);

         

         //LOGG_PRINT(LOG_INFO_E, NULL, "PP_EV input image w, h, stride = %d %d %d\n",inWidth, inHeight, inStride);
         //LOGG_PRINT(LOG_INFO_E, NULL, "PP_EV output image w, h, stride = %d %d %d\n",outWidth, outHeight, outStride);
         
         if (msgParamsP->actionMap & (1<<INU_PP_DEPTH2PLY))
         {
            inu_point_cloud_data    *dataP = (inu_point_cloud_data *)jobDb->inputList.inu_dataPhy[1];
            ///////////////////
            threadParams.jobDescriptor = jobDescriptor;
            threadParams.dataIn  = (unsigned short *)(image->data.dataPhyAddr);     // pointer to the input depth image
            threadParams.dataOut = (short *)(dataP->data.dataPhyAddr);        // pointer for the point cloud outputs, each point is x y and z
            threadParams.dataHdr = &(dataP->point_cloud_dataHdr); // pointer for the point cloud data header
            threadParams.overlapping_rows_buffer = (UINT32 *)(msgParamsP->ddrP);       // pointer for the buffer that holds the voxel indexes of the overlapping rows between w strips
            threadParams.input_width  = inWidth;            // input depth image width 
            threadParams.input_height = inHeight;              // input depth image height   
            //if (msgParamsP->actionMap &(1<<INU_PP_VGF))
            //   threadParams.vgf_flag     = 1;
            //else
            //   threadParams.vgf_flag     = 0;
            threadParams.vgf_flag = msgParamsP->vgf_flag;
            //Trigger thread (by sending msg to its msg que)
            //evIntCondSignal(&VGF_PPG_condition); // Trigger the post processing thread
            if(CLIENT_APIG_sendMsg(&PP_EVP_msgQue,(UINT8 *)&threadParams, sizeof(PP_EVP_threadParamsStructT)) != SUCCESS_E)
            {
               LOGG_PRINT(LOG_ERROR_E,(ERRG_codeE)NULL, "Failed to send msg to PP thread\n");   
               SCHEDG_pushFinishedJob(jobDescriptor);
               evIntCondSignal(&SCHEDG_Condition);
            }
            else 
            {
               LOGG_PRINT(LOG_DEBUG_E,(ERRG_codeE)NULL, "vgf success\n");        
            }
            
         }
         else
         {
            
            SCHEDG_pushFinishedJob(jobDescriptor);
            evIntCondSignal(&SCHEDG_Condition);
         }        
         
         PP_frameIdx ++;

         break;
      }
      case PP_EV_START: 
      {
      
         DATA_BASEG_readDataBase((UINT8*)msgParamsP, DATA_BASEG_PP, 0);
         PP_EVP_init();

         LOGG_PRINT(LOG_INFO_E, (ERRG_codeE)NULL,"VGF init start\n");               
         ppBaseline = msgParamsP->baseline / 1000.0;
         ppFx  = msgParamsP->fx;
         ppFy  = msgParamsP->fy;
         ppCxL = msgParamsP->cxL;
         ppCxR = msgParamsP->cxR;
         ppCy  = msgParamsP->cy;
         ppMaxDepthMm = msgParamsP->maxDepthMm;
         LOGG_PRINT(LOG_INFO_E, (ERRG_codeE)NULL,"PP Start: action map %d \n", msgParamsP->actionMap);
         LOGG_PRINT(LOG_INFO_E, (ERRG_codeE)NULL,"baseline %f fx %f fy %f cxL %f cxR %f cy %f \n", ppBaseline, ppFx, ppFy, ppCxL, ppCxR, ppCy);
         LOGG_PRINT(LOG_INFO_E, (ERRG_codeE)NULL,"maxDepthMm %d\n", ppMaxDepthMm);
         
         vgf_init_params.voxel_leaf_x_size = msgParamsP->voxel_leaf_x_size;//20;
         vgf_init_params.voxel_leaf_y_size = msgParamsP->voxel_leaf_y_size;//20;
         vgf_init_params.voxel_leaf_z_size = msgParamsP->voxel_leaf_z_size;//20;
               
         vgf_init_params.focal_point_x  =  ppFx;      //389.196869;//500.14325;  // msgParamsP->fx;  // 500.14325;
         vgf_init_params.focal_point_y  =  ppFy;      //389.196869;//500.14325;  // msgParamsP->fy;  // 500.14325;
         vgf_init_params.center_point_x =  ppCxL;  //640.00000; //317.739441; // msgParamsP->cxL; // 317.739441;
         vgf_init_params.center_point_y =  ppCy;      //400.000000;//199.270355; // msgParamsP->cy;  // 199.270355;
         vgf_init_params.MaxDepthMm     =  (short)ppMaxDepthMm; 
         evInuVoxelGridFiltering_init(&vgf_init_params);
         LOGG_PRINT(LOG_INFO_E, (ERRG_codeE)NULL,"Finished VGF init\n");      

            
         PP_EVP_msgQue.maxMsgs = CLIENT_APIG_QUE_MAX_ELEMENTS;
         PP_EVP_msgQue.msgSize = sizeof(PP_EVP_threadParamsStructT);
         if(CLIENT_APIG_createMsgQue(&PP_EVP_msgQue, 0) != SUCCESS_E)
         {
            LOGG_PRINT(LOG_ERROR_E, (ERRG_codeE)NULL,"PP msg queue create failed\n");
         }
         
            
         //Create a thread on core 0 
         ThreadType = evNewThread(PP_EVP_processThread, EVTH_INIT_CPU, VGF_THREAD_CPU, EVTH_INIT_ARGPTR, NULL, EVTH_INIT_PRIORITY, VGF_THREAD_PRIORITY, EVTH_INIT_PREEMPTIBLE, 1, EVTH_INIT_LAST);

         if(ThreadType == NULL)
         {
            LOGG_PRINT(LOG_ERROR_E, (ERRG_codeE)NULL,"Failed to open a new thread\n");
         }

         
         SCHEDG_pushFinishedJob(jobDescriptor);
         evIntCondSignal(&SCHEDG_Condition);

         break;
      }
      case PP_EV_STOP:
      {
         LOGG_PRINT(LOG_INFO_E, (ERRG_codeE)NULL,"PP Stop\n"); 

         CLIENT_APIG_deleteMsgQue(&PP_EVP_msgQue);

         SCHEDG_pushFinishedJob(jobDescriptor);
         evIntCondSignal(&SCHEDG_Condition);

         break;
      }
      default:
      {
         LOGG_PRINT(LOG_ERROR_E, (ERRG_codeE)NULL, NULL, "PP error : Job data not valid\n");
         
         break;
      }
   }  
}




