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

#ifndef INU_UTILS_H
#define INU_UTILS_H

#include "math.h"
#include "evthreads.h"
#include <ev_ocl.h>

#define E_CONST                 2.71828F

struct Box
{
	float x, y, w, h;
};

//#define INU_DEBUG_MEAS 1
#define MAX_TIMING_ENTRIES 60
#define MAX_TIMING_TEXT_LEN 30

struct inu_timing_info
{
	uint64_t start_time;
	uint64_t end_time;
	EvThreadType   thread;
	char text[MAX_TIMING_TEXT_LEN];
};

float inu_exp(float val);
float overlap(float x1, float w1, float x2, float w2);
float box_intersection(Box a, Box b);
float box_union(Box a, Box b);
float box_iou(Box a, Box b);
void exp_lut_init(void);
int init_ocl_mutexes(void);
void inuCallOcl(ev_ocl_kernel_data d, void **args);
void inu_init_timing_info(void);
void inu_register_timing_info(uint64_t start_time, uint64_t end_time, char* text);
void inu_print_timing_info(bool flush_flag = false);

#endif

