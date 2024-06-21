#include "inu2_internal.h"
#include "internal_cmd.h"
#include "err_defs.h"
#include "log.h"

#if DEFSG_IS_GP
#include "data_base.h"
#include "cmem.h"
#include "inu_time.h"
#include "mem_pool.h"
#include "cde_mngr_new.h"
#endif

/****************************************************************************
 ***************       L O C A L    T Y P E D E F S           ***************
 ****************************************************************************/
typedef struct inu_load_background__privData
{
   BOOL                ioctlFlashAck;
   MEM_POOLG_handleT   handle;
#if DEFSG_IS_GP   
   MEM_POOLG_handleT   backgroundPoolH;
   CDE_MNGRG_channnelInfoT  *channelInfo;
#endif
}inu_load_background__privData;


/****************************************************************************
***************     P R E    D E F I N I T I O N     OF      ***************
***************     L O C A L         F U N C T I O N S      ***************
****************************************************************************/
static inu_ref__VTable _vtable;
static bool _bool_vtable_initialized = 0;
static const char* name = "INU_LOAD_BACKGROUND";

#if DEFSG_IS_GP
MEM_POOLG_bufDescT *VISION_PROC_GPP_backgroundBuffers;


/****************************************************************************
*
*  Function Name: inu_load_background__allocForBackground
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: system service
*
****************************************************************************/
ERRG_codeE inu_load_background__allocForBackground(inu_load_background *me, INTERNAL_CMDG_VisionProcAllocBackgroundHdrT *hdrP)
{
   inu_load_background__privData *privP = (inu_load_background__privData*)me->privP;
   ERRG_codeE ret;

   ret = inu_ref__addMemPool((inu_ref*)me, MEM_POOLG_TYPE_ALLOC_CMEM_E, hdrP->backgroundSize + 2048, 1, NULL, 0, &privP->backgroundPoolH);
   if (ERRG_FAILED(ret))
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, "failed to alloc memory for background, size %d\n",hdrP->backgroundSize);
   }
   else
   {
      LOGG_PRINT(LOG_INFO_E, NULL, "alloc success! background, size %d\n",hdrP->backgroundSize);
   }
   return ret;
}


/****************************************************************************
*
*  Function Name: inu_load_background__handleBackground
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: system service
*
****************************************************************************/
static ERRG_codeE inu_load_background__handleBackground(inu_load_background *me, INTERNAL_CMDG_VisionProcLoadBackgroundHdrT * backgroundChunkHdrP, MEM_POOLG_bufDescT *bufDescP)
{
   ERRG_codeE                 retCode     = SYSTEM__RET_SUCCESS;
   inu_load_background__privData *privP = (inu_load_background__privData*)me->privP;
   DATA_BASEG_visionProcDataDataBaseT *visionProcDataBaseP;
   UINT32 dspTarget = 0;//TO DO slamDb.dspTarget;
   DATA_BASEG_databaseE dataBase = DATA_BASEG_VISION_PROC;
   UINT32 backgroundPhysP;
	
   if (bufDescP)
   {
      retCode = MEM_POOLG_getDataPhyAddr(bufDescP,&backgroundPhysP);
      if (ERRG_SUCCEEDED(retCode))
      {
         VISION_PROC_GPP_backgroundBuffers = bufDescP;
         
         LOGG_PRINT(LOG_INFO_E, NULL, "received background at %x\n", backgroundPhysP);

         DATA_BASEG_accessDataBase((UINT8**)&visionProcDataBaseP, dataBase);
         visionProcDataBaseP->backgroundRemovalParams.backgroundImage = backgroundPhysP;
         visionProcDataBaseP->handle = 1;
         DATA_BASEG_accessDataBaseRelease(dataBase);
         //Send receive chunk background Ack to host 
         INTERNAL_CMDG_VisionProcLoadBackgroundHdrT backgroundTblAck;
         
         backgroundTblAck.backgroundId = backgroundChunkHdrP->backgroundId;
         if (!(inu_ref__standAloneMode(me)))
         {
            retCode = inu_ref__copyAndSendDataAsync((inu_ref*)me,INTERNAL_CMDG_SEND_VISION_PROC_LOAD_BACKGROUND_E,&backgroundTblAck, NULL, 0);
         }
         else
         {
            privP->ioctlFlashAck = TRUE;
         }
      }
   }
   else
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, "error receiving background\n");
   }

   return(retCode);
}

#endif

/****************************************************************************
*
*  Function Name: inu_load_background__name
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: load_background
*
****************************************************************************/
static const char* inu_load_background__name(inu_ref *me)
{
   FIX_UNUSED_PARAM_WARN(me);
   return name;
}

/****************************************************************************
*
*  Function Name: inu_load_background__dtor
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: load_background
*
****************************************************************************/
static void inu_load_background__dtor(inu_ref *me)
{
   inu_load_background__privData *privP = (inu_load_background__privData*)((inu_load_background*)me)->privP;
   inu_ref__vtable_get()->p_dtor(me);
#if DEFSG_IS_HOST
   MEM_POOLG_closePool(privP->handle);
#else
   CDE_MNGRG_memcpyCloseChan(privP->channelInfo);
#endif
   free(privP);
}


/****************************************************************************
*
*  Function Name: inu_load_background__ctor
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: load_background
*
****************************************************************************/
ERRG_codeE inu_load_background__ctor(inu_load_background *me, inu_load_background__CtorParams *ctorParamsP)
{
   ERRG_codeE ret;
   inu_load_background__privData *privP;
   ret = inu_ref__ctor(&me->ref, &ctorParamsP->ref_params);
   if (ERRG_SUCCEEDED(ret))
   {
         privP = (inu_load_background__privData*)malloc(sizeof(inu_load_background__privData));
         if (!privP)
         {
            return INU_LOAD_BACKGROUND__ERR_OUT_OF_MEM;
         }
         memset(privP,0,sizeof(inu_load_background__privData));
         me->privP = privP;

#if DEFSG_IS_HOST
         MEM_POOLG_cfgT cfg;
         cfg.bufferSize = 512*1024*1024;
         cfg.type = MEM_POOLG_TYPE_ALLOC_USER_E;
         cfg.numBuffers = 1;
         cfg.freeCb = NULL;

         ret = MEM_POOLG_initPool(&privP->handle, &cfg);
#else
         ret = CDE_MNGRG_memcpyOpenChan(&privP->channelInfo, 0x100, NULL, NULL); // DMA copy programm for VISION_PROC reload
#ifdef CDE_DRVG_VIRTUAL_CHANNELS
         if (ERRG_FAILED(ret))
         {
            return INU_LOAD_BACKGROUND__ERR_UNEXPECTED;
         }

         ret = CDE_DRVG_assignPhysicalChannel(privP->channelInfo->dmaChannelHandle);
#endif
#endif
   }
   return ret;
}

/****************************************************************************
*
*  Function Name: inu_load_background__rxIoctl
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: load_background
*
****************************************************************************/
static int inu_load_background__rxIoctl(inu_ref *me, void *msgP, int msgCode)
{
   int ret;
   ret = inu_ref__vtable_get()->p_rxSyncCtrl(me, msgP, msgCode);

   switch (msgCode)
   {
      case(INTERNAL_CMDG_SEND_VISION_PROC_LOAD_BACKGROUND_ALLOC_E):
      {
#if DEFSG_IS_GP
         ret = inu_load_background__allocForBackground((inu_load_background*)me, (INTERNAL_CMDG_VisionProcAllocBackgroundHdrT*)msgP);
         break;
#endif
      }

      case(INTERNAL_CMDG_SEND_BACKGROUND_RELEASE_E):
      {
#if DEFSG_IS_GP
         inu_load_background           *voc = (inu_load_background*)me;
         inu_load_background__privData *privP = (inu_load_background__privData*)voc->privP;
         MEM_POOLG_free(VISION_PROC_GPP_backgroundBuffers);
         ret = inu_ref__removeMemPool((inu_ref*)me, privP->backgroundPoolH);                  
#endif
         break;
      }
      default:
      break;
   }
   return ret;
}

/****************************************************************************
*
*  Function Name: inu_load_background__rxData
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: load_background
*
****************************************************************************/
static int inu_load_background__rxData(inu_ref *ref, UINT32 msgCode, UINT8 *msgP, UINT8 *dataP, UINT32 dataLen, void **bufDescP)
{
   inu_load_background           *me = (inu_load_background*)ref;
#if DEFSG_IS_HOST
   inu_load_background__privData *privP = (inu_load_background__privData*)me->privP;
#endif
   int ret = 0;

   ret = inu_ref__vtable_get()->p_rxAsyncData(ref, msgCode, msgP, dataP, dataLen, bufDescP);
   switch (msgCode)
   {
      case(INTERNAL_CMDG_SEND_VISION_PROC_LOAD_BACKGROUND_E):
      {
#if DEFSG_IS_GP
         inu_load_background__handleBackground(me, (INTERNAL_CMDG_VisionProcLoadBackgroundHdrT*)msgP, *bufDescP);
#else
         privP->ioctlFlashAck = TRUE;
#endif
         break;
      }
   }

   return ret;
}
      
/****************************************************************************
*
*  Function Name: inu_load_background__vtable_init
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: load_background
*
****************************************************************************/
void inu_load_background__vtable_init()
{
   if (!_bool_vtable_initialized) 
   {
      inu_ref__vtableInitDefaults(&_vtable);
      _vtable.p_name = inu_load_background__name;
      _vtable.p_dtor = inu_load_background__dtor;
      _vtable.p_ctor = (inu_ref__Ctor*)inu_load_background__ctor;

      _vtable.p_rxSyncCtrl  = inu_load_background__rxIoctl;
      _vtable.p_rxAsyncData = inu_load_background__rxData;

      _bool_vtable_initialized = true;
   }
}

/****************************************************************************
*
*  Function Name: inu_load_background__vtable_get
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: load_background
*
****************************************************************************/
const inu_ref__VTable *inu_load_background__vtable_get(void)
{
   inu_load_background__vtable_init();
   return &_vtable;
}
      
/****************************************************************************
*
*  Function Name: inu_load_background__new
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: load_background
*
****************************************************************************/
ERRG_codeE inu_load_background__new(inu_load_backgroundH *meH, inu_load_background__initParams *initParamsP)
{
   inu_load_background__CtorParams ctorParams;
   ctorParams.ref_params.device  = initParamsP->deviceH;
   strcpy(ctorParams.ref_params.userName, "inu_Load_Background"); 
   return inu_factory__new((inu_ref**)meH, &ctorParams, INU_LOAD_BACKGROUND_REF_TYPE);
}

/****************************************************************************
*
*  Function Name: inu_load_background__delete
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: load_background
*
****************************************************************************/
void inu_load_background__delete(inu_load_backgroundH meH)
{
   inu_factory__delete((inu_ref*)meH, 0);
}
      
      
/****************************************************************************
*
*  Function Name: inu_load_background__loadBackground
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: system service
*
****************************************************************************/
ERRG_codeE inu_load_background__loadBackground(inu_load_backgroundH meH, inu_load_background_memory_t *memP, inu_load_background_hdr_t *backgroundHdrP)
{
   ERRG_codeE ret = INU_LOAD_BACKGROUND__RET_SUCCESS;
   INTERNAL_CMDG_VisionProcLoadBackgroundHdrT backgroundChunkHdr;
   enum { SLEEP_USEC_E = (1 * 1000) };
   UINT32  startSec, deltaMsec = 0;
   UINT16  startMsec;
   UINT32  timeoutMsec = SLEEP_USEC_E * 200;
   UINT64 usec1,usec2;
   inu_load_background__privData *privP = (inu_load_background__privData*)((inu_load_background*)meH)->privP;
   MEM_POOLG_bufDescT *bufDescP;
   INTERNAL_CMDG_VisionProcAllocBackgroundHdrT allocHdr;
   FILE *file;

   OS_LYRG_getUsecTime(&usec1);

   allocHdr.backgroundId = backgroundHdrP->backgroundId;
   allocHdr.backgroundSize = memP->bufsize;
   ret = inu_ref__sendCtrlSync((inu_ref*)meH,INTERNAL_CMDG_SEND_VISION_PROC_LOAD_BACKGROUND_ALLOC_E,&allocHdr, INU_REF__SYNC_DFLT_TIMEOUT_MSEC);
   if (ERRG_SUCCEEDED(ret))
   {
      if (!(inu_ref__standAloneMode(meH)))
      {
         ret = MEM_POOLG_alloc(privP->handle,memP->bufsize,&bufDescP);
      }
      else
      {
         ret = inu_ref__allocBuf((inu_ref*)meH, memP->bufsize, &bufDescP);
      }
      if (ERRG_SUCCEEDED(ret))
      {
         privP->ioctlFlashAck = FALSE;

         if (!(inu_ref__standAloneMode(meH)))
         {
            bufDescP->dataP   = (UINT8*)memP->bufP;
            bufDescP->startP  = (UINT8*)memP->bufP;
            bufDescP->size    = memP->bufsize;
            bufDescP->dataLen = memP->bufsize;
         }
         else
         {
            file = fopen(memP->path, "rb");  // r for read, b for binary
            if (file)
            {
               fread(bufDescP->dataP, memP->bufsize, 1, file);
               fclose(file);
               bufDescP->dataLen = memP->bufsize;
               bufDescP->startP = bufDescP->dataP;
               bufDescP->size    = memP->bufsize;
#if DEFSG_IS_GP
               CMEM_cacheWb(bufDescP->dataP, memP->bufsize);
#endif

            }
            else
            {
               printf("ERROR fopen");
            }
            
         }

#if DEFSG_IS_GP
         inu_load_background__handleBackground(meH, &backgroundChunkHdr, bufDescP);
#else
         ret = inu_ref__sendDataAsync((inu_ref*)meH, INTERNAL_CMDG_SEND_VISION_PROC_LOAD_BACKGROUND_E, &backgroundChunkHdr, bufDescP);
#endif

         //wait for ACK
         OS_LYRG_getTime(&startSec, &startMsec);
         do
         {
            if (privP->ioctlFlashAck == TRUE)
            {
               privP->ioctlFlashAck = FALSE;
               break;
            }
            OS_LYRG_usleep(SLEEP_USEC_E);
            deltaMsec = OS_LYRG_deltaMsec(startSec, startMsec);
         } while (deltaMsec <= timeoutMsec);
      }
   }

   OS_LYRG_getUsecTime(&usec2);
   LOGG_PRINT(LOG_INFO_E, NULL, "Finished sending background. size = %d, total time = %llu\n",memP->bufsize,usec2-usec1);

   return ret;
}

/****************************************************************************
*
*  Function Name: inu_load_background__release
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: load_background
*
****************************************************************************/
ERRG_codeE inu_load_background__release(inu_load_backgroundH meH)
{
   INTERNAL_CMDG_BackgroundReleaseHdrT msg;
   
   return inu_ref__sendCtrlSync((inu_ref*)meH, INTERNAL_CMDG_SEND_BACKGROUND_RELEASE_E, &msg, INU_REF__SYNC_DFLT_TIMEOUT_MSEC);
   
}




