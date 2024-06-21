#include "inu2_internal.h"
#include "internal_cmd.h"
#include "err_defs.h"
#include "assert.h"
#include "log.h"

#if DEFSG_IS_GP
#include "data_base.h"
#include "cmem.h"
#include "inu_time.h"
#include "mem_pool.h"
#include "cde_mngr_new.h"
#include "inu_storage_layout.h"
#include "spi_flash_drv.h"
#include "spi_flash.h"
#include "io_pal.h"
#endif

/****************************************************************************
 ***************       L O C A L    T Y P E D E F S           ***************
 ****************************************************************************/
// cdnn params
#define SYSTEMP_CDNN_CHUNK_DATA_LEN       (65536)
#define SYSTEMP_CDNN_CHUNK_SIZE_FACTOR    (16)

#ifdef __UART_ON_FPGA__
#define INU_FPGA_BIN_ADDR                 (0x8c400000)
#define INU_FPGA_BIN_SIZE                 (1938920) //1939432
#endif

typedef struct inu_load_network__privData
{
   BOOL                    ioctlFlashAck;
   MEM_POOLG_handleT       handle;
#if DEFSG_IS_GP
   MEM_POOLG_handleT       networkPoolH[CDNN_MAX_NETWORKS];
   CDE_MNGRG_channnelInfoT *channelInfo;
#endif
}inu_load_network__privData;


/****************************************************************************
***************     P R E    D E F I N I T I O N     OF      ***************
***************     L O C A L         F U N C T I O N S      ***************
****************************************************************************/
static inu_ref__VTable _vtable;
static bool _bool_vtable_initialized = 0;
static const char* name = "INU_LOAD_NETWORK";

#if DEFSG_IS_GP
static CDNN_GPP_networksT CDNN_GPP_networks[CDNN_MAX_NETWORKS] = {{0}};
MEM_POOLG_bufDescT *CDNN_GPP_networksBuffers[CDNN_MAX_NETWORKS];
UINT32 inu_load_network_cdnnWorkSpacePhyAddress;
UINT32 inu_load_network_cdnnWorkSpaceVirtAddress;

UINT32 inu_load_network__get_isNetworkLoaded(inu_ref *me, UINT32 netId)
{
   if (netId >= CDNN_MAX_NETWORKS)
   {
      INU_REF__LOGG_PRINT(me,LOG_ERROR_E,NULL,"network id %d is out of boundaries [%d-%d]\n", netId, INU_LOAD_NETWORK__CDNN_0_E, INU_LOAD_NETWORK__IAE_TOTAL_CDNN_E);
      return 0;
   }
   return CDNN_GPP_networks[netId].isNetworkLoaded;
}

/****************************************************************************
*
*  Function Name: inu_load_network__allocForNetwork
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
ERRG_codeE inu_load_network__allocForNetwork(inu_load_network *me, INTERNAL_CMDG_CdnnAllocNetworkHdrT *hdrP)
{
   inu_load_network__privData *privP = (inu_load_network__privData*)me->privP;
   ERRG_codeE ret;

   ret = inu_ref__addMemPool((inu_ref*)me, MEM_POOLG_TYPE_ALLOC_CMEM_E, hdrP->networkSize + 2048, 1, NULL, 0, &privP->networkPoolH[hdrP->networkId]);
   if (ERRG_FAILED(ret))
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, "failed to alloc memory for network %d, size %d\n",hdrP->networkId,hdrP->networkSize);
   }
   else
   {
      LOGG_PRINT(LOG_INFO_E, NULL, "alloc success! network %d, size %d\n",hdrP->networkId,hdrP->networkSize);
   }
   return ret;
}

/****************************************************************************
*
*  Function Name: inu_load_network__searchInFlash
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
static ERRG_codeE inu_load_network__searchInFlash(UINT32 networkId, UINT32 *flashAddress, UINT32 *networkSize)
{
   SPI_FLASH_DRVG_flashParametersT  flashParameters;
   ERRG_codeE                       ret = INU_LOAD_NETWORK__RET_SUCCESS;
   InuSectionHeaderT                CnnDataSectionHeader;
   InuCnnDataHeaderT                CnnDataHeader;
   IO_PALG_deviceIdE                deviceId = IO_SPI_FLASH_1_E;
   UINT8                            flashNum = MAIN_FLASH;
   UINT32                           address  = SPI_FLASHG_getBlockSize(flashNum) * SECTION_CNN_DATA_START_BLOCK;

   //read cnn data section header
   flashParameters.flashRead.startFlashAddress  = address;
   flashParameters.flashRead.readFlashSize      = sizeof(InuSectionHeaderT);
   flashParameters.flashRead.flashBufP          = (UINT8*)&CnnDataSectionHeader;
   ret = IO_PALG_ioctl(IO_PALG_getHandle(deviceId), SPI_FLASH_DRVG_READ_CMD_E, &flashParameters);
   if (ERRG_FAILED(ret))
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, "reading from flash failed\n");
      return ret;
   }

   address += sizeof(InuSectionHeaderT);

   if (CnnDataSectionHeader.magicNumber == SECTION_CNN_DATA_MAGIC_NUMBER)
   {
      UINT32 sectionEndAddr = address + CnnDataSectionHeader.sectionDataSize;

      do
      {
         //read cnn data header
         flashParameters.flashRead.startFlashAddress  = address;
         flashParameters.flashRead.readFlashSize      = sizeof(InuCnnDataHeaderT);
         flashParameters.flashRead.flashBufP          = (UINT8*)&CnnDataHeader;
         ret = IO_PALG_ioctl(IO_PALG_getHandle(deviceId), SPI_FLASH_DRVG_READ_CMD_E, &flashParameters);
         if (ERRG_FAILED(ret))
         {
            LOGG_PRINT(LOG_ERROR_E, NULL, "reading from flash failed\n");
            return ret;
         }

         address += sizeof(InuCnnDataHeaderT);

         if (CnnDataHeader.networkId == 0xffffffff || (address + CnnDataHeader.networkSize) > sectionEndAddr)
         {
            //list is over, or exeeds section size. stop searching
            break;
         }

         if (CnnDataHeader.networkId == networkId)
         {
            //the required network was found
            *flashAddress = address;
            *networkSize = CnnDataHeader.networkSize;

            return INU_LOAD_NETWORK__RET_SUCCESS;
         }
         else
         {
            //keep searching the network
            address += CnnDataHeader.networkSize;
         }
      } while ((address + sizeof(InuCnnDataHeaderT)) < sectionEndAddr);

      LOGG_PRINT(LOG_ERROR_E, NULL, "couldn't find network %d in flash\n", networkId);
      ret = INU_LOAD_NETWORK__ERR_INVALID_ARGS;
   }
   else
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, "wrong storage magic number \n");
   }

   return ret;
}

/****************************************************************************
*
*  Function Name: inu_load_network__readFromFlash
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
static ERRG_codeE inu_load_network__readFromFlash(UINT32 flashAddress, MEM_POOLG_bufDescT *bufDescP)
{
   SPI_FLASH_DRVG_flashParametersT  flashParameters;
   ERRG_codeE                       ret = INU_DEVICE__RET_SUCCESS;
   IO_PALG_deviceIdE                deviceId = IO_SPI_FLASH_1_E;
   UINT8                            flashNum = MAIN_FLASH;

   //read cnn data
   flashParameters.flashRead.startFlashAddress  = flashAddress;
   flashParameters.flashRead.readFlashSize      = bufDescP->size;
   flashParameters.flashRead.flashBufP          = bufDescP->dataP;
   ret = IO_PALG_ioctl(IO_PALG_getHandle(deviceId), SPI_FLASH_DRVG_READ_CMD_E, &flashParameters);
   if (ERRG_SUCCEEDED(ret))
   {
      LOGG_PRINT(LOG_INFO_E, NULL, "loaded network from flash successfully\n");
   }
   else
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, "loaded network from flash failed\n");
   }
   return ret;
}

/****************************************************************************
*
*  Function Name: inu_load_network__handleNetwork
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
static ERRG_codeE inu_load_network__handleNetwork(inu_load_network *me, INTERNAL_CMDG_CdnnLoadNetworkHdrT *cdnnChunkHdrP, MEM_POOLG_bufDescT *bufDescP)
{
   ERRG_codeE                 retCode     = SYSTEM__RET_SUCCESS;
   inu_load_network__privData *privP = (inu_load_network__privData*)me->privP;
   CDE_DRVG_channelHandleT h;
   DATA_BASEG_cnnReloadDmaCopyParamsT dmaCopy;
   DATA_BASEG_cdnnDataDataBaseT *cdnnDataBaseP;
   UINT32 networkPhysP;
   DATA_BASEG_databaseE dataBase = DATA_BASEG_CDNN;
   if (bufDescP)
   {
      retCode = MEM_POOLG_getDataPhyAddr(bufDescP,&networkPhysP);
      if (ERRG_SUCCEEDED(retCode))
      {
         if (cdnnChunkHdrP->networkId >= CDNN_MAX_NETWORKS)
         {
            INU_REF__LOGG_PRINT(me, LOG_ERROR_E, NULL, "illegal network id (%d), only %d networks are allowed\n", cdnnChunkHdrP->networkId, CDNN_MAX_NETWORKS);
            assert(0);
         }

         CDNN_GPP_networksBuffers[cdnnChunkHdrP->networkId]           = bufDescP;
         CDNN_GPP_networks[cdnnChunkHdrP->networkId].isNetworkLoaded  = 1;
         CDNN_GPP_networks[cdnnChunkHdrP->networkId].engineType       = cdnnChunkHdrP->engineType;

         if (cdnnChunkHdrP->engineType == ICCG_CMD_TARGET_DSPB)
         {
            dataBase = DATA_BASEG_CDNN_EV;
         }
         DATA_BASEG_accessDataBase((UINT8**)&cdnnDataBaseP, dataBase);
         cdnnDataBaseP->networkId = cdnnChunkHdrP->networkId;
         cdnnDataBaseP->networkP         = (UINT8 *)networkPhysP;
         cdnnDataBaseP->networkSize      = bufDescP->dataLen;
         cdnnDataBaseP->rawScale         = cdnnChunkHdrP->rawScale;
         cdnnDataBaseP->channelSwap      = cdnnChunkHdrP->channelSwap;
         cdnnDataBaseP->resultsFormat    = cdnnChunkHdrP->resultsFormat;
         cdnnDataBaseP->makeImageSquare  = cdnnChunkHdrP->isYoloNetwork;
         cdnnDataBaseP->bitAccuracy      = cdnnChunkHdrP->bitAccuracy;
         cdnnDataBaseP->pipeDepth        = cdnnChunkHdrP->pipeDepth;
         cdnnDataBaseP->ProcesssingFlags = cdnnChunkHdrP->ProcesssingFlags;

         //for CEVA
         cdnnDataBaseP->ddrP                    = (UINT8 *)inu_load_network_cdnnWorkSpacePhyAddress;
         cdnnDataBaseP->ddrSize                 = CDNN_CEVA_WEBCAM_RGB_IMAGE_SIZE + CDNN_CEVA_ENGINE_MAX_SIZE;
         cdnnDataBaseP->cdnnEngineExtMemoryP    = (UINT8 *)inu_load_network_cdnnWorkSpacePhyAddress + CDNN_CEVA_WEBCAM_RGB_IMAGE_SIZE;
         cdnnDataBaseP->cdnnEngineExtMemorySize = CDNN_CEVA_ENGINE_MAX_SIZE;
         cdnnDataBaseP->networkResultsBlockSize = CDNNG_MAX_BLOB_SIZE;
         LOGG_PRINT(LOG_INFO_E, NULL, "received network %d at %x\n", cdnnChunkHdrP->networkId, networkPhysP);

         h = privP->channelInfo->dmaChannelHandle;

         retCode = MEM_MAPG_convertVirtualToPhysical2((UINT32)h->program,  &dmaCopy.baseAddr, MEM_MAPG_CRAM_CDE_PARAMS_E);
         if (ERRG_FAILED(retCode))
         {
            INU_REF__LOGG_PRINT(me,LOG_ERROR_E,NULL,"error config DMA for EV usage (0x%x, 0x%x)\n",(UINT32)h->program,  &dmaCopy.baseAddr);
         }

         dmaCopy.sourceAddr      = dmaCopy.baseAddr + h->srcParams.addressOffset;
         dmaCopy.destinationAddr = dmaCopy.baseAddr + h->dstParams.addressOffset;
         dmaCopy.loopCounter     = dmaCopy.baseAddr + h->dstParams.buffSizeOffset;
         dmaCopy.dmaCore         = h->coreNum;
#ifdef CDE_DRVG_VIRTUAL_CHANNELS
         dmaCopy.dmaChannel      = h->hwChannelNum;
#else
         dmaCopy.dmaChannel      = h->channelNum;
#endif
         INU_REF__LOGG_PRINT(me,LOG_DEBUG_E,NULL,"cnn dma channel: base %p, src %p, dst %p, loopcnt %p, core %d, channel %d \n", dmaCopy.baseAddr, dmaCopy.sourceAddr, dmaCopy.destinationAddr, dmaCopy.loopCounter, dmaCopy.dmaCore, dmaCopy.dmaChannel );
         memcpy(&cdnnDataBaseP->dmaCopyParams, &dmaCopy, sizeof(dmaCopy));

         DATA_BASEG_accessDataBaseRelease(dataBase);
         INU_REF__LOGG_PRINT(me,LOG_DEBUG_E,NULL,"dspTarget %d numImages %d rawScale %d\n", cdnnChunkHdrP->engineType, cdnnDataBaseP->numImages, cdnnDataBaseP->rawScale);
         //Send msg to dsp
         retCode = inu_device__sendDspSyncMsg((inu_device*)inu_ref__getDevice((inu_ref*)me),DATA_BASEG_ALG_CDNN,3,cdnnChunkHdrP->engineType);
         if (ERRG_FAILED(retCode))
         {
            INU_REF__LOGG_PRINT(me,LOG_ERROR_E,NULL,"error sending dsp message\n");
         }

         //Send receive chunk cdnn Ack to host
         INTERNAL_CMDG_CdnnLoadNetworkHdrT cdnnTblAck;

         cdnnTblAck.networkId = cdnnChunkHdrP->networkId;
         if (!(inu_ref__standAloneMode(me)))
         {
            retCode = inu_ref__copyAndSendDataAsync((inu_ref*)me,INTERNAL_CMDG_SEND_CDNN_LOAD_NETWORK_E,&cdnnTblAck, NULL, 0);
         }
         else
         {
            privP->ioctlFlashAck = TRUE;
         }
      }
   }
   else
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, "error receiving network\n");
   }

   return(retCode);
}
#endif

/****************************************************************************
*
*  Function Name: inu_load_network__name
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: load_network
*
****************************************************************************/
static const char* inu_load_network__name(inu_ref *me)
{
   FIX_UNUSED_PARAM_WARN(me);
   return name;
}

/****************************************************************************
*
*  Function Name: inu_load_network__dtor
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: load_network
*
****************************************************************************/
static void inu_load_network__dtor(inu_ref *me)
{
   inu_load_network__privData *privP = (inu_load_network__privData*)((inu_load_network*)me)->privP;
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
*  Function Name: inu_load_network__ctor
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: load_network
*
****************************************************************************/
ERRG_codeE inu_load_network__ctor(inu_load_network *me, inu_load_network__CtorParams *ctorParamsP)
{
   ERRG_codeE ret;
   inu_load_network__privData *privP;
   ret = inu_ref__ctor(&me->ref, &ctorParamsP->ref_params);
   if (ERRG_SUCCEEDED(ret))
   {
         privP = (inu_load_network__privData*)malloc(sizeof(inu_load_network__privData));
         if (!privP)
         {
            return INU_LOAD_NETWORK__ERR_OUT_OF_MEM;
         }
         memset(privP,0,sizeof(inu_load_network__privData));
         me->privP = privP;

         //ret = inu_ref__addMemPool((inu_ref*)me, MEM_POOLG_TYPE_ALLOC_HEAP_E, 1000, 6, NULL, 0);
#if DEFSG_IS_HOST
         MEM_POOLG_cfgT cfg;
         cfg.bufferSize = 512*1024*1024;
         cfg.type = MEM_POOLG_TYPE_ALLOC_USER_E;
         cfg.numBuffers = 1;
         cfg.freeCb = NULL;

         ret = MEM_POOLG_initPool(&privP->handle, &cfg);
#else
         if (ctorParamsP->initWorkSpace)
         {
            ret = MEM_MAPG_alloc(/*CDNN_CEVA_WEBCAM_RGB_IMAGE_SIZE + CDNN_CEVA_ENGINE_MAX_SIZE*/ 1024,
                                 &inu_load_network_cdnnWorkSpacePhyAddress,
                                 &inu_load_network_cdnnWorkSpaceVirtAddress,
                                 0);
            LOGG_PRINT(LOG_INFO_E, 0,  "DDR memory for cdnn workspace %x size(%d)\n", inu_load_network_cdnnWorkSpacePhyAddress, CDNN_CEVA_WEBCAM_RGB_IMAGE_SIZE + CDNN_CEVA_ENGINE_MAX_SIZE);
            if (ERRG_FAILED(ret))
            {
               LOGG_PRINT(LOG_ERROR_E, 0,  "ERROR: Failed to init memory for cdnn workspace (%d)\n",CDNN_CEVA_WEBCAM_RGB_IMAGE_SIZE + CDNN_CEVA_ENGINE_MAX_SIZE);
               return INU_LOAD_NETWORK__ERR_OUT_OF_MEM;
            }
         }
#ifdef __UART_ON_FPGA__
            //CDNN_GPP_networks[0].networkPhysP    = (intptr_t)INU_FPGA_BIN_ADDR;
            //CDNN_GPP_networks[0].networkSize     = INU_FPGA_BIN_SIZE;
            //CDNN_GPP_networks[0].rawScale        = CDNNG_INPUT_IMAGE_RAW_SCALE_256;
            //CDNN_GPP_networks[0].channelSwap     = CDNNG_NETWORK_CHANNEL_SWAP;
            //CDNN_GPP_networks[0].resultsFormat   = CDNNG_FLOATING_POINT_QUERY;
            //CDNN_GPP_networks[0].isYoloNetwork   = CDNNG_OTHER_CNN;
            //CDNN_GPP_networks[0].bitAccuracy     = CDNNG_BIT_ACCURACY_16BIT;
            CDNN_GPP_networks[0].engineType     = INU_LOAD_NETWORK__EV61_E;
            CDNN_GPP_networks[0].isNetworkLoaded = 1;
#endif
         ret = CDE_MNGRG_memcpyOpenChan(&privP->channelInfo, 0x100, NULL, NULL); // DMA copy programm for CDNN reload
#ifdef CDE_DRVG_VIRTUAL_CHANNELS
         if (ERRG_FAILED(ret))
         {
            return INU_LOAD_NETWORK__ERR_UNEXPECTED;
         }

         ret = CDE_DRVG_assignPhysicalChannel(privP->channelInfo->dmaChannelHandle);
#endif
#endif
   }

   return ret;
}

/****************************************************************************
*
*  Function Name: inu_load_network__rxIoctl
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: load_network
*
****************************************************************************/
static int inu_load_network__rxIoctl(inu_ref *me, void *msgP, int msgCode)
{
   int ret;
   ret = inu_ref__vtable_get()->p_rxSyncCtrl(me, msgP, msgCode);

   switch (msgCode)
   {
      case(INTERNAL_CMDG_SEND_CDNN_LOAD_NETWORK_ALLOC_E):
      {
#if DEFSG_IS_GP
         ret = inu_load_network__allocForNetwork((inu_load_network*)me, (INTERNAL_CMDG_CdnnAllocNetworkHdrT*)msgP);
         break;
#endif
      }

      case(INTERNAL_CMDG_SEND_CDNN_RELEASE_ALL_E):
      {
#if DEFSG_IS_GP
            int i;
            inu_load_network           *cdnn = (inu_load_network*)me;
            inu_load_network__privData *privP = (inu_load_network__privData*)cdnn->privP;
            for (i=0; i<CDNN_MAX_NETWORKS; i++)
            {
               if (CDNN_GPP_networks[i].isNetworkLoaded)
               {
                  MEM_POOLG_free(CDNN_GPP_networksBuffers[i]);
                  ret = inu_ref__removeMemPool((inu_ref*)me, privP->networkPoolH[i]);
                  CDNN_GPP_networks[i].isNetworkLoaded = 0;
               }
            }
            MEM_MAPG_free((UINT32*)inu_load_network_cdnnWorkSpaceVirtAddress);
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
*  Function Name: inu_load_network__rxData
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: load_network
*
****************************************************************************/
static int inu_load_network__rxData(inu_ref *ref, UINT32 msgCode, UINT8 *msgP, UINT8 *dataP, UINT32 dataLen, void **bufDescP)
{
   inu_load_network           *me = (inu_load_network*)ref;
#if DEFSG_IS_HOST
   inu_load_network__privData *privP = (inu_load_network__privData*)me->privP;
#endif
   int ret = 0;

   ret = inu_ref__vtable_get()->p_rxAsyncData(ref, msgCode, msgP, dataP, dataLen, bufDescP);
   switch (msgCode)
   {
      case(INTERNAL_CMDG_SEND_CDNN_LOAD_NETWORK_E):
      {
#if DEFSG_IS_GP
         inu_load_network__handleNetwork(me, (INTERNAL_CMDG_CdnnLoadNetworkHdrT*)msgP, *bufDescP);
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
*  Function Name: inu_load_network__vtable_init
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: load_network
*
****************************************************************************/
void inu_load_network__vtable_init()
{
   if (!_bool_vtable_initialized)
   {
      inu_ref__vtableInitDefaults(&_vtable);
      _vtable.p_name = inu_load_network__name;
      _vtable.p_dtor = inu_load_network__dtor;
      _vtable.p_ctor = (inu_ref__Ctor*)inu_load_network__ctor;

      _vtable.p_rxSyncCtrl  = inu_load_network__rxIoctl;
      _vtable.p_rxAsyncData = inu_load_network__rxData;

      _bool_vtable_initialized = true;
   }
}

/****************************************************************************
*
*  Function Name: inu_load_network__vtable_get
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: load_network
*
****************************************************************************/
const inu_ref__VTable *inu_load_network__vtable_get(void)
{
   inu_load_network__vtable_init();
   return &_vtable;
}

/****************************************************************************
*
*  Function Name: inu_load_network__new
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: load_network
*
****************************************************************************/
ERRG_codeE inu_load_network__new(inu_load_networkH *meH, inu_load_network__initParams *initParamsP)
{
   inu_load_network__CtorParams ctorParams;
   ctorParams.ref_params.device  = initParamsP->deviceH;
   strcpy(ctorParams.ref_params.userName, "inu_Load_Network");
   return inu_factory__new((inu_ref**)meH, &ctorParams, INU_LOAD_NETWORK_REF_TYPE);
}

/****************************************************************************
*
*  Function Name: inu_load_network__delete
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: load_network
*
****************************************************************************/
void inu_load_network__delete(inu_load_networkH meH)
{
   inu_factory__delete((inu_ref*)meH, 0);
}

/****************************************************************************
*
*  Function Name: inu_load_network__loadNetwork
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
ERRG_codeE inu_load_network__loadNetwork(inu_load_networkH meH, inu_load_network_memory_t *memP, inu_load_network_hdr_t *networkHdrP)
{
   ERRG_codeE ret = INU_LOAD_NETWORK__RET_SUCCESS;
   INTERNAL_CMDG_CdnnLoadNetworkHdrT networkChunkHdr;
   enum { SLEEP_USEC_E = (1 * 1000) };
   UINT32  startSec, deltaMsec = 0;
   UINT16  startMsec;
   UINT32  timeoutMsec = SLEEP_USEC_E * 200;
   UINT64 usec1,usec2;
   inu_load_network__privData *privP = (inu_load_network__privData*)((inu_load_network*)meH)->privP;
   MEM_POOLG_bufDescT *bufDescP;
   INTERNAL_CMDG_CdnnAllocNetworkHdrT allocHdr;
   UINT32 ddrWorkspaceSize = CDNN_CEVA_WEBCAM_RGB_IMAGE_SIZE + CDNN_CEVA_ENGINE_MAX_SIZE;
   UINT32 maxInternalworkspaceSize = CDNN_CEVA_INTERNAL_MAX_SIZE;
   UINT32 flashAddress = 0;
   FILE *file;

   OS_LYRG_getUsecTime(&usec1);

   if(networkHdrP->version != CDNNG_CURRENT_VERSION)
   {
      UINT32 engineV = CDNNG_CURRENT_VERSION;
      LOGG_PRINT(LOG_ERROR_E, NULL, "Mismatch between net and engine, net version = %d, engine version = %d\n", networkHdrP->version, engineV);
      ret = INU_LOAD_NETWORK__ERR_NOT_SUPPORTED;
      return ret;
   }

   if(networkHdrP->ddrWorkspaceSize > ddrWorkspaceSize)
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, "ddr workspace for network id %d exceeded. %d > %d\n", networkHdrP->networkId, networkHdrP->ddrWorkspaceSize, ddrWorkspaceSize);
      ret = INU_LOAD_NETWORK__ERR_NOT_SUPPORTED;
      return ret;
   }

   if(networkHdrP->internalworkspaceSize > maxInternalworkspaceSize)
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, "int workspace for network id %d exceeded. %d > %d\n", networkHdrP->networkId, networkHdrP->internalworkspaceSize, maxInternalworkspaceSize);
      ret = INU_LOAD_NETWORK__ERR_NOT_SUPPORTED;
      return ret;
   }

   if (networkHdrP->networkId >= CDNN_MAX_NETWORKS)
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, "illegal network id (%d), only %d networks are allowed\n", networkHdrP->networkId, CDNN_MAX_NETWORKS);
      ret = INU_LOAD_NETWORK__ERR_NOT_SUPPORTED;
      return ret;
   }

   // if memP is NULL, we didn't recieve a path to cnn binary in FS, read the cnn from flash
   if (inu_ref__standAloneMode(meH) && !memP)
   {
#if DEFSG_IS_GP
      UINT32 networkSize = 0;

      ret = inu_load_network__searchInFlash(networkHdrP->networkId, &flashAddress, &networkSize);
      if (ERRG_SUCCEEDED(ret))
      {
         allocHdr.networkId = networkHdrP->networkId;
         allocHdr.networkSize = networkSize;
      }
      else
      {
         LOGG_PRINT(LOG_ERROR_E, NULL, "loading network id %d from flash failed\n", networkHdrP->networkId);
         return ret;
      }
#endif
   }
   else
   {
      allocHdr.networkId = networkHdrP->networkId;
      allocHdr.networkSize = memP->bufsize;
   }

   ret = inu_ref__sendCtrlSync((inu_ref*)meH,INTERNAL_CMDG_SEND_CDNN_LOAD_NETWORK_ALLOC_E, &allocHdr, INU_REF__SYNC_DFLT_TIMEOUT_MSEC);
   if (ERRG_SUCCEEDED(ret))
   {
      if (!(inu_ref__standAloneMode(meH)))
      {
         ret = MEM_POOLG_alloc(privP->handle, allocHdr.networkSize, &bufDescP);
      }
      else
      {
         ret = inu_ref__allocBuf((inu_ref*)meH, allocHdr.networkSize, &bufDescP);
      }
      if (ERRG_SUCCEEDED(ret))
      {
         privP->ioctlFlashAck = FALSE;
         networkChunkHdr.networkId = networkHdrP->networkId;
         networkChunkHdr.engineType = networkHdrP->engineType;
         networkChunkHdr.rawScale = networkHdrP->rawScale;
         networkChunkHdr.channelSwap = networkHdrP->channelSwap;
         networkChunkHdr.resultsFormat = networkHdrP->resultsFormat;
         networkChunkHdr.bitAccuracy= networkHdrP->bitAccuracy;
         networkChunkHdr.isYoloNetwork = networkHdrP->netType;
         networkChunkHdr.pipeDepth = networkHdrP->pipeDepth;
         networkChunkHdr.ProcesssingFlags = networkHdrP->ProcesssingFlags;
         networkChunkHdr.networkSize = allocHdr.networkSize;

         if (!(inu_ref__standAloneMode(meH)))
         {
            bufDescP->dataP   = (UINT8*)memP->bufP;
            bufDescP->startP  = (UINT8*)memP->bufP;
            bufDescP->size    = memP->bufsize;
            bufDescP->dataLen = memP->bufsize;
         }
         else if (memP) // if memp is not NULL, use memP->path to get the CNN binary from FS
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
         else // if memP is NULL, we didn't recieve a path to cnn binary in FS, read the cnn from flash
         {
            bufDescP->startP  = (UINT8*)bufDescP->dataP;
            bufDescP->size    = allocHdr.networkSize;
            bufDescP->dataLen = allocHdr.networkSize;

            ret = inu_load_network__readFromFlash(flashAddress, bufDescP);
            if (ERRG_SUCCEEDED(ret))
            {
               CMEM_cacheWb(bufDescP->dataP, allocHdr.networkSize);
            }
            else
            {
               LOGG_PRINT(LOG_ERROR_E, NULL, "loading network id %d from flash failed\n", networkHdrP->networkId);
               return ret;
            }
         }
#endif

#if DEFSG_IS_GP
         inu_load_network__handleNetwork(meH, &networkChunkHdr, bufDescP);
#else
         ret = inu_ref__sendDataAsync((inu_ref*)meH, INTERNAL_CMDG_SEND_CDNN_LOAD_NETWORK_E, &networkChunkHdr, bufDescP);
#endif

         //wait for ACK
         OS_LYRG_getTime(&startSec, &startMsec);
         do
         {
            if (privP->ioctlFlashAck == TRUE)
            {
               privP->ioctlFlashAck = FALSE;

               OS_LYRG_getUsecTime(&usec2);
               LOGG_PRINT(LOG_INFO_E, NULL, "Finished sending network. size = %d, total time = %llu\n", bufDescP->size, usec2-usec1);

               return ret;
            }
            OS_LYRG_usleep(SLEEP_USEC_E);
            deltaMsec = OS_LYRG_deltaMsec(startSec, startMsec);
         } while (deltaMsec <= timeoutMsec);
      }
   }

   LOGG_PRINT(LOG_ERROR_E, NULL, "Failed loading network\n");

   return ret;
}

/****************************************************************************
*
*  Function Name: inu_load_network__releaseAll
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: load_network
*
****************************************************************************/
ERRG_codeE inu_load_network__releaseAll(inu_load_networkH meH)
{
   INTERNAL_CMDG_CdnnReleaseAllHdrT msg;

   return inu_ref__sendCtrlSync((inu_ref*)meH, INTERNAL_CMDG_SEND_CDNN_RELEASE_ALL_E, &msg, INU_REF__SYNC_DFLT_TIMEOUT_MSEC);

}


