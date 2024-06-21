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
#include "inu_matrix_max_rows.h"	

//Declaration of OpenCL kernel
EVOCL_KERNEL_DECL(ev_inu_ocl_matrix_max_rows_kernel);

//===============================================================================
//  inputs: 1) __fp16 *input_mat - pointer to input matrix of 16 bits float numbers.
//          2) int rows  - number of rows of the input matrix.
//          3) int cols  - number of columns of the input matix.
//          4) float *max_values  - pointer of the output vector of type float (32 bits each).
//          5) bool is_first_sec  - used for split the function to the two cores. (true for the first section (part) of the function.
//                                  false for the second  (last) section
//          6) float split_th     - used for split the function to the two cores. floor(split_th*rows) for the first section. 
//                                  the rest for the last section.
//                                - the split to two cores, not implemented yet.
//  process: output the maximum value of each input row.
//===============================================================================
void EV_matrix_rows_max_values(__fp16 *input_mat, int rows, int cols, float *max_values ,\
                               bool is_first_sec, float split_th)
{
	
	ocl_ctl_matrix_max_rows ctl;
	ctl.in_matrix_ptr	= input_mat;
	ctl.out_res_ptr		= max_values;
	
	ctl.in_height	= rows;
	ctl.in_width	= cols;
	
	ctl.split_th	= split_th;
	ctl.is_first_section	= is_first_sec;
	

	LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL, "input_mat:%p max_values:%p rows:%d cols:%d\n", input_mat, max_values, rows, cols);

	// add error print if channels is not multiple of 32

#ifdef OCL_MAT_MAX_ROWS_PRINT_EN
	LOGG_PRINT(LOG_INFO_E, (ERRG_codeE)NULL, "input matrix size [h,w] = [ %d, %d ] \n",rows, cols);
	LOGG_PRINT(LOG_INFO_E, (ERRG_codeE)NULL, "Split: is_first_sec=%d , split_th=%f\n", is_first_sec, split_th);
#endif

#ifdef matrix_max_rows_CYCLES_MEAS	
    uint64_t start, end;
	start = evGetTimeInCycles();
#endif

    ctl.local_mem_ptr   = (unsigned char*) EVSS_CFG_VCCM_START;
    ctl.local_mem_size  = EVSS_CFG_VCCM_SIZE;

	void * args[] = {&ctl, max_values, 
	                       input_mat};
	callOclManualKernel(OCL_KERNEL(ev_inu_ocl_matrix_max_rows_kernel), args);

#ifdef matrix_max_rows_CYCLES_MEAS
    float ev_clock_rate = CYCLES_PER_MS;
    end = evGetTimeInCycles();
	LOGG_PRINT(LOG_INFO_E, (ERRG_codeE)NULL, "Execution took %llu cycles, at clock rate:%d ,execution time=%f ms\n", (end - start),(int)ev_clock_rate,(float)(end - start)/ev_clock_rate);
#endif
} 
//===============================================================================

//} // extern "C"
    
