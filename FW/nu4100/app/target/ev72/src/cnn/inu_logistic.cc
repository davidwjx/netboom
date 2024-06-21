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
#include "inu_logistic.h"
#include "logistic_lut_float.h"	 

//Declaration of OpenCL kernel
EVOCL_KERNEL_DECL(ev_inu_ocl_logistic_kernel);

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
//  inputs: 1) void * in_ptr      - pointer to input vector of type short or char.
//          2) float * out_ptr    - pointer to the output after regration.
//          3) int in_vec_length  - input vector length.
//          4) float scale        - scale factor to devide the input.
//          5) int element_size   - according the input type, '1' for input char, '2' for input short.
//          5) bool is_first_sec  - used for split the function to the two cores. (true for the first section (part) of the function.
//                                  false for the second  (last) section
//          6) float split_th     - used for split the function to the two cores. floor(split_th*rows) for the first section. 
//                                  the rest for the last section.
//  process:        tmp_out(k)    = (float)in_char_vec[k]/scale , in_char_vec[k] is in type short or char.
//                  output(k)     = 1.0/(1+ exp(-1.0*tmp_out(k)));
//===============================================================================
void inu_logistic(void * in_ptr, float * out_ptr, int in_vec_length ,float scale,int element_size, bool is_first_sec ,float split_th)
{
	ocl_ctl_logistic ctl;
	ctl.in_ptr		= in_ptr;
	ctl.out_ptr		= out_ptr;
	
	ctl.in_vec_len	= in_vec_length;
	ctl.element_size = element_size;
	
	ctl.split_th	= split_th;
	ctl.is_first_section	= is_first_sec;

    int scale_factor;
	int scale_num_bits;
	float scale_f = LOGISTIC_LUT_RESOLUTION/scale;
    scale_fit(scale_f, 15, scale_factor, scale_num_bits);
	ctl.scale_factor		= scale_factor;
	ctl.scale_shift_bits	= scale_num_bits;
	
	LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL, "in_vec_length =  %d \n",in_vec_length);
	LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL, "scale_f = %f, scale_factor = %d,  scale_num_bits = %d \n",scale_f, scale_factor,scale_num_bits);
	LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL, "Split: is_first_section=%d , split_th=%f \n", is_first_sec, split_th);

#ifdef logistic_CYCLES_MEAS	
    uint64_t start, end;
	start = evGetTimeInCycles();
#endif

    ctl.local_mem_ptr   = (unsigned char*) EVSS_CFG_VCCM_START;
    ctl.local_mem_size  = EVSS_CFG_VCCM_SIZE;

	void * args[] = {&ctl, out_ptr, 
	                       in_ptr};
	ctl.logistic_lut_ptr  = logistic_ocl_lut;
	
	callOclManualKernel(OCL_KERNEL(ev_inu_ocl_logistic_kernel), args);


#ifdef logistic_CYCLES_MEAS
    float ev_clock_rate = CYCLES_PER_MS;
    end = evGetTimeInCycles();
	LOGG_PRINT(LOG_INFO_E, (ERRG_codeE)NULL, "Execution took %llu cycles, at clock rate:%d ,execution time=%f ms len:%d\n", (end - start),(int)ev_clock_rate,(float)(end - start)/ev_clock_rate, in_vec_length);
#endif
} 
//===============================================================================

//} // extern "C"
    
