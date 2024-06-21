/****************************************************************************
 *
 *   FileName: yolo_v3_pp.h
 *
 *   Author:  
 *
 *   Date: 
 *
 *   Description: 
 *   
 ****************************************************************************/

#ifndef YOLACT_PP_H
#define YOLACT_PP_H

//#include <opencv2/opencv.hpp>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cnn_hal.h"
#include "inu_app_pipe.h"

#define YOLACT_PP_PROCESS_TIME 1

//Default values (will be updated from network binary
#define INU_YOLACT_NETWORK_DIMENSION 368
#define INU_YOLACT_NUMBER_OF_CLASSES 80
#define MAX_NUM_OF_CLASSES 100
#define INU_YOLACT_CONF_THRESHOLD 0.1F
#define INU_YOLACT_NMS_THRESHOLD 0.4F
#define YOLACT_NMS_ALL_CLASSES_THRESH 0.5
#define INU_YOLACT_SCORE_THRESHOLD 0.15F
#define INU_YOLACT_NUMBER_OF_TAILS 16
#define INU_YOLACT_TOP_K 15
#define INU_YOLACT_MAX_DETECTIONS 10
#define E_CONST 2.71828F


/****************************************************************************
 ***************     G L O B A L        D E F N I T I O N S    **************
 ****************************************************************************/

typedef struct
{
	unsigned int jobDescriptor;
	int num_of_tails;
	EV_CNN_TAIL tails[EV_CNN_MAX_TAILS];
	int image_rows;
	int image_cols;
	int net_input_size;
	int net_id;
	unsigned int frame_id;
	void *blob_output_header;
	void *output_ptr;
} YOLACT_PPG_infoT;


typedef struct {
	int width;
	int Height;
	int Inputs;
	int elements;
	float scale;
} yolact_tail_info;

typedef struct {
	float x, y, w, h;
} yolact_box;

typedef struct {
	float val;
	int index;
} _sorted_indexes;

/****************************************************************************
 ***************      E X T E R N A L       D A T A           ***************
****************************************************************************/

extern int		inu_yolact_network_dimension;
extern int		inu_yolact_number_of_classes;
extern float	inu_yolact_nms_threshold;
extern float	inu_yolact_conf_threshold;
extern float	inu_yolact_score_threshold;

/****************************************************************************
 *************************     PROROTYPES    ********************************
 ****************************************************************************/

void *ev_inu_yolact_post_thread(void *arg);
void* ev_inu_yolact_second_post_thread(void* arg);
void yolact_pp_init(int coreId);
void inu_yolact_init(void);
void inu_yolact_sort_tails(void);
void inu_yolact_convert_to_float(void);
float inu_tanh(float val);
void inu_yolact_parser(short **sorted_tails, yolact_tail_info *sorted_tails_info);
void yolact_mutex_lock(void);
void yolact_mutex_unlock(void);

#endif
