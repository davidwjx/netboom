/****************************************************************************
 *
 *   FileName: bokeh_main.cc
 *
 *   Author:  Arviv B.
 *
 *   Date: 
 *
 *   Description: 
 *   
 ****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <opencv2/opencv.hpp>
#include <stdlib.h>
#include <cstring>
#include <algorithm>
#include <evss/kdisp.h>
#include <inu_utils.h>
#include "inu_app_pipe.h"
#include "log.h"
#include "inu_proto_mask_mul.h"	
#include "logistic_f_lut.h"

//Declaration of OpenCL kernel
EVOCL_KERNEL_DECL(ev_inu_ocl_proto_mask_mul_kernel);

//===============================================================================
//  inputs: 1) int width  - width of input proto blob in order H,W,C. (hight,width, channels)
//          2) int hight  - hight of input proto blob in order H,W,C.
//          3) int channels  - number of input channels of input proto blob in order H,W,C.
//          4) int num_elements  - number of elements at the mask matrix of size (num_elements, channels).
//          5) float *proto_data  - pointer of the input proto blob of type float , size (hight,width,channels).
//          6) float *mask_data   - pointer of input mask matrix of type float, size (hight,width,num_elements).
//          7) float *mask_proto_result  - pointer of output float result, size (num_elements, channels).
//          8) bool is_first_sec  - used for split the function to the two cores. (true for the first section (part) of the function.
//                                  false for the second  (last) section
//          9) float split_th     - used for split the function to the two cores. floor(split_th*rows) for the first section. 
//                                  the rest for the last section.
//  process: tmp_out(y,x,e) = SUM(Proto(y,x,:)*Mask(e,:)), the vectors Proto(y,x,:) and  Mask(e,:) are of length channels.
//                           output is sigmoid of tmp_out(y,x,e)  (1./(1 + exp(-tmp_out(y,x,e) ))
//===============================================================================
void Ev_multiply_mask(int width, int hight, int channels, int num_elements, \
                      float *proto_data,  float *mask_data, float *mask_proto_result, \
					  bool is_first_sec, float split_th)
{
	
	ocl_ctl_proto_mask_mul ctl;
	ctl.in_proto_ptr	= proto_data;
	ctl.in_mask_ptr		= mask_data;
	ctl.out_res_ptr		= mask_proto_result;
	ctl.sigmoid_lut     = logistic_lut;
	
	ctl.in_height	= hight;
	ctl.in_width	= width;
	ctl.in_channels	= channels;
	
	ctl.num_elements	= num_elements;
	
	ctl.split_th	= split_th;
	ctl.is_first_section	= is_first_sec;
	
	// add error print if channels is not multiple of 32

#ifdef OCL_PROTO_MASK_PRINT_EN
	LOGG_PRINT(LOG_INFO_E, (ERRG_codeE)NULL, "proto  size [h,w,c] = [ %d, %d , %d ] \n",hight, width, channels);
	LOGG_PRINT(LOG_INFO_E, (ERRG_codeE)NULL, "mask   size [h,w] = [ %d, %d] \n",num_elements, channels);
	LOGG_PRINT(LOG_INFO_E, (ERRG_codeE)NULL, "Split: is_first_sec=%d , split_th=%f\n", is_first_sec, split_th);
#endif

#ifdef proto_mask_CYCLES_MEAS	
    uint64_t start, end;
	start = evGetTimeInCycles();
#endif

    ctl.local_mem_ptr   = (unsigned char*) EVSS_CFG_VCCM_START;
    ctl.local_mem_size  = EVSS_CFG_VCCM_SIZE;

	void * args[] = {&ctl, mask_proto_result, 
	                       proto_data, mask_data};
	callOclManualKernel(OCL_KERNEL(ev_inu_ocl_proto_mask_mul_kernel), args);

#ifdef proto_mask_CYCLES_MEAS
    float ev_clock_rate = CYCLES_PER_MS;
    end = evGetTimeInCycles();
	LOGG_PRINT(LOG_INFO_E, (ERRG_codeE)NULL, "Execution took %llu cycles, at clock rate:%d ,execution time=%f ms\n", (end - start),(int)ev_clock_rate,(float)(end - start)/ev_clock_rate);
#endif
} 
//===============================================================================

//} // extern "C"
    
