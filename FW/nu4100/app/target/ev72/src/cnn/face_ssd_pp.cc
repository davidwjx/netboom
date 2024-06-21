/****************************************************************************
*
*   FileName: face_ssd_pp.cc
*
*   Author:
*
*   Date:
*
*   Description:
*
****************************************************************************/
#include "client_api.h"
#include "face_ssd_pp.h"
#include <cnn_dev.h>

#ifndef INU_HAPS
#include "inu2_internal.h"
#include "log.h"
#include "sched.h"
#include "ictl_drv.h"
#endif

CLIENT_APIG_msgQueT  face_ssd_msgQue;

static int                  num_of_objects = 0;
static EvCnnBoundingBox     object_boxes[SSD_OBJECT_MAX_NUMBER];

box_struct boxes[MAX_TAIL_ELEMENTS];
int number_of_boxes = 0;

static int num_boxes_above_thresh = 0;
static float *softmax_confs = NULL;
static int softmax_confs_alloc_size = 0;
static int ssd_priors_index = -1;
static prior_struct *priors[SSD_NUM_OF_IMAGE_SIZES];

#if SSD_NUM_OF_IMAGE_SIZES == 3
static int image_size[SSD_NUM_OF_IMAGE_SIZES][2] = { { 160, 120 },{ 320, 240 },{ 640, 480 } };
static int feature_map_w_h_list[SSD_NUM_OF_IMAGE_SIZES][2][4] = {
	{ { 20, 10, 5, 3 },{ 15, 8, 4, 2 } },
	{ { 40, 20, 10, 5 },{ 30, 15, 8, 4 } },
	{ { 80, 40, 20, 10 },{ 60, 30, 15, 8 } } };
static int prior_size[SSD_NUM_OF_IMAGE_SIZES] = { 1118, 4420, 17640 };
#else
static int image_size[SSD_NUM_OF_IMAGE_SIZES][2] = { { 128, 96 } ,{ 160, 120 },{ 320, 240 },{ 480, 360 },{ 640, 480 },{ 1280, 960 } };
static int feature_map_w_h_list[SSD_NUM_OF_IMAGE_SIZES][2][4] = {
	{ { 16, 8, 4, 2 },{ 12, 6, 3, 2 } },
	{ { 20, 10, 5, 3 },{ 15, 8, 4, 2 } },
	{ { 40, 20, 10, 5 },{ 30, 15, 8, 4 } },
	{ { 60, 30, 15, 8 },{ 45, 23, 12, 6 } },
	{ { 80, 40, 20, 10 },{ 60, 30, 15, 8 } },
	{ { 160, 80, 40, 20 },{ 120, 60, 30, 15 } } };
static int prior_size[SSD_NUM_OF_IMAGE_SIZES] = { 708, 1118, 4420, 9984, 17640, 70500 };
#endif


static float min_boxes[4][3] = { { 10, 16, 24 }, { 32, 48, -1}, { 64, 96, -1}, { 128, 192, 256 } };

/* face_ssd_pp_init */
void face_ssd_pp_init(int coreId)
{
	int rc;
	EvThreadType t;

	face_ssd_msgQue.maxMsgs = CLIENT_APIG_QUE_MAX_ELEMENTS;
	face_ssd_msgQue.msgSize = sizeof(face_ssd_pp_infoT);

	if (CLIENT_APIG_createMsgQue(&face_ssd_msgQue, 0) != SUCCESS_E)
	{
		LOGG_PRINT(LOG_ERROR_E, (ERRG_codeE)NULL, "face ssd msg queue create failed\n");
		return;
	}

	ssd_post_process_prepare_priors(160);
	ssd_post_process_prepare_priors(320);
	ssd_post_process_prepare_priors(640);
	exp_lut_init();

	t = evNewThread(ev_inu_face_ssd_post_thread, EVTH_INIT_CPU, coreId, EVTH_INIT_ARGPTR, NULL, EVTH_INIT_PRIORITY, FACE_SSD_PP_THREAD_PRIORITY, EVTH_INIT_PREEMPTIBLE, 1, EVTH_INIT_LAST);
	if (t == NULL)
	{
		LOGG_PRINT(LOG_ERROR_E, (ERRG_codeE)NULL, "Failed to open ev_inu_face_ssd_post_thread thread\n");
		return;
	}
}

/* ev_inu_face_ssd_post_thread */
void *ev_inu_face_ssd_post_thread(void *arg)
{
#ifndef INU_HAPS
	inu_cdnn_data__hdr_t        *resultsHeaderP;
	inu_cdnn_data__tailHeaderT  *tailHeaderP;
	face_ssd_pp_infoT			face_ssd_post_params;
	UINT32						localThreadParamsSize;
	int							status;

	LOGG_PRINT(LOG_INFO_E, (ERRG_codeE)NULL, "face ssd post process thread started on core %d\n", evGetCpu());

	while (1)
	{
		localThreadParamsSize = sizeof(face_ssd_post_params);
		status = CLIENT_APIG_recvMsg(&face_ssd_msgQue, (UINT8*)&face_ssd_post_params, &localThreadParamsSize, 0 /*not used*/);
		if (status == SUCCESS_E) {
			//			LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL, "ev_inu_face_ssd_post_thread started on core %d frame:%d\n", evGetCpu(), face_ssd_post_params.frame_id);

#ifdef FACE_SSD_PP_PROCESS_TIME
		uint64_t face_ssd_clock1, face_ssd_clock2;
		face_ssd_clock1 = getRTC();
#endif

		ssd_post_process(face_ssd_post_params.locations, face_ssd_post_params.locations_scale, face_ssd_post_params.confs, face_ssd_post_params.confs_scale, face_ssd_post_params.num_of_elements, face_ssd_post_params.num_of_classes);

		num_of_objects = 0;
		for (int i = 0; i < number_of_boxes && num_of_objects < SSD_OBJECT_MAX_NUMBER; i++) {
			object_boxes[num_of_objects].x_min = boxes[i].min_x;
			object_boxes[num_of_objects].x_max = boxes[i].max_x;
			object_boxes[num_of_objects].y_min = boxes[i].min_y;
			object_boxes[num_of_objects].y_max = boxes[i].max_y;
			object_boxes[num_of_objects].confidence = boxes[i].confidence;
			object_boxes[num_of_objects].label = boxes[i].class_id;
			object_boxes[num_of_objects].debug = 0;

			LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL, "object_boxes %d: %f %f %f %f  conf:%f\n", i, object_boxes[i].x_min, object_boxes[i].y_min, object_boxes[i].x_max, object_boxes[i].y_min, object_boxes[i].confidence);

			num_of_objects++;
		}

#ifdef FACE_SSD_PP_PROCESS_TIME
		face_ssd_clock2 = getRTC();
		float ev_clock_rate = 807000;
		LOGG_PRINT(LOG_INFO_E, (ERRG_codeE)NULL, "face_ssd post time (%d) is cycles:%lld %fms frame:%d\n", (int)ev_clock_rate, (face_ssd_clock2 - face_ssd_clock1), (float)(face_ssd_clock2 - face_ssd_clock1) / ev_clock_rate, face_ssd_post_params.frame_id);
#endif

		/* Fill in the result header and result buffer */
		resultsHeaderP = (inu_cdnn_data__hdr_t *)face_ssd_post_params.blob_output_header;
		tailHeaderP = resultsHeaderP->tailHeader;
		resultsHeaderP->network_id = face_ssd_post_params.net_id;
		resultsHeaderP->engineType = INU_LOAD_NETWORK__EV61_E;
		resultsHeaderP->frameId = face_ssd_post_params.frame_id;
		resultsHeaderP->numOfTails = 1;
		resultsHeaderP->tailHeader[0].elementSize = sizeof(EvCnnBoundingBoxes);
		resultsHeaderP->tailHeader[0].elementCount = num_of_objects;
		resultsHeaderP->tailHeader[0].inputs = num_of_objects;
		resultsHeaderP->tailHeader[0].width = 0;
		resultsHeaderP->tailHeader[0].height = 0;
		resultsHeaderP->tailHeader[0].cnnFields.tailSynopsysFields.scale = 1;
		resultsHeaderP->offsetToBlob[0] = 0;

		int *out_p;
		out_p = (int *)face_ssd_post_params.output_ptr;
		out_p[0] = num_of_objects;
		if (num_of_objects)
		{
			memcpy(&out_p[1], object_boxes, num_of_objects * sizeof(EvCnnBoundingBox));
		}
		resultsHeaderP->totOutputSize = sizeof(int) + num_of_objects * sizeof(EvCnnBoundingBox);

			evDataCacheFlushLines(resultsHeaderP, resultsHeaderP + sizeof(inu_cdnn_data__hdr_t));
			evDataCacheFlushLines(face_ssd_post_params.output_ptr, (unsigned char *)(face_ssd_post_params.output_ptr) + resultsHeaderP->totOutputSize);
			LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL, "tails num %d total output size %d cache line %d\n", resultsHeaderP->numOfTails, resultsHeaderP->totOutputSize, evDataCacheLineSize());
			SCHEDG_pushFinishedJob(face_ssd_post_params.jobDescriptor);
			evIntCondSignal(&SCHEDG_Condition);
#endif
		}
		//			LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL, "ev_inu_face_ssd_post_thread ended on core %d\n", evGetCpu());
	}
	return(NULL);
}

/* face_ssd_set_input_size */
void face_ssd_set_input_size(int face_ssd_width)
{
	/* Set index to prior tables */
	for (int i = 0; i < SSD_NUM_OF_IMAGE_SIZES; i++) {
		if (image_size[i][0] == face_ssd_width) {
			ssd_priors_index = i;
			break;
		}
	}

	LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL, "face_ssd_set_input_size: ssd_priors_index:%d  image_size: %d\n", ssd_priors_index, face_ssd_width);
}

/* ssd_post_process_prepare_priors */
void ssd_post_process_prepare_priors(int face_ssd_width)
{
	int index=-1;
	float shrinkage_list[2][4];
	float scale_w, scale_h, x_center, y_center; int prior_index = 0;

	/* Determine index to tables */
	for (int i = 0; i < SSD_NUM_OF_IMAGE_SIZES; i++) {
		if (image_size[i][0] == face_ssd_width) {
			index = i;
			break; 
		}
	}
	LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL, "ssd_post_process_prepare_priors: index:%d  image_size: %d  %d\n", index, image_size[index][0], image_size[index][1]);

	/* allocate memory for the priors */
	priors[index] = (prior_struct *)malloc(sizeof(prior_struct) * prior_size[index]);

	for (int i = 0; i < 2; i++) {
		LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL, "shrinkage_list:\n");
		for (int j = 0; j < 4; j++) {
			shrinkage_list[i][j] = (float)image_size[index][i] / (float)feature_map_w_h_list[index][i][j];
			LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL, "%f  ", shrinkage_list[i][j]);
		}
		LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL, "\n");
	}

	for (int feature_ind = 0; feature_ind < 4; feature_ind++) {
		//scale_w = image_size[0] / shrinkage_list[0][index]
		//scale_h = image_size[1] / shrinkage_list[1][index]
		scale_w = image_size[index][0] / shrinkage_list[0][feature_ind];
		scale_h = image_size[index][1] / shrinkage_list[1][feature_ind];
		LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL, "scale_w: %f scale_h:%f\n", scale_w, scale_h);

		for (float j = 0; j < feature_map_w_h_list[index][1][feature_ind]; j++) {
			for (float i = 0; i < feature_map_w_h_list[index][0][feature_ind]; i++) {
				x_center = (i + 0.5F) / scale_w;
				y_center = (j + 0.5F) / scale_h;
				LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL, "x_center:%f y_center:%f\n", x_center, y_center);
				LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL, "min_boxes:%f %f %f\n", min_boxes[feature_ind][0], min_boxes[feature_ind][1], min_boxes[feature_ind][2]);

				for (int m = 0; m < 3; m++) {
					float min_box;
					
					min_box = min_boxes[feature_ind][m];
					if (min_box > 0) {
						priors[index][prior_index].x_center = x_center;
						priors[index][prior_index].y_center = y_center;
						priors[index][prior_index].w = fmin(1, min_box / image_size[index][0]);
						priors[index][prior_index].h = fmin(1, min_box / image_size[index][1]);
						LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL, "min_box:%f image_size: %d %d\n", min_box, image_size[index][0], image_size[index][1]);
						LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL, "Prior %d: x:%f y:%f w:%f h:%f\n", prior_index, priors[index][prior_index].x_center, priors[index][prior_index].y_center, priors[index][prior_index].w, priors[index][prior_index].h);
						prior_index++;
					}
				}
			}
		}
	}
}


/* softmax  - softmax(Xi) = exp(Xi) / Sum(exp(Xi))  */
void ssd_softmax(short *in, float scale, float *out, int elements, int group_size)
{
	LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL, "softmax groups:%d out:%p\n", elements, out);

	for (int i = 0; i < elements*group_size; i = i + group_size) {
#if 0
		// Code for case that the graph does not do the softmax
		float sum;
		sum = 0;
		for (int j = 0; j < group_size; j++) {
			//out[i + j] = powf(E_CONST, (float)(in[i + j])/scale);
			out[i + j] = inu_exp((float)(in[i + j])/scale);
			sum += out[i + j];
		}
		for (int j = 0; j < group_size; j++) {
			out[i + j] = out[i + j] / sum;
		}
		LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL, "In:%f %f out:%f %f\n", in[i], in[i + 1], out[i], out[i + 1]);
#else
		for (int j = 0; j < group_size; j++) {
			out[i + j] = (float)(in[i + j]) / scale;
		}
#endif

	}
}

/* nms_comparator */
static int ssd_nms_comparator(const void *pa, const void *pb)
{
	box_struct *a = (box_struct *)pa;
	box_struct *b = (box_struct *)pb;
	float diff = a->confidence - b->confidence;
	if (diff < 0) return 1;
	else if (diff > 0) return -1;
	return 0;
}

/* do_ssd_nms */
static void do_ssd_nms(void)
{
	/* Sort the filtered boxes by thier probs */
	qsort(boxes, num_boxes_above_thresh, sizeof(box_struct), ssd_nms_comparator);

	/* Do the NMS */
	for (int i = 0; i < num_boxes_above_thresh; i++) {
		if (boxes[i].confidence > 0) {
			for (int j = i + 1; j < num_boxes_above_thresh; j++) {
				if (boxes[i].class_id == boxes[j].class_id && box_iou(boxes[i].center_box, boxes[j].center_box) > IOU_THRESH) {
					LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL, "NMS: remove box: %f %f %f %f conf:%f\n", boxes[j].center_box.x, boxes[j].center_box.y, boxes[j].center_box.w, boxes[j].center_box.h, boxes[j].confidence);
					boxes[j].confidence = 0;
				}
				else {
					LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL, "NMS keep: box: %f %f %f %f classj:%d prob:%f IOU:%f\n",
						boxes[j].center_box.x, boxes[j].center_box.y, boxes[j].center_box.w, boxes[j].center_box.h, boxes[j].confidence, box_iou(boxes[i].center_box, boxes[j].center_box));
				}
			}
		}
	}
}

/* ssd_post_process */
void ssd_post_process(short *locs, float locs_scale, short *confs, float conf_scale, int elements, int number_of_classes)
{
	float tmp_boxes[4];

	/* Allocate memory for the softmax */
	if (softmax_confs_alloc_size < sizeof(float)*elements*number_of_classes) {
		if (softmax_confs) free(softmax_confs);
		softmax_confs = (float *)malloc(sizeof(float)*elements*number_of_classes);
		if (!softmax_confs) {
			LOGG_PRINT(LOG_ERROR_E, (ERRG_codeE)NULL, "Failed to allocate softmax_confs elements:%d classes:%d\n", elements, number_of_classes);
		}
	}
	ssd_softmax(confs, conf_scale, softmax_confs, elements, number_of_classes);

	/* locations to boxes */
	num_boxes_above_thresh = 0;
	for (int i = 0; i < elements; i++) {
		int location_ind = i * 4;
		for(int class_id=1; class_id<number_of_classes; class_id++) {
			int softmax_index = i*number_of_classes + class_id;
			if (softmax_confs[softmax_index] >= CONF_THRESH) {
				/* Calculate to center form */
				tmp_boxes[0] = (float)locs[location_ind]/ locs_scale * CENTER_VARIANCE * priors[ssd_priors_index][i].w + priors[ssd_priors_index][i].x_center;
				tmp_boxes[1] = (float)locs[location_ind + 1]/ locs_scale * CENTER_VARIANCE * priors[ssd_priors_index][i].h + priors[ssd_priors_index][i].y_center;
				tmp_boxes[2] = inu_exp((float)locs[location_ind + 2] / locs_scale * SIZE_VARIANCE) * priors[ssd_priors_index][i].w;
				tmp_boxes[3] = inu_exp((float)locs[location_ind + 3] / locs_scale * SIZE_VARIANCE) * priors[ssd_priors_index][i].h;

				/* convert to corners form */
				boxes[num_boxes_above_thresh].min_x = tmp_boxes[0] - tmp_boxes[2] / 2;
				boxes[num_boxes_above_thresh].max_x = tmp_boxes[0] + tmp_boxes[2] / 2;
				boxes[num_boxes_above_thresh].min_y = tmp_boxes[1] - tmp_boxes[3] / 2;
				boxes[num_boxes_above_thresh].max_y = tmp_boxes[1] + tmp_boxes[3] / 2;
				boxes[num_boxes_above_thresh].confidence = softmax_confs[softmax_index];
				boxes[num_boxes_above_thresh].class_id = class_id;
				boxes[num_boxes_above_thresh].center_box.w = boxes[num_boxes_above_thresh].max_x - boxes[num_boxes_above_thresh].min_x;
				boxes[num_boxes_above_thresh].center_box.h = boxes[num_boxes_above_thresh].max_y - boxes[num_boxes_above_thresh].min_y;
				boxes[num_boxes_above_thresh].center_box.x = boxes[num_boxes_above_thresh].min_x + boxes[num_boxes_above_thresh].center_box.w / 2;
				boxes[num_boxes_above_thresh].center_box.h = boxes[num_boxes_above_thresh].min_y + boxes[num_boxes_above_thresh].center_box.h / 2;

				// Check that box is within limits
				if (boxes[num_boxes_above_thresh].min_x >= 0 && boxes[num_boxes_above_thresh].min_y >= 0 &&
					boxes[num_boxes_above_thresh].max_x < 1 && boxes[num_boxes_above_thresh].max_y < 1) {
					num_boxes_above_thresh++;
				}
			}
		}
	}
#if 0
	LOGG_PRINT(LOG_INFO_E, (ERRG_codeE)NULL, "before nms %d:\n", num_boxes_above_thresh);
	for (int i = 0; i < num_boxes_above_thresh; i++) {
		LOGG_PRINT(LOG_INFO_E, (ERRG_codeE)NULL, "boxes %d: %f %f %f %f  conf:%f  class:%d\n", i, boxes[i].min_x, boxes[i].min_y, boxes[i].max_x, boxes[i].max_y, boxes[i].confidence, boxes[i].class_id);
	}
#endif

	do_ssd_nms();

	number_of_boxes = 0;
	for (int i = 0; i < num_boxes_above_thresh; i++) {
		if (boxes[i].confidence != 0) {
			if (i != number_of_boxes) {
				boxes[number_of_boxes] = boxes[i];
			}
			number_of_boxes++;
		}
	}

#if 0
	LOGG_PRINT(LOG_INFO_E, (ERRG_codeE)NULL, "after nms boxes:%d:\n", number_of_boxes);
	for (int i = 0; i < number_of_boxes; i++) {
		LOGG_PRINT(LOG_INFO_E, (ERRG_codeE)NULL, "boxes %d: %f %f %f %f  conf:%f class:%d\n", i, boxes[i].min_x, boxes[i].min_y, boxes[i].max_x, boxes[i].max_y, boxes[i].confidence, boxes[i].class_id);
	}
#endif

}

