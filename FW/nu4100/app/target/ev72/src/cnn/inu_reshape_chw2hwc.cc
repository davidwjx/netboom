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

//#include <ev_layer.h> 
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
#include "inu_reshape_chw2hwc.h"
#include "tanh_lut_int_Q10.h"	 

//Declaration of OpenCL kernel
EVOCL_KERNEL_DECL(ev_inu_ocl_reshape_chw2hwc_scale_kernel);
EVOCL_KERNEL_DECL(ev_inu_ocl_reshape_chw2hwc_scale_tanh_kernel);
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
//  inputs: 1) short * in_ptr  - pointer to input blob of size (in_channels,in_height,in_width).
//          2) short * out_ptr - pointer to the output bolo of size (in_height,in_width,in_channels).
//          3) int  in_height  - height of input blob.
//          4) int  in_width   - width of input blob.
//          4) int  in_channels- num channels of input blob.
//          3) float scale     -  scale to divide the input.
//          4)  bool do_tanh,  - flag if to perform tanh on the output blob.
//          5) bool is_first_sec  - used for split the function to the two cores. (true for the first section (part) of the function.
//                                  false for the second  (last) section
//          6) float split_th     - used for split the function to the two cores. floor(split_th*rows) for the first section. 
//                                  the rest for the last section.
//  process: reshape input from CHW to HWC and 1.divide by scale, 2. multiply by 1024 3. perform tanh according flag.
//===============================================================================
void inu_reshape_chw2hwc(short * in_ptr, short * out_ptr,int  in_height, int  in_width, int in_channels, \
						 float scale,  bool do_tanh,  bool  is_first_section,float split_th )
{
	ocl_ctl_reshape_chw2hwc ctl;
	ctl.in_ptr		= in_ptr;
	ctl.out_ptr		= out_ptr;
	
	ctl.in_height	= in_height;
	ctl.in_width	= in_width;
	ctl.in_channels	= in_channels;
	
	ctl.split_th	= split_th;
	ctl.is_first_section	= is_first_section;

    int scale_factor;
	int scale_num_bits;
	float one_over_scale = 1.0/scale;
    scale_fit(one_over_scale, 15, scale_factor, scale_num_bits);
	ctl.one_over_scale_factor		= scale_factor;
	ctl.one_over_scale_shift_bits	= scale_num_bits;
	
	LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL, "input  size [h,w,c] = [ %d, %d, %d] \n",in_height, in_width,in_channels);
	LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL, "one_over_scale = %f, scale_factor = %d,  scale_num_bits = %d \n",one_over_scale, scale_factor,scale_num_bits);
	LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL, "Split: is_first_section=%d , split_th=%f\n", is_first_section, split_th);

#ifdef reshape_CYCLES_MEAS	
    uint64_t start, end;
	start = evGetTimeInCycles();
#endif

    ctl.local_mem_ptr   = (unsigned char*) EVSS_CFG_VCCM_START;
    ctl.local_mem_size  = EVSS_CFG_VCCM_SIZE;

	void * args[] = {&ctl, out_ptr, 
	                       in_ptr};
	if (do_tanh)
	{
		ctl.tanh_Q10_lut  = tanh_lut_int_Q10;
		callOclManualKernel(OCL_KERNEL(ev_inu_ocl_reshape_chw2hwc_scale_tanh_kernel), args);
	}
	else
	{
		callOclManualKernel(OCL_KERNEL(ev_inu_ocl_reshape_chw2hwc_scale_kernel), args);
	}

#ifdef reshape_CYCLES_MEAS
    float ev_clock_rate = CYCLES_PER_MS;
    end = evGetTimeInCycles();
 	printf("Execution took %llu cycles, at clock rate:%d ,execution time=%f ms\n", (end - start),(int)ev_clock_rate,(float)(end - start)/ev_clock_rate);
#endif
} 
//===============================================================================

//} // extern "C"
    
