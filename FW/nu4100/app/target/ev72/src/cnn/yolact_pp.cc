/****************************************************************************
 *
 *   FileName: yolact_pp.cc
 *
 *   Author:  
 *
 *   Date: 
 *
 *   Description: 
 *   
 ****************************************************************************/

#include <opencv2/opencv.hpp>
#include "client_api.h"
#include "yolact_pp.h"
#include "tanh_lut.h"
#include "exp_lut.h"
#include "exp_lut_Q10.h"
#include "inu_reshape_chw2hwc.h"
#include "inu_softmax.h"
#include "inu2_internal.h"
#include "log.h"
#include "sched.h"
#include "ictl_drv.h"
#include "ev_init.h"
#include "ev_ocl_copy_interface.h"
#include "inu_convert2float.h"
#include "inu_resize_f.h"
#include "inu_combine_c.h"
#include "inu_proto_mask_mul.h"
#include "inu_matrix_max_rows.h"

#define RESHAPE_OPERATION 0
#define CONVERT_OPERATION 1
#define RESIZE_OPERATION  2
#define COMBINE_OPERATION 3


#define FLOAT32_TYPE true
#define FLOAT16_TYPE false
//#define RESIZE_INPUT_TYPE float
#define RESIZE_INPUT_TYPE __fp16

const char* operation_text[4] = { "RESHAPE", "CONVERT", "RESIZE", "COMBINE" };

typedef struct
{
	int operation;
	float split_th;

	struct {
		short* short_p;
		void* float_p;
		int elements;
		float float_factor;
		bool float_type;
	} to_float_info;

	struct {
		short* in_ptr;
		short* out_ptr;
		int in_H;
		int in_W; 
		int in_ch;
		float scale;
		bool do_tanh;
	} reshape_info;

	struct {
		int in_x;
		int in_y;
		RESIZE_INPUT_TYPE* in_mat;
		int out_x;
		int out_y;
		unsigned char* out_mat;
		float thresh;
		int class_id;
		int input_element_size;
	} resize_info;

	struct {
		int num_mats;
		int x_size;
		int y_size;
		unsigned char** mats_ptrs;
		unsigned char* out_mat;
	} combine_info;

} YOLACT_SECOND_INFO;

#define YOLACT_MEAS_TIME LOG_DEBUG_E
#define YOLACT_TOTAL_MEAS_TIME LOG_DEBUG_E

#define NUM_OF_ASPECT_RATIOS	3
#define NUM_OF_CONV_SIZE		5

#define MASK_DIM        32
#define E_CONST 2.71828F
#define YOLACT_SCORE_THRESH 0.15

static EvMutexType yolact_mutex;
static int scales[NUM_OF_CONV_SIZE] = { 24, 48, 96, 192, 384 };
static int conv_sizes[NUM_OF_CONV_SIZE] = { 46, 23, 12, 6, 3 };
static bool do_tanh[INU_YOLACT_NUMBER_OF_TAILS] = { false, false, true, false, true, false, false, true, false, false, true, false, false , true , false , false };
#define PROTO_TAIL  1
#define BBOX_TAIL_1 6
#define BBOX_TAIL_2 9
#define BBOX_TAIL_3 12
#define BBOX_TAIL_4 14
#define BBOX_TAIL_5 15
#define MASK_TAIL_1 2
#define MASK_TAIL_2 4
#define MASK_TAIL_3 7
#define MASK_TAIL_4 10
#define MASK_TAIL_5 13
#define CONF_TAIL_1 0
#define CONF_TAIL_2 3
#define CONF_TAIL_3 5
#define CONF_TAIL_4 8
#define CONF_TAIL_5 11

/****************************************************************************
 ***************       L O C A L         D A T A              ***************
 ****************************************************************************/

EvIntCondType               YOLACT_PPG_condition, YolactSecondCondSignal;
CLIENT_APIG_msgQueT			yolact_second_msgQue;
YOLACT_PPG_infoT            yolact_post_params;
YOLACT_SECOND_INFO			yolact_second_info;

static int yolact_image_rows;
static int yolact_image_cols;

static float ev_clock_rate;
static unsigned char* zeros_buffer = NULL;
static unsigned char* segments_mats[INU_YOLACT_MAX_DETECTIONS];
static unsigned char* result_segments_mat;

static yolact_tail_info input_tails_info[INU_YOLACT_NUMBER_OF_TAILS];

static float *sorted_tails[INU_YOLACT_NUMBER_OF_TAILS] = { NULL };
static short *sorted_tails_ints[INU_YOLACT_NUMBER_OF_TAILS] = { NULL };
static yolact_tail_info sorted_tails_info[INU_YOLACT_NUMBER_OF_TAILS];

static float float_factor = 1.0 / 1024.;

static uint64_t meas_time_1, meas_time_2;
static uint64_t total_999 = 0;

static float *reshaped_tails[INU_YOLACT_NUMBER_OF_TAILS] = { NULL };
static short* short_reshaped_tails[INU_YOLACT_NUMBER_OF_TAILS] = { NULL };

static int reshaped_tails_elements[INU_YOLACT_NUMBER_OF_TAILS] = { 0 };

static int order_of_tails[INU_YOLACT_NUMBER_OF_TAILS] = { 0 };
static int yolact_num_of_classes = INU_YOLACT_NUMBER_OF_CLASSES;
static float aspect_ratios[NUM_OF_ASPECT_RATIOS] = { 1, 0.5, 2 };
static float *priors[NUM_OF_CONV_SIZE] = { NULL };
static int priors_sizes[NUM_OF_CONV_SIZE] = { 0 };
static float *all_priors = NULL;
static float* max_values_array = NULL;
static int max_values_array_elements;
static int all_prior_elements;
static int conf_data_elements;
static int loc_data_elements;
static int mask_data_elements;
static int kept_elements_after_iou;
static int conf_score_elements, max_num_scores_elements, dets_number;
static int decoded_boxes_data_elements;
static float* conf_data_32 = NULL;
static __fp16 *conf_data_16 = NULL;
static short *short_conf_data = NULL;
static float *loc_data = NULL;
static float *mask_data = NULL;
static float *mask_data_kept = NULL;
static float *mask_sorted[MAX_NUM_OF_CLASSES] = { NULL};
static float *decoded_boxes_data = NULL;
static float *decoded_boxes_kept = NULL;
static float *boxes_sorted[MAX_NUM_OF_CLASSES] = { NULL};
static float *conf_scores[MAX_NUM_OF_CLASSES] = { NULL};
static float *scores_kept = NULL;
static int *classes_kept = NULL;
static int *sorted_scores_indexes = NULL;
static int *scores_sorted_indexes[MAX_NUM_OF_CLASSES] = { NULL};
static float iou_max[MAX_NUM_OF_CLASSES][INU_YOLACT_TOP_K];
static float iou[MAX_NUM_OF_CLASSES][INU_YOLACT_TOP_K][INU_YOLACT_TOP_K];

static float boxes_above_thresh[INU_YOLACT_MAX_DETECTIONS][4];
static float mask_above_thresh[INU_YOLACT_MAX_DETECTIONS][32];
static float *mask_proto_result; // [92][92][INU_YOLACT_MAX_DETECTIONS];
static int *masks_crop_matrix; // [92][92][INU_YOLACT_MAX_DETECTIONS];
static int masks_crop_matrix_size = 0;
static RESIZE_INPUT_TYPE*mask_cropped_contig; // [INU_YOLACT_MAX_DETECTIONS][92][92];
static int mask_cropped_contig_size = 0;
static int classes_above_thres[INU_YOLACT_MAX_DETECTIONS];
static float scores_above_thresh[INU_YOLACT_MAX_DETECTIONS];
static cv::Mat masks_mats[INU_YOLACT_MAX_DETECTIONS], masks_mats_resized[INU_YOLACT_MAX_DETECTIONS];;
static float *mats_data_ptr[INU_YOLACT_MAX_DETECTIONS];
static EvCnnBoundingBox yolact_boxes[INU_YOLACT_MAX_DETECTIONS];
static int elements_above_thresh = 0;

/* yolact_mutex_lock */
void yolact_mutex_lock(void)
{
	evMutexLock(&yolact_mutex);
}

/* yolact_mutex_unlock */
void yolact_mutex_unlock(void)
{
	evMutexUnLock(&yolact_mutex);
}


/* wait_for_second_yolact */
void wait_for_second_yolact(float split_th)
{
	/* Wait for the second render to finish */
	if (split_th < 1) {
		evIntCondWait(&YolactSecondCondSignal);
	}
	LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL, "wait_for_second_yolact done at:%lld\n", getRTC());
}

/* send_to_second */
void send_to_second_yolact(float split_th)
{
	YOLACT_SECOND_INFO* yolact_second_info_p;
	yolact_second_info_p = &yolact_second_info;

	yolact_second_info_p->split_th = split_th;

	if (split_th < 1) {
		if (CLIENT_APIG_sendMsg(&yolact_second_msgQue, (UINT8*)&yolact_second_info_p, sizeof(yolact_second_info_p)) != SUCCESS_E)
		{
			LOGG_PRINT(LOG_ERROR_E, (ERRG_codeE)NULL, "Failed to send msg to yolact_second_msgQue thread\n");
			return;
		}
		else {
			LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL, "Send msg to yolact_second_msgQue  success at:%lld\n", getRTC());
			evYieldThread();
		}
	}
}

static void init_meas_999(void)
{
	LOGG_PRINT(YOLACT_MEAS_TIME, (ERRG_codeE)NULL, "convert to float in cycles:%lld %fms ID:999\n", total_999, (float)total_999 / (ev_clock_rate * 1000));
	total_999 = 0;
}

static void start_meas()
{
	meas_time_1 = getRTC();
}

static void end_meas(char* text)
{
	meas_time_2 = getRTC();

	if (!strcmp(text, "convert2float")) {
		total_999 += meas_time_2 - meas_time_1;
	}
	else {
		LOGG_PRINT(YOLACT_MEAS_TIME, (ERRG_codeE)NULL, "meas_time in cycles:%lld %fms %s\n", (meas_time_2 - meas_time_1), (float)(meas_time_2 - meas_time_1) / (ev_clock_rate * 1000), text);
	}
}

/* yolact_malloc */
void* yolact_malloc(int size) {
	void* ptr;
	ptr = evMemAlloc(size, EV_MR_USE_CNDC, -1);
	return(ptr);
}

void yolact_pp_init(int coreId)
{
	int rc;
	EvThreadType t;

	ev_clock_rate = EV_INITG_getEvFreqMhz();

	rc = evIntCondInit(&YOLACT_PPG_condition);
	if (rc != EVTH_ERROR_OK)
	{
		LOGG_PRINT(LOG_ERROR_E, (ERRG_codeE)0, "YOLACT_PPG_init: Failed to create yolact_proc_condition\n");
		return;
	}

	rc = evIntCondInit(&YolactSecondCondSignal);
	if (rc != EVTH_ERROR_OK)
	{
		LOGG_PRINT(LOG_ERROR_E, (ERRG_codeE)0, "YOLACT_PPG_init: Failed to create YolactSecondCondSignal\n");
		return;
	}

	rc = evMutexInit(&yolact_mutex);
	if (rc != EVTH_ERROR_OK)
	{
		LOGG_PRINT(LOG_ERROR_E, (ERRG_codeE)0, "ev_inu_api_init: Failed to create yolact_mutex\n");
		return;
	}

	yolact_second_msgQue.maxMsgs = CLIENT_APIG_QUE_MAX_ELEMENTS;
	yolact_second_msgQue.msgSize = sizeof(void*);
	if (CLIENT_APIG_createMsgQue(&yolact_second_msgQue, 0) != SUCCESS_E)
	{
		LOGG_PRINT(LOG_ERROR_E, (ERRG_codeE)NULL, "yolact second msg queue create failed\n");
		return;
	}
	else {
		LOGG_PRINT(LOG_INFO_E, (ERRG_codeE)0, "bokeh_flow_init: yolact second OK with size:%d\n", yolact_second_msgQue.msgSize);
	}

	t = evNewThread(ev_inu_yolact_post_thread, EVTH_INIT_CPU, coreId, EVTH_INIT_ARGPTR, NULL, EVTH_INIT_PRIORITY, EVTH_MAX_PRIORITIES - 1, EVTH_INIT_LAST);
	if (t == NULL)
	{
		LOGG_PRINT(LOG_ERROR_E, (ERRG_codeE)NULL, "Failed to open ev_inu_yolact_post_thread thread\n");
		return;
	}

	t = evNewThread(ev_inu_yolact_second_post_thread, EVTH_INIT_CPU, 1-coreId, EVTH_INIT_ARGPTR, NULL, EVTH_INIT_PRIORITY, EVTH_MAX_PRIORITIES - 1, EVTH_INIT_LAST);
	if (t == NULL)
	{
		LOGG_PRINT(LOG_ERROR_E, (ERRG_codeE)NULL, "Failed to open ev_inu_yolact_second_post_thread thread\n");
		return;
	}
}


/* prepare_order_of_tails */
static void prepare_order_of_tails(void)
{
	EV_CNN_TAIL tails[INU_YOLACT_NUMBER_OF_TAILS];

	memcpy(tails, yolact_post_params.tails, sizeof(EV_CNN_TAIL)*INU_YOLACT_NUMBER_OF_TAILS);

	for (int index = 0; index < INU_YOLACT_NUMBER_OF_TAILS; index++) {
		int biggest_index = -1;
		int biggest_elements = -1;

		for (int i = 0; i < INU_YOLACT_NUMBER_OF_TAILS; i++) {
			if (tails[i].elementCount > biggest_elements && tails[i].elementCount > 0) {
				biggest_elements = tails[i].elementCount;
				biggest_index = i;
			}
		}
		order_of_tails[index] = biggest_index;
		sorted_tails_info[index].width = tails[biggest_index].width;
		sorted_tails_info[index].Height = tails[biggest_index].height;
		sorted_tails_info[index].Inputs = tails[biggest_index].channels;
		sorted_tails_info[index].elements = tails[biggest_index].elementCount;
		sorted_tails_info[index].scale = tails[biggest_index].scale;
		tails[biggest_index].elementCount = 0;
	}
}

/* ev_inu_yolact_second_post_thread */
void* ev_inu_yolact_second_post_thread(void* arg)
{
	int status;
	uint64_t start, end;
	YOLACT_SECOND_INFO* yolact_second_info_p = NULL;
	UINT32  localThreadParamsSize = sizeof(yolact_second_info_p);

	LOGG_PRINT(LOG_INFO_E, (ERRG_codeE)NULL, "Yolact second thread started on core %d\n", evGetCpu());

	while (1) {
		status = CLIENT_APIG_recvMsg(&yolact_second_msgQue, (UINT8*)&yolact_second_info_p, &localThreadParamsSize, 0 /*not used*/);
		if (status == SUCCESS_E) {
			LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL, "yolact_second_post: Got message operation:%d\n", yolact_second_info_p->operation);
			start = getRTC();
			switch (yolact_second_info_p->operation) {
				case RESHAPE_OPERATION:
					inu_reshape_chw2hwc(
						yolact_second_info_p->reshape_info.in_ptr,
						yolact_second_info_p->reshape_info.out_ptr,
						yolact_second_info_p->reshape_info.in_H,
						yolact_second_info_p->reshape_info.in_W,
						yolact_second_info_p->reshape_info.in_ch,
						yolact_second_info_p->reshape_info.scale,
						yolact_second_info_p->reshape_info.do_tanh,
						false, yolact_second_info_p->split_th);
					break;
				case CONVERT_OPERATION:
					inu_convert2float(
						yolact_second_info_p->to_float_info.short_p,
						yolact_second_info_p->to_float_info.float_p,
						yolact_second_info_p->to_float_info.elements,
						yolact_second_info_p->to_float_info.float_factor,
						yolact_second_info_p->to_float_info.float_type,
						false, yolact_second_info_p->split_th);
					break;
				case RESIZE_OPERATION:
					//LOGG_PRINT(LOG_INFO_E, (ERRG_codeE)NULL, "before resize\n");
					Ev_resize_float_mat(
						yolact_second_info_p->resize_info.in_x, 
						yolact_second_info_p->resize_info.in_y, 
						yolact_second_info_p->resize_info.in_mat, 
						yolact_second_info_p->resize_info.out_x, 
						yolact_second_info_p->resize_info.out_y, 
						yolact_second_info_p->resize_info.out_mat, 
						yolact_second_info_p->resize_info.thresh, 
						yolact_second_info_p->resize_info.class_id, 
						yolact_second_info_p->resize_info.input_element_size,
						false, yolact_second_info_p->split_th);
					//LOGG_PRINT(LOG_INFO_E, (ERRG_codeE)NULL, "after resize\n");
					break;
				case COMBINE_OPERATION:
					Ev_yolact_combine(
						yolact_second_info_p->combine_info.num_mats, 
						yolact_second_info_p->combine_info.x_size,
						yolact_second_info_p->combine_info.y_size,
						yolact_second_info_p->combine_info.mats_ptrs,
						yolact_second_info_p->combine_info.out_mat,
						false, yolact_second_info_p->split_th);
					break;
				default:
					LOGG_PRINT(LOG_ERROR_E, (ERRG_codeE)NULL, "ev_inu_yolact_second_post_thread: bad operation: %d\n", yolact_second_info_p->operation);
					break;
			}
			
			end = getRTC();

			/* Signal end of second yolact processing */
			evIntCondSignal(&YolactSecondCondSignal);

			LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL, "second (%s) cycles:%lld %fms\n", operation_text[yolact_second_info_p->operation], (end - start), (float)(end - start) / (ev_clock_rate * 1000));
		}
		else {
			LOGG_PRINT(LOG_ERROR_E, (ERRG_codeE)NULL, "second_thread: failed to recv\n");
		}
	}
}


/* ev_inu_yolact_post_thread */
void *ev_inu_yolact_post_thread(void *arg)
{
#ifndef INU_HAPS
	inu_cdnn_data__hdr_t        *resultsHeaderP;
	inu_cdnn_data__tailHeaderT  *tailHeaderP;
	static bool inu_yolact_init_done = false;

	//		LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL, "evGetStackUsage: EVTH_THREADS_PER_CPU: %d\n", evthThreadsPerCpu);
	//		for (int i = 0; i < evthThreadsPerCpu; i++)
	//	    {
	//			LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL, "evGetStackUsage: 0:%d 1:%d\n", evGetStackUsage(0, i), evGetStackUsage(1, i));
	//		}
	LOGG_PRINT(YOLACT_TOTAL_MEAS_TIME, (ERRG_codeE)NULL, "YOLACT post process thread started on core %d\n", evGetCpu());

	while (1)
	{
		static bool first_cnn = true;
		evIntCondWait(&YOLACT_PPG_condition);
		yolact_mutex_lock();
		LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL, "ev_inu_yolact_post_thread started on core %d frame:%d\n", evGetCpu(), yolact_post_params.frame_id);

		if (!inu_yolact_init_done) {
			inu_yolact_init_done = true;
			inu_yolact_init();
		}

		yolact_image_rows = yolact_post_params.image_rows;
		yolact_image_cols = yolact_post_params.image_cols;

#ifdef YOLACT_PP_PROCESS_TIME
		uint64_t yolact_clock1, yolact_clock2;
		yolact_clock1 = getRTC();
#endif
		if (first_cnn) {
			first_cnn = false;
			prepare_order_of_tails();
		}

		inu_yolact_sort_tails(); // Sort the tails by thier size in reverse order
		inu_yolact_parser(sorted_tails_ints, sorted_tails_info);

#ifdef YOLACT_PP_PROCESS_TIME
		yolact_clock2 = getRTC();
		LOGG_PRINT(YOLACT_TOTAL_MEAS_TIME, (ERRG_codeE)NULL, "yolact post time (%d) is cycles:%lld %fms elements:%d\n", (int)ev_clock_rate, (yolact_clock2 - yolact_clock1), (float)(yolact_clock2 - yolact_clock1) / (ev_clock_rate*1000), elements_above_thresh);
#endif

		/* Fill in the result header and result buffer */
		resultsHeaderP = (inu_cdnn_data__hdr_t *)yolact_post_params.blob_output_header;
		tailHeaderP = resultsHeaderP->tailHeader;
		resultsHeaderP->network_id = yolact_post_params.net_id;
		resultsHeaderP->engineType = INU_LOAD_NETWORK__EV61_E;
		resultsHeaderP->frameId = yolact_post_params.frame_id;
		resultsHeaderP->numOfTails = 2;

		/* First tail has the image mask: yolact_image_cols * yolact_image_rows */
		resultsHeaderP->tailHeader[0].elementSize = 1;
		resultsHeaderP->tailHeader[0].elementCount = yolact_image_cols * yolact_image_rows;
		resultsHeaderP->tailHeader[0].inputs = 1;
		resultsHeaderP->tailHeader[0].width = yolact_image_cols;
		resultsHeaderP->tailHeader[0].height = yolact_image_rows;
		resultsHeaderP->tailHeader[0].cnnFields.tailSynopsysFields.scale = 1;
		resultsHeaderP->offsetToBlob[0] = 0;

		LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL, "Tail 0: width:%d height:%d\n", resultsHeaderP->tailHeader[0].width, resultsHeaderP->tailHeader[0].height);
		signed char *out_p;
		out_p = (signed char *)yolact_post_params.output_ptr;

		/* Copy the first tail to out_p */
		memcpy(&out_p[resultsHeaderP->offsetToBlob[0]], result_segments_mat, resultsHeaderP->tailHeader[0].elementSize * resultsHeaderP->tailHeader[0].elementCount);

		resultsHeaderP->totOutputSize = resultsHeaderP->tailHeader[0].elementCount;

		/* Second tails has the boxes. First 4 bytes has number of boxes */
		int* num_of_boxes_addr = (int*)&out_p[resultsHeaderP->totOutputSize];
		*num_of_boxes_addr = elements_above_thresh;

		if (elements_above_thresh) {
			memcpy(num_of_boxes_addr+1, yolact_boxes, elements_above_thresh * sizeof(EvCnnBoundingBox));
		}

		resultsHeaderP->tailHeader[1].elementSize = sizeof(EvCnnBoundingBox);
		resultsHeaderP->tailHeader[1].elementCount = elements_above_thresh;
		resultsHeaderP->tailHeader[1].inputs = 1;
		resultsHeaderP->tailHeader[1].width = 1;
		resultsHeaderP->tailHeader[1].height = 1;
		resultsHeaderP->tailHeader[1].cnnFields.tailSynopsysFields.scale = 1;
		resultsHeaderP->offsetToBlob[1] = resultsHeaderP->totOutputSize;

		resultsHeaderP->totOutputSize += elements_above_thresh * sizeof(EvCnnBoundingBox) + sizeof(int);

		evDataCacheFlushLines(resultsHeaderP, resultsHeaderP + sizeof(inu_cdnn_data__hdr_t));
		evDataCacheFlushLines(yolact_post_params.output_ptr, (unsigned char *)(yolact_post_params.output_ptr) + resultsHeaderP->totOutputSize);
		LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL, "tails num %d total output size %d cache line %d\n", resultsHeaderP->numOfTails, resultsHeaderP->totOutputSize, evDataCacheLineSize());
		SCHEDG_pushFinishedJob(yolact_post_params.jobDescriptor);
		evIntCondSignal(&SCHEDG_Condition);
#endif

		//		LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL, "ev_inu_yolact_post_thread ended on core %d\n", evGetCpu());
		yolact_mutex_unlock();
	}
	return(NULL);
}

/* resize_class_to_image_size */
void resize_class_to_image_size(int in_cols, int in_rows, RESIZE_INPUT_TYPE* in_image, int out_cols, int out_rows, unsigned char* out_image, float thresh, unsigned char class_id) {
	cv::Mat in_mat, resized_mat;
	static float *resized_mat_buffer = NULL;
	uint64_t start, end;
	int input_element_size = sizeof(RESIZE_INPUT_TYPE);

	start = getRTC();

#if 1
	float split_th = 0.5;

	evDataCacheFlushInvalidateLines(out_image, out_image + out_cols * out_rows);
	evDataCacheFlushInvalidateLines(in_image, in_image + in_cols * in_rows);

	if (split_th != 1) {
		yolact_second_info.operation = RESIZE_OPERATION;
		yolact_second_info.split_th = split_th;
		yolact_second_info.resize_info.in_x = in_cols;
		yolact_second_info.resize_info.in_y = in_rows;
		yolact_second_info.resize_info.in_mat = in_image;
		yolact_second_info.resize_info.out_x = out_cols;
		yolact_second_info.resize_info.out_y = out_rows;
		yolact_second_info.resize_info.out_mat = out_image;
		yolact_second_info.resize_info.thresh = thresh;
		yolact_second_info.resize_info.class_id = class_id + 1;
		yolact_second_info.resize_info.input_element_size = input_element_size;
		LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL, "resize before send to second: %f\n", split_th);
		send_to_second_yolact(split_th);
	}

	Ev_resize_float_mat(in_cols, in_rows, in_image, out_cols, out_rows, out_image, thresh, class_id+1, input_element_size, true, split_th);

	end = evGetTimeInCycles();
	LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL, "resize first time (%dMhz) is cycles:%lld %fms\n", (int)ev_clock_rate, (end - start), (float)(end - start) / (ev_clock_rate * 1000));

	wait_for_second_yolact(split_th);
	LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL, "after wait for second\n");

#else
	if (!resized_mat_buffer) {
		resized_mat_buffer = (float*)yolact_malloc(sizeof(float) * out_cols * out_rows);
		if (!resized_mat_buffer) {
			LOGG_PRINT(LOG_ERROR_E, (ERRG_codeE)NULL, "resize_class_to_image_size: Failed to allocate resized_mat_buffer\n");
			return;
		}
	}

	in_mat = cv::Mat(in_rows, in_cols, CV_32FC1, (uchar*)in_image);
	resized_mat = cv::Mat(out_rows, out_cols, CV_32FC1, (uchar*)resized_mat_buffer);
	resize(in_mat, resized_mat, cv::Size(out_cols, out_rows), 0, 0, cv::INTER_LINEAR);

	/* Set the class ID's */
	memset(out_image, 0, out_cols * out_rows);
	float* resize_ptr = (float*)resized_mat.data;
	for (int pixel = 0; pixel < out_cols * out_rows; pixel++) {
		if (resize_ptr[pixel] >= thresh) {
			out_image[pixel] = class_id+1; // Keeps 0 classID to background
		}
	}
#endif

	end = getRTC();
	LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL, "resize time (%dMhz) is %fms rows:%d cols:%d\n", (int)ev_clock_rate, (float)(end - start) / (ev_clock_rate * 1000), out_rows, out_cols);
}

/* combine_results */
void combine_results(int elements_above_thresh, int cols, int rows, unsigned char **result_segments_mat, unsigned char *out_mat)
{
	uint64_t start, end;
	start = getRTC();

#if 1
	float split_th = 0.5;
	for (int i = 0; i < elements_above_thresh; i++) {
		evDataCacheFlushInvalidateLines(result_segments_mat[i], result_segments_mat[i] + cols * rows);
	}
	evDataCacheFlushInvalidateLines(out_mat, out_mat + cols * rows);

	/* If only one segment, just copy it to the output, else combine */
	if (elements_above_thresh > 1) {
		if (split_th != 1) {
			yolact_second_info.operation = COMBINE_OPERATION;
			yolact_second_info.split_th = split_th;
			yolact_second_info.combine_info.num_mats = elements_above_thresh;
			yolact_second_info.combine_info.x_size = cols;
			yolact_second_info.combine_info.y_size = rows;
			yolact_second_info.combine_info.mats_ptrs = result_segments_mat;
			yolact_second_info.combine_info.out_mat = out_mat;
			LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL, "combine before send to second: %f\n", split_th);
			send_to_second_yolact(split_th);
		}
		Ev_yolact_combine(elements_above_thresh, cols, rows, result_segments_mat, out_mat, true, split_th);
		wait_for_second_yolact(split_th);

/// scalar
#if 0
		static unsigned char tmp_mat[1600 * 1200];
		copy_kernel(zeros_buffer, tmp_mat, cols * rows);

		for (int pixel = 0; pixel < cols * rows; pixel++) {
			unsigned char ref_val = 0;
			for (int mat_ind = 0; mat_ind < elements_above_thresh; mat_ind++) {
				if (result_segments_mat[mat_ind][pixel]) {
					if (result_segments_mat[mat_ind][pixel] > ref_val) {
						tmp_mat[pixel] = result_segments_mat[mat_ind][pixel];
						ref_val = result_segments_mat[mat_ind][pixel];
					}
				}
			}
		}

		// compare
		int err_count = 0;
		for (int i = 0; i < cols * rows; i++) {
			if (tmp_mat[i] != out_mat[i]) {
				err_count++;
			}
		}
		LOGG_PRINT(LOG_INFO_E, (ERRG_codeE)NULL, "combine erer_count:%d\n", err_count);
#endif
////
	}
	else {
		copy_kernel(result_segments_mat[0], out_mat, cols * rows);
	}
#else
	//memset(out_mat, 0, cols * rows);
	//evDataCacheFlushInvalidateLines(out_mat, out_mat + cols * rows);
	evDataCacheFlushInvalidateLines(out_mat, out_mat + cols * rows);
	copy_kernel(zeros_buffer, out_mat, cols * rows);

	for (int pixel = 0; pixel < cols * rows; pixel++) {
		for (int mat_ind = 0; mat_ind < elements_above_thresh; mat_ind++) {
			if (result_segments_mat[mat_ind][pixel]) {
				out_mat[pixel] = result_segments_mat[mat_ind][pixel];
				break;
			}
		}
	}
#endif

	end = getRTC();
	LOGG_PRINT(YOLACT_MEAS_TIME, (ERRG_codeE)NULL, "combine time (%dMhz) is  %fms rows:%d cols:%d\n", (int)ev_clock_rate, (float)(end - start) / (ev_clock_rate * 1000), rows, cols);
}

/* compare_short_to_float */
int compare_short_to_float(short* short_vec, float* float_vec, int elements, int tail_id, char* text)
{
	float max_diff = -9999;
	int max_index = -1, count_diff;
	float scalar_val, benny_val;

	count_diff = 0;
	for (int i = 0; i < elements; i++) {
		float vec_val = (float)short_vec[i] * float_factor;
		float diff = abs(vec_val - float_vec[i]);
		if (diff > 0.1) count_diff++;
		if (diff > max_diff) {
			max_diff = diff;
			max_index = i;
			scalar_val = float_vec[i];
			benny_val = vec_val;
		}
	}
	LOGG_PRINT(LOG_INFO_E, (ERRG_codeE)NULL, "%s:%d maxdif:%f scalar:%f vec:%f cnt:%d max_ind:%d elem:%d\n", text, tail_id, max_diff, scalar_val, benny_val, count_diff, max_index, elements);

	return(max_index);
}

/* short_to_float */
void short_to_float(short* short_p, void* out_pointer, int elements, bool float_type)
{
	float split_th = 0.5;
	float* float_p;
	__fp16* fp16_p;

	start_meas();

	if (float_type) {
		float_p = (float*)out_pointer;
		evDataCacheFlushInvalidateLines(float_p, float_p + elements);
	}
	else {
		fp16_p = (__fp16*)out_pointer;
		evDataCacheFlushInvalidateLines(fp16_p, fp16_p + elements);
	}

	if (split_th != 1) {
		yolact_second_info.operation = CONVERT_OPERATION;
		yolact_second_info.split_th = split_th;
		yolact_second_info.to_float_info.short_p = short_p;
		yolact_second_info.to_float_info.float_p = out_pointer;
		yolact_second_info.to_float_info.float_factor = float_factor;
		yolact_second_info.to_float_info.elements = elements;
		yolact_second_info.to_float_info.float_type = float_type;
		send_to_second_yolact(split_th);
	}


	inu_convert2float(short_p, float_p, elements, float_factor, float_type, true, split_th);
	wait_for_second_yolact(split_th);

	end_meas("convert2float");
}

/* yolact_convert_and_reshape  - doing python operations: reshape(1, size1, size2, size3) and permute(0, 2, 3, 1) */
void yolact_convert_and_reshape(int tail_id, short* in_tail, int elements, int size1, int size2, int size3, float scale, bool do_tanh)
{
	float split_th;

	/* Split only the 2 first tails that takes some time */
	if (tail_id < 2) {
		split_th = 0.5;
	}
	else {
		split_th = 1.0;
	}

	uint64_t start, end;

	//printf("yolact_convert_and_reshape: elements:%d  size_mult:%d\n", elements, size1*size2*size3);

	if (!reshaped_tails[tail_id]) {
		reshaped_tails[tail_id] = (float*)yolact_malloc(sizeof(float) * elements);
		reshaped_tails_elements[tail_id] = elements;
	}

	if (!short_reshaped_tails[tail_id]) {
		LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL, "Alloc short_reshaped_tails %d elements:%d\n", tail_id, elements);
		short_reshaped_tails[tail_id] = (short*)yolact_malloc(sizeof(short) * elements);
	}

#if 0
	int i1, i2, i3;
	int in_index, out_index;
	out_index = 0;
	for (i2 = 0; i2 < size2; i2++) {
		for (i3 = 0; i3 < size3; i3++) {
			for (i1 = 0; i1 < size1; i1++) {
				in_index = i1 * (size2 * size3) + i2 * size3 + i3;
				reshaped_tails[tail_id][out_index] = in_tail[in_index] / scale;
				if (do_tanh) {
					reshaped_tails[tail_id][out_index] = inu_tanh(reshaped_tails[tail_id][out_index]);
				}
				out_index++;
			}
		}
	}
	evDataCacheFlushInvalidateLines(reshaped_tails[tail_id], reshaped_tails[tail_id] + elements);
#endif

	/* vectorized function. size1: channels size2:hight size3:width*/
	LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL, "before reshape tail %d elements:%d size1:%d size2:%d size3:%d tan:%d\n", tail_id, elements, size1, size2, size3, do_tanh);

	start = evGetTimeInCycles();
	evDataCacheFlushInvalidateLines(short_reshaped_tails[tail_id], short_reshaped_tails[tail_id] + elements);

	if (split_th != 1) {
		yolact_second_info.operation = RESHAPE_OPERATION;
		yolact_second_info.split_th = split_th;
		yolact_second_info.reshape_info.in_ptr = in_tail;
		yolact_second_info.reshape_info.out_ptr = short_reshaped_tails[tail_id];
		yolact_second_info.reshape_info.in_H = size2;
		yolact_second_info.reshape_info.in_W = size3;
		yolact_second_info.reshape_info.in_ch = size1;
		yolact_second_info.reshape_info.scale = scale;
		yolact_second_info.reshape_info.do_tanh = do_tanh;
		send_to_second_yolact(split_th);
	}

	inu_reshape_chw2hwc(in_tail, short_reshaped_tails[tail_id], size2, size3, size1, scale, do_tanh, true, split_th);

	wait_for_second_yolact(split_th);
	end = evGetTimeInCycles();
	LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL, "reshape time (%dMhz) is cycles:%lld %fms\n", (int)ev_clock_rate, (end - start), (float)(end - start) / (ev_clock_rate * 1000));


#if 0
	compare_short_to_float(short_reshaped_tails[tail_id], reshaped_tails[tail_id], elements, tail_id, "reshape");
#endif
}

/* inu_yolact_sort_tails */
void inu_yolact_sort_tails(void)
{
	short *short_blob = (short *)yolact_post_params.output_ptr;
	short *tails_ptrs[INU_YOLACT_NUMBER_OF_TAILS];

	for (int i = 0; i < INU_YOLACT_NUMBER_OF_TAILS; i++) {
		tails_ptrs[i] = short_blob;
		short_blob += yolact_post_params.tails[i].elementCount;
	}

	for (int i = 0; i < INU_YOLACT_NUMBER_OF_TAILS; i++) {
		int tail_id = order_of_tails[i];
		sorted_tails_ints[i] = tails_ptrs[tail_id];
	}
}

/* inu_exp */
static float inu_exp(float val)
{
	float result;
	int index;
	static float  step_size = 9.765625e-04F; //abs(Max_x - Min_x) / Lut_size = 9.765625e-04

	if (val >= 2) {
		result = powf(E_CONST, val);
	}
	else {
		if (val < -6) val = -6;
		index = (int)(val / step_size + 0.5) + 6145;
		result = inu_exp_lut[index];
	}
	return result;
}


/* inu_tanh */
float inu_tanh(float val) {
	float result;
	int index;
	static float max_x = 8.F;
	static float step_size = 2 * max_x / INU_TANH_LUT_SIZE;

	if (val > max_x) {
		val = max_x;
	}
	else if (val < -max_x) {
		val = -max_x;
	}

	index = (val / step_size + 0.5) + INU_TANH_LUT_SIZE / 2 + 1;
	result = inu_tanh_lut[index];
	if (result > 1) {
		LOGG_PRINT(LOG_ERROR_E, (ERRG_codeE)NULL, "inu_tanh: index:%d val:%f result:%f\n", index, val, result);
	}
	return(result);
}

/* prep_box */
static yolact_box prep_box(float *box_vals)
{
	float xmin, xmax, ymin, ymax;
	yolact_box b;
	xmin = box_vals[0];
	ymin = box_vals[1];
	xmax = box_vals[2];
	ymax = box_vals[3];
	b.w = (xmax - xmin); // width
	b.h = (ymax - ymin); // hight
	b.x = xmin + b.w / 2; // center x
	b.y = ymin + b.h / 2; // center y
	return(b);
}

/* overlap */
static float overlap(float x1, float w1, float x2, float w2)
{
	float l1 = x1 - w1 / 2;
	float l2 = x2 - w2 / 2;
	float left = l1 > l2 ? l1 : l2;
	float r1 = x1 + w1 / 2;
	float r2 = x2 + w2 / 2;
	float right = r1 < r2 ? r1 : r2;
	return right - left;
}

/* box_intersection */
static float box_intersection(yolact_box a, yolact_box b)
{
	float w = overlap(a.x, a.w, b.x, b.w);
	float h = overlap(a.y, a.h, b.y, b.h);
	if (w < 0 || h < 0) return 0;
	float area = w*h;
	return area;
}

/* box_union */
static float box_union(yolact_box a, yolact_box b)
{
	float i = box_intersection(a, b);
	float u = a.w*a.h + b.w*b.h - i;
	return u;
}

/* box_iou */
static float box_iou(yolact_box a, yolact_box b)
{
	return box_intersection(a, b) / box_union(a, b);
}

/* cmp_sorted_indexes */
static int cmp_sorted_indexes(const void* a, const void* b)
{
	_sorted_indexes* a1 = (_sorted_indexes*)a;
	_sorted_indexes* a2 = (_sorted_indexes*)b;
	if ((*a1).val > (*a2).val)
		return -1;
	else if ((*a1).val < (*a2).val)
		return 1;
	else
		return 0;
}

/* get_sorted_indexes_descending */
static void get_sorted_indexes_descending(float *float_vals, int *indexes_array, int len)
{
#if 1
	static _sorted_indexes* tmp_float_vals = NULL;

	LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL, "get_sorted_indexes_descending: len:%d max_num_scores_elements:%d\n", len, max_num_scores_elements);

	if (!tmp_float_vals) {
		tmp_float_vals = (_sorted_indexes*)yolact_malloc(sizeof(_sorted_indexes) * max_num_scores_elements);
		if (!tmp_float_vals) {
			LOGG_PRINT(LOG_ERROR_E, (ERRG_codeE)NULL, "get_sorted_indexes: Failed to allocate tmp_float_vals\n");
			return;
		}
		else {
			LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL, "allocated tmp_float_vals\n");
		}
	}

	for (int i = 0; i < len; i++) {
		tmp_float_vals[i].index = i;
		tmp_float_vals[i].val = float_vals[i];
	}

	qsort(tmp_float_vals, len, sizeof(_sorted_indexes), cmp_sorted_indexes);

	for (int i = 0; i < len; i++) {
		indexes_array[i] = tmp_float_vals[i].index;
	}
#else
	static float *tmp_float_vals = NULL;

	if (!tmp_float_vals) {
		tmp_float_vals = (float *)yolact_malloc(sizeof(float)*max_num_scores_elements);
		if (!tmp_float_vals) {
			LOGG_PRINT(LOG_ERROR_E, (ERRG_codeE)NULL, "get_sorted_indexes: Failed to allocate tmp_float_vals\n");
		}
	}

	memcpy(tmp_float_vals, float_vals, sizeof(float)*len);
	for (int i = 0; i < len; i++) {
		float max_val = -99999999F;
		int max_index = -1;
		for (int j = 0; j < len; j++) {
			if (tmp_float_vals[j] > max_val) {
				max_val = tmp_float_vals[j];
				max_index = j;
			}
		}
		indexes_array[i] = max_index;
		tmp_float_vals[max_index] = -999999F;
	}
#endif
}

/* inu_yolact_init */
void inu_yolact_init(void)
{
	int prior_ind;
	float x, y, w, h, ar, scale;

	LOGG_PRINT(LOG_INFO_E, (ERRG_codeE)NULL, "inu_yolact_init yolact_post_params.net_input_size:%d\n", yolact_post_params.net_input_size);

	all_prior_elements = 0;
	/* Prepare the priors */
	for (int c_ind = 0; c_ind < NUM_OF_CONV_SIZE; c_ind++) {
		int prior_size = conv_sizes[c_ind] * conv_sizes[c_ind] * NUM_OF_ASPECT_RATIOS * 4;
		all_prior_elements += prior_size;
		priors_sizes[c_ind] = prior_size;
		priors[c_ind] = (float *)yolact_malloc(sizeof(float) * prior_size);
		if (!priors[c_ind]) {
			LOGG_PRINT(LOG_ERROR_E, (ERRG_codeE)NULL, "init_yoloact: failed to allocate %d float elements. c_ind:%d\n", prior_size, c_ind);
			return;
		}
		prior_ind = 0;
		for (int j = 0; j < conv_sizes[c_ind]; j++) {
			scale = (float)scales[c_ind];
			for (int i = 0; i < conv_sizes[c_ind]; i++) {
				x = ((float)i + 0.5F) / (float)conv_sizes[c_ind];
				y = ((float)j + 0.5F) / (float)conv_sizes[c_ind];

				for (int ars_ind = 0; ars_ind < NUM_OF_ASPECT_RATIOS; ars_ind++) {
					ar = sqrt(aspect_ratios[ars_ind]);
					//w = scale * ar / INU_YOLOACT_MAX_SIZE;
					w = scale * ar / yolact_post_params.net_input_size;
					h = w;
					priors[c_ind][prior_ind++] = x;
					priors[c_ind][prior_ind++] = y;
					priors[c_ind][prior_ind++] = w;
					priors[c_ind][prior_ind++] = h;
				}
			}
		}

		//LOGG_PRINT(LOG_ERROR_E, (ERRG_codeE)NULL, "\nc_ind:%d\n", c_ind);
		//for (int ii = 0; ii < prior_size; ii=ii+4) {
		//	LOGG_PRINT(LOG_INFO_E, (ERRG_codeE)NULL, "%f %f %f %f\n", priors[c_ind][ii], priors[c_ind][ii + 1], priors[c_ind][ii + 2], priors[c_ind][ii + 3]);
		//}
	}

	all_priors = (float *)yolact_malloc(sizeof(float) * all_prior_elements);
	if (!all_priors) {
		LOGG_PRINT(LOG_ERROR_E, (ERRG_codeE)NULL, "Failed to allocate for all_priors\n");
		return;
	}
	else {
		int all_prior_ind = 0;
		for (int i = 0; i < NUM_OF_CONV_SIZE; i++) {
			memcpy(&all_priors[all_prior_ind], priors[i], priors_sizes[i] * sizeof(float));
			all_prior_ind += priors_sizes[i];
		}
	}

	LOGG_PRINT(LOG_INFO_E, (ERRG_codeE)NULL, "inu_yolact_init done OK\n");

}

//#define MEASURE_TANH 1
/* yolact_tanh - Convert array values to tanh*/
static void yolact_tanh(float *vals, int elements)
{
#ifdef MEASURE_TANH
	static SYSTEMTIME t1, t2;
	int tanh_time_diff;

	GetSystemTime(&t1);
#endif

	for (int i = 0; i < elements; i++) {
		//vals[i] = tanhf(vals[i]);
		vals[i] = inu_tanh(vals[i]);
	}

#ifdef MEASURE_TANH
	GetSystemTime(&t2);

	tanh_time_diff = (t2.wSecond - t1.wSecond) * 1000 +
		(t2.wMilliseconds - t1.wMilliseconds);
	LOGG_PRINT(LOG_INFO_E, (ERRG_codeE)NULL, "tanh time %dms for %d elements\n", tanh_time_diff, elements);
#endif

}

/* yolact_reshape  - doing python operations: reshape(1, size1, size2, size3) and permute(0, 2, 3, 1) */
static void yolact_reshape(int tail_id, float *in_tail, int elements, int size1, int size2, int size3)
{
	int i1, i2, i3;
	int in_index, out_index;

	//LOGG_PRINT(LOG_INFO_E, (ERRG_codeE)NULL, "yolact_reshape: elements:%d  size_mult:%d\n", elements, size1*size2*size3);

	if (!reshaped_tails[tail_id]) {
		reshaped_tails[tail_id] = (float *)yolact_malloc(sizeof(float)*elements);
		reshaped_tails_elements[tail_id] = elements;
	}

	out_index = 0;
	for (i2 = 0; i2 < size2; i2++) {
		int i2_size3 = i2 * size3;
		for (i3 = 0; i3 < size3; i3++) {
			int size2_size3 = size2*size3;
			int i2_size3_i3 = i2_size3 + i3;
			for (i1 = 0; i1 < size1; i1++) {
				//in_index = i1 * (size2*size3) + i2 * size3 + i3;
				in_index = i1 * size2_size3 + i2_size3_i3;
				reshaped_tails[tail_id][out_index] = in_tail[in_index];
				out_index++;
			}
		}
	}
}

/* softmax  - softmax(Xi) = exp(Xi) / Sum(exp(Xi))  */
static void softmax(float *in, float *out, int elements, int group_size)
{
	float sum;

	//LOGG_PRINT(LOG_INFO_E, (ERRG_codeE)NULL, "softmax groups:%d out:%p\n", elements, out);

	for (int i = 0; i < elements; i = i + group_size) {
		sum = 0;
		for (int j = 0; j < group_size; j++) {
			//out[i + j] = powf(E_CONST, (in[i + j]));
			out[i + j] = inu_exp(in[i + j]);
			sum += out[i + j];
		}
		for (int j = 0; j < group_size; j++) {
			out[i + j] = out[i + j] / sum;
		}
		//LOGG_PRINT(LOG_INFO_E, (ERRG_codeE)NULL, "In:%f %f %f out:%f %f %f\n", in[i], in[i + 1], in[i + 2], out[i], out[i + 1], out[i + 2]);
	}
}

/* allocate_buffers */
static void allocate_buffers(void)
{
	/* Allocate conf_data */
	int elements = reshaped_tails_elements[CONF_TAIL_1] + reshaped_tails_elements[CONF_TAIL_2] + reshaped_tails_elements[CONF_TAIL_3] + reshaped_tails_elements[CONF_TAIL_4] + reshaped_tails_elements[CONF_TAIL_5];
	LOGG_PRINT(LOG_INFO_E, (ERRG_codeE)NULL, "Number of conf elements for conf: %d\n", elements);
	conf_data_32 = (float *)yolact_malloc(sizeof(float) * elements);
	if (!conf_data_32) {
		LOGG_PRINT(LOG_ERROR_E, (ERRG_codeE)NULL, "Failed to allocate meory for conf_data_32\n");
		return;
	}

	conf_data_16 = (__fp16*)yolact_malloc(sizeof(__fp16) * elements);
	if (!conf_data_16) {
		LOGG_PRINT(LOG_ERROR_E, (ERRG_codeE)NULL, "Failed to allocate meory for conf_data_16\n");
		return;
	}
	short_conf_data = (short*)yolact_malloc(sizeof(short) * elements);
	if (!short_conf_data) {
		LOGG_PRINT(LOG_ERROR_E, (ERRG_codeE)NULL, "Failed to allocate meory for short_conf_data\n");
		return;
	}
	conf_data_elements = elements;

	/* Allocate loc_data */
	loc_data_elements = reshaped_tails_elements[BBOX_TAIL_1] + reshaped_tails_elements[BBOX_TAIL_2] + reshaped_tails_elements[BBOX_TAIL_3] + reshaped_tails_elements[BBOX_TAIL_4] + reshaped_tails_elements[BBOX_TAIL_5];
	//LOGG_PRINT(LOG_INFO_E, (ERRG_codeE)NULL, "Number of conf elements for loc: %d\n", loc_data_elements);
	loc_data = (float*)yolact_malloc(sizeof(float) * loc_data_elements);
	if (!loc_data) {
		LOGG_PRINT(LOG_ERROR_E, (ERRG_codeE)NULL, "Failed to allocate meory for loc_data\n");
		return;
	}

	/* Allocate mask buffers */
	mask_data_elements = reshaped_tails_elements[MASK_TAIL_1] + reshaped_tails_elements[MASK_TAIL_2] + reshaped_tails_elements[MASK_TAIL_3] + reshaped_tails_elements[MASK_TAIL_4] + reshaped_tails_elements[MASK_TAIL_5];
	LOGG_PRINT(LOG_INFO_E, (ERRG_codeE)NULL, "Number of conf elements for mask_data: %d\n", mask_data_elements);
	mask_data = (float*)yolact_malloc(sizeof(float) * mask_data_elements);
	if (!mask_data) {
		LOGG_PRINT(LOG_ERROR_E, (ERRG_codeE)NULL, "Failed to allocate memory for mask_data\n");
		return;
	}

	//LOGG_PRINT(LOG_INFO_E, (ERRG_codeE)NULL, "Number of conf elements for mask_data_kept: %d\n", mask_data_elements);
	mask_data_kept = (float*)yolact_malloc(sizeof(float) * mask_data_elements);
	if (!mask_data_kept) {
		LOGG_PRINT(LOG_ERROR_E, (ERRG_codeE)NULL, "Failed to allocate memory for mask_data_kept\n");
		return;
	}

	mask_proto_result = (float*)yolact_malloc(sizeof(float) * INU_YOLACT_MAX_DETECTIONS * sorted_tails_info[PROTO_TAIL].Height * sorted_tails_info[PROTO_TAIL].width);
	if (!mask_proto_result) {
		LOGG_PRINT(LOG_ERROR_E, (ERRG_codeE)NULL, "Failed to allocate memory for mask_proto_result\n");
		return;
	}

	masks_crop_matrix_size = sizeof(int) * INU_YOLACT_MAX_DETECTIONS * sorted_tails_info[PROTO_TAIL].Height * sorted_tails_info[PROTO_TAIL].width;
	masks_crop_matrix = (int*)yolact_malloc(masks_crop_matrix_size);
	if (!masks_crop_matrix) {
		LOGG_PRINT(LOG_ERROR_E, (ERRG_codeE)NULL, "Failed to allocate memory for masks_crop_matrix\n");
		return;
	}

	mask_cropped_contig_size = sizeof(RESIZE_INPUT_TYPE) * INU_YOLACT_MAX_DETECTIONS * sorted_tails_info[PROTO_TAIL].Height * sorted_tails_info[PROTO_TAIL].width;
	mask_cropped_contig = (RESIZE_INPUT_TYPE*)yolact_malloc(mask_cropped_contig_size);
	if (!mask_cropped_contig) {
		LOGG_PRINT(LOG_ERROR_E, (ERRG_codeE)NULL, "Failed to allocate memory for mask_cropped_contig\n");
		return;
	}

	//printf("Number of conf elements for mask_sorted: %d\n", mask_data_elements);
	for (int i = 0; i < yolact_num_of_classes; i++) {
		mask_sorted[i] = (float*)yolact_malloc(sizeof(float) * mask_data_elements);
		if (!mask_sorted[i]) {
			LOGG_PRINT(LOG_ERROR_E, (ERRG_codeE)NULL, "mask_sorted to allocate memory for mask_sorted %d\n", i);
			return;
		}
	}

	/* Allocate memory for boxes */
	decoded_boxes_data_elements = reshaped_tails_elements[BBOX_TAIL_1] + reshaped_tails_elements[BBOX_TAIL_2] + reshaped_tails_elements[BBOX_TAIL_3] + reshaped_tails_elements[BBOX_TAIL_4] + reshaped_tails_elements[BBOX_TAIL_5];
	//LOGG_PRINT(LOG_INFO_E, (ERRG_codeE)NULL, Number of conf elements for boxes_data: %d\n", decoded_boxes_data_elements);
	decoded_boxes_data = (float*)yolact_malloc(sizeof(float) * decoded_boxes_data_elements);
	if (!decoded_boxes_data) {
		LOGG_PRINT(LOG_ERROR_E, (ERRG_codeE)NULL, "Failed to allocate memory for boxes_data\n");
		return;
	}

	//LOGG_PRINT(LOG_INFO_E, (ERRG_codeE)NULL, "Number of conf elements for decoded_boxes_kept: %d\n", decoded_boxes_data_elements);
	decoded_boxes_kept = (float*)yolact_malloc(sizeof(float) * decoded_boxes_data_elements);
	if (!decoded_boxes_kept) {
		LOGG_PRINT(LOG_ERROR_E, (ERRG_codeE)NULL, "Failed to allocate memory for boxes_data\n");
		return;
	}

	//LOGG_PRINT(LOG_INFO_E, (ERRG_codeE)NULL, "Number of conf elements for boxes_sorted: %d\n", decoded_boxes_data_elements);
	for (int i = 0; i < yolact_num_of_classes; i++) {
		boxes_sorted[i] = (float*)yolact_malloc(sizeof(float) * decoded_boxes_data_elements);
		if (!boxes_sorted[i]) {
			LOGG_PRINT(LOG_ERROR_E, (ERRG_codeE)NULL, "Failed to allocate m for boxes_sorted %d\n", i);
			return;
		}
	}

	/* Allocate memory for confidence */
	for (int i = 0; i < yolact_num_of_classes; i++) {
		//LOGG_PRINT(LOG_INFO_E, (ERRG_codeE)NULL, "Number of conf elements for conf_scores(2): %d\n", all_prior_elements / 4); // Allocates maximum size
		conf_scores[i] = (float*)yolact_malloc(sizeof(float) * all_prior_elements / 4);
		if (!conf_scores[i]) {
			LOGG_PRINT(LOG_ERROR_E, (ERRG_codeE)NULL, "Failed to allocate memory for conf_scores[%d]\n", i);
			return;
		}

		scores_sorted_indexes[i] = (int*)yolact_malloc(sizeof(int) * all_prior_elements / 4);
		if (!scores_sorted_indexes[i]) {
			LOGG_PRINT(LOG_ERROR_E, (ERRG_codeE)NULL, "Failed to allocate memory for scores_sorted_indexes[%d]\n", i);
			return;
		}
	}

	//LOGG_PRINT(LOG_INFO_E, (ERRG_codeE)NULL, "Number of conf elements for scores_kept/indexes: %d\n", all_prior_elements / 4);
	scores_kept = (float*)yolact_malloc(sizeof(float) * all_prior_elements / 4);
	if (!scores_kept) {
		LOGG_PRINT(LOG_ERROR_E, (ERRG_codeE)NULL, "Failed to allocate memory for scores_kept\n");
		return;
	}

	sorted_scores_indexes = (int*)yolact_malloc(sizeof(int) * all_prior_elements / 4);
	if (!sorted_scores_indexes) {
		LOGG_PRINT(LOG_ERROR_E, (ERRG_codeE)NULL, "Failed to allocate memory for sorted_scores_indexes\n");
		return;
	}

	classes_kept = (int*)yolact_malloc(sizeof(int) * all_prior_elements / 4);
	if (!classes_kept) {
		LOGG_PRINT(LOG_ERROR_E, (ERRG_codeE)NULL, "Failed to allocate memory for classes_kept\n");
		return;
	}

	/* Allocate buffers for the last stage */
	result_segments_mat = (unsigned char*)yolact_malloc(yolact_image_rows * yolact_image_cols);
	if (!classes_kept) {
		LOGG_PRINT(LOG_ERROR_E, (ERRG_codeE)NULL, "Failed to allocate memory for result_segments_mat\n");
		return;
	}

	for (int i = 0; i < INU_YOLACT_MAX_DETECTIONS; i++) {
		segments_mats[i] = (unsigned char*)yolact_malloc(yolact_image_rows * yolact_image_cols);
		if (!segments_mats[i]) {
			LOGG_PRINT(LOG_ERROR_E, (ERRG_codeE)NULL, "Failed to allocate memory for segments_mats[%d]\n", i);
			return;
		}
	}

	/* Prepare a vector of image size of zeros */
	zeros_buffer = (unsigned char*)yolact_malloc(yolact_image_rows * yolact_image_cols);
	if (!zeros_buffer) {
		LOGG_PRINT(LOG_ERROR_E, (ERRG_codeE)NULL, "Failed to allocate memory for zeros_buffer\n");
		return;
	}
	memset(zeros_buffer, 0, yolact_image_rows* yolact_image_cols);
	evDataCacheFlushInvalidateLines(zeros_buffer, zeros_buffer + yolact_image_rows * yolact_image_cols);

	max_values_array_elements = all_prior_elements / 4;
	max_values_array = (float*)yolact_malloc(max_values_array_elements * sizeof(float));
	if (!max_values_array) {
		LOGG_PRINT(LOG_ERROR_E, (ERRG_codeE)NULL, "Failed to allocate max_values_array\n");
		return;
	}
	else {
		LOGG_PRINT(LOG_INFO_E, (ERRG_codeE)NULL, "Allocated %d elements for max_values_array\n", max_values_array_elements);
	}
}

/* inu_yolact_parser */
void inu_yolact_parser(short **sorted_tails_ints, yolact_tail_info *sorted_tails_info)
{
	int cpy_ind;
	float split_th = 1.0;

	/*
	extern short tanh_lut_int_Q10[];
	unsigned int compute_checksum(unsigned char* data, int len);
	int checksum = compute_checksum((unsigned char*)tanh_lut_int_Q10, 2049*sizeof(short));
	LOGG_PRINT(LOG_INFO_E, (ERRG_codeE)NULL, "inu_yolact_parser tanh_lut_int_Q10 checksum: %d\n", checksum);
	*/

	init_meas_999();

	start_meas();

	for (int tail_id = 0; tail_id < INU_YOLACT_NUMBER_OF_TAILS; tail_id++) {
		int elements = sorted_tails_info[tail_id].elements;
		int width = sorted_tails_info[tail_id].width;
		int height = sorted_tails_info[tail_id].Height;
		int inputs = sorted_tails_info[tail_id].Inputs;
		float scale = sorted_tails_info[tail_id].scale;

		LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL, "tail %d W:%d H:%d in:%d elem:%d do_tanh:%d\n", tail_id, width, height, inputs, elements, do_tanh[tail_id]);
		yolact_convert_and_reshape(tail_id, sorted_tails_ints[tail_id], elements, inputs, height, width, scale, do_tanh[tail_id]);
	}

	end_meas("reshape");

	/* Convert tails to float. confidence tails are passed as shorts to vectorized softmax */
	for (int tail_id = 0; tail_id < INU_YOLACT_NUMBER_OF_TAILS; tail_id++) {
		if (tail_id != CONF_TAIL_1 && tail_id != CONF_TAIL_2 && tail_id != CONF_TAIL_3 && tail_id != CONF_TAIL_4 && tail_id != CONF_TAIL_5) {
			short_to_float(short_reshaped_tails[tail_id], (void *)reshaped_tails[tail_id], reshaped_tails_elements[tail_id], FLOAT32_TYPE);
		}
	}

	if (!loc_data) {
		allocate_buffers();
	}

	/* Append and do softmax on the conf data*/
	start_meas();

	int out_ind = 0;
	int group_size = INU_YOLACT_NUMBER_OF_CLASSES + 1;
#if 0
	softmax(reshaped_tails[CONF_TAIL_1], &conf_data[out_ind], reshaped_tails_elements[CONF_TAIL_1], group_size);
	out_ind = out_ind + reshaped_tails_elements[CONF_TAIL_1];
	softmax(reshaped_tails[CONF_TAIL_2], &conf_data[out_ind], reshaped_tails_elements[CONF_TAIL_2], group_size);
	out_ind = out_ind + reshaped_tails_elements[CONF_TAIL_2];
	softmax(reshaped_tails[CONF_TAIL_3], &conf_data[out_ind], reshaped_tails_elements[CONF_TAIL_3], group_size);
	out_ind = out_ind + reshaped_tails_elements[CONF_TAIL_3];
	softmax(reshaped_tails[CONF_TAIL_4], &conf_data[out_ind], reshaped_tails_elements[CONF_TAIL_4], group_size);
	out_ind = out_ind + reshaped_tails_elements[CONF_TAIL_4];
	softmax(reshaped_tails[CONF_TAIL_5], &conf_data[out_ind], reshaped_tails_elements[CONF_TAIL_5], group_size);
	out_ind = out_ind + reshaped_tails_elements[CONF_TAIL_5];

	end_meas(11);
#else

	out_ind = 0;
	int total_conf_elements = reshaped_tails_elements[CONF_TAIL_1] + reshaped_tails_elements[CONF_TAIL_2] + reshaped_tails_elements[CONF_TAIL_3] + reshaped_tails_elements[CONF_TAIL_4] + reshaped_tails_elements[CONF_TAIL_5];
	evDataCacheFlushInvalidateLines(short_conf_data, short_conf_data + total_conf_elements);

	inu_softmax(short_reshaped_tails[CONF_TAIL_1], &short_conf_data[out_ind], reshaped_tails_elements[CONF_TAIL_1] / group_size, group_size, true, split_th);
	out_ind = out_ind + reshaped_tails_elements[CONF_TAIL_1];

	inu_softmax(short_reshaped_tails[CONF_TAIL_2], &short_conf_data[out_ind], reshaped_tails_elements[CONF_TAIL_2] / group_size, group_size, true, split_th);
	out_ind = out_ind + reshaped_tails_elements[CONF_TAIL_2];

	inu_softmax(short_reshaped_tails[CONF_TAIL_3], &short_conf_data[out_ind], reshaped_tails_elements[CONF_TAIL_3] / group_size, group_size, true, split_th);
	out_ind = out_ind + reshaped_tails_elements[CONF_TAIL_3];

	inu_softmax(short_reshaped_tails[CONF_TAIL_4], &short_conf_data[out_ind], reshaped_tails_elements[CONF_TAIL_4] / group_size, group_size, true, split_th);
	out_ind = out_ind + reshaped_tails_elements[CONF_TAIL_4];

	inu_softmax(short_reshaped_tails[CONF_TAIL_5], &short_conf_data[out_ind], reshaped_tails_elements[CONF_TAIL_5] / group_size, group_size, true, split_th);
	out_ind = out_ind + reshaped_tails_elements[CONF_TAIL_5];
#endif
	end_meas("softmax");

#if 0
	compare_short_to_float(short_conf_data, conf_data, out_ind, 0, "softmax");
#endif


	/* Convert from short to float */
	evDataCacheFlushInvalidateLines(conf_data_16, conf_data_16 + out_ind);
	evDataCacheFlushInvalidateLines(conf_data_32, conf_data_32 + out_ind);

	start_meas();
	inu_convert2float(short_conf_data, (void*)conf_data_32, out_ind, float_factor, FLOAT32_TYPE, true, 1.0);
	end_meas("convert32");

	start_meas();
	inu_convert2float(short_conf_data, (void*)conf_data_16, out_ind, float_factor, FLOAT16_TYPE, true, 1.0);
	end_meas("convert16");

	start_meas();
	/* Append the box data */
	if (!loc_data) {
		loc_data_elements = reshaped_tails_elements[BBOX_TAIL_1] + reshaped_tails_elements[BBOX_TAIL_2] + reshaped_tails_elements[BBOX_TAIL_3] + reshaped_tails_elements[BBOX_TAIL_4] + reshaped_tails_elements[BBOX_TAIL_5];
		//LOGG_PRINT(LOG_INFO_E, (ERRG_codeE)NULL, "Number of conf elements for loc: %d\n", loc_data_elements);
		loc_data = (float *)yolact_malloc(sizeof(float)*loc_data_elements);
		if (!loc_data) {
			LOGG_PRINT(LOG_ERROR_E, (ERRG_codeE)NULL, "Failed to allocate meory for loc_data\n");
			return;
		}
	}


	cpy_ind = 0;

	evDataCacheFlushInvalidateLines(loc_data, loc_data + loc_data_elements);
	copy_kernel((unsigned char*)reshaped_tails[BBOX_TAIL_1], (unsigned char*)&loc_data[cpy_ind], reshaped_tails_elements[BBOX_TAIL_1] * sizeof(float));
	cpy_ind += reshaped_tails_elements[BBOX_TAIL_1];
	copy_kernel((unsigned char*)reshaped_tails[BBOX_TAIL_2], (unsigned char*)&loc_data[cpy_ind], reshaped_tails_elements[BBOX_TAIL_2] * sizeof(float));
	cpy_ind += reshaped_tails_elements[BBOX_TAIL_2];
	copy_kernel((unsigned char*)reshaped_tails[BBOX_TAIL_3], (unsigned char*)&loc_data[cpy_ind], reshaped_tails_elements[BBOX_TAIL_3] * sizeof(float));
	cpy_ind += reshaped_tails_elements[BBOX_TAIL_3];
	copy_kernel((unsigned char*)reshaped_tails[BBOX_TAIL_4], (unsigned char*)&loc_data[cpy_ind], reshaped_tails_elements[BBOX_TAIL_4] * sizeof(float));
	cpy_ind += reshaped_tails_elements[BBOX_TAIL_4];
	copy_kernel((unsigned char*)reshaped_tails[BBOX_TAIL_5], (unsigned char*)&loc_data[cpy_ind], reshaped_tails_elements[BBOX_TAIL_5] * sizeof(float));

	/* Append the mask data */
	cpy_ind = 0;
	evDataCacheFlushInvalidateLines(mask_data, mask_data + mask_data_elements);
	copy_kernel((unsigned char*)reshaped_tails[MASK_TAIL_1], (unsigned char*)&mask_data[cpy_ind], reshaped_tails_elements[MASK_TAIL_1] * sizeof(float));
	cpy_ind += reshaped_tails_elements[MASK_TAIL_1];
	copy_kernel((unsigned char*)reshaped_tails[MASK_TAIL_2], (unsigned char*)&mask_data[cpy_ind], reshaped_tails_elements[MASK_TAIL_2] * sizeof(float));
	cpy_ind += reshaped_tails_elements[MASK_TAIL_2];
	copy_kernel((unsigned char*)reshaped_tails[MASK_TAIL_3], (unsigned char*)&mask_data[cpy_ind], reshaped_tails_elements[MASK_TAIL_3] * sizeof(float));
	cpy_ind += reshaped_tails_elements[MASK_TAIL_3];
	copy_kernel((unsigned char*)reshaped_tails[MASK_TAIL_4], (unsigned char*)&mask_data[cpy_ind], reshaped_tails_elements[MASK_TAIL_4] * sizeof(float));
	cpy_ind += reshaped_tails_elements[MASK_TAIL_4];
	copy_kernel((unsigned char*)reshaped_tails[MASK_TAIL_5], (unsigned char*)&mask_data[cpy_ind], reshaped_tails_elements[MASK_TAIL_5] * sizeof(float));

	end_meas("prep_loc_mask 1+1");

	start_meas();

	/* Prepare the decoded boxes in center-size notation. From x, y, w, h to minx, miny, maxx, maxy */
	float boxes_variances[2] = { 0.1F, 0.2F };
	float tmp_box[4];

	start_meas();

	for (int i = 0; i < all_prior_elements / 4; i++) {
		int index = i * 4;
		tmp_box[0] = all_priors[index] + loc_data[index] * boxes_variances[0] * all_priors[index + 2];
		tmp_box[1] = all_priors[index + 1] + loc_data[index + 1] * boxes_variances[0] * all_priors[index + 3];
		//tmp_box[2] = all_priors[index + 2] * powf(E_CONST, (loc_data[index + 2] * boxes_variances[1]));
		//tmp_box[3] = all_priors[index + 3] * powf(E_CONST, (loc_data[index + 3] * boxes_variances[1]));
		tmp_box[2] = all_priors[index + 2] * inu_exp(loc_data[index + 2] * boxes_variances[1]);
		tmp_box[3] = all_priors[index + 3] * inu_exp(loc_data[index + 3] * boxes_variances[1]);

		decoded_boxes_data[index] = tmp_box[0] - tmp_box[2] / 2;
		decoded_boxes_data[index + 1] = tmp_box[1] - tmp_box[3] / 2;
		decoded_boxes_data[index + 2] = tmp_box[2] + decoded_boxes_data[index];
		decoded_boxes_data[index + 3] = tmp_box[3] + decoded_boxes_data[index + 1];
		//if (i < 4) {
		//	LOGG_PRINT(LOG_INFO_E, (ERRG_codeE)NULL, "tmp_box: %f %f %f %f\n", tmp_box[0], tmp_box[1], tmp_box[2], tmp_box[3]);
		//	LOGG_PRINT(LOG_INFO_E, (ERRG_codeE)NULL, "loc_data: %f %f %f %f\n", loc_data[index], loc_data[index + 1], loc_data[index + 2], loc_data[index + 3]);
		//	LOGG_PRINT(LOG_INFO_E, (ERRG_codeE)NULL, "priors: %f %f %f %f\n", all_priors[index], all_priors[index+1], all_priors[index+2], all_priors[index+3]);
		//	LOGG_PRINT(LOG_INFO_E, (ERRG_codeE)NULL, " box %d: %f %f %f %f\n", i, decoded_boxes_data[index], decoded_boxes_data[index + 1], decoded_boxes_data[index + 2], decoded_boxes_data[index + 3]);
		//}
	}

	end_meas("decoded boxes");

	/* Detect  - Keep conf_scores. shape of [classes, x] x->Conf above threshold */
	start_meas();

	static int conf_pred_indexes[INU_YOLACT_NUMBER_OF_CLASSES + 1];
	static float vals[INU_YOLACT_NUMBER_OF_CLASSES + 1];
	conf_score_elements = 0;
	max_num_scores_elements = all_prior_elements / 4;

#if 1
	evDataCacheFlushInvalidateLines(max_values_array, max_values_array + max_values_array_elements);
	EV_matrix_rows_max_values(conf_data_16, max_num_scores_elements, yolact_num_of_classes + 1, max_values_array, true, 1.0);

	for (int i = 0; i < max_num_scores_elements; i++) {
		int box_index, box_index_kept, mask_index, mask_index_kept;

		//if (i == 0) {
		//	LOGG_PRINT(LOG_INFO_E, (ERRG_codeE)NULL, "%f %f %f %f %f %f %f %f %f %f\n",
		//		conf_data_16[0], conf_data_16[1], conf_data_16[2], conf_data_16[3], conf_data_16[4],
		//		conf_data_16[5], conf_data_16[6], conf_data_16[7], conf_data_16[8], conf_data_16[9]);
		//	LOGG_PRINT(LOG_INFO_E, (ERRG_codeE)NULL, "max: %f\n", max_values_array[0]);
		//}

		if (max_values_array[i] > INU_YOLACT_CONF_THRESHOLD) {
			int ind_base = i * (yolact_num_of_classes + 1);
			__fp16* conf_data_p = &conf_data_16[ind_base + 1];
			for (int j = 0; j < yolact_num_of_classes; j++) {
				conf_scores[j][conf_score_elements] = conf_data_p[j];
			}

			box_index = i * 4;
			box_index_kept = conf_score_elements * 4;
			memcpy(&decoded_boxes_kept[box_index_kept], &decoded_boxes_data[box_index], sizeof(float) * 4);
			mask_index = i * 32;
			mask_index_kept = conf_score_elements * 32;
			memcpy(&mask_data_kept[mask_index_kept], &mask_data[mask_index], sizeof(float) * 32);

			//if (conf_score_elements < 5) {
			//	LOGG_PRINT(LOG_INFO_E, (ERRG_codeE)NULL, "conf_scores: %f  %f\n", conf_scores[0][conf_score_elements], conf_scores[1][conf_score_elements]);
			//	LOGG_PRINT(LOG_INFO_E, (ERRG_codeE)NULL, "box_kept: %f %f %f %f\n", decoded_boxes_kept[box_index_kept], decoded_boxes_kept[box_index_kept + 1], decoded_boxes_kept[box_index_kept + 2], decoded_boxes_kept[box_index_kept + 3]);
			//	LOGG_PRINT(LOG_INFO_E, (ERRG_codeE)NULL, "mask_kept: %f %f ... %f %f\n", mask_data_kept[mask_index_kept], mask_data_kept[mask_index_kept + 1], mask_data_kept[mask_index_kept + 30], mask_data_kept[mask_index_kept + 31]);
			//}
			conf_score_elements++;
		}
	}
#else
	for (int i = 0; i < max_num_scores_elements; i++) {
		int box_index, box_index_kept, mask_index, mask_index_kept;
		bool conf_above_threshold = false;
		int ind_base = i * (yolact_num_of_classes + 1);
		float* conf_data_p = &conf_data_32[ind_base + 1];
		float* vals_p = vals;
		for (int j = 1; j < yolact_num_of_classes + 1; j++) { // skip the first one. It is background
			*vals_p = *conf_data_p;
			if (*vals_p > INU_YOLACT_CONF_THRESHOLD) {
				conf_above_threshold = true;
				break;
				//LOGG_PRINT(LOG_INFO_E, (ERRG_codeE)NULL, "*** conf_above_threshold TRUE ***\n");
			}
			conf_data_p++;
			vals_p++;
		}

		if (conf_above_threshold) {
			for (int j = 0; j < yolact_num_of_classes; j++) {
				conf_scores[j][conf_score_elements] = vals[j];
			}

			box_index = i * 4;
			box_index_kept = conf_score_elements * 4;
			memcpy(&decoded_boxes_kept[box_index_kept], &decoded_boxes_data[box_index], sizeof(float) * 4);
			mask_index = i * 32;
			mask_index_kept = conf_score_elements * 32;
			memcpy(&mask_data_kept[mask_index_kept], &mask_data[mask_index], sizeof(float) * 32);

			//if (conf_score_elements < 5) {
			//	LOGG_PRINT(LOG_INFO_E, (ERRG_codeE)NULL, "conf_scores: %f  %f\n", conf_scores[0][conf_score_elements], conf_scores[1][conf_score_elements]);
			//	LOGG_PRINT(LOG_INFO_E, (ERRG_codeE)NULL, "box_kept: %f %f %f %f\n", decoded_boxes_kept[box_index_kept], decoded_boxes_kept[box_index_kept + 1], decoded_boxes_kept[box_index_kept + 2], decoded_boxes_kept[box_index_kept + 3]);
			//	LOGG_PRINT(LOG_INFO_E, (ERRG_codeE)NULL, "mask_kept: %f %f ... %f %f\n", mask_data_kept[mask_index_kept], mask_data_kept[mask_index_kept + 1], mask_data_kept[mask_index_kept + 30], mask_data_kept[mask_index_kept + 31]);
			//}
			conf_score_elements++;
		}
	}
#endif
	LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL, "conf_score_elements: %d\n", conf_score_elements);

	end_meas("above_thresh");

	/* Do fast_nms */

	start_meas();

	if (conf_score_elements > 0) {
		/* Get the sorted indexes in descending order for conf_scores 1 & 2*/
		for (int j = 0; j < yolact_num_of_classes; j++) {
			get_sorted_indexes_descending(conf_scores[j], scores_sorted_indexes[j], conf_score_elements);
		}

		/* Keep only up to INU_YOLACT_TOP_K boxes and masks */
		dets_number = conf_score_elements;
		if (dets_number > INU_YOLACT_TOP_K) {
			dets_number = INU_YOLACT_TOP_K;
		}
		for (int i = 0; i < dets_number; i++) {
			for (int j = 0; j < yolact_num_of_classes; j++) {
				memcpy(&boxes_sorted[j][i * 4], &decoded_boxes_kept[scores_sorted_indexes[j][i] * 4], sizeof(float) * 4);
				memcpy(&mask_sorted[j][i * 32], &mask_data_kept[scores_sorted_indexes[j][i] * 32], sizeof(float) * 32);
			}
		}

#if 0
		for (int box_ind = 0; box_ind < 2; box_ind++) {
			LOGG_PRINT(LOG_INFO_E, (ERRG_codeE)NULL, "sorted boxes %d\n", box_ind);
			for (int i = 0; i < dets_number; i++) {
				LOGG_PRINT(LOG_INFO_E, (ERRG_codeE)NULL, "%f %f %f %f\n", boxes_sorted[box_ind][i * 4], boxes_sorted[box_ind][i * 4 + 1], boxes_sorted[box_ind][i * 4 + 2], boxes_sorted[box_ind][i * 4 + 3]);
			}
		}
#endif
		/* Calculate the intersection */
		yolact_box b[2];

		for (int box_ind = 0; box_ind < yolact_num_of_classes; box_ind++) {
			for (int i = 0; i < dets_number; i++) {
				b[0] = prep_box(&boxes_sorted[box_ind][i * 4]);
				//LOGG_PRINT(LOG_INFO_E, (ERRG_codeE)NULL, "\n");
				for (int j = 0; j < dets_number; j++) {
					b[1] = prep_box(&boxes_sorted[box_ind][j * 4]);
					/* Put zeros in lower triangle of the matrix of size [dets_number (i)][dets_number (j)]*/
					if (j > i) { //amnon
						iou[box_ind][i][j] = box_iou(b[0], b[1]);
					}
					else {
						iou[box_ind][i][j] = 0;
					}
					//LOGG_PRINT(LOG_INFO_E, (ERRG_codeE)NULL, "box_ind:%d i:%d j:%d intersect:%f\n", box_ind, i, j, iou[box_ind][i][j]);
				}
			}
		}

		/* Calculate the iou_max. It creates matrix of [classes][dets_number] */
		for (int box_ind = 0; box_ind < yolact_num_of_classes; box_ind++) {
			for (int i = 0; i < dets_number; i++) {
				float max_val = -999999;
				for (int j = 0; j < dets_number; j++) {
					float val = iou[box_ind][j][i];
					if (val > max_val) {
						max_val = val;
					}
				}
				iou_max[box_ind][i] = max_val;
				//LOGG_PRINT(LOG_INFO_E, (ERRG_codeE)NULL, "iou_max[%1d][%2d]: %f\n", box_ind, i, iou_max[box_ind][i]);
			}
		}

		/* Keep the boxes & masks accoring to the YOLACT_NMS_THRESH (reusing the xxx_kept arrays) */
		kept_elements_after_iou = 0;
		for (int box_ind = 0; box_ind < yolact_num_of_classes; box_ind++) {
			for (int i = 0; i < dets_number; i++) {
				if (iou_max[box_ind][i] <= inu_yolact_nms_threshold) {
					int score_index;
					memcpy(&decoded_boxes_kept[kept_elements_after_iou * 4], &boxes_sorted[box_ind][i * 4], sizeof(float) * 4);
					memcpy(&mask_data_kept[kept_elements_after_iou * 32], &mask_sorted[box_ind][i * 32], sizeof(float) * 32);
					score_index = scores_sorted_indexes[box_ind][i];
					scores_kept[kept_elements_after_iou] = conf_scores[box_ind][score_index];
					classes_kept[kept_elements_after_iou] = box_ind;
#if 0
					LOGG_PRINT(LOG_INFO_E, (ERRG_codeE)NULL, "kept box %d: %f %f %f %f\n", kept_elements_after_iou, decoded_boxes_kept[kept_elements_after_iou * 4], decoded_boxes_kept[kept_elements_after_iou * 4 + 1], decoded_boxes_kept[kept_elements_after_iou * 4 + 2], decoded_boxes_kept[kept_elements_after_iou * 4 + 3]);
					LOGG_PRINT(LOG_INFO_E, (ERRG_codeE)NULL, "kept mask %d: %f %f %f %f\n", kept_elements_after_iou, mask_data_kept[kept_elements_after_iou * 32], mask_data_kept[kept_elements_after_iou * 32 + 1], mask_data_kept[kept_elements_after_iou * 32 + 2], mask_data_kept[kept_elements_after_iou * 32 + 3]);
					LOGG_PRINT(LOG_INFO_E, (ERRG_codeE)NULL, "score %d: %f\n", kept_elements_after_iou, scores_kept[kept_elements_after_iou]);
#endif
					kept_elements_after_iou++;
				}
			}
		}

		get_sorted_indexes_descending(scores_kept, sorted_scores_indexes, kept_elements_after_iou);

		LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL, "kept_elements_after_iou: %d inu_yolact_score_threshold:%f\n", kept_elements_after_iou, inu_yolact_score_threshold);

#if 0
		for (int i = 0; i < kept_elements_after_iou; i++) {
			int index = sorted_scores_indexes[i];
			LOGG_PRINT(LOG_INFO_E, (ERRG_codeE)NULL, "%d) class %d\n", i, classes_kept[index]);
			LOGG_PRINT(LOG_INFO_E, (ERRG_codeE)NULL, "%d) score: %f\n", i, scores_kept[index]);
			LOGG_PRINT(LOG_INFO_E, (ERRG_codeE)NULL, "%d) Box: %f %f %f %f\n", i, decoded_boxes_kept[index * 4], decoded_boxes_kept[index * 4 + 1], decoded_boxes_kept[index * 4 + 2], decoded_boxes_kept[index * 4 + 3]);
			LOGG_PRINT(LOG_INFO_E, (ERRG_codeE)NULL, "%d) kept mask %d: %f %f %f %f\n", i, kept_elements_after_iou, mask_data_kept[index * 32], mask_data_kept[index * 32 + 1], mask_data_kept[index * 32 + 2], mask_data_kept[index * 32 + 3]);
		}
#endif

		/* Keep only values that thier score is above threshold */
		static yolact_box b_prepared[INU_YOLACT_MAX_DETECTIONS];
		elements_above_thresh = 0;
		for (int i = 0; i < kept_elements_after_iou; i++) {
			int index = sorted_scores_indexes[i];

			if (scores_kept[index] > inu_yolact_score_threshold && elements_above_thresh < INU_YOLACT_MAX_DETECTIONS) {
				scores_above_thresh[elements_above_thresh] = scores_kept[index];
				classes_above_thres[elements_above_thresh] = classes_kept[index];
				memcpy(&mask_above_thresh[elements_above_thresh], &mask_data_kept[index * 32], 32 * sizeof(float));
				memcpy(&boxes_above_thresh[elements_above_thresh], &decoded_boxes_kept[index * 4], 4 * sizeof(float));
#if 0
				LOGG_PRINT(LOG_INFO_E, (ERRG_codeE)NULL, "%d) class above thresh %d\n", elements_above_thresh, classes_above_thres[elements_above_thresh]);
				LOGG_PRINT(LOG_INFO_E, (ERRG_codeE)NULL, "%d) score above thresh: %f\n", elements_above_thresh, scores_above_thresh[elements_above_thresh]);
				LOGG_PRINT(LOG_INFO_E, (ERRG_codeE)NULL, "%d) Box above thresh: %f %f %f %f\n", elements_above_thresh, boxes_above_thresh[elements_above_thresh][0], boxes_above_thresh[elements_above_thresh][1], boxes_above_thresh[elements_above_thresh][2], boxes_above_thresh[elements_above_thresh][3]);
				LOGG_PRINT(LOG_INFO_E, (ERRG_codeE)NULL, "%d) mask above thresh: %f %f %f %f\n", elements_above_thresh, mask_above_thresh[elements_above_thresh][0], mask_above_thresh[elements_above_thresh][1], mask_above_thresh[elements_above_thresh][2], mask_above_thresh[elements_above_thresh][3]);
#endif
#if 1
				/* Remove overlapping boxes regardless the class */
				b_prepared[elements_above_thresh] = prep_box(boxes_above_thresh[elements_above_thresh]);
				bool overlap = false;
				float max_iou = 0;
				for (int j = 0; j < elements_above_thresh; j++) {
					float iou = box_iou(b_prepared[elements_above_thresh], b_prepared[j]);
					if (iou > max_iou) max_iou = iou;
					if (iou > YOLACT_NMS_ALL_CLASSES_THRESH) {
						overlap = true;
						break;
					}
				}
				if (!overlap) {
					//LOGG_PRINT(LOG_INFO_E, (ERRG_codeE)NULL, "class: %d max_iou:%f\n", classes_above_thres[elements_above_thresh], max_iou);
					elements_above_thresh++;
				}
				else {
					//LOGG_PRINT(LOG_INFO_E, (ERRG_codeE)NULL, "overlap class: %d max_iou:%f\n", classes_above_thres[elements_above_thresh], max_iou);
				}
#else
				elements_above_thresh++;
#endif
			}
		}
		end_meas("nms");

		LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL, "elements_above_thresh: %d\n", elements_above_thresh);

		start_meas();

		/* Multiply: proto_data (92, 92, 32) * mask_above_thresh.transposed (elements_above_thresh, 32) -> mask_proto_result[92][92][elements_above_thresh]*/
		float* proto_data = reshaped_tails[PROTO_TAIL];
#if 1
		{
			Ev_multiply_mask(sorted_tails_info[PROTO_TAIL].width, sorted_tails_info[PROTO_TAIL].Height, sorted_tails_info[PROTO_TAIL].Inputs, elements_above_thresh, proto_data, &mask_above_thresh[0][0], mask_proto_result, true, 1.0);
		}
#else
		int proto_index;
		int width_size = sorted_tails_info[PROTO_TAIL].width *sorted_tails_info[PROTO_TAIL].Inputs;
		for (int i1 = 0; i1 < sorted_tails_info[PROTO_TAIL].Height; i1++) {
			int i1_offset = i1*sorted_tails_info[PROTO_TAIL].width*elements_above_thresh;
			for (int i2 = 0; i2 < sorted_tails_info[PROTO_TAIL].width; i2++) {
				int i2_offset = i1_offset + i2*elements_above_thresh;
				//proto_index = i1*sorted_tails_info[PROTO_TAIL].width *sorted_tails_info[PROTO_TAIL].Inputs + i2 * sorted_tails_info[PROTO_TAIL].Inputs;
				proto_index = i1*width_size + i2 * sorted_tails_info[PROTO_TAIL].Inputs;
				for (int j = 0; j < elements_above_thresh; j++) {
					float sum = 0;
					for (int m = 0; m < sorted_tails_info[PROTO_TAIL].Inputs; m++) {
						sum += proto_data[proto_index + m] * mask_above_thresh[j][m];
					}
					//int index = i1*sorted_tails_info[0].width * elements_above_thresh + i2*elements_above_thresh + j;
					int index = i2_offset + j;
					mask_proto_result[index] = 1 / (1 + inu_exp(-sum));
				}
			}
		}
#endif
		end_meas("proto*mask");

		start_meas();

		/* crop the mask_proto_result, leave values only that are within the boxes */
		//memset(masks_crop_matrix, 0, masks_crop_matrix_size);
		evDataCacheFlushInvalidateLines(masks_crop_matrix, masks_crop_matrix + masks_crop_matrix_size/sizeof(int));
		copy_kernel(zeros_buffer, (unsigned char*)masks_crop_matrix, masks_crop_matrix_size);
		for (int box_ind = 0; box_ind < elements_above_thresh; box_ind++) {
			int xmin, xmax, ymin, ymax;
			xmin = (int)(boxes_above_thresh[box_ind][0] * sorted_tails_info[PROTO_TAIL].width);
			if (xmin < 0) xmin = 0;
			xmax = (int)(boxes_above_thresh[box_ind][2] * sorted_tails_info[PROTO_TAIL].width);
			if (xmax > sorted_tails_info[PROTO_TAIL].width - 1) xmax = sorted_tails_info[PROTO_TAIL].width - 1;
			ymin = (int)(boxes_above_thresh[box_ind][1] * sorted_tails_info[PROTO_TAIL].width);
			if (ymin < 0) ymin = 0;
			ymax = (int)(boxes_above_thresh[box_ind][3] * sorted_tails_info[PROTO_TAIL].width);
			if (ymax > sorted_tails_info[PROTO_TAIL].width - 1) ymax = sorted_tails_info[PROTO_TAIL].width - 1;

			for (int x = xmin; x <= xmax; x++) {
				for (int y = ymin; y <= ymax; y++) {
					//int index = i1*sorted_tails_info[PROTO_TAIL].width * elements_above_thresh + i2*elements_above_thresh + j;
					int index = y*sorted_tails_info[PROTO_TAIL].width * elements_above_thresh + x * elements_above_thresh + box_ind;
					masks_crop_matrix[index] = 1;
					//masks_crop_matrix[y][x][box_ind] = 1;
				}
			}
		}

		for (int i1 = 0; i1 < sorted_tails_info[PROTO_TAIL].Height; i1++) {
			for (int i2 = 0; i2 < sorted_tails_info[PROTO_TAIL].width; i2++) {
				for (int elem_ind = 0; elem_ind < elements_above_thresh; elem_ind++) {
					int index = i1*sorted_tails_info[PROTO_TAIL].Height * elements_above_thresh + i2 * elements_above_thresh + elem_ind;
					int index_cropped_contig = elem_ind * sorted_tails_info[PROTO_TAIL].Height*sorted_tails_info[PROTO_TAIL].width + i1 * sorted_tails_info[PROTO_TAIL].width + i2;
					if (masks_crop_matrix[index] == 0) {
						mask_cropped_contig[index_cropped_contig] = 0; // [elem_ind][i1][i2] = 0;
					}
					else {
						mask_cropped_contig[index_cropped_contig] = (RESIZE_INPUT_TYPE)mask_proto_result[index]; // [elem_ind][i1][i2] = mask_proto_result[index];
					}
				}
			}
		}

		end_meas("mask_crop");

		/* Resize */
		start_meas();
		LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL, "resize %d images\n", elements_above_thresh);
		for (int elem_ind = 0; elem_ind < elements_above_thresh; elem_ind++) {
			int in_image_index = elem_ind * sorted_tails_info[PROTO_TAIL].Height * sorted_tails_info[PROTO_TAIL].width;
			resize_class_to_image_size(sorted_tails_info[PROTO_TAIL].width, sorted_tails_info[PROTO_TAIL].Height, &mask_cropped_contig[in_image_index],
				yolact_image_cols, yolact_image_rows, segments_mats[elem_ind], 0.5, (unsigned char)(classes_above_thres[elem_ind]));
		}
		end_meas("resize");

		/* Combine */
		//LOGG_PRINT(LOG_INFO_E, (ERRG_codeE)NULL, "BEFORE COMBINE\n");
		combine_results(elements_above_thresh, yolact_image_cols, yolact_image_rows, segments_mats, result_segments_mat);

		/* Prepare the boxes */
		for (int i = 0; i < elements_above_thresh; i++) {
			yolact_boxes[i].x_min = boxes_above_thresh[i][0];
			yolact_boxes[i].x_max = boxes_above_thresh[i][2];
			yolact_boxes[i].y_min = boxes_above_thresh[i][1];
			yolact_boxes[i].y_max = boxes_above_thresh[i][3];
			yolact_boxes[i].confidence = scores_above_thresh[i];
			yolact_boxes[i].label = classes_above_thres[i];
			yolact_boxes[i].debug = 0;
		}
	} // 	if (conf_score_elements > 0)
	else{
		//memset(result_segments_mat, 0, yolact_image_cols* yolact_image_rows);
		elements_above_thresh = 0;
		LOGG_PRINT(LOG_INFO_E, (ERRG_codeE)NULL, "clear: cols:%d rows:%d\n", yolact_image_cols, yolact_image_rows);
		evDataCacheFlushInvalidateLines(result_segments_mat, result_segments_mat + yolact_image_cols * yolact_image_rows);
		copy_kernel(zeros_buffer, result_segments_mat, yolact_image_cols* yolact_image_rows);
	}
}
