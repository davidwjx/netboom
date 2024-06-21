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
#include <opencv2/opencv.hpp>
#include "client_api.h"
#include "inu_utils.h"
#include <cnn_dev.h>

#ifndef INU_HAPS
#include "inu2_internal.h"
#include "log.h"
#include "sched.h"
#include "ictl_drv.h"
#include "ev_init.h"
#endif
#include "inu_softmax.h"
#include "exp_lut_Q10.h"	 

//Declaration of OpenCL kernel
EVOCL_KERNEL_DECL(ev_inu_ocl_softmax_kernel);

//===============================================================================
static
void scale_fit(float scale, int abs_bits, int &int_scale, int &frac_bits)
{
    if (fabs(1.f - scale) < FLT_EPSILON) {
        int_scale = (1 << abs_bits) - 1;
        frac_bits = abs_bits;
        return;
    }

    int exp_val;
    int_scale = frexpf(scale, &exp_val) * (1 << abs_bits);
    frac_bits = abs_bits - exp_val;
}	
//===============================================================================
//  inputs: 1) short * in_ptr matrix of type short in Q10 format, the size of input num_groups rows X num_classes_plus1 columns.
//          2) short * out_ptr matrix of type short in Q10 format.
//          3) int num_groups - number of rows of num_classes_plus1 columns at the input matix.
//          4) int num_classes_plus1 - the length of each group.
//          5) bool is_first_sec  - used for split the function to the two cores. (true for the first section (part) of the function.
//                                  false for the second  (last) section
//          6) float split_th     - used for split the function to the two cores. floor(split_th*rows) for the first section. 
//                                  the rest for the last section.
//  process: perform softmax  { exp(Xj) / sumi(exp(Xi) }for num_groups of size num_classes_plus1 elements. the input & output format is short in Q10.
//===============================================================================
void inu_softmax(short * in_ptr, short * out_ptr, int num_groups ,int num_classes_plus1, bool is_first_sec ,float split_th)
{
	ocl_ctl_softmax ctl;
	ctl.in_ptr		= in_ptr;
	ctl.out_ptr		= out_ptr;
	
	ctl.exp_lut_ptr = exp_lut_Q10;
	
	ctl.num_groups	= num_groups;
	ctl.num_classes_plus1 = num_classes_plus1;
	
	ctl.split_th	= split_th;
	ctl.is_first_section	= is_first_sec;

    int scale_factor;
	int scale_num_bits;
	float scale_f = EXP_LUT_RESOLUTION/(float)(1<<Q_IN);
    scale_fit(scale_f, 15, scale_factor, scale_num_bits);
	ctl.scale_factor		= scale_factor;
	ctl.scale_shift_bits	= scale_num_bits;
	
	LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL, "num_groups =  %d, num_classes_plus1=%d \n",num_groups ,num_classes_plus1);
	LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL, "in_vec_length =  %d \n",num_groups*num_classes_plus1);

#ifdef softmax_CYCLES_MEAS	
    uint64_t start, end;
	start = evGetTimeInCycles();
#endif

    ctl.local_mem_ptr   = (unsigned char*) EVSS_CFG_VCCM_START;
    ctl.local_mem_size  = EVSS_CFG_VCCM_SIZE;

	void * args[] = {&ctl, out_ptr, 
	                       in_ptr};
	
	callOclManualKernel(OCL_KERNEL(ev_inu_ocl_softmax_kernel), args);


#ifdef softmax_CYCLES_MEAS
    float ev_clock_rate = CYCLES_PER_MS;
    end = evGetTimeInCycles();
	LOGG_PRINT(LOG_ERROR_E, (ERRG_codeE)NULL, "Execution took %llu cycles, at clock rate:%d ,execution time=%f ms\n", (end - start),(int)ev_clock_rate,(float)(end - start)/ev_clock_rate);
#endif
} 
//===============================================================================

//} // extern "C"
    
