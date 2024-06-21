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
#include "inu2_internal.h"
#include "log.h"
#include "sched.h"
#include "ictl_drv.h"
#include "ev_init.h"
#include "inu_convert2float.h"

//Declaration of OpenCL kernel
EVOCL_KERNEL_DECL(ev_inu_ocl_convert2f_kernel);

//===============================================================================
//  inputs: 1) short * in_ptr  - pointer to input matrix of type short numbers.
//          2) voidv * out_ptr - pointer to the output matrix.(output format is float or half).
//          3) int num_elemnts - number of elements to convert.
//          4) float scaler    - scaler to multiply the inputs.
//          5) bool float_output - define the input type, '1'- for float,'0'- for half.
//          5) bool is_first_sec  - used for split the function to the two cores. (true for the first section (part) of the function.
//                                  false for the second  (last) section
//          6) float split_th     - used for split the function to the two cores. floor(split_th*rows) for the first section. 
//                                  the rest for the last section.
//  process: convert the (input short * scaler ) to float of format float or half according the float_output flag.
//===============================================================================
void inu_convert2float(short * in_ptr, void * out_ptr, int num_elemnts ,float scaler, bool float_output, bool is_first_sec ,float split_th)
{
	OCL_conver2f_PRINT_EN_DEBUG ctl;
	ctl.in_ptr		= in_ptr;
	ctl.out_ptr		= out_ptr;
	ctl.float_output = float_output;
	
	ctl.in_vec_len = num_elemnts;
	ctl.scaler     = scaler;
	
	ctl.split_th	= split_th;
	ctl.is_first_section	= is_first_sec;
	
	//printf("num_elemnts =  %d, scaler=%f , float_output =%d \n",num_elemnts ,scaler, float_output);

#ifdef softmax_CYCLES_MEAS	
    uint64_t start, end;
	start = evGetTimeInCycles();
#endif

    ctl.local_mem_ptr   = (unsigned char*) EVSS_CFG_VCCM_START;
    ctl.local_mem_size  = EVSS_CFG_VCCM_SIZE;

	void * args[] = {&ctl, out_ptr, 
	                       in_ptr};
	
	callOclManualKernel(OCL_KERNEL(ev_inu_ocl_convert2f_kernel), args);


#ifdef softmax_CYCLES_MEAS
    float ev_clock_rate = CYCLES_PER_MS;
    end = evGetTimeInCycles();
 	printf("Execution took %llu cycles, at clock rate:%d ,execution time=%f ms\n", (end - start),(int)ev_clock_rate,(float)(end - start)/ev_clock_rate);
#endif
} 
//===============================================================================

//} // extern "C"
    
