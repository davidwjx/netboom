/*
 * File - inu_cnn_structs.h
 *
 * Copyright (C) 2014 All rights reserved to Inuitive  
 *
*/

#define USE_OPENCV_RESIZE 1

#include <opencv2/opencv.hpp>
#include "inu_app_pipe.h"
#include "evthreads.h"
#include <internal/ev_layer.h>
#include "ictl_drv.h"
#include <cnn_dev.h>

#ifndef INU_HAPS
#include "inu2_internal.h"
#include "sched.h"
#endif
#include "client_api.h"
#include "yolo_v3_pp.h"
#include "face_ssd_pp.h"
#include "ev_ocl_copy_interface.h"
#ifdef VER_4100
#include "yolact_pp.h"
#endif

#ifndef USE_OPENCV_RESIZE
#ifndef INU_HAPS
#include "ev_ocl_user_scaleimage_interface.h"
#else
#include "../../../../evlayers/src/ev_user_defined_layers/ev_ocl_user_scaleimage_interface.h"
#endif
#endif
#include "ConvertYuvToRgb888.h"
#include <ev_ocl.h>
#include "ev_init.h"

//Declaration of OpenCL kernel
EVOCL_KERNEL_DECL(ocl_convertYUV2RGB);
EVOCL_KERNEL_DECL(ocl_convertYUV2RGB_NoSplit);

/* For YOLO V3 post process */
int                         inu_yolo_network_dimension  = 320; //Just a default init value
int                         inu_yolo_number_of_classes = 80;
float                       inu_yolo_nms_threshold = 0.2;
float                       inu_yolo_conf_threshold = 0.3;
extern CLIENT_APIG_msgQueT	yolo_v3_msgQue;

/* For face ssd post process */
extern CLIENT_APIG_msgQueT	face_ssd_msgQue;

#ifdef VER_4100
/* For yolact post process */
int							inu_yolact_network_dimension = INU_YOLACT_NETWORK_DIMENSION;
int                         inu_yolact_number_of_classes = INU_YOLACT_NUMBER_OF_CLASSES;
float                       inu_yolact_nms_threshold = INU_YOLACT_NMS_THRESHOLD;
float                       inu_yolact_conf_threshold = INU_YOLACT_CONF_THRESHOLD;
float						inu_yolact_score_threshold = INU_YOLACT_SCORE_THRESHOLD;
int							inu_yolact_image_rows = 600; // 1200 or 600
int							inu_yolact_image_cols = 800; // 1600 or 800
extern YOLACT_PPG_infoT     yolact_post_params;
extern EvIntCondType        YOLACT_PPG_condition;
#endif

//#define	CNN_PROCESS_TIME	1
#if CNN_PROCESS_TIME
uint64_t evclock1, evclock2;
#endif

#ifdef USE_OPENCV_RESIZE
static bool use_opencv_resize = 1;
#else
static bool use_opencv_resize = 0;
#endif

#ifdef PROFILING_OUTPUT_TEST
static unsigned char profiling_buffer[PROFILING_BUFFER_SIZE];
#endif

extern CDNN_EVG_netInfoT CDNN_EVG_netInfo[CDNN_MAX_NETWORKS];

static InuGraphRunner *runner = NULL;
static int images_width[EV_CNN_MAX_IO_MAPS], images_height[EV_CNN_MAX_IO_MAPS], images_channels[EV_CNN_MAX_IO_MAPS];
static ev_cnn_user_flow_func user_flow_funcs[EV_CNN_MAX_USER_FLOWS] = {NULL};

static EV_CNN_PROCESS_IMAGES *pending_requests_fifo[EV_CNN_MAX_PROCESS_FIFO];
static InputData *input_images_pending[EV_CNN_MAX_PROCESS_FIFO];
static EV_CNN_PROCESS_IMAGES *in_progress_fifo[EV_CNN_MAX_PROCESS_FIFO];
static InputData *input_images_ongoing[EV_CNN_MAX_PROCESS_FIFO];
static uint8_t *output_areas[EV_CNN_MAX_PROCESS_FIFO] = {NULL};

static EV_CNN_INJECT_INFO *inject_info_pending[EV_CNN_MAX_PROCESS_FIFO] = {NULL};
static EV_CNN_INJECT_INFO *inject_info_ongoing[EV_CNN_MAX_PROCESS_FIFO] = {NULL};


static EvMutexType req_mutex;
static int inu_preprocess_thread_cpu = CNN_PRE_THREAD_CPU;
static int inu_pipe_depth = 1;
static int inu_ProcesssingFlags = 0;
static int in_progress_counter = 0;
static int pending_requests_counter = 0;
static int dropped_requests_counter = 0;
static int processed_counter = 0;
static int output_areas_counter = 0;
static bool load_net_done = false;
static bool release_net_request = false; // Indicates that a release net was requested
static int release_net_id;
static int release_net_jobDescriptor;
static bool release_net_ongoing = false;
static bool vision_proc_mode = false;
static bool print_on_action_cond = false;
static bool print_on_sleep = false;
static unsigned int inu_keep_alive_counter = 0;
static bool inu_keep_alive_enabled = false;

void* vdspAppThread(void* parameters);

extern  EvMutexType     CNNmutex;
extern  EvCondType      CNNcondition;
extern  UINT32          CNNCounter;
int current_net_id = -1;
static int ack_the_release_operation = false; // If implicit release operation, no need to ack
static EvIntCondType        action_Condition;
static EvIntCondType		release_Condition;
typedef void(*evAppCallback) (void);
extern evAppCallback appCallback;

// For Message Queue
CLIENT_APIG_msgQueT CNN_PRE_msgQue;

typedef void (*ev_cnn_callback) (const char *);
void register_cnn_log(ev_cnn_callback callback);

void inu_log_cnn(const char *text)
{
	LOGG_PRINT(LOG_ERROR_E, (ERRG_codeE)NULL, "inu_log_cnn - EV_FATAL: %s\n",text);	
}

#define MAX_MSG_LEN 70
static void inu_harness_log(int cpu, EV_CONTEXT_FROM context, const char* msg) {
	int len = strlen(msg);
	char line[MAX_MSG_LEN+1];
	for (int i = 0; i < len; i = i + MAX_MSG_LEN) {
		int len_to_copy = MIN(MAX_MSG_LEN, len - i);
		memcpy(line, &msg[i], len_to_copy);
		line[len_to_copy] = '\0';
		LOGG_PRINT(LOG_ERROR_E, (ERRG_codeE)NULL, "engine: %s\n", line);
	}

	if (evGetCpu() == 0) {
		LOGG_PRINT(LOG_ERROR_E, (ERRG_codeE)NULL, "inu_harness_log\n");
		inu_print_timing_info(true);
	}
}

const evHarness evSystemHarness = {
	NULL, inu_harness_log, NULL, NULL };

extern "C" int _write(int handle, const char* bufptr, unsigned int count)
{
	char line[60];
	memcpy(line, bufptr, 40);
	line[40] = 0;
	LOGG_PRINT(LOG_ERROR_E, (ERRG_codeE)NULL, "_write: %s\n", line);
	return count;
}
extern "C" ssize_t write(int handle, const char* bufptr, unsigned int count)
{
	return _write(handle, bufptr, count);
}



/* inu_malloc */
//void *inu_malloc(int size, char* file, int num)
void* inu_malloc(int size)
{
   void *ptr;
   //if(size > 100000) LOGG_PRINT(LOG_INFO_E, (ERRG_codeE)NULL, "inu_malloc size:%d %s %d\n", size, file+60, num); 
   ptr = evMemAlloc(size, EV_MR_GLOBAL_HEAP, -1);

   return(ptr);
}

/* inu_free */
void inu_free(void *ptr)
{
   //free(ptr);
   evMemFree(ptr);
   //LOGG_PRINT(LOG_INFO_E, (ERRG_codeE)NULL, "inu_free: ptr:%p\n", ptr);
}


/* evInuGetTime */
uint64_t evInuGetTime(void)
{
  uint64_t current;

  current = getRTC(); // getRTC , evGetTime
  return current;
}

/* free_input_images */
void free_input_images(InputData *inputs, EV_CNN_PROCESS_IMAGES *request)
{
   for(int image_id=0; image_id<request->number_of_images; image_id++) {
      // For images: Needs to free only input_plane[0], as the 3 planes are allocated with the same malloc
      // For general tensor: input is not allocated (just uysing the input pointer with no pre-process), so no need to free it
      if(inputs[image_id].input_plane[0] && !request->input_images[image_id].tensor_input) {
            //inu_free(inputs[image_id].input_plane[0]);
            evMemFree(inputs[image_id].input_plane[0]);
			//LOGG_PRINT(LOG_INFO_E, (ERRG_codeE)NULL, "Freed input_plane[0] for image %d\n", image_id);
      }
   }
}

/* inu_get_element_type_size */
int inu_get_element_type_size(EV_CNN_TYPE element_type)
{
   int size = 0;

   switch(element_type) {
     case EV_CNN_ELEMENT_TYPE_INT8:
     case EV_CNN_ELEMENT_TYPE_UINT8:
        size = 1;
        break;

     case EV_CNN_ELEMENT_TYPE_12INT16:
     case EV_CNN_ELEMENT_TYPE_12UINT16:
     case EV_CNN_ELEMENT_TYPE_16INT16:
     case EV_CNN_ELEMENT_TYPE_16UINT16:
        size = 2;
        break;

     case EV_CNN_ELEMENT_TYPE_FLOAT32:
     case EV_CNN_ELEMENT_TYPE_FLOAT64:
        size = 4;
        break;
   }

   return(size);
}




/* ev_inu_handle_injected_ack - Move the result to the injected result buffer & message */
void ev_inu_handle_injected_ack(EV_CNN_PROCESS_IMAGE_ACK *ack_msg, EV_CNN_INJECT_INFO *inject_info)
{
    static EV_CNN_PROCESS_IMAGE_ACK inject_ack;
    static bool first_set = true;

#ifndef INU_HAPS
	inu_cdnn_data__hdr_t *resultsHeaderP;
	inu_cdnn_data__tailHeaderT *tailHeaderP;
#endif

	uint64_t start, end;
	
	start = getRTC(); // getRTC , evGetTime
	
    /* First set */
    if(first_set) {
       inject_ack.msg_header.msg_code = EV_CNN_MSG_INJECT_IMAGES_ACK;
       inject_ack.msg_header.msg_len = sizeof(EV_CNN_PROCESS_IMAGE_ACK);
       inject_ack.net_id = ack_msg->net_id;
       inject_ack.frame_id = ack_msg->frame_id;
       inject_info->number_of_processed_sets = 0;
       inject_ack.num_of_tails = 0;
	   inject_ack.blob_output_header = ack_msg->blob_output_header;
	   inject_ack.jobDescriptor= ack_msg->jobDescriptor;
	   inject_ack.intCondSignal= ack_msg->intCondSignal;
       inject_info->current_injected_output_ptr = inject_info->total_injected_output_ptr;
	   //LOGG_PRINT(LOG_INFO_E, (ERRG_codeE)NULL, "handle injected first set\n");
      first_set = false;
    }

    for(int i=0; i<ack_msg->num_of_tails; i++) {
        int tail_num = inject_ack.num_of_tails + i;
        int size_of_tail;
         
		if (tail_num > EV_CNN_MAX_TAILS) {
			LOGG_PRINT(LOG_ERROR_E, (ERRG_codeE)NULL, "ev_inu_handle_injected_ack: Too many tails");
			break;
		}

        inject_ack.tails[tail_num] = ack_msg->tails[i];

        /* Move the result to the injected output result */
        size_of_tail = ack_msg->tails[i].elementSize * ack_msg->tails[i].elementCount;
        inject_ack.tails[tail_num].ptr = inject_info->current_injected_output_ptr;
        inject_info->current_injected_output_ptr += size_of_tail;
    }
	inject_ack.num_of_tails += ack_msg->num_of_tails;
	if (inject_ack.num_of_tails > EV_CNN_MAX_TAILS) {
		inject_ack.num_of_tails = EV_CNN_MAX_TAILS;
	}

    inject_info->number_of_processed_sets++;

    /* If we are done, send the reply */
    if(inject_info->number_of_processed_sets == inject_info->number_of_injected_sets) {
#ifndef INU_HAPS
		/* Handle yolo post processing */
		if (inu_ProcesssingFlags & PROC_FLAGS_YOLO_V3_POSTPROCESS || inu_ProcesssingFlags & PROC_FLAGS_YOLO_V7_POSTPROCESS) {
			static FixedOutput outputs[3];
			static int order_of_tails[3] = { 0, 1, 2 };
			static bool first_parse = true;
			static int image_rows = 960;//not used
			static int image_cols = 1280;//not used

			/* On first time, Determine tails order - They are in incrementing order in size */
			if (first_parse)
			{
				int index, tail_index, next_tail_index;
				first_parse = false;
				index = 0;
				while (index < inject_ack.num_of_tails-1)
				{
					tail_index = order_of_tails[index];
					next_tail_index = order_of_tails[index + 1];
					if (inject_ack.tails[next_tail_index].elementCount < inject_ack.tails[tail_index].elementCount)
					{
						int tmp = order_of_tails[index];
						order_of_tails[index] = order_of_tails[index + 1];
						order_of_tails[index + 1] = tmp;
						index = 0;
					}
					else
					{
						index++;
					}
				}
				if (inject_ack.num_of_tails == 3) {
				LOGG_PRINT(LOG_INFO_E, (ERRG_codeE)NULL, "Order of YOLO tails is: %d %d %d\n", order_of_tails[0], order_of_tails[1], order_of_tails[2]);
				}
				else {
					LOGG_PRINT(LOG_INFO_E, (ERRG_codeE)NULL, "Order of YOLO tails is: %d %d\n", order_of_tails[0], order_of_tails[1]);
				}
			}

			for (int i = 0; i < inject_ack.num_of_tails; i++) {
				int tail_index = order_of_tails[i];
				outputs[i].Y = inject_ack.tails[tail_index].height;
				outputs[i].X = inject_ack.tails[tail_index].width;
				outputs[i].scale = inject_ack.tails[tail_index].scale;

				outputs[i].blob = (short *)inject_ack.tails[tail_index].ptr;
				outputs[i].size = inject_ack.tails[tail_index].elementCount * inject_ack.tails[tail_index].elementSize;
				outputs[i].element_size = inject_ack.tails[tail_index].elementSize;

				LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL, "Y:%d X:%d scale:%f e_size:%d e_Count:%d ptr:%p\n", outputs[i].Y, outputs[i].X, outputs[i].scale, inject_ack.tails[tail_index].elementSize, inject_ack.tails[tail_index].elementCount, outputs[i].blob);
			}

			YOLO_V3_PPG_infoT    yolo_v3_post_params;
			yolo_v3_post_params.jobDescriptor = inject_ack.jobDescriptor;
			yolo_v3_post_params.outputs = outputs;
			yolo_v3_post_params.image_rows = image_rows;
			yolo_v3_post_params.image_cols = image_cols;
			yolo_v3_post_params.net_id = inject_ack.net_id;
			yolo_v3_post_params.frame_id = inject_ack.frame_id;
			yolo_v3_post_params.blob_output_header = inject_ack.blob_output_header;
			yolo_v3_post_params.output_ptr = inject_info->total_injected_output_ptr;
			if (inu_ProcesssingFlags & PROC_FLAGS_YOLO_V3_POSTPROCESS) {
				yolo_v3_post_params.version = 3;
			}
			else {
				yolo_v3_post_params.version = 7;
			}

			if (CLIENT_APIG_sendMsg(&yolo_v3_msgQue, (UINT8 *)&yolo_v3_post_params, sizeof(yolo_v3_post_params)) != SUCCESS_E)
			{
				LOGG_PRINT(LOG_ERROR_E, (ERRG_codeE)NULL, "Failed to send msg to yoloV3 thread\n");
				SCHEDG_pushFinishedJob(inject_ack.jobDescriptor);
				evIntCondSignal(&SCHEDG_Condition);
			}
			else {
				LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL, "Send to yoloV3 success\n");
			}
		}
		/* Handle ssd face post processing */
		else if (inu_ProcesssingFlags & PROC_FLAGS_FACE_SSD_POSTPROCESS) {
			static bool first_parse = true;
			static int locations_tail = 0;
			static int confidences_tail = 1;
			static int face_ssd_width=160, face_ssd_height=120, face_ssd_channels=3;
			static short *confidences=NULL;
			static short *locations=NULL;
			face_ssd_pp_infoT    face_ssd_post_params;

			/* On first time, determine which tail is the locations & which is the confidences */
			if (first_parse)
			{
				first_parse = false;

				if (inject_ack.tails[0].elementCount < inject_ack.tails[1].elementCount) {
					locations_tail = 1;
					confidences_tail = 0;
				}
				else {
					locations_tail = 0;
					confidences_tail = 1;
				}

				confidences = (short *)inu_malloc(sizeof(short)*MAX_TAIL_ELEMENTS * 2);
				locations = (short *)inu_malloc(sizeof(short)*MAX_TAIL_ELEMENTS * 4);
				if (!confidences || !locations) {
					LOGG_PRINT(LOG_ERROR_E, (ERRG_codeE)NULL, "Failed to locate mem for confidences:%p locations:%p\n", confidences, locations);
				}

				runner->getInputDimentions(0, face_ssd_width, face_ssd_height, face_ssd_channels);
				face_ssd_set_input_size(face_ssd_width);
				LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL, "face_ssd image w:%d h:%d\n", face_ssd_width, face_ssd_height);
			}

			LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL, "after face_ssd_pp_condition\n");
			face_ssd_post_params.jobDescriptor = inject_ack.jobDescriptor;
			face_ssd_post_params.confs = (short *)inject_ack.tails[confidences_tail].ptr;
			face_ssd_post_params.confs_scale = inject_ack.tails[confidences_tail].scale;
			face_ssd_post_params.locations = (short *)inject_ack.tails[locations_tail].ptr;
			face_ssd_post_params.locations_scale = inject_ack.tails[locations_tail].scale;
			face_ssd_post_params.image_rows = face_ssd_height;
			face_ssd_post_params.image_cols = face_ssd_width;
			face_ssd_post_params.num_of_classes = inject_ack.tails[confidences_tail].width;
			face_ssd_post_params.num_of_elements = inject_ack.tails[confidences_tail].height;
			face_ssd_post_params.net_id = inject_ack.net_id;
			face_ssd_post_params.frame_id = inject_ack.frame_id;
			face_ssd_post_params.blob_output_header = inject_ack.blob_output_header;
			face_ssd_post_params.output_ptr = inject_info->total_injected_output_ptr;
			memcpy(confidences, inject_ack.tails[confidences_tail].ptr, inject_ack.tails[confidences_tail].elementCount * inject_ack.tails[confidences_tail].elementSize);
			memcpy(locations, inject_ack.tails[locations_tail].ptr, inject_ack.tails[locations_tail].elementCount * inject_ack.tails[locations_tail].elementSize);
			face_ssd_post_params.confs = confidences;
			face_ssd_post_params.locations = locations;

			if (CLIENT_APIG_sendMsg(&face_ssd_msgQue, (UINT8 *)&face_ssd_post_params, sizeof(face_ssd_post_params)) != SUCCESS_E)
			{
				LOGG_PRINT(LOG_ERROR_E, (ERRG_codeE)NULL, "Failed to send msg to face ssd thread\n");
				SCHEDG_pushFinishedJob(inject_ack.jobDescriptor);
				evIntCondSignal(&SCHEDG_Condition);
			}
			else {
				LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL, "Send to face ssd success\n");
			}
		}
#ifdef VER_4100
		/* Handle yoloact post processing */
		else if (inu_ProcesssingFlags & PROC_FLAGS_YOLACT_POSTPROCESS) {
			/* Wait for the last post process to finish */
			yolact_mutex_lock();

			LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL, "after yolact_mutex_lock\n");
			yolact_post_params.jobDescriptor = inject_ack.jobDescriptor;
			yolact_post_params.num_of_tails = inject_ack.num_of_tails;
			memcpy(yolact_post_params.tails, inject_ack.tails, inject_ack.num_of_tails * sizeof(EV_CNN_TAIL));
			yolact_post_params.image_rows = inu_yolact_image_rows;
			yolact_post_params.image_cols = inu_yolact_image_cols;
			yolact_post_params.net_input_size = inu_yolact_network_dimension;
			yolact_post_params.net_id = inject_ack.net_id;
			yolact_post_params.frame_id = inject_ack.frame_id;
			yolact_post_params.blob_output_header = inject_ack.blob_output_header;
			yolact_post_params.output_ptr = inject_info->total_injected_output_ptr;

			yolact_mutex_unlock();
			evIntCondSignal(&YOLACT_PPG_condition); // Trigger the post processing thread
		} // end of PROC_FLAGS_YOLACT_POSTPROCESS
#endif
		else {
			resultsHeaderP = (inu_cdnn_data__hdr_t *)inject_ack.blob_output_header;
			tailHeaderP = resultsHeaderP->tailHeader;
			resultsHeaderP->network_id = inject_ack.net_id;
			resultsHeaderP->engineType = INU_LOAD_NETWORK__EV61_E;
			resultsHeaderP->frameId = inject_ack.frame_id;
			resultsHeaderP->numOfTails = inject_ack.num_of_tails;

			//Take the outputs
			for (int i = 0; i < resultsHeaderP->numOfTails; i++)
			{
				resultsHeaderP->tailHeader[i].elementSize = inject_ack.tails[i].elementSize;
				resultsHeaderP->tailHeader[i].elementCount = inject_ack.tails[i].elementCount;
				resultsHeaderP->tailHeader[i].inputs = inject_ack.tails[i].channels;
				resultsHeaderP->tailHeader[i].width = inject_ack.tails[i].width;
				resultsHeaderP->tailHeader[i].height = inject_ack.tails[i].height;
				resultsHeaderP->tailHeader[i].cnnFields.tailSynopsysFields.scale = inject_ack.tails[i].scale;
				resultsHeaderP->offsetToBlob[i] = (unsigned int)inject_ack.tails[i].ptr - (unsigned int)inject_ack.tails[0].ptr;
				LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL, "scale: %f size=%d count=%d\n", inject_ack.tails[i].scale, inject_ack.tails[i].elementSize, inject_ack.tails[i].elementCount);
				LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL, "cahnnels=%d width=%d height=%d\n", inject_ack.tails[i].channels, inject_ack.tails[i].width, inject_ack.tails[i].height);
			}
			resultsHeaderP->totOutputSize = runner->ev_cnn_get_total_output_size() * inject_info->number_of_injected_sets;

#ifdef INU_EVTRACER_ENABLED
			{
				unsigned char* trace_buffer;
				int size;
				size = runner->save_trace(&trace_buffer);
				LOGG_PRINT(LOG_INFO_E, (ERRG_codeE)NULL, "tracer buffer:%p size:%d\n", trace_buffer, size);
				resultsHeaderP->numOfTails = 1;
				resultsHeaderP->offsetToBlob[0] = 0;
				resultsHeaderP->tailHeader[0].elementSize = 1;
				resultsHeaderP->tailHeader[0].elementCount = size;
				resultsHeaderP->tailHeader[0].inputs = 1;
				resultsHeaderP->tailHeader[0].width = size;
				resultsHeaderP->tailHeader[0].height = 1;
				memcpy(inject_ack.tails[0].ptr, trace_buffer, size);
				evDataCacheFlushLines(inject_ack.tails[0].ptr, (unsigned char*)inject_ack.tails[0].ptr + size);
			}
#endif

#ifdef PROFILING_OUTPUT_TEST
			ev_cnn_status stat;
			unsigned int profiling_data_size=0;
			stat = runner->get_profiling(profiling_buffer, &profiling_data_size, PROFILING_BUFFER_SIZE);
			if (CNN_STAT_OK != stat) {
				LOGG_PRINT(LOG_INFO_E, (ERRG_codeE)NULL, "get_profiling Failed\n");
			}
			else {
				LOGG_PRINT(LOG_INFO_E, (ERRG_codeE)NULL, "get_profiling OK profiling_data_size:%d\n", profiling_data_size);
				resultsHeaderP->numOfTails = 1;
				resultsHeaderP->offsetToBlob[0] = 0;
				resultsHeaderP->tailHeader[0].elementSize = 1;
				resultsHeaderP->tailHeader[0].elementCount = profiling_data_size;
				resultsHeaderP->tailHeader[0].inputs = 1;
				resultsHeaderP->tailHeader[0].width = profiling_data_size;
				resultsHeaderP->tailHeader[0].height = 1;
				memcpy(inject_ack.tails[0].ptr, profiling_buffer, profiling_data_size);
				evDataCacheFlushLines(inject_ack.tails[0].ptr, (unsigned char *)inject_ack.tails[0].ptr + profiling_data_size);
			}
#endif

			LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL, "tails num %d total output size %d COND 0x%x 0x%x\n", resultsHeaderP->numOfTails, resultsHeaderP->totOutputSize, &SCHEDG_Condition, inject_ack.intCondSignal);
			/* Signel the vision process flow, or signal end of process */
			if (vision_proc_mode) {
				evIntCondSignal(inject_ack.intCondSignal);
				//inu_register_timing_info(start, getRTC(), "post_proc");
				LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL, "Signal end at: %lld\n", getRTC());
			}
			else {
				uint64_t start_flush = getRTC();
				evDataCacheFlushLines(resultsHeaderP, resultsHeaderP + 1); // Flush not required in vision_proc_mode
				inu_register_timing_info(start_flush, getRTC(), "flush_header");
				//evDataCacheFlushLines(inject_info->total_injected_output_ptr, (inject_info->total_injected_output_ptr) + (resultsHeaderP->totOutputSize));
				SCHEDG_pushFinishedJob(inject_ack.jobDescriptor);
				evIntCondSignal(inject_ack.intCondSignal);
			}
		}
#endif
			   
       inu_free(inject_info_ongoing[0]);
       first_set = true;
	   //inu_register_timing_info(getRTC(), getRTC(), "end ACK");
    }
}

/* ev_inu_release_net */
void ev_inu_release_net(void)
{
	uint64_t start, end;

	LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL, "ev_inu_releae_net \n");


	start = getRTC();

    evMutexLock(&req_mutex); // Start lock critical section
	delete runner;

	runner = NULL;
	load_net_done = false;
	release_net_request = false;
	current_net_id = -1;

	if (ack_the_release_operation) {
		LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL,"ev_inu_process_thread: Released net\n");	
#ifndef INU_HAPS
		SCHEDG_pushFinishedJob(release_net_jobDescriptor);
		evIntCondSignal(&SCHEDG_Condition);
	}
	else {
		LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL, "ev_inu_process_thread: Implicit Released net: %d\n", release_net_id);
	}
#endif
	evMutexUnLock(&req_mutex); // Start lock critical section
	release_net_ongoing = false;
	LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL, "End of ev_inu_releae_net \n");

	end = getRTC();
	LOGG_PRINT(TIME_DEBUG_INFO, (ERRG_codeE)NULL, "release time is cycles:%lld %fms   from:%lld to:%lld\n",  (end - start), (float)(end - start) / CYCLES_PER_MS, start, end);
}

/* ev_inu_handle_pipe */
void ev_inu_handle_pipe(void)
{
   ev_cnn_status pushStatus, pullStatus, status;
   EV_CNN_PROCESS_IMAGE_ACK ack_msg;

     /* Do nothiong if there is no runner active (No NET was loaded) */
      if(!runner || !load_net_done) {
        LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL,"ev_inu_process_thread: runner is not there\n");
		evSleepThread(CYCLES_PER_MS);
		return;
      }
      //inu_debug_to_ddr(0xABAB0000);
      /* If a release request is pending, clear the pending requests and wait until all ongoing are done, before releasing the net */
      if(release_net_request) {
         LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL, "ev_inu_handle_pipe: release_net_request\n");
         evMutexLock(&req_mutex); // Start lock critical section

         /* Release the net, if no more ongoing requests */
         LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL,"Release: in_progress_counter: %d\n", in_progress_counter);
         if(in_progress_counter == 0 && pending_requests_counter == 0) {
			  release_net_ongoing = true;
			  release_net_request = false;
			  evIntCondSignal(&release_Condition);
         }

         evMutexUnLock(&req_mutex); // End lock critical section

         if(!runner) {
			 evSleepThread(CYCLES_PER_MS);
            return;
         }
      }
	  
	  if(release_net_ongoing) return;
	  
      /* Get status of process */
      status = runner->getProcessingStatus(&pushStatus, &pullStatus);
      if(status != CNN_STAT_OK) {
         //inu_debug_to_ddr(0xABAB0099);
         inu_debug_to_ddr(status );
         LOGG_PRINT(LOG_ERROR_E, (ERRG_codeE)NULL,"ev_inu_process_thread: Failed to get evCnnStreamProcess\n");
         return;
      }


      LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL, "Status  push:%d  pull:%d\n", pushStatus, pullStatus);
      LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL, "pending:%d  in_progress:%d  dropped:%d  processed:%d\n", pending_requests_counter, in_progress_counter, dropped_requests_counter, processed_counter);

      /* Pull the next output if ready */
      if(pullStatus == CNN_STAT_STRM_OUT_RDY && in_progress_counter > 0) {
  		//inu_debug_to_ddr(0xABABDBDB);
         processed_counter++;
//	         ack_msg.msg_header.msg_code = EV_CNN_MSG_PROCESS_IMAGE_ACK;
         ack_msg.msg_header.msg_len = sizeof(EV_CNN_PROCESS_IMAGE_ACK);
         ack_msg.status = EV_CNN_STATUS_OK;
         ack_msg.net_id = in_progress_fifo[0]->net_id;
         ack_msg.frame_id = in_progress_fifo[0]->frame_id;
         ack_msg.blob_output_header = in_progress_fifo[0]->blob_output_header;
         ack_msg.jobDescriptor = in_progress_fifo[0]->jobDescriptor;
         ack_msg.intCondSignal= in_progress_fifo[0]->intCondSignal;
         runner->pullOneSet(&ack_msg);

#if CNN_PROCESS_TIME
	   evclock2 = getRTC();

//		   float ev_clock_rate = CYCLES_PER_MS;
	        float ev_clock_rate;
           static int process_time_counter = 0;
           if(process_time_counter == 0)
			  {
//		           ev_clock_rate = CYCLES_PER_MS;
	           ev_clock_rate = EV_INITG_getEvFreqMhz();
			   LOGG_PRINT(LOG_INFO_E, (ERRG_codeE)NULL, "proc time (%dMhz) net:%d is cycles:%lld %fms from:%lld to:%lld\n", (int)ev_clock_rate, ack_msg.net_id, (evclock2 - evclock1), (float)(evclock2 - evclock1) / (ev_clock_rate*1000), evclock1, evclock2);
			   inu_register_timing_info(evclock1, evclock2, "cnn proc time");
           }
           process_time_counter++;
           if(process_time_counter >= 0) {
              process_time_counter = 0;
           }
#endif
			
         inu_debug_to_ddr(0xABABDBDB);
         inu_debug_to_ddr(in_progress_fifo[0]->frame_id);
         inu_debug_to_ddr(_lr(AUX_RTC_LOW));
		 inu_debug_to_ddr(_lr(AUX_RTC_HIGH));

         if(inject_info_ongoing[0]) {
             ev_inu_handle_injected_ack(&ack_msg, inject_info_ongoing[0]);
         }
         inu_debug_to_ddr(0xBBCCDDDD);
		 inu_debug_to_ddr(in_progress_fifo[0]->frame_id);
         inu_debug_to_ddr(_lr(AUX_RTC_LOW));
		 inu_debug_to_ddr(_lr(AUX_RTC_HIGH));

         /* Push back list of ongoing requests */
         free_input_images(input_images_ongoing[0], in_progress_fifo[0]);
         inu_free(input_images_ongoing[0]);
         inu_free(in_progress_fifo[0]);
         inject_info_ongoing[0] = NULL;

         for(int i=0; i<in_progress_counter-1; i++) {
             in_progress_fifo[i] = in_progress_fifo[i+1];
             input_images_ongoing[i] = input_images_ongoing[i+1];
             inject_info_ongoing[i] = inject_info_ongoing[i+1];
         }
         in_progress_counter--;

         LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL,"ev_inu_process_thread: PULLED a request from pipe. in_progress_counter:%d\n", in_progress_counter);
      }

      /* Push the next pending request if possible */
      if(pushStatus == CNN_STAT_STRM_IN_REQ && pending_requests_counter > 0 && in_progress_counter < inu_pipe_depth) {
         evMutexLock(&req_mutex); // Start lock critical section



         in_progress_fifo[in_progress_counter] = pending_requests_fifo[0]; // Move to in_progress
         input_images_ongoing[in_progress_counter] = input_images_pending[0];
         output_areas[output_areas_counter] = in_progress_fifo[in_progress_counter]->blob_output_address;
         inject_info_ongoing[in_progress_counter] = inject_info_pending[0];
         output_areas_counter++;

         inu_debug_to_ddr(0xBBCCDDEE);
		 inu_debug_to_ddr(in_progress_fifo[0]->frame_id);
         inu_debug_to_ddr(_lr(AUX_RTC_LOW));
		 inu_debug_to_ddr(_lr(AUX_RTC_HIGH));

#if CNN_PROCESS_TIME
	evclock1 = getRTC();
#endif
         status = runner->pushOneSet(input_images_ongoing[in_progress_counter]);
         in_progress_counter++;

         /* Push back list of pending requests */
         for(int i=0; i<pending_requests_counter-1; i++) {
             pending_requests_fifo[i] = pending_requests_fifo[i+1];
             input_images_pending[i] = input_images_pending[i+1];
             inject_info_pending[i] = inject_info_pending[i+1];
         }
         pending_requests_counter--;
         input_images_pending[pending_requests_counter] = NULL;
         input_images_pending[pending_requests_counter] = NULL;
         inject_info_pending[pending_requests_counter] = NULL;

         LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL,"PUSHED request to pipe. pending_requests:%d  in_progress:%d at:%lld\n", pending_requests_counter, in_progress_counter, getRTC());

         evMutexUnLock(&req_mutex); // End lock critical section

      }


      /* Prepare the next output */
      if((pullStatus == CNN_STAT_STRM_OUT_RDY || pullStatus == CNN_STAT_STRM_OUT_REQ) && in_progress_counter > 0)
      {
         status = runner->prepareOutputSet(output_areas[0]);
         if(status != CNN_STAT_OK) {
            LOGG_PRINT(LOG_ERROR_E, (ERRG_codeE)NULL,"ev_inu_process_thread: prepareOutputSet failed\n");
         }
         /* Push back the output areas */
         for(int i=0; i<output_areas_counter-1; i++) {
             output_areas[i] = output_areas[i+1];
         }
         output_areas_counter--;
      }
	  
}

/* ev_inu_set_keep_alive */
void ev_inu_set_keep_alive(bool state)
{
	inu_keep_alive_enabled = state;
}

/* ev_inu_reset_keep_alive_counter */
void ev_inu_reset_keep_alive_counter(void)
{
	inu_keep_alive_counter = 0;
}

/* ev_inu_keep_alive_thread */
static void* ev_inu_keep_alive_thread(void* arg)
{
	float ev_clock_rate = EV_INITG_getEvFreqMhz();
	int num_of_ms_to_sleep = 100; // sleep for 100ms
	int print_every_num_of_secs = 10;
	unsigned int cycles_to_sleep = ev_clock_rate * 1000 * num_of_ms_to_sleep;
	int counter_to_print = 1000 / num_of_ms_to_sleep * print_every_num_of_secs; // Print if reached that counter

	LOGG_PRINT(LOG_INFO_E, (ERRG_codeE)NULL, "ev_inu_keep_alive_thread thread started on core %d FreqMhz:%d\n", evGetCpu(), (int)ev_clock_rate);

	while (1) {
		evSleepThread(cycles_to_sleep);
		if (inu_keep_alive_enabled) {
			if (inu_keep_alive_counter >= counter_to_print) {
				LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL, "ARC is alive. in_progress:%d\n", in_progress_counter);
				inu_keep_alive_counter = 0;
			}
			inu_keep_alive_counter++;
		}
	}
}



/* ev_inu_preprocess_thread */
static void *ev_inu_preprocess_thread(void *arg)
{
	int status;
	EV_CNN_INJECT_IMAGES *new_pp_message = NULL;
	UINT32  localThreadParamsSize;
	uint64_t pre_start_sleep, pre_wakeup = 0;

	LOGG_PRINT(LOG_INFO_E, (ERRG_codeE)NULL, "CNN preprocess thread started on core %d\n", evGetCpu());
	localThreadParamsSize = sizeof(new_pp_message);
	while (1)
	{
		pre_start_sleep = getRTC();
		status = CLIENT_APIG_recvMsg(&CNN_PRE_msgQue, (UINT8*)&new_pp_message, &localThreadParamsSize, 0 /*not used*/);
		if (status == SUCCESS_E) {
			pre_wakeup = getRTC();
			//inu_register_timing_info(pre_start_sleep, pre_wakeup, "pre wakeup. Sleep:");
			LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL, "ev_inu_preprocess_thread: Got req at: %lld\n", getRTC());
			switch (new_pp_message->msg_header.msg_code) {
			case EV_CNN_MSG_INJECT_IMAGES:
				{
					status = ev_inu_inject_request(new_pp_message);
					if (status != CNN_STAT_OK) {
						LOGG_PRINT(LOG_ERROR_E, (ERRG_codeE)NULL, "ev_inu_preprocess_thread: failed to add an inject request\n");
					}
					else {
						LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL, "ev_inu_preprocess_thread: inject was OK\n");
					}
				}
				break;

			case EV_CNN_MSG_LOAD_NET:
				{
					uint64_t start_load = getRTC();
					status = ev_inu_handle_load_net((EV_CNN_LOAD_NET*)new_pp_message);
					if (status != CNN_STAT_OK) {
						LOGG_PRINT(LOG_ERROR_E, (ERRG_codeE)NULL, "ev_inu_preprocess_thread: ev_inu_handle_load_net failed\n");
					}
					else {
						LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL, "ev_inu_preprocess_thread: ev_inu_handle_load_net was OK\n");
					}

					uint64_t end_load = getRTC();
					inu_register_timing_info(start_load, end_load, "switch net for next frame");

					free(new_pp_message);
				}
				break;

			case EV_CNN_MSG_RELEASE_NET:
				ev_inu_handle_release_net_pipe((EV_CNN_RELEASE_NET*)new_pp_message);
				free(new_pp_message);
				break;
			}
		}
	}
}

/* ev_inu_process_thread */
static void * ev_inu_process_thread(void *arg)
{
    LOGG_PRINT(LOG_INFO_E, (ERRG_codeE)NULL, "CNN thread started on core %d EVTH_MAX_PRIORITIES:%d\n", evGetCpu(), EVTH_MAX_PRIORITIES);
    while(1)
    {
		/* If a frame is in progress, this thread will be waked up whenever a CNN stage is done. */
		/* If no frame is in progress, we wait for something to triggeer it. Like: new frame or release request */
		if (in_progress_counter == 0) {
			evIntCondWait(&action_Condition);
		}
        ev_inu_handle_pipe();
#if USE_CNN_THREAD_TRIGGERING
        evMutexLock(&CNNmutex);
        if (CNNCounter == 0)
        {
            evCondTimedWait(&CNNcondition, &CNNmutex, 500000);
        }

        if (CNNCounter)
        {
            CNNCounter--;
        }
        evMutexUnLock(&CNNmutex);
#else
        //evYieldThread();
#endif
   }
}

/* ev_inu_inject_request */
int ev_inu_inject_request(EV_CNN_INJECT_IMAGES *inject_req)
{
    int status = EV_CNN_STATUS_FAIL;
    int element_size, total_size;
    unsigned char *p;
    EV_CNN_PROCESS_IMAGES *one_request;
    EV_CNN_INJECT_INFO *inject_info = NULL;
	uint64_t start_prep, end_prep;

    LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL,"ev_inu_inject_request: blob_output_address:%p  number_of_injected_sets:%d\n", inject_req->blob_output_address, inject_req->number_of_injected_sets);
	if (inject_req->net_id != current_net_id) {
		EV_CNN_LOAD_NET_ACK *load_net_ack;
		EV_CNN_LOAD_NET load_net_message;
		LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL, "ev_inu_inject_request: requested to load diff net: %d  current_net_id:%d\n", inject_req->net_id, current_net_id);
		if (current_net_id != -1) {
			inu_release_and_wait(current_net_id);
		}
		load_net_message.net_id = inject_req->net_id;
		load_net_message.bin_address = CDNN_EVG_netInfo[inject_req->net_id].bin_address;
		load_net_message.bin_size = CDNN_EVG_netInfo[inject_req->net_id].bin_size;
		load_net_message.pipe_depth = CDNN_EVG_netInfo[inject_req->net_id].pipe_depth;
		load_net_message.ProcesssingFlags = CDNN_EVG_netInfo[inject_req->net_id].ProcesssingFlags;
		uint64_t start_load = getRTC();
		if (ev_inu_handle_load_net(&load_net_message) != CNN_STAT_OK) {
			return(status); // Failed
		}
		uint64_t end_load = getRTC();
		inu_register_timing_info(start_load, end_load, "switch_net");
	}

	start_prep = getRTC();

    inject_info = (EV_CNN_INJECT_INFO *)inu_malloc(sizeof(EV_CNN_INJECT_INFO));
    if(!inject_info) {
           LOGG_PRINT(LOG_ERROR_E, (ERRG_codeE)NULL,"Failed to allocate memory to inject_info\n");
           inu_free(inject_req);
           return(status);
    }

    inject_info->number_of_injected_sets = inject_req->number_of_injected_sets;
    inject_info->number_of_processed_sets = 0;

     /* Allocated internally the blob output address */
     if(!inject_req->blob_output_address) {
        total_size = runner->ev_cnn_get_total_output_size() * inject_info->number_of_injected_sets;
        LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL,"ev_inu_inject_request: Total output size to allocate: %d * %d = %d\n", runner->ev_cnn_get_total_output_size(), inject_info->number_of_injected_sets, total_size);
        inject_info->total_injected_output_ptr = (unsigned char *)inu_malloc(total_size);
     
        if(inject_info->total_injected_output_ptr) {
           inject_req->blob_output_address = inject_info->total_injected_output_ptr;
        }
        else {
           LOGG_PRINT(LOG_ERROR_E, (ERRG_codeE)NULL,"Failed to allocate memory to total injected output of size: %d\n", total_size);
           inu_free(inject_info);
           inu_free(inject_req);
           return(status);
        }
        inject_info->internal_allocation = true;
     }
     /* blob output address allocated by the caller */
     else {
        inject_info->internal_allocation = false;
        inject_info->total_injected_output_ptr = inject_req->blob_output_address;
     }
     inject_info->current_injected_output_ptr = inject_info->total_injected_output_ptr;

   /* For each request, build a EV_CNN_PROCESS_IMAGES request and use the ev_inu_new_request function to process it */
   for(int i=0; i<inject_info->number_of_injected_sets; i++) {
       one_request = (EV_CNN_PROCESS_IMAGES *)inu_malloc(sizeof(EV_CNN_PROCESS_IMAGES));
       if(!one_request) {
	   LOGG_PRINT(LOG_ERROR_E, (ERRG_codeE)0, "ev_inu_inject_request: Failed to allocate for EV_CNN_PROCESS_IMAGES number_of_injected_sets:%d\n", inject_info->number_of_injected_sets);
           inu_free(inject_req);
          return(status);
       }

//	       one_request->msg_header.msg_code = EV_CNN_MSG_PROCESS_IMAGES;
       one_request->msg_header.msg_len = sizeof(EV_CNN_PROCESS_IMAGES);
       one_request->net_id = inject_req->net_id;
       one_request->number_of_images = inject_req->number_of_images_in_a_set;
       one_request->frame_id = inject_req->frame_id;
       one_request->blob_output_header = inject_req->blob_output_header;
       one_request->jobDescriptor = inject_req->jobDescriptor;
       one_request->intCondSignal= inject_req->intCondSignal;

       inu_debug_to_ddr(0xABAB0111);
       inu_debug_to_ddr(inject_req->frame_id);
       inu_debug_to_ddr(_lr(AUX_RTC_LOW));
       inu_debug_to_ddr(_lr(AUX_RTC_HIGH));
       for(int image_in_set=0; image_in_set<inject_req->number_of_images_in_a_set; image_in_set++) {
            one_request->input_images[image_in_set].channels = inject_req->input_sets[image_in_set].channels;
            one_request->input_images[image_in_set].width = inject_req->input_sets[image_in_set].width;
            one_request->input_images[image_in_set].height = inject_req->input_sets[image_in_set].height;
            one_request->input_images[image_in_set].channel_swap = inject_req->input_sets[image_in_set].channel_swap;
            one_request->input_images[image_in_set].resize = inject_req->input_sets[image_in_set].resize;
            one_request->input_images[image_in_set].yuv_format = inject_req->input_sets[image_in_set].yuv_format;
            one_request->input_images[image_in_set].tensor_input = inject_req->input_sets[image_in_set].tensor_input;
			one_request->input_images[image_in_set].num_of_tensor_pointers = inject_req->input_sets[image_in_set].num_of_tensor_pointers;
			memcpy(one_request->input_images[image_in_set].tensor_pointers, inject_req->input_sets[image_in_set].tensor_pointers, inject_req->input_sets[image_in_set].num_of_tensor_pointers * sizeof(void *));
            one_request->input_images[image_in_set].element_type = inject_req->input_sets[image_in_set].element_type;
            
            /* Calculate size of an image in the set */
            element_size = inu_get_element_type_size(inject_req->input_sets[image_in_set].element_type) * inject_req->input_sets[image_in_set].channels * inject_req->input_sets[image_in_set].width * inject_req->input_sets[image_in_set].height;

            p = (unsigned char *)inject_req->input_sets[image_in_set].images_ptr + (element_size * i);
            one_request->input_images[image_in_set].image_ptr = (void *)p;
            LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL,"Set: %d size of one injected image: %d base:ptr:%p  image_ptr:%p\n", i, element_size, inject_req->input_sets[image_in_set].images_ptr, p);

            /* Calculate the next output size */
            one_request->blob_output_address = inject_req->blob_output_address + i * runner->ev_cnn_get_total_output_size();

			LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL, "***inject: c:%d w:%d h:%d tensor:%d(pointers: %d) elem:%d image:%p blob_output_address:%p\n",
				one_request->input_images[image_in_set].channels,
				one_request->input_images[image_in_set].width,
				one_request->input_images[image_in_set].height,
				one_request->input_images[image_in_set].tensor_input,
				one_request->input_images[image_in_set].num_of_tensor_pointers,
				one_request->input_images[image_in_set].element_type,
				one_request->input_images[image_in_set].image_ptr,
				one_request->blob_output_address);
       }

       status = ev_inu_new_request(one_request, inject_info);
       if(status) {
	       LOGG_PRINT(LOG_ERROR_E, (ERRG_codeE)NULL, "ev_inu_inject_request: ev_inu_new_request failed\n");
       }

	   print_on_action_cond = true;
	   evIntCondSignal(&action_Condition);
   }

   inu_free(inject_req);

   end_prep = getRTC();
   float ev_clock_rate = EV_INITG_getEvFreqMhz();
   LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL, "pre time (%dMhz) is cycles:%lld %fms from %lld to:%lld\n", (int)ev_clock_rate, (end_prep - start_prep), (float)(end_prep - start_prep) / (ev_clock_rate * 1000), start_prep, end_prep);
   inu_register_timing_info(start_prep, end_prep, "cnn pre time");
   return(status);
}

/* ev_inu_new_request */
int ev_inu_new_request(EV_CNN_PROCESS_IMAGES *new_req, EV_CNN_INJECT_INFO *inject_info)
{
    int status = EV_CNN_STATUS_FAIL;

    LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL,"ev_inu_new_request start...\n");

    inu_debug_to_ddr(0x01010101);

    /* If a release net request is pending, drop the request */
    if(release_net_request) {
       dropped_requests_counter++;
       
       inu_free(new_req);
       LOGG_PRINT(LOG_ERROR_E, (ERRG_codeE)NULL, "ev_inu_new_request: Failed to to pending release request\n");
       return(status);
    }

    evMutexLock(&req_mutex);

    inu_debug_to_ddr(0x02020202);
    /* If queue is full, remove oldest request (The assumption is that injected requests at this mode has only 1 image_set to process) */
    if(pending_requests_counter >= EV_CNN_MAX_PROCESS_FIFO) {
         /* Free the odlest request */
         free_input_images(input_images_pending[0], pending_requests_fifo[0]);
         inu_free(input_images_pending[0]);
         inu_free(pending_requests_fifo[0]);
         if(inject_info_pending[0]) {
            inu_free(inject_info_pending[0]);
            inject_info_pending[0] = NULL;
         }

         /* Push back list of pending requests */
         for(int i=0; i<pending_requests_counter-1; i++) {
             pending_requests_fifo[i] = pending_requests_fifo[i+1];
             input_images_pending[i] = input_images_pending[i+1];
             inject_info_pending[i] = inject_info_pending[i+1];
         }
         pending_requests_counter--;
         dropped_requests_counter++;
         LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL,"Removed oldest pending request. dropped:%d pending:%d\n", dropped_requests_counter, pending_requests_counter);
    }

    inject_info_pending[pending_requests_counter] = inject_info;
    pending_requests_fifo[pending_requests_counter] = new_req;
    LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL,"Saved new request into pending_requests_fifo into pos:%d\n", pending_requests_counter);
    status = ev_prepare_req_to_pipe(new_req, pending_requests_counter);

	pending_requests_counter++;
	inu_debug_to_ddr(0x03030303);
    evMutexUnLock(&req_mutex);
    return(status);
}

/* ev_prepare_req_to_pipe */
EV_CNN_STATUS_INT ev_prepare_req_to_pipe(EV_CNN_PROCESS_IMAGES *process_image_msg, int pending_index)
{
   EV_CNN_STATUS_INT status = EV_CNN_STATUS_OK;
   cv::Mat input_image, input_resized, bgr_mats[EV_MAX_INPUT_IMAGES_PER_CNN][3];
   int data_type = CV_8UC3;
   int image_id = 0, size;
   bool do_yuv_split;
   uint8_t *yuv_split[3] = { NULL, NULL, NULL };
   int scaler_frame_pixels=0;
   int yuv_alloc_size = 0;
   uint64_t start_prep, end_prep;
   float ev_clock_rate = EV_INITG_getEvFreqMhz();
   bool image_data_modification = true;


   start_prep = getRTC();

#ifndef USE_OPENCV_RESIZE
   char  *out_p;
   void* thread_ret;
   cv::Mat my_resized;
   my_image_t in_image, out_image;
#endif

   input_images_pending[pending_index] = (InputData*)inu_malloc(sizeof(InputData) * EV_MAX_INPUT_IMAGES_PER_CNN);
   LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL,"ev_prepare_req_to_pipe (%d)...\n", process_image_msg->number_of_images);
   LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL, "Tensor pointers: %d  %X %X %X %X\n",
	   process_image_msg->input_images[0].num_of_tensor_pointers,
	   process_image_msg->input_images[0].tensor_pointers[0],
	   process_image_msg->input_images[0].tensor_pointers[1],
	   process_image_msg->input_images[0].tensor_pointers[2],
	   process_image_msg->input_images[0].tensor_pointers[3]);

   LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL, "net: %d flag:%d num:%d images:%d w:%d h:%d\n", process_image_msg->net_id, CDNN_EVG_netInfo[process_image_msg->net_id].ProcesssingFlags, runner->ev_cnn_num_of_inputs(), process_image_msg->number_of_images, process_image_msg->input_images[1].width, process_image_msg->input_images[1].height);

   /* If processing yolact and we have 2 images, set the yolact image size according to the second image. The second image is ignored */
   if ((CDNN_EVG_netInfo[process_image_msg->net_id].ProcesssingFlags & PROC_FLAGS_YOLACT_POSTPROCESS) && process_image_msg->number_of_images > 1) {
	   set_yolact_input_image_size(process_image_msg->input_images[1].width, process_image_msg->input_images[1].height);
   }

   process_image_msg->number_of_images = runner->ev_cnn_num_of_inputs(); // Limit number of images to the CNN

   for (image_id = 0; image_id < process_image_msg->number_of_images; image_id++) {
	   do_yuv_split = false;
	   image_data_modification = true; // Assume that some resize/split/yuv2rgb etc.. is done.

	   /* Handle general tensor case */
	   if (process_image_msg->input_images[image_id].tensor_input) {

		   runner->getInputDimentions(image_id, images_width[image_id], images_height[image_id], images_channels[image_id]);
		   LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL, "tensor channels:%d w:%d h:%d\n", images_channels[image_id], images_width[image_id], images_height[image_id]);

		   /* Check for valid tensor size */
		   if (process_image_msg->input_images[image_id].channels != images_channels[image_id] || 
			   process_image_msg->input_images[image_id].width != images_width[image_id] || 
			   process_image_msg->input_images[image_id].height != images_height[image_id]  ) {
			   LOGG_PRINT(LOG_ERROR_E, (ERRG_codeE)NULL, "Invalid tensor size image:%d c:%d w:%d h:%d (Expected c:%d w:%d h:%d)\n", image_id, 
				   process_image_msg->input_images[image_id].channels, process_image_msg->input_images[image_id].width, process_image_msg->input_images[image_id].height,
				   images_channels[image_id], images_width[image_id], images_height[image_id]);
			   return(EV_CNN_STATUS_FAIL);
		   }

		   if (process_image_msg->input_images[image_id].channels > EV_CNN_MAX_TENSOR_PLANES) {
			   LOGG_PRINT(LOG_ERROR_E, (ERRG_codeE)NULL, "Invalid tensor image:%d channels:%d > %d\n", image_id, process_image_msg->input_images[image_id].channels, EV_CNN_MAX_TENSOR_PLANES);
			   return(EV_CNN_STATUS_FAIL);
		   }

		   if (process_image_msg->input_images[image_id].num_of_tensor_pointers) {
			   LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL, "Tensor pointers: %d\n", process_image_msg->input_images[image_id].num_of_tensor_pointers);
			   for (int i = 0; i < process_image_msg->input_images[image_id].num_of_tensor_pointers; i++) {
				  input_images_pending[pending_index][image_id].input_plane[i] = process_image_msg->input_images[image_id].tensor_pointers[i];
			   }
		   }
		   else {
			   for (int i = 0; i < process_image_msg->input_images[image_id].channels; i++) {
				   if (process_image_msg->input_images[image_id].element_type == EV_CNN_ELEMENT_TYPE_UINT8 || process_image_msg->input_images[image_id].element_type == EV_CNN_ELEMENT_TYPE_INT8) {
					   LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL, "Tensor channel %d 8 bits W:%d H:%d\n", i, process_image_msg->input_images[image_id].width, process_image_msg->input_images[image_id].height);
					   uchar *data = (uchar*)process_image_msg->input_images[image_id].image_ptr;
					   input_images_pending[pending_index][image_id].input_plane[i] = &data[process_image_msg->input_images[image_id].width *process_image_msg->input_images[image_id].height* i];
				   }
				   else {
					   LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL, "channel %d 16 bits\n", i);
					   unsigned short *data = (unsigned short*)process_image_msg->input_images[image_id].image_ptr;
					   input_images_pending[pending_index][image_id].input_plane[i] = &data[process_image_msg->input_images[image_id].width *process_image_msg->input_images[image_id].height* i];
				   }
			   }
		   }
		   LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL, "Final: %X %X %X %X\n",
			   input_images_pending[pending_index][image_id].input_plane[0],
			   input_images_pending[pending_index][image_id].input_plane[1],
			   input_images_pending[pending_index][image_id].input_plane[2],
			   input_images_pending[pending_index][image_id].input_plane[3]);
		   input_images_pending[pending_index][image_id].element_type = (_ev_cnn_element_type_e)process_image_msg->input_images[image_id].element_type;
		   input_images_pending[pending_index][image_id].layout_type = CNN_LAYOUT_3DFMAPS;
		   continue;
	   }

      /* Prepare the input image into the Mat */
      if(process_image_msg->input_images[image_id].channels == 3 && (process_image_msg->input_images[image_id].element_type == EV_CNN_ELEMENT_TYPE_UINT8 || process_image_msg->input_images[image_id].element_type == EV_CNN_ELEMENT_TYPE_INT8)) {
		LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL,"input_type: CV_8UC3\n");
		data_type = CV_8UC3;
      }
      else if (process_image_msg->input_images[image_id].channels == 1 && (process_image_msg->input_images[image_id].element_type == EV_CNN_ELEMENT_TYPE_12UINT16 ||
	                                                                      process_image_msg->input_images[image_id].element_type == EV_CNN_ELEMENT_TYPE_12INT16) ) {

           /* Convert 8 bits to RGB (888)*/
           if(inu_ProcesssingFlags & PROC_FLAGS_CONVERT_TO_888) {
			static uchar *fe_buffer = NULL;
			static int fe_buffer_size = 0;
			int size;

	        LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL, "Convert 12 bits to 888\n");
		  size = process_image_msg->input_images[image_id].width * process_image_msg->input_images[image_id].height * 3;

		  /* Allocate new buffer if not allocated yet or requires different size */
		  if (fe_buffer_size != size) {
			  if (fe_buffer) inu_free(fe_buffer); // Free previouse allocation. Image requires different size
			  LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL, "Allocating bufer for FE/IR size: %d  width:%d height:%d\n", size, process_image_msg->input_images[image_id].width, process_image_msg->input_images[image_id].height);
			  fe_buffer = (uchar *)inu_malloc(size);
			  if (!fe_buffer) {
				  LOGG_PRINT(LOG_ERROR_E, (ERRG_codeE)NULL, "Failed to allocate for fe_buffer\n");
				  fe_buffer_size = 0;
				  return(EV_CNN_STATUS_FAIL);
			  }
			  fe_buffer_size = size;
		  }
		  
		  unsigned short *data = (unsigned short*)process_image_msg->input_images[image_id].image_ptr;
		  data_type = CV_8UC3;
		  process_image_msg->input_images[image_id].image_ptr = fe_buffer;
		  process_image_msg->input_images[image_id].channels = 3;
		  process_image_msg->input_images[image_id].element_type = EV_CNN_ELEMENT_TYPE_UINT8;
		  process_image_msg->input_images[image_id].yuv_format = 0;
		  for (int h = 0; h<process_image_msg->input_images[image_id].height; h++) {
			  for (int w = 0; w<process_image_msg->input_images[image_id].width; w++) {
				  int pos = h * process_image_msg->input_images[image_id].width + w;
				  unsigned char val = (unsigned char)(data[pos] >> 2);
				  fe_buffer[pos * 3] = val;
				  fe_buffer[pos * 3 + 1] = val;
				  fe_buffer[pos * 3 + 2] = val;
			  }
			}
		}
      	}
		else if (process_image_msg->input_images[image_id].channels == 1 && (process_image_msg->input_images[image_id].element_type == EV_CNN_ELEMENT_TYPE_UINT8 ||
			    process_image_msg->input_images[image_id].element_type == EV_CNN_ELEMENT_TYPE_INT8)) {

			   LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL,"input_type: CV_8UC1\n");
			   data_type = CV_8UC1;
			   /* Convert 10 bits to RGB (888)*/
			   if (inu_ProcesssingFlags & PROC_FLAGS_CONVERT_TO_888) {
				   static uchar* rgb_buffer = NULL;
				   static int rgb_buffer_size = 0;
				   int size;

				   LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL, "Convert GRAY to RGB\n");
				   size = process_image_msg->input_images[image_id].width * process_image_msg->input_images[image_id].height * 3;

				   /* Allocate new buffer if not allocated yet or requires different size */
				   if (rgb_buffer_size != size) {
					   if (rgb_buffer) inu_free(rgb_buffer); // Free previouse allocation. Image requires different size
					   LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL, "Allocating bufer for GRAY size: %d  width:%d height:%d\n", size, process_image_msg->input_images[image_id].width, process_image_msg->input_images[image_id].height);
					   rgb_buffer = (uchar*)inu_malloc(size);
					   if (!rgb_buffer) {
						   LOGG_PRINT(LOG_ERROR_E, (ERRG_codeE)NULL, "Failed to allocate for rgb_buffer\n");
						   rgb_buffer_size = 0;
						   return(EV_CNN_STATUS_FAIL);
					   }
					   rgb_buffer_size = size;
				   }

				   unsigned char* data = (unsigned char*)process_image_msg->input_images[image_id].image_ptr;
				   data_type = CV_8UC3;
				   process_image_msg->input_images[image_id].image_ptr = rgb_buffer;
				   process_image_msg->input_images[image_id].channels = 3;
				   process_image_msg->input_images[image_id].element_type = EV_CNN_ELEMENT_TYPE_UINT8;
				   process_image_msg->input_images[image_id].yuv_format = 0;
				   for (int h = 0; h < process_image_msg->input_images[image_id].height; h++) {
					   for (int w = 0; w < process_image_msg->input_images[image_id].width; w++) {
						   int pos = h * process_image_msg->input_images[image_id].width + w;
						   rgb_buffer[pos * 3] = data[pos];
						   rgb_buffer[pos * 3 + 1] = data[pos];
						   rgb_buffer[pos * 3 + 2] = data[pos];
					   }
				   }
			   }
		else if (inu_ProcesssingFlags & PROC_FLAGS_CONVERT_12_BITS_TO_8) {
			static uchar *raw8_buffer = NULL;
			static int raw8_buffer_size = 0;
			int size;

			LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL, "Convert 12 bits to raw8\n");

			size = process_image_msg->input_images[image_id].width * process_image_msg->input_images[image_id].height;

			/* Allocate new buffer if not allocated yet or requires different size */
			if (raw8_buffer_size != size) {
				if (raw8_buffer_size) inu_free(raw8_buffer); // Free previouse allocation. Image requires different size
				LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL, "Allocating bufer for RAW8 size: %d  width:%d height:%d\n", size, process_image_msg->input_images[image_id].width, process_image_msg->input_images[image_id].height);
				raw8_buffer = (uchar *)inu_malloc(size);
				if (!raw8_buffer) {
					LOGG_PRINT(LOG_ERROR_E, (ERRG_codeE)NULL, "Failed to allocate for raw8_buffer\n");
					raw8_buffer_size = 0;
					return(EV_CNN_STATUS_FAIL);
				}
				raw8_buffer_size = size;
			}

			unsigned short *data = (unsigned short*)process_image_msg->input_images[image_id].image_ptr;
			data_type = CV_8UC1;
			process_image_msg->input_images[image_id].image_ptr = raw8_buffer;
			process_image_msg->input_images[image_id].channels = 1;
			process_image_msg->input_images[image_id].element_type = EV_CNN_ELEMENT_TYPE_UINT8;
			process_image_msg->input_images[image_id].yuv_format = 0;
			for (int h = 0; h<process_image_msg->input_images[image_id].height; h++) {
				for (int w = 0; w<process_image_msg->input_images[image_id].width; w++) {
					int pos = h * process_image_msg->input_images[image_id].width + w;
					unsigned char val = (unsigned char)(data[pos] >> 2);
					raw8_buffer[pos] = val;
				}
			}
		}
		else {
			data_type = CV_16UC1;
		  }
	  }
      else if(process_image_msg->input_images[image_id].channels == 1 && (process_image_msg->input_images[image_id].element_type == EV_CNN_ELEMENT_TYPE_16UINT16|| process_image_msg->input_images[image_id].element_type == EV_CNN_ELEMENT_TYPE_16INT16)) {
		LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL,"input_type: CV_16UC1\n");
		data_type = CV_16UC1;
      }
      else {
        LOGG_PRINT(LOG_ERROR_E, (ERRG_codeE)NULL, "Image: %d Illegal number of channels(%d) or element type (%d) !!!\n", image_id, process_image_msg->input_images[image_id].channels, process_image_msg->input_images[image_id].element_type);
        return(EV_CNN_STATUS_FAIL);
      }

      /* Check if needs to convert from yuv to RGB */
      if(process_image_msg->input_images[image_id].yuv_format) {
         static unsigned char * rgb_buffer=NULL;
		 static int rgb_allocated_size = 0;
		 
		 /* Check if to reallocate RGB buffer */
		 int rgb_size = process_image_msg->input_images[image_id].width * process_image_msg->input_images[image_id].height * 3;
		 if((rgb_size > rgb_allocated_size) && rgb_buffer) {
			  LOGG_PRINT(LOG_INFO_E, (ERRG_codeE)NULL, "Reallocating buffer for YUV2RGB conversion. prev size:%d\n", rgb_allocated_size);
			  evMemFree(rgb_buffer);
			  rgb_buffer = NULL;
		 }
		 if (!rgb_buffer) {
				//rgb_buffer = (unsigned char *)evMemAlloc(1280*960*3, EV_MR_USE_CNDC, -1);
				LOGG_PRINT(LOG_INFO_E, (ERRG_codeE)NULL, "Allocating buffer for YUV2RGB conversion of size: %d\n", rgb_size);
				rgb_buffer = (unsigned char *)evMemAlloc(rgb_size, EV_MR_USE_CNDC, -1);
				rgb_allocated_size = rgb_size;
		}
        int size_factor = 1;

		 /* The split can be done with the yuv conversion if PROC_FLAGS_CONVERT_888_BITS_TO_8 is not set */
		 runner->getInputDimentions(image_id, images_width[image_id], images_height[image_id], images_channels[image_id]);
		 if (!(inu_ProcesssingFlags & PROC_FLAGS_CONVERT_888_BITS_TO_8) &&
			 (process_image_msg->input_images[image_id].width < images_width[image_id] + 16 && process_image_msg->input_images[image_id].height == images_height[image_id])) {
			 do_yuv_split = true;
			 yuv_alloc_size = images_width[image_id] * images_height[image_id];
			 yuv_split[0] = (uint8_t *)runner->GetImageSpace(yuv_alloc_size * 3);
			 yuv_split[1] = yuv_split[0] + yuv_alloc_size;
			 yuv_split[2] = yuv_split[1] + yuv_alloc_size;

			 /* Determine number of pixels to be cut off from each row, as the scaler is in 16pixles units */
			 scaler_frame_pixels = (process_image_msg->input_images[image_id].width - images_width[image_id]) / 2;
			 LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL, "Do split as part of YUV. scaler_frame_pixels:%d yuv_alloc_size:%d\n", scaler_frame_pixels, yuv_alloc_size);
		 }

         LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL,"YUV convert: W:%d H:%d ptr:%p do_yuv_split:%d\n", process_image_msg->input_images[image_id].width, process_image_msg->input_images[image_id].height, process_image_msg->input_images[image_id].image_ptr, do_yuv_split);
#ifdef CNN_PROCESS_TIME
		 long long start_yuv_convert;
		 start_yuv_convert = getRTC();
#endif
#if 1
		 //Optimized vector convert YUV422 to RGB888
       convertYUV2RGB_ctl_t ctl;   
 
       void *args[] = { (unsigned char*)process_image_msg->input_images[image_id].image_ptr, rgb_buffer, &ctl};
       ctl.processW = process_image_msg->input_images[image_id].width * 2; //Width in bytes
       ctl.processH = process_image_msg->input_images[image_id].height;
       ctl.scaler_frame_pixels = scaler_frame_pixels; //Numbr of pixels to clip from start and end of each row (total number to clip per row is scaler_frame_pixels * 2) 
       ctl.images_width = images_width[image_id]; //CNN input image width (in pixels)
       ctl.yuv_swap = process_image_msg->input_images[image_id].channel_swap;
       ctl.yuv_split = yuv_split;

		 if (do_yuv_split)
       {
		 inuCallOcl(OCL_KERNEL(ocl_convertYUV2RGB), args);
	   }
       else
       {
		 //invalidate chache for downscale by the arc later (as the kernel writes through cache with DMA)
		 //Do this invalidation before the kernel, to make sure the flush will not overwrite the rgb_buffer
		 evDataCacheFlushInvalidate();
		 inuCallOcl(OCL_KERNEL(ocl_convertYUV2RGB_NoSplit), args);
       }
       
#else
		 //Scalar C convert YUV422 to RGB888
		 FORMAT_CONVERTP_VYUY16ToRgb888Kernel_c((unsigned char*)process_image_msg->input_images[image_id].image_ptr, rgb_buffer, process_image_msg->input_images[image_id].width * 2, process_image_msg->input_images[image_id].height, process_image_msg->input_images[image_id].width * 2, process_image_msg->input_images[image_id].width * 3, size_factor,
			 do_yuv_split, yuv_split, scaler_frame_pixels, process_image_msg->input_images[image_id].channel_swap);
		 if (do_yuv_split)
       {
				 evDataCacheFlushLines(yuv_split[0], yuv_split[0] + yuv_alloc_size * 3);
		 }

#endif

#ifdef CNN_PROCESS_TIME
              long long yuv_convert_time = getRTC() - start_yuv_convert;
              LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL, "Converting YUV (%dMhz) : %lld[cycles], %f[ms]\n", (int)ev_clock_rate, yuv_convert_time, (float)yuv_convert_time / (ev_clock_rate * 1000));
#endif

         if(size_factor == 2) {
			process_image_msg->input_images[image_id].width = process_image_msg->input_images[image_id].width/2;
			process_image_msg->input_images[image_id].height = process_image_msg->input_images[image_id].height/2;
         }

		 /* If the split was done, no need to put into cv::Mat */
		 if (!do_yuv_split)
       {
			 input_image = cv::Mat(process_image_msg->input_images[image_id].height, process_image_msg->input_images[image_id].width, data_type, rgb_buffer);
		 }
      }
      else {
         input_image = cv::Mat(process_image_msg->input_images[image_id].height, process_image_msg->input_images[image_id].width, data_type, (uchar*)process_image_msg->input_images[image_id].image_ptr);
      }

      /* Resize the input image if required, and split was not done by the YUV conversion */
      if(!do_yuv_split && process_image_msg->input_images[image_id].resize) {
         runner->getInputDimentions(image_id, images_width[image_id], images_height[image_id], images_channels[image_id]);
        LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL,"input: width:%d height:%d output:width:%d height:%d\n", input_image.cols, input_image.rows, images_width[image_id], images_height[image_id]);
        if(input_image.cols == images_width[image_id] && input_image.rows == images_height[image_id]) {
           LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL,">>>>>>> NO RESIZE for image:\n");
           input_resized = input_image;
		   image_data_modification = false;
        }
        else {
         if(use_opencv_resize || 
           (input_image.elemSize()/input_image.channels() == 1 && input_image.channels() ==3 && (images_width[image_id]*images_height[image_id]) < 18000) ) { // small RGB images should be done with opencv

#if 1
        LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL, "In W:%d Out w:%d\n", process_image_msg->input_images[image_id].width, images_width[image_id]);

		/* If height is the same, and width is less than 16 bytes larger, we will crop the image instead of resize, as it was resized by the HW */
        if(process_image_msg->input_images[image_id].height == images_height[image_id] && images_width[image_id] + 16 > process_image_msg->input_images[image_id].width) {
           cv::Rect roi;

           roi.x = (process_image_msg->input_images[image_id].width - images_width[image_id])/2;
           roi.y = 0;
           roi.width = images_width[image_id];
           roi.height = images_height[image_id];

           LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL, "ROI: x:%d y:%d w:%d h:%d\n", roi.x, roi.y, roi.width, roi.height);

           input_resized = input_image(roi);
	}
	else {
            resize(input_image, input_resized, cv::Size(images_width[image_id], images_height[image_id]));
	}
#else
         resize(input_image, input_resized, cv::Size(images_width[image_id], images_height[image_id]));
#endif
		 LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL, "After resize of image %d from: width:%d  height:%d  to: width:%d height:%d element_size:%d\n", image_id, input_image.cols, input_image.rows, input_resized.cols, input_resized.rows, input_resized.elemSize());	 
         }
#ifndef USE_OPENCV_RESIZE
     else {
        LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL,"Resize with my_resize\n");
	    in_image.data = input_image.data;
	    in_image.width = input_image.cols;
	    in_image.height = input_image.rows;
	    in_image.channels = input_image.channels();
	    in_image.bytes_per_pixel_per_channel = (int)(input_image.elemSize()/input_image.channels());

	    out_p = (char *)inu_malloc(images_height[image_id] * images_width[image_id] * input_image.elemSize());
	    if(!out_p) {
	       LOGG_PRINT(LOG_ERROR_E, (ERRG_codeE)NULL,"Failed to allcate memory for the output mat\n");
	       exit(-1);
	    }


	    out_image.data = out_p;
	    out_image.width = images_width[image_id];
	    out_image.height = images_height[image_id];
	    out_image.channels = input_image.channels();
	    out_image.bytes_per_pixel_per_channel = (int)(input_image.elemSize()/input_image.channels());

            //evDataCacheFlushInvalidate();

	    LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL," in_image rows:%d cols:%d channels:%d ElemSize:%d\n", in_image.height, in_image.width, in_image.channels, in_image.bytes_per_pixel_per_channel);

	    // open Thread on core 1
	    LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL,"Running VDSP graph in a separate thread on the core 1.\n");
		
	    VdspResizeParams vpars = {&in_image, &out_image};
	    EvThreadType  vdspThread = evNewThread(vdspAppThread,
			    EVTH_INIT_CPU, 0,
			    EVTH_INIT_PRIORITY, 0,
			    EVTH_INIT_ARGPTR, (void*)&vpars,
			    EVTH_INIT_LAST);

	    if (vdspThread == 0)
	    {
		LOGG_PRINT(LOG_ERROR_E, (ERRG_codeE)NULL,"Vdsp thread running error. Emergency shutdown!!!\n");
		//return  my_status;
	    }

	    if (evJoinThread(vdspThread, &thread_ret) == 0)
	    {
		LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL,"evJoinThread to VDSP thread returned OK.\n");
		LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL,"Thread returnned %d\n", (int)thread_ret);
		 //my_status = 0;
	    }
	    else {
		LOGG_PRINT(LOG_ERROR_E, (ERRG_codeE)NULL,"Problems with joining to VDSP thread.\n");
	    }

        LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL," out_image rows:%d cols:%d channels:%d ElemSize:%d\n", out_image.height, out_image.width, out_image.channels, out_image.bytes_per_pixel_per_channel);

	    // Save resized image as jpg or tff
	    if(out_image.bytes_per_pixel_per_channel == 1)
	    {
		    if (out_image.channels ==3)
		    {
			input_resized = cv::Mat(out_image.height, out_image.width, CV_8UC3, out_p);
		    }
		    else
		    {
			input_resized = cv::Mat(out_image.height, out_image.width, CV_8UC1, out_p);
		    }
	    }
	    else
	    {
	       input_resized = cv::Mat(out_image.height, out_image.width, CV_16UC1, out_p);
	       LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL,"depth my_resized: cols:%d rows:%d channels:%d elemSize:%d\n", input_resized.cols, input_resized.rows, input_resized.channels(), input_resized.elemSize());
	       //imwrite("my_resized.tif", input_resized);
	    }

            inu_free((void*)out_p);
          }
#endif
        }
        LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL,"finished resize\n");
      }
      else if(!do_yuv_split) {
         input_resized = input_image;
      }

	  /* Check if needs to convert 3 channels to 1 */
      if (process_image_msg->input_images[image_id].channels == 3 && (inu_ProcesssingFlags & PROC_FLAGS_CONVERT_888_BITS_TO_8)) {
		  uint64_t start_gray_convert, end_gray_convert;
		  static uint64 last_start_gray=0;

		  start_gray_convert = getRTC();
		  cv::cvtColor(input_resized, input_resized, CV_RGB2GRAY);
		  end_gray_convert = getRTC();
		  LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL, "convert took: %lld  %fms  delta:%f\n", end_gray_convert- start_gray_convert, (float)(end_gray_convert - start_gray_convert)/ (ev_clock_rate*1000), (float)(start_gray_convert - last_start_gray) / ev_clock_rate);
		  last_start_gray = start_gray_convert;
		  process_image_msg->input_images[image_id].channels = 1;
		  process_image_msg->input_images[image_id].element_type = EV_CNN_ELEMENT_TYPE_UINT8;
		  process_image_msg->input_images[image_id].yuv_format = 0;
      }
      if(process_image_msg->input_images[image_id].channels == 3 && !do_yuv_split) {
         size = input_resized.cols * input_resized.rows;
         input_images_pending[pending_index][image_id].input_plane[0] = runner->GetImageSpace(size*3);
         input_images_pending[pending_index][image_id].input_plane[1] = (uint8_t *)input_images_pending[pending_index][image_id].input_plane[0] + size;
         input_images_pending[pending_index][image_id].input_plane[2] = (uint8_t *)input_images_pending[pending_index][image_id].input_plane[1] + size;
         if(!input_images_pending[pending_index][image_id].input_plane[0] || !input_images_pending[pending_index][image_id].input_plane[1] || !input_images_pending[pending_index][image_id].input_plane[2]) {
                LOGG_PRINT(LOG_ERROR_E, (ERRG_codeE)NULL, "Failed to allocated memory for input_images\n");
				return(EV_CNN_STATUS_FAIL);
         }
    
         /* Save the data, and swap channels if needed */
         if(process_image_msg->input_images[image_id].channel_swap) {
            bgr_mats[image_id][0] = cv::Mat(input_resized.rows, input_resized.cols, CV_8UC1, input_images_pending[pending_index][image_id].input_plane[2]);
            bgr_mats[image_id][2] = cv::Mat(input_resized.rows, input_resized.cols, CV_8UC1, input_images_pending[pending_index][image_id].input_plane[0]);
         }
         else {
            bgr_mats[image_id][0] = cv::Mat(input_resized.rows, input_resized.cols, CV_8UC1, input_images_pending[pending_index][image_id].input_plane[0]);
            bgr_mats[image_id][2] = cv::Mat(input_resized.rows, input_resized.cols, CV_8UC1, input_images_pending[pending_index][image_id].input_plane[2]);
         }
            bgr_mats[image_id][1] = cv::Mat(input_resized.rows, input_resized.cols, CV_8UC1, input_images_pending[pending_index][image_id].input_plane[1]);

      }
      else if(!do_yuv_split) {
		  LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL, "one channel only elemSize:%d cols:%d rows:%d\n", input_resized.elemSize(), input_resized.cols, input_resized.rows);
         size = input_resized.elemSize() * input_resized.cols * input_resized.rows;
		 input_images_pending[pending_index][image_id].input_plane[0] = runner->GetImageSpace(size);
         if(!input_images_pending[pending_index][image_id].input_plane[0]) {
			 LOGG_PRINT(LOG_ERROR_E, (ERRG_codeE)NULL, "Failed to allocate input_plane[0] for one channel\n");
		     return(EV_CNN_STATUS_FAIL);
         }
         bgr_mats[image_id][0] = cv::Mat(input_resized.rows, input_resized.cols, data_type, input_images_pending[pending_index][image_id].input_plane[0]);
         input_images_pending[pending_index][image_id].input_plane[1] = NULL;
         input_images_pending[pending_index][image_id].input_plane[2] = NULL;
      }

      LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL,"Before split: data: %p  %p  %p\n", bgr_mats[image_id][0].data, bgr_mats[image_id][1].data, bgr_mats[image_id][2].data);

      /* Split the input matrix to RGB mats */
      if(process_image_msg->input_images[0].channels == 3) {
		  /* In case of yuv_split, update the pointers */
		  if (do_yuv_split) {
			input_images_pending[pending_index][image_id].input_plane[0] = yuv_split[0];
			input_images_pending[pending_index][image_id].input_plane[2] = yuv_split[2];
			input_images_pending[pending_index][image_id].input_plane[1] = yuv_split[1];
			LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL, "YUV split addrs:%p %p %p\n",  input_images_pending[pending_index][image_id].input_plane[0], input_images_pending[pending_index][image_id].input_plane[1], input_images_pending[pending_index][image_id].input_plane[2]);
		  }
		  else {
#ifdef CNN_PROCESS_TIME
		 long long start_split = getRTC();
 #endif
         split(input_resized, bgr_mats[image_id]);
	     evDataCacheFlushLines(input_images_pending[pending_index][image_id].input_plane[0], (uint8_t *)input_images_pending[pending_index][image_id].input_plane[0]+size*3);

#ifdef CNN_PROCESS_TIME
		long long split_cycles = getRTC() - start_split;
		LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL, "opencv split (%dMHz) is cycles:%lld %fms\n", (int)ev_clock_rate, split_cycles, (float)split_cycles / (ev_clock_rate*1000));
#endif
		  }
      }
      else {
		  if (image_data_modification) {
			  //memcpy(input_images_pending[pending_index][image_id].input_plane[0], input_resized.data, size);
			  copy_kernel((unsigned char*)input_resized.data, (unsigned char*)input_images_pending[pending_index][image_id].input_plane[0], size);
			  evDataCacheFlushLines(input_images_pending[pending_index][image_id].input_plane[0], (uint8_t*)input_images_pending[pending_index][image_id].input_plane[0] + size);
			  LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL, "After split: data: %p  %p  %p\n", bgr_mats[image_id][0].data, bgr_mats[image_id][1].data, bgr_mats[image_id][2].data);
		  }
		  /* No image data modification was done. Treat that as a tensor input */
		  else {
			  evMemFree(input_images_pending[pending_index][image_id].input_plane[0]);
			  input_images_pending[pending_index][image_id].input_plane[0] = process_image_msg->input_images[image_id].image_ptr;
			  process_image_msg->input_images[image_id].tensor_input = true;
			  LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL, "Handle input without change image:%d\n", image_id);
		  }
      }

      input_images_pending[pending_index][image_id].element_type = (_ev_cnn_element_type_e)process_image_msg->input_images[image_id].element_type;
      input_images_pending[pending_index][image_id].layout_type = CNN_LAYOUT_3DFMAPS;

      /* Set the output address if not given by the caller */
      if(!process_image_msg->blob_output_address) {
          process_image_msg->blob_output_address = runner->getBlobOutputAddress();
      }
   }

   return(status);
}


/* ev_inu_get_runner */
InuGraphRunner *ev_inu_get_runner(void)
{
	return(runner);
}

/*ev_inu_getBlobOutputAddress */
uint8_t * ev_inu_getBlobOutputAddress(void)
{
   return(runner->getBlobOutputAddress());
}

void inu_release_and_wait(int net_id)
{
	release_net_request = true;
	release_net_id = net_id;
	ack_the_release_operation = false;

	evIntCondSignal(&action_Condition);
	evIntCondWait(&release_Condition);
	ev_inu_release_net();

	LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL, "inu_release_and_wait done ------------\n");

}
/* ev_inu_handle_load_net */
int ev_inu_handle_load_net(EV_CNN_LOAD_NET *load_net_msg)
{
    int status = CNN_STAT_OK;

    LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL,"ev load net_id: %d\n", load_net_msg->net_id);
    inu_debug_to_ddr(0xA0000001);


	//CDNN_EVG_netInfo[load_net_msg->net_id].bin_address = load_net_msg->bin_address;
	//CDNN_EVG_netInfo[load_net_msg->net_id].bin_size = load_net_msg->bin_size;
	
    if(runner == NULL) {
        runner = new InuGraphRunner;
    }
    else {
		if (load_net_msg->net_id != current_net_id) {
			LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL, "ev_inu_handle_load_net. requested to load different net_id\n");
			inu_release_and_wait(load_net_msg->net_id);
			runner = new InuGraphRunner;
		}
		else {
			LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL, "ev_inu_handle_load_net. requested to load same net_id\n");
			return(status);
		}
    }
    inu_debug_to_ddr(0xA0000004);

    if(status == CNN_STAT_OK) {
		/*-------------------------------------------------------------------------------------*/
		/* Check if last part of the file has extra bytes for anchores. Last byte = 0xCAFECAFE */
		/* *(end_ptr -1) -> 0xCAFECAFE                                                         */
		/* *(end_ptr -2) -> Version number                                                     */
		/* *(end_ptr -3) -> Number of classes                                                  */
		/* *(end_ptr -4) -> Confidence threshold %                                             */
		/* *(end_ptr -5) -> NMS threshold %                                                    */
		/* *(end_ptr -6) -> Number of anchores. Each of sizeof(int)                            */
		/* *(end_ptr - 6+number_of_anchores(sizedof(int)) ..  *(end_ptr -6) -> Anchores        */
		/*-------------------------------------------------------------------------------------*/
		int actual_size                 = load_net_msg->bin_size;
		uintptr_t *end_ptr              = (uintptr_t *)(load_net_msg->bin_address + actual_size);
		unsigned int last_word          = *(end_ptr - 1);
		unsigned int version            = *(end_ptr - 2);
		unsigned int number_of_classes  = *(end_ptr - 3);
		float conf_thresh               = (float)*(end_ptr - 4) / 100;
		float nms_thresh                = (float)*(end_ptr - 5) / 100;
		unsigned int number_of_anchores = *(end_ptr - 6);

		LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL, "bin_address:%p size:%d int_ptr:%p\n", load_net_msg->bin_address, actual_size, end_ptr);
		LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL, "number_of_anchores:%d  last_word:%X\n", number_of_anchores, last_word);
		LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL, "ver:%d  conf_thresh:%f nms_thresh:%f\n", version, conf_thresh, nms_thresh);

		if (last_word == 0xCAFECAFE) //--> YOLO_V3 anchors info was added to binnary
        {
			actual_size = actual_size - ((number_of_anchores+6) * sizeof(int));

			/* Copy the anchores to their place */
			uintptr_t *anchores_ptr = end_ptr - (number_of_anchores + 6);
			set_yolo_anchores(anchores_ptr, number_of_anchores);
			inu_yolo_number_of_classes  = number_of_classes;
			inu_yolo_nms_threshold      = nms_thresh;
			inu_yolo_conf_threshold     = conf_thresh;
            yolo_set_number_of_classes(inu_yolo_number_of_classes);
            yolo_set_nms(inu_yolo_nms_threshold);
            yolo_set_conf(inu_yolo_conf_threshold);
            yolo_set_num_of_anchors(number_of_anchores>>1); //number of [x,y] anchors pairs
            LOGG_PRINT(LOG_INFO_E, (ERRG_codeE)NULL, "Piggy detected: nms:%.2f conf_th:%.2f num_class:%d num_anchors:%d ptx_ver:%d\n",inu_yolo_nms_threshold, inu_yolo_conf_threshold, inu_yolo_number_of_classes, number_of_anchores, version );
		}

		/* Some graphs has a problem if the ctl structure is not freed on switch */
		bool free_flag = false;
		if (load_net_msg->ProcesssingFlags & PROC_FLAGS_FREE_ON_SWITCH) {
			free_flag = true;
		}

		uint64_t tt1, tt2;

		tt1 = getRTC();
		status = runner->loadCnnGraph(load_net_msg->net_id, (uintptr_t)load_net_msg->bin_address, actual_size, free_flag);
		tt2 = getRTC();
		LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL, "load time %fms  from:%lld to %lld\n", (float)(tt2 - tt1) / CYCLES_PER_MS, tt1, tt2);

        if(status != CNN_STAT_OK) {
			delete runner;
            runner = NULL;
			LOGG_PRINT(LOG_ERROR_E, (ERRG_codeE)NULL, "***** loadCnnGraph failed addr:%p  bin_size:%d*****\n", load_net_msg->bin_address, load_net_msg->bin_size);
		    inu_debug_to_ddr(0xA0000005);
			current_net_id = -1;
        }
        else {
            memset(images_width, 0, sizeof(images_width));
            memset(images_height, 0, sizeof(images_height));
            memset(images_channels, 0, sizeof(images_channels));
            runner->prepareForImages(images_width, images_height, images_channels);
            load_net_done = true;
			inu_debug_to_ddr(0xA0000006);
			inu_ProcesssingFlags = load_net_msg->ProcesssingFlags;
            if(load_net_msg->pipe_depth > 0) {
               inu_pipe_depth = load_net_msg->pipe_depth;
            }
            else {
               inu_pipe_depth = 1;
            }

			LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL,"ev_inu_handle_load_net:: Setting pipe depth to: %d  ProcesssingFlags: %d\n", inu_pipe_depth, inu_ProcesssingFlags);
			if (inu_ProcesssingFlags & PROC_FLAGS_YOLO_V3_POSTPROCESS || inu_ProcesssingFlags & PROC_FLAGS_YOLO_V7_POSTPROCESS) {
				if (runner->ev_cnn_num_of_tails() == 3) {
					if (inu_ProcesssingFlags & PROC_FLAGS_YOLO_V3_POSTPROCESS) {
						LOGG_PRINT(LOG_INFO_E, (ERRG_codeE)NULL, "Set to full yolo mode\n");
						set_full_yolo();
					}
					else {
						LOGG_PRINT(LOG_INFO_E, (ERRG_codeE)NULL, "Set to full yolo7 mode\n");
						set_full_yolo7();
					}
				}
				else {
					LOGG_PRINT(LOG_INFO_E, (ERRG_codeE)NULL, "Set to tiny yolo mode\n");
					set_tiny_yolo();
				}
				inu_yolo_network_dimension = images_width[0];
				LOGG_PRINT(LOG_INFO_E, (ERRG_codeE)NULL, "ev_inu_handle_load_net:: Setting inu_yolo_network_dimension:%d inu_yolo_classes:%d\n", inu_yolo_network_dimension, inu_yolo_number_of_classes);
			}

			inu_debug_to_ddr(0xA0000007);
			current_net_id = load_net_msg->net_id;
        }
    }

	inu_debug_to_ddr(0xA0000008);

    runner->init_trace();
    inu_debug_to_ddr(0xA0000009);

    return(status);
}

/* ev_inu_handle_release_net_pipe */
void ev_inu_handle_release_net_pipe(EV_CNN_RELEASE_NET *release_net_msg)
{
    LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL,"ev_inu_handle_release_net_pipe: net_id:%d\n", release_net_msg->net_id);
    if (runner)
    {
	    release_net_request = true;
	    release_net_id = release_net_msg->net_id;
		release_net_jobDescriptor = release_net_msg->jobDescriptor;
		ack_the_release_operation = true;
		
		evIntCondSignal(&action_Condition);
		evIntCondWait(&release_Condition);
		ev_inu_release_net();
    }
	else
	{
	
#ifndef INU_HAPS
        SCHEDG_pushFinishedJob(release_net_jobDescriptor);
	        evIntCondSignal(&SCHEDG_Condition);

#endif
	}  
}


/* ev_inu_handle_message */
int ev_inu_handle_message(EV_MSG_HEADER *message)
{
    int status = CNN_STAT_OK;

    LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL,"ev_inu_handle_message: got %d\n", message->msg_code);

    switch(message->msg_code) {
        case EV_CNN_MSG_LOAD_NET:
			{
				inu_debug_to_ddr(0xC0000001);
				/* Fill in the required information for the net id */
				EV_CNN_LOAD_NET* load_net_message = (EV_CNN_LOAD_NET*)message;
				int net_id = load_net_message->net_id;
				load_net_message->bin_address = CDNN_EVG_netInfo[net_id].bin_address;
				load_net_message->bin_size = CDNN_EVG_netInfo[net_id].bin_size;
				load_net_message->pipe_depth = CDNN_EVG_netInfo[net_id].pipe_depth;
				load_net_message->ProcesssingFlags = CDNN_EVG_netInfo[net_id].ProcesssingFlags;
				uint64_t start_send = getRTC();
				print_on_sleep = true;
				if (CLIENT_APIG_sendMsg(&CNN_PRE_msgQue, (UINT8*)&load_net_message, sizeof(load_net_message)) != SUCCESS_E)
				{
					LOGG_PRINT(LOG_ERROR_E, (ERRG_codeE)NULL, "Failed to send EV_CNN_MSG_LOAD_NET to CNN Pre-Process thread\n");
					evIntCondSignal(&SCHEDG_Condition);
				}
				else {
					LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL, "Send EV_CNN_MSG_LOAD_NET to PP_EVP_msgQue (%p)  success\n", &CNN_PRE_msgQue);
				}
				inu_debug_to_ddr(0xC0000002);
			}
            break;

        case EV_CNN_MSG_RELEASE_NET:
        case EV_CNN_MSG_INJECT_IMAGES:
			/* Send message to the pre-process thread */
			{
				uint64_t start_send = getRTC();
				if (CLIENT_APIG_sendMsg(&CNN_PRE_msgQue, (UINT8*)&message, sizeof(message)) != SUCCESS_E)
				{
					LOGG_PRINT(LOG_ERROR_E, (ERRG_codeE)NULL, "Failed to send message to CNN Pre-Process thread\n");
					SCHEDG_pushFinishedJob(((EV_CNN_RELEASE_NET*)message)->jobDescriptor);
					evIntCondSignal(&SCHEDG_Condition);
				}
				else {
					//inu_register_timing_info(start_send, getRTC(), "send frame");
					LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL, "Send message to PP_EVP_msgQue (%p)  success\n", &CNN_PRE_msgQue);
				}
			}
            break;

        default:
            LOGG_PRINT(LOG_ERROR_E, (ERRG_codeE)NULL,"ev_inu_handle_message: Got invalid code: %d\n", message->msg_code);
            break;
    }

//	    evYieldThread();

    return(status);
}

/* ev_inu_register_user_flow_function */
int ev_inu_register_user_flow_function(int user_flow_id, ev_cnn_user_flow_func user_flow_func)
{
    if(user_flow_id < EV_CNN_MAX_USER_FLOWS) {
	user_flow_funcs[user_flow_id] = user_flow_func;
	return(CNN_STAT_OK);
    }
    else {
	return(CNN_STAT_OK);
    }

}

/* ev_inu_set_preprocess_thread_cpu */
void ev_inu_set_preprocess_thread_cpu(int cpu)
{
	LOGG_PRINT(LOG_INFO_E, (ERRG_codeE)0, "ev_inu_set_preprocess_thread_cpu to %d\n", cpu);
	inu_preprocess_thread_cpu = cpu;
}

/* ev_inu_api_init */
int ev_inu_api_init(int coreId)
{
    int rc;
    EvThreadType t;

	inu_debug_init();

    inu_debug_to_ddr(0x56785678);

    /* Init pending_requests_fifo */
    memset(pending_requests_fifo, NULL, sizeof(pending_requests_fifo));
    memset(in_progress_fifo, NULL, sizeof(in_progress_fifo));

	/* Create mutexes */
	rc = evMutexInit(&req_mutex);
	if (rc != EVTH_ERROR_OK)
	{
		LOGG_PRINT(LOG_ERROR_E, (ERRG_codeE)0, "ev_inu_api_init: Failed to create req_mutex\n");
		return(EV_CNN_STATUS_FAIL);
	}

	rc = init_ocl_mutexes();
	if (rc != EV_CNN_STATUS_OK) {
		return(rc);
	}

	rc = evIntCondInit(&action_Condition);
	if (rc != EVTH_ERROR_OK)
	{
		LOGG_PRINT(LOG_ERROR_E, (ERRG_codeE)0, "ev_inu_api_init: Failed to create action_Condition\n");
		return(rc);
	}

	rc = evIntCondInit(&release_Condition);
	if (rc != EVTH_ERROR_OK)
	{
		LOGG_PRINT(LOG_ERROR_E, (ERRG_codeE)0, "ev_inu_api_init: Failed to create release_Condition\n");
		return(rc);
	}

    /* Start the inu_processing_thread (CNN_HAL)*/
    inu_debug_to_ddr(0x56785679);
    t = evNewThread(ev_inu_process_thread, EVTH_INIT_CPU, coreId, EVTH_INIT_ARGPTR, NULL, EVTH_INIT_PRIORITY, CNN_HAL_THREAD_PRIORITY, EVTH_INIT_PREEMPTIBLE, 1, EVTH_INIT_LAST);
    if(t == NULL)
    {
        LOGG_PRINT(LOG_ERROR_E, (ERRG_codeE)NULL, "Failed to open ev_inu_process_thread thread\n");
        return(EV_CNN_STATUS_FAIL);
    }

	/* Init the CNN preprocess thread */
	CNN_PRE_msgQue.maxMsgs = CLIENT_APIG_QUE_MAX_ELEMENTS;
	CNN_PRE_msgQue.msgSize = sizeof(void *); // Pointer of new message is passed to the CNN pre-process thread

	if (CLIENT_APIG_createMsgQue(&CNN_PRE_msgQue, 0) != SUCCESS_E)
	{
		LOGG_PRINT(LOG_ERROR_E, (ERRG_codeE)NULL, "CNN_PRE msg queue create failed\n");
		return(EV_CNN_STATUS_FAIL);
	}
	else {
		LOGG_PRINT(LOG_INFO_E, (ERRG_codeE)NULL, "CNN_PRE msg queue create OK. %p\n", &CNN_PRE_msgQue);
	}

	t = evNewThread(ev_inu_preprocess_thread, EVTH_INIT_CPU, inu_preprocess_thread_cpu, EVTH_INIT_ARGPTR, NULL, EVTH_INIT_PRIORITY, CNN_PRE_THREAD_PRIORITY, EVTH_INIT_PREEMPTIBLE, 1, EVTH_INIT_LAST);
	if (t == NULL)
	{
		LOGG_PRINT(LOG_ERROR_E, (ERRG_codeE)NULL, "Failed to open ev_inu_preprocess_thread thread\n");
		return(EV_CNN_STATUS_FAIL);
	}
	else {
		LOGG_PRINT(LOG_INFO_E, (ERRG_codeE)NULL, "opened ev_inu_preprocess_thread thread\n");
	}


	t = evNewThread(ev_inu_keep_alive_thread, EVTH_INIT_CPU, 1-inu_preprocess_thread_cpu, EVTH_INIT_ARGPTR, NULL, EVTH_INIT_PRIORITY, EVTH_MAX_PRIORITIES-1, EVTH_INIT_PREEMPTIBLE, 1, EVTH_INIT_LAST);
	if (t == NULL)
	{
		LOGG_PRINT(LOG_ERROR_E, (ERRG_codeE)NULL, "Failed to open ev_inu_keep_alive_thread thread\n");
		return(EV_CNN_STATUS_FAIL);
	}
	else {
		LOGG_PRINT(LOG_INFO_E, (ERRG_codeE)NULL, "opened ev_inu_preprocess_thread thread\n");
	}

	//register_cnn_log(inu_log_cnn);

    return(EV_CNN_STATUS_OK);	
}

/* ev_inu_is_pipe_free */
bool ev_inu_is_pipe_free(void)
{
   if(in_progress_counter)
      return(false);
   else
      return(true);
}

/* ev_inu_is_pending_free */
bool ev_inu_is_pending_free(void)
{
   if(pending_requests_counter)
      return(false);
   else
      return(true);
}

/* ev_inu_set_vision_proc_mode */
void ev_inu_set_vision_proc_mode(void) {
	vision_proc_mode = true;
}

/* set_yolact_input_image_size */
void set_yolact_input_image_size(int width, int height)
{
#ifdef VER_4100
	inu_yolact_image_rows = height;
	inu_yolact_image_cols = width;
	LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL, "set_yolact_input_image_size: %d %d\n", height, width);
#endif
}


/* FORMAT_CONVERTP_VYUY16ToRgb888Kernel_c */
void FORMAT_CONVERTP_VYUY16ToRgb888Kernel_c(unsigned char *src, unsigned char *dst, int processW, int processH, int srcProcessStride, int dstProcessStride, int size_factor, bool do_yuv_split, uint8_t **yuv_split, int scaler_frame_pixels, bool yuv_swap)
{
	unsigned int *vyuyP;
	unsigned char *rgbP;
	unsigned int valDw, nextValDw;
	int r, g, b;
	short y1, y2, u1, v1;
	static int T[3][3] = { { 8192, 0, 11452 },{ 8192, -2803, -5833 },{ 8192, 14440, 0 } };
	int col, row;
	uint8_t *split_ptr0=NULL, *split_ptr1=NULL, *split_ptr2=NULL;

	if (do_yuv_split) {
		if (yuv_swap) {
			split_ptr0 = yuv_split[0];
			split_ptr1 = yuv_split[1];
			split_ptr2 = yuv_split[2];
		}
		else {
			split_ptr0 = yuv_split[2];
			split_ptr1 = yuv_split[1];
			split_ptr2 = yuv_split[0];
		}
	}

	processW >>= 1;

	LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL, "YUV convert. processW:%d processH:%d addrs:%p %p %p\n", processW, processH, split_ptr0, split_ptr1, split_ptr2);
	LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL, "YUV convert. do_yuv_split:%d, scaler_frame_pixels:%d size_factor:%d\n", do_yuv_split, scaler_frame_pixels, size_factor);

	for (row = 0; row < processH; row++) {
		if(size_factor == 2 && (row&0x1) != 0) { // Skip line if size_Factor=2
			continue;
		}

		int row_dst_stride = row / size_factor * dstProcessStride / size_factor;
		int row_src_stride = row * srcProcessStride;

		for (col = scaler_frame_pixels; col < processW - scaler_frame_pixels; col += 2) {
			vyuyP = (unsigned int*)(src + row_src_stride + col * sizeof(unsigned short));
			rgbP = dst + row_dst_stride + col / size_factor * 3;
			valDw = vyuyP[0];
			nextValDw = vyuyP[1];

#if 1
			//pixel order is:
			//YUYVYUYV = 8 bytes, 4 pixels

			y1 = (valDw & 0xFF);
			y1 = y1;
			u1 = ((valDw >> 8) & 0xFF);
			u1 = u1 - 128;
			y2 = ((valDw >> 16) & 0xFF);
			y2 = y2;
			v1 = ((valDw >> 24) & 0xFF);
			v1 = v1 - 128;
#else
			//pixel order is:
			//UYVYUYVY = 8 bytes, 4 pixels

			u1 = (valDw & 0xFF);
			u1 = u1 - 128;
			y1 = ((valDw >> 8) & 0xFF);
			y1 = y1 - 16;
			v1 = ((valDw >> 16) & 0xFF);
			v1 = v1 - 128;
			y2 = ((valDw >> 24) & 0xFF);
			y2 = y2 - 16;
#endif

			//////////////   pixel 1  ///////////////////
			r = T[0][0] * y1 + T[0][1] * u1 + T[0][2] * v1;
			g = T[1][0] * y1 + T[1][1] * u1 + T[1][2] * v1;
			b = T[2][0] * y1 + T[2][1] * u1 + T[2][2] * v1;
			
			r >>= 13;
			g >>= 13;
			b >>= 13;

			if (r > 255)
				r = 255;
			if (g > 255)
				g = 255;
			if (b > 255)
				b = 255;


			if (r < 0)
				r = 0;
			if (g < 0)
				g = 0;
			if (b < 0)
				b = 0;

			if (do_yuv_split) 
         {
				*split_ptr0++ = (unsigned char)b;
				*split_ptr1++ = (unsigned char)g;
				*split_ptr2++ = (unsigned char)r;
			}
			else
         {
				*rgbP++ = (unsigned char)r; // Replace B & R
				*rgbP++ = (unsigned char)g;
				*rgbP++ = (unsigned char)b;
			}

			//////////////   pixel 2  ///////////////////
			if(size_factor == 1)
         {
				r = T[0][0] * y2 + T[0][1] * u1 + T[0][2] * v1;
				g = T[1][0] * y2 + T[1][1] * u1 + T[1][2] * v1;
				b = T[2][0] * y2 + T[2][1] * u1 + T[2][2] * v1;

				r >>= 13;
				g >>= 13;
				b >>= 13;

				if (r > 255)
					r = 255;
				if (g > 255)
					g = 255;
				if (b > 255)
					b = 255;
	
				if (r < 0)
					r = 0;
				if (g < 0)
					g = 0;
				if (b < 0)
					b = 0;

				if (do_yuv_split)
            {
					*split_ptr0++ = (unsigned char)b;
					*split_ptr1++ = (unsigned char)g;
					*split_ptr2++ = (unsigned char)r;
				}
				else 
            {
					*rgbP++ = (unsigned char)r; // Replace B & R
					*rgbP++ = (unsigned char)g;
					*rgbP++ = (unsigned char)b;
				}
         }
		}
  	}
}

