/****************************************************************************
 *
 *   FileName: cdnn_ev.c
 *
 *   Author:  
 *
 *   Date: 
 *
 *   Description: 
 *   
 ****************************************************************************/
#include <stdio.h>
#include <opencv2/opencv.hpp>
#include "inu_app_pipe.h"
#include "inu2_internal.h"
#include "data_base.h"
#include "err_defs.h"
#include "log.h"
#include "evthreads.h"
//#include "reload_pmv.h"
#include "nu4100_pss_regs.h"
#include "sched.h"
#include "ictl_drv.h"
#include "yolo_v3_pp.h"
#include "face_ssd_pp.h"

/****************************************************************************
***************     L O C A L        D E F I N I T I O N S    ***************
****************************************************************************/
typedef enum
{
	CDNN_EV_OPERATE,
	CDNN_EV_START,
	CDNN_EV_STOP,
	CDNN_EV_LOAD_NETWORK,
}CDNN_EV_appStateE;
	

/****************************************************************************
***************                     D A T A                  ***************
****************************************************************************/
CDNN_EVG_netInfoT CDNN_EVG_netInfo[CDNN_MAX_NETWORKS];
DATA_BASEG_cdnnDataDataBaseT     CDNN_EVG_msgParams;
UINT32 CDNN_EVP_framenum;
UINT8  *CDNN_EV_inputImageP;
UINT8  *CDNN_EVP_networkP;
UINT32 CDNN_EVP_networkSize;
UINT32 CDNN_EVP_imageWidth;
UINT32 CDNN_EVP_imageHeight;
UINT32 CDNN_EVG_offsetToImage;
UINT32 CDNN_EVG_makeImageSquare;
UINT32 CDNN_EVG_numChannels;
CDNNG_channelPixelTypeE CDNN_EVG_pixelTypeE;
static UINT32 CDNN_EVP_ddrWorkspaceAddr;
static UINT32 CDNN_EVP_ddrWorkspaceSize;
static UINT8 *CDNN_EVP_ddrRgbImageP;

static CDNN_CEVAG_algConfigT		CDNN_EVP_algConfig;
EvMutexType     CNNmutex;
EvCondType      CNNcondition;
UINT32          CNNCounter = 0;

/****************************************************************************
***************      L O C A L         D E C L A R A T I O N S  *************
****************************************************************************/

/****************************************************************************
***************     L O C A L        D E F I N I T I O N S    ***************
****************************************************************************/

typedef struct
{
   UINT32                     deviceBaseAddress;
} PSS_DRVP_deviceDescT;

//set bits in a register through ARC cache
#define CDNN_EVG_SET_BITS(reg,position,len,value)\
    {       \
            UINT32 tmp=*reg;\
            _Uncached volatile UINT32* regP = (_Uncached volatile UINT32*)reg;\
            tmp&=~(((UINT32)0xFFFFFFFF>>(32-(len)))<<(position));\
            tmp|=((value&((UINT32)0xFFFFFFFF>>(32-(len))))<<(position));\
            *regP=tmp;\
    }
//ebanle/disable CNN memories reuse
#define CDNN_EVG_REUSE_W(value) CDNN_EVG_SET_BITS( (PSS_ARC0_FIQ_3_0_REG),PSS_ARC0_FIQ_3_0_CNN_MEM_REUSE_POS, PSS_ARC0_FIQ_3_0_CNN_MEM_REUSE_LEN,value)

/****************************************************************************
 ***************       L O C A L         D A T A              ***************
 ****************************************************************************/
static PSS_DRVP_deviceDescT   PSS_DRVP_deviceDesc;
UINT8 * pmvRelaodParams=NULL;




/****************************************************************************
**************   L O C A L      F U N C T I O N S             ***************
****************************************************************************/
	
static ERRG_codeE CDNN_EVP_init(UINT8* ddrWorkspaceP, UINT32 ddrWorkspaceSize)
{
	CDNN_EVP_ddrWorkspaceAddr = (UINT32)ddrWorkspaceP;
	CDNN_EVP_ddrWorkspaceSize = ddrWorkspaceSize;			

	//allocate 1 buffer for conversion from YUV 4:2:2 to RGB 8:8:8
	CDNN_EVP_ddrRgbImageP = (UINT8*)CDNN_EVP_ddrWorkspaceAddr;
    evMutexInit(&CNNmutex);    
    evCondInit(&CNNcondition);
	ERRG_codeE status = (ERRG_codeE)SUCCESS_E;
	return status;
}

#if 0
/****************************************************************************
*
*  Function Name: CDNN_EVP_runFrame
*
*  Description: This routine handles the process of 1  frame
*
*  Inputs: 
*			msgParamsP - frame parameters
*
*  Outputs:
*
*  Returns:
*
*  Context:
*
****************************************************************************/
static ERRG_codeE CDNN_EVP_runFrame(DATA_BASEG_cdnnDataDataBaseT *msgParamsP, UINT32 cdnnBlobAddr)
{
   CDNN_CEVAG_FrameParamsT *frameParamsP = &(msgParamsP->FrameParams);
	ERRG_codeE retVal = (ERRG_codeE)SUCCESS_E;
 	int	s32status = 0;
	inu_cdnn_data__hdr_t *resultsHeaderP;
	inu_cdnn_data__tailHeaderT *tailHeaderP;
	UINT32 isRGB;
	UINT32 numImages = msgParamsP->numImages;
	UINT32 imageIdx;
	
	resultsHeaderP = (inu_cdnn_data__hdr_t *)msgParamsP->networkResultsBlockP;
	tailHeaderP = resultsHeaderP->tailHeader;
	isRGB = msgParamsP->isRGB;

	if(isRGB)
	{
		CDNN_EV_inputImageP = (UINT8*)frameParamsP->inPhysicalAddr;
	}
	else
	{
		CDNN_EVG_offsetToImage = 0;

		if((CDNN_EVG_makeImageSquare) && (frameParamsP->frameWidth > frameParamsP->frameHeight))
		{
			//	make the image squared by padding zeroes on top and bottom
			UINT32 halfDiff = ((frameParamsP->frameWidth - frameParamsP->frameHeight) >> 1);
			CDNN_EVG_offsetToImage = 3 * halfDiff * frameParamsP->frameWidth;
			CDNN_EVP_imageHeight = CDNN_EVP_imageWidth;
		}
		CDNN_EV_inputImageP = CDNN_EVP_ddrRgbImageP + CDNN_EVG_offsetToImage;

		//s32status = FORMAT_CONVERTG_convertFrame((UINT8*)frameParamsP->inPhysicalAddr, CDNN_EV_inputImageP, CDNN_CEVA_formatConvertHandle);
	}

	resultsHeaderP->numOfTails = 0;
	resultsHeaderP->totOutputSize = 0;
	
	for(imageIdx = 0 ; imageIdx < numImages; imageIdx++)
	{

		//assuming RGB format
		CDNN_EV_inputImageP += (frameParamsP->frameWidth * frameParamsP->frameHeight * msgParamsP->pixelSize);
	}
		

	CDNN_EVP_framenum++;

	if(s32status)
	{
		retVal = (ERRG_codeE)FAIL_E;
	}
   return (retVal);
}

#endif

/****************************************************************************
*
*  Function Name: CDNN_EVP_getDataType
*
*  Description: switch pixel type to element type
*
*  Inputs: 
*			pixelTypeE
*
*  Outputs:
*
*  Returns:
*
*  Context:
*
****************************************************************************/
UINT32 CDNN_EVP_getDataType(CDNNG_channelPixelTypeE pixelTypeE)
{
	switch (pixelTypeE)
	{
		case CDNNG_PIXEL_TYPE_UINT8:
			return EV_CNN_ELEMENT_TYPE_UINT8;
		case CDNNG_PIXEL_TYPE_INT8:
			return EV_CNN_ELEMENT_TYPE_INT8;
		case CDNNG_PIXEL_TYPE_UINT16:
			return EV_CNN_ELEMENT_TYPE_16UINT16;
		case CDNNG_PIXEL_TYPE_INT16:
			return EV_CNN_ELEMENT_TYPE_16INT16;
		case CDNNG_PIXEL_TYPE_12UINT16:				
			return EV_CNN_ELEMENT_TYPE_12UINT16;
		case CDNNG_PIXEL_TYPE_12INT16:
			return EV_CNN_ELEMENT_TYPE_12INT16;
		case CDNNG_PIXEL_TYPE_FLOAT32:
			return EV_CNN_ELEMENT_TYPE_FLOAT32;
		case CDNNG_PIXEL_TYPE_FLOAT64:
			return EV_CNN_ELEMENT_TYPE_FLOAT64;
		default:
			LOGG_PRINT(LOG_ERROR_E, (ERRG_codeE)NULL, "cnn element type not supported %d\n", pixelTypeE);
			return 0;
	}
			
}


/****************************************************************************
*
*  Function Name: CDNN_EVG_prepare_processing
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
EV_CNN_INJECT_IMAGES* CDNN_EVG_prepare_processing(DATA_BASEG_cdnnDataDataBaseT* msgParamsP, DATA_BASEG_jobDescriptorT* jobDb, UINT16 jobDescriptor)
{
	EV_CNN_INJECT_IMAGES* process_image;
	UINT32 databaseListNum = jobDb->inputList.databaseListNum;
	inu_cdnn_data* cdnn_data = (inu_cdnn_data*)jobDb->inputList.inu_dataPhy[databaseListNum - 1];
	UINT32 outBaseAddress = cdnn_data->data.dataPhyAddr;
	msgParamsP->networkResultsBlockP = (UINT8*)&(cdnn_data->cdnn_dataHdr);
	uint64_t current;

	current = getRTC();

	process_image = (EV_CNN_INJECT_IMAGES*)inu_malloc(sizeof(EV_CNN_INJECT_IMAGES));
	if (!process_image) {
		LOGG_PRINT(LOG_ERROR_E, (ERRG_codeE)NULL, "CDNN_EVG_prepare_processing: failed to allocate process_image\n");
		return(NULL);
	}

	process_image->msg_header.msg_code = EV_CNN_MSG_INJECT_IMAGES;
	process_image->msg_header.msg_len = sizeof(EV_CNN_INJECT_IMAGES);
	process_image->net_id = cdnn_data->cdnn_dataHdr.network_id;//msgParamsP->networkId;
	process_image->number_of_images_in_a_set = databaseListNum - 1;
	process_image->jobDescriptor = jobDescriptor;
	process_image->intCondSignal = &SCHEDG_Condition;
	process_image->blob_output_address = (UINT8*)outBaseAddress;
	process_image->blob_output_header = msgParamsP->networkResultsBlockP;

	for (int image_num = 0; image_num < process_image->number_of_images_in_a_set; image_num++)
	{
		bool tensor_input = false;
		UINT32 isRGB = 0;
		UINT32 pixelSize = 0;
		UINT32 numChannels = 0;
		UINT32 numImages = 0;
		UINT32 inPhysicalAddr = 0;
		CDNNG_channelPixelTypeE pixelTypeE = CDNNG_PIXEL_TYPE_UINT8;

		inu_image* image = (inu_image*)jobDb->inputList.inu_dataPhy[image_num];

		CDNN_EVP_imageWidth = image->imageHdr.imgDescriptor.stride;
		CDNN_EVP_imageHeight = image->imageHdr.imgDescriptor.bufferHeight;
		CDNN_EV_inputImageP = (UINT8*)(image->data.dataPhyAddr);
		inPhysicalAddr = image->data.dataPhyAddr;
		//msgParamsP->FrameParams.frameWidth = CDNN_EVP_imageWidth;
		//msgParamsP->FrameParams.frameHeight = CDNN_EVP_imageHeight;

		numImages = image->imageHdr.imgDescriptor.numInterleaveImages;
		isRGB = 0;
		pixelSize = image->imageHdr.imgDescriptor.bitsPerPixel / 8;

		switch (image->imageHdr.imgDescriptor.format)
		{
		case NUCFG_FORMAT_DEPTH_E:
		{
			numChannels = CDNNG_1_CHANNEL;
			pixelTypeE = CDNNG_PIXEL_TYPE_12UINT16;
			isRGB = 1;
		}
		break;
		case  NUCFG_FORMAT_GREY_16_E:
		case NUCFG_FORMAT_RAW10_E:
		case NUCFG_FORMAT_RAW12_E:
		{
			numChannels = CDNNG_1_CHANNEL;
			pixelTypeE = CDNNG_PIXEL_TYPE_12UINT16; // CDNNG_PIXEL_TYPE_UINT16;
			isRGB = 1;
		}
		break;
		case NUCFG_FORMAT_RAW8_E:
		{
			numChannels = CDNNG_1_CHANNEL;
			pixelTypeE = CDNNG_PIXEL_TYPE_UINT8;
			isRGB = 1;
		}
		break;

		case NUCFG_FORMAT_RGB888_E:
		{
			numChannels = CDNNG_3_CHANNELS;
			pixelTypeE = CDNNG_PIXEL_TYPE_UINT8;
			isRGB = 1;
		}
		break;

		case NUCFG_FORMAT_YUV422_8BIT_E:
			numChannels = CDNNG_3_CHANNELS;
			pixelTypeE = CDNNG_PIXEL_TYPE_UINT8;
			isRGB = 0;
			break;

		case NUCFG_FORMAT_GEN_8_E: // general tensor case
			numChannels = image->imageHdr.imgDescriptor.bitsPerPixel / 8;
			pixelTypeE = CDNNG_PIXEL_TYPE_UINT8;
			isRGB = 0;
			tensor_input = true;
			break;

		case NUCFG_FORMAT_GEN_12_E: // general tensor case
			numChannels = image->imageHdr.imgDescriptor.bitsPerPixel / 16;
			pixelTypeE = CDNNG_PIXEL_TYPE_12UINT16;
			isRGB = 0;
			tensor_input = true;
			break;

		case NUCFG_FORMAT_GEN_16_E: // general tensor case
			numChannels = image->imageHdr.imgDescriptor.bitsPerPixel / 16;
			pixelTypeE = CDNNG_PIXEL_TYPE_UINT16;
			isRGB = 0;
			tensor_input = true;
			break;

		case NUCFG_FORMAT_BAYER_16_E:
			numChannels = image->imageHdr.imgDescriptor.bitsPerPixel / 16;
			pixelTypeE = CDNNG_PIXEL_TYPE_UINT16;
			isRGB = 0;
			tensor_input = true;
			break;

		default:
		{
			LOGG_PRINT(LOG_ERROR_E, (ERRG_codeE)NULL, "CNN error : image format not valid\n");
		}
		break;
		}

		CDNN_EVG_numChannels = numChannels;
		CDNN_EVG_pixelTypeE = pixelTypeE;
		LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL, "RunFrame: net %d time %llu in %p out %p ImageW = %d ImageH = %d numImages %d\n", process_image->net_id, current, msgParamsP->FrameParams.inPhysicalAddr, outBaseAddress, CDNN_EVP_imageWidth, CDNN_EVP_imageHeight, numImages);

		process_image->number_of_injected_sets = numImages;
		process_image->frame_id = image->imageHdr.dataHdr.dataIndex;
		cdnn_data->cdnn_dataHdr.dataHdr.timestamp = image->imageHdr.dataHdr.timestamp;
		cdnn_data->cdnn_dataHdr.dataHdr.dataIndex = image->imageHdr.dataHdr.dataIndex;
		process_image->input_sets[image_num].channels = numChannels;
		process_image->input_sets[image_num].width = CDNN_EVP_imageWidth;
		process_image->input_sets[image_num].height = CDNN_EVP_imageHeight;
		process_image->input_sets[image_num].images_ptr = (void*)inPhysicalAddr;
		process_image->input_sets[image_num].channel_swap = CDNN_EVG_netInfo[process_image->net_id].channelSwap;
		process_image->input_sets[image_num].resize = true;
		process_image->input_sets[image_num].element_type = (EV_CNN_TYPE)CDNN_EVP_getDataType(CDNN_EVG_pixelTypeE);
		process_image->input_sets[image_num].yuv_format = !isRGB;
		process_image->input_sets[image_num].tensor_input = tensor_input;
		process_image->input_sets[image_num].num_of_tensor_pointers = 0;
	}

	return(process_image);
}

/****************************************************************************
***************                M A I N        F U N C T I O N  **************
****************************************************************************/

/****************************************************************************
*
*  Function Name: CDNN_EVG_app
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
void CDNN_EVG_app(void *jobDescriptorP, UINT16 jobDescriptor, inu_function__operateParamsT *paramsP)
{

    DATA_BASEG_jobDescriptorT *jobDb = (DATA_BASEG_jobDescriptorT*)jobDescriptorP;
	ERRG_codeE  status;
	DATA_BASEG_cdnnDataDataBaseT *msgParamsP = &CDNN_EVG_msgParams;
	int ret;
    static bool cnnHalFirstStartFlag = true;
    static bool yoloV3PpFirstStartFlag = true;
	static bool faceSsdPpFirstStartFlag = true;
	static bool yolactPpFirstStartFlag = true;
	EV_CNN_INJECT_IMAGES* process_image;
	EV_CNN_RELEASE_NET* release_net;
	LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL, "entry point %d\n",jobDb->data);

	switch (jobDb->data)
	{
		case CDNN_EV_OPERATE:
		{
			//initialize a specific alg' data struct
			if(CDNN_EVP_algConfig.enableAlg == TRUE)
			{
				process_image = CDNN_EVG_prepare_processing(msgParamsP, jobDb, jobDescriptor);
				
				 ret = ev_inu_handle_message((EV_MSG_HEADER *)process_image);
	             if(ret) 
				 {
					LOGG_PRINT(LOG_ERROR_E, (ERRG_codeE)NULL, "ev_inu_new_request: failed to add a new request\n");
	             }
                 //Signal CNN thread
#if USE_CNN_THREAD_TRIGGERING
  
//	                 LOGG_PRINT(LOG_INFO_E, (ERRG_codeE)NULL, "Signal CNN thread for new frame. counter=%d\n", CNNCounter);   
                 evMutexLock(&CNNmutex);  
                 CNNCounter++;
                 evCondSignal(&CNNcondition);    
                 evMutexUnLock(&CNNmutex);
#endif
				 
			}
			else
			{
                SCHEDG_pushFinishedJob(jobDescriptor);
                evIntCondSignal(&SCHEDG_Condition);
			}
			break;
		}
		case CDNN_EV_START:
		{
			
			LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL, "Start CNN\n");	
			DATA_BASEG_readDataBase((UINT8*)msgParamsP, DATA_BASEG_CDNN_EV, 0);
			
			//enable algorithm 
			CDNN_EVP_algConfig.enableAlg = TRUE;
			
			status = CDNN_EVP_init(msgParamsP->ddrP, msgParamsP->ddrSize);
			if(status)
			{
				LOGG_PRINT(LOG_ERROR_E, (ERRG_codeE)NULL, "CDNN_EVP_init returned %d\n", status);
			}

			if (cnnHalFirstStartFlag)
            {
				cnnHalFirstStartFlag = false; 
				ev_inu_api_init(CNN_HAL_THREAD_CPU);
			}
            

			SCHEDG_pushFinishedJob(jobDescriptor);
			evIntCondSignal(&SCHEDG_Condition);
			
			break;
		}
		case CDNN_EV_STOP:
		{
			//disable the algorithm
			CDNN_EVP_algConfig.enableAlg = FALSE;
			
			DATA_BASEG_readDataBase((UINT8*)msgParamsP, DATA_BASEG_CDNN_EV, 0);
			
			LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL, "Stop network %d\n", msgParamsP->networkId);

			release_net = (EV_CNN_RELEASE_NET *)inu_malloc(sizeof(EV_CNN_RELEASE_NET));
			release_net->msg_header.msg_code = EV_CNN_MSG_RELEASE_NET;
			release_net->msg_header.msg_len = sizeof(EV_CNN_RELEASE_NET);
			release_net->net_id = msgParamsP->networkId;
			release_net->jobDescriptor = jobDescriptor;
			
			ret = ev_inu_handle_message((EV_MSG_HEADER*)release_net);
			if (ret)
			{
				LOGG_PRINT(LOG_ERROR_E, (ERRG_codeE)NULL, "ev_inu_new_request: failed to add release request\n");
			}

            evMutexDestroy(&CNNmutex);    
            evCondDestroy(&CNNcondition);				
			break;
		}
		
		case CDNN_EV_LOAD_NETWORK:
		{
			
			DATA_BASEG_readDataBase((UINT8*)msgParamsP, DATA_BASEG_CDNN_EV, 0);
			
			CDNN_EVP_networkP = msgParamsP->networkP;
			CDNN_EVP_networkSize = msgParamsP->networkSize; 
			LOGG_PRINT(LOG_INFO_E, (ERRG_codeE)NULL, "net_id %d networkP=%x networkSize=%d\n", msgParamsP->networkId, CDNN_EVP_networkP, CDNN_EVP_networkSize); 
#if 0	 
			UINT32 i;
			UINT32 checksum = 0;
			UINT8 *qdata_ddr_addr;
			
			qdata_ddr_addr = (UINT8*)CDNN_EVP_networkP;
			for (i = 0; i < CDNN_EVP_networkSize; i++) {
				checksum += qdata_ddr_addr[i];
			}
			LOGG_PRINT(LOG_INFO_E,(ERRG_codeE)NULL, "Network checksum %d size %d addr %x\n", checksum, CDNN_EVP_networkSize, (UINT32)CDNN_EVP_networkP);
#endif

			if (!pmvRelaodParams)
			{
				pmvRelaodParams = (UINT8*)evMemAlloc(sizeof(DATA_BASEG_cnnReloadDmaCopyParamsT), EV_MR_USE_CNDC, -1);
			}
			memcpy(pmvRelaodParams,&msgParamsP->dmaCopyParams ,sizeof(DATA_BASEG_cnnReloadDmaCopyParamsT) );
			/////////////////////////////////////////////////////
			CDNN_EVG_netInfo[msgParamsP->networkId].bin_address = (UINT32)CDNN_EVP_networkP;
			CDNN_EVG_netInfo[msgParamsP->networkId].bin_size = CDNN_EVP_networkSize;
			CDNN_EVG_netInfo[msgParamsP->networkId].pipe_depth = msgParamsP->pipeDepth;
			CDNN_EVG_netInfo[msgParamsP->networkId].ProcesssingFlags = msgParamsP->ProcesssingFlags;
			CDNN_EVG_netInfo[msgParamsP->networkId].channelSwap = msgParamsP->channelSwap;
			
			if (((CDNN_EVG_netInfo[msgParamsP->networkId].ProcesssingFlags & PROC_FLAGS_YOLO_V3_POSTPROCESS) || (CDNN_EVG_netInfo[msgParamsP->networkId].ProcesssingFlags & PROC_FLAGS_YOLO_V7_POSTPROCESS)) && yoloV3PpFirstStartFlag)
            {
                yoloV3PpFirstStartFlag = false;
                YOLO_V3_PPG_init(YOLO_PP_THREAD_CPU);
			}

			if ((CDNN_EVG_netInfo[msgParamsP->networkId].ProcesssingFlags & PROC_FLAGS_FACE_SSD_POSTPROCESS) && faceSsdPpFirstStartFlag)
			{
				faceSsdPpFirstStartFlag = false;
				face_ssd_pp_init(FACE_SSD_PP_THREAD_CPU);
			}

			if ((CDNN_EVG_netInfo[msgParamsP->networkId].ProcesssingFlags & PROC_FLAGS_YOLACT_POSTPROCESS) && yolactPpFirstStartFlag)
			{
				void yolact_pp_init(int coreId);

				yolactPpFirstStartFlag = false;
				yolact_pp_init(EV_CORE_ID_ZERO);
			}
			
			SCHEDG_pushFinishedJob(jobDescriptor);
			evIntCondSignal(&SCHEDG_Condition);
			//Register a cb function to reload CNN engine PMV memory with ARM DMA
			//evSetCnnMemLoader(EV_CNN_MEM_TNNV_PM, CDNN_EVP_cnnMemReloadFunction);

			/////////////////////////////////////////////////////
			
			break;
		}
		default:
		{
			LOGG_PRINT(LOG_ERROR_E, (ERRG_codeE)NULL, "CDNN error : Job data not valid\n");
			
			break;
		}
	}
	
}

/****************************************************************************
*
*  Function Name: CDNN_EVG_netInfoGetChannelSwap
*
*  Description: get channel swap from network parameters
*
*  Inputs:
*
*  Outputs:
*
*  Returns: 
*
*  Context: client cnn
*
****************************************************************************/
UINT32 CDNN_EVG_netInfoGetChannelSwap(UINT32 netId)
{

	return CDNN_EVG_netInfo[netId].channelSwap;
}

/****************************************************************************
*
*  Function Name: CDNN_EVG_netInfoGetProcesssingFlags
*
*  Description: get Processsing Flags from network parameters
*
*  Inputs:
*
*  Outputs:
*
*  Returns: 
*
*  Context: client cnn
*
****************************************************************************/
UINT32 CDNN_EVG_netInfoGetProcesssingFlags(UINT32 netId)
{

	return CDNN_EVG_netInfo[netId].ProcesssingFlags;
}


