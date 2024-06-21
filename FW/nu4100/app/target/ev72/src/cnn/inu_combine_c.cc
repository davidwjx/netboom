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
#include "inu_combine_c.h"	

//Declaration of OpenCL kernel
EVOCL_KERNEL_DECL(ev_inu_ocl_combine_uchar_kernel);

//===============================================================================
//  inputs: 1) int num_mats  - number of matrix to combine.
//          2) int x_size    - number of columns of the input matrix.
//          3) int y_size    - number of rows of the input matix.
//          4) unsigned char ** mats_ptrs  - pointer to aaray of pointers of the input matrix to combine.
//          5) unsigned char *out_mat      - pointer to the combined outout matrix.
//          5) bool is_first_sec  - used for split the function to the two cores. (true for the first section (part) of the function.
//                                  false for the second  (last) section
//          6) float split_th     - used for split the function to the two cores. floor(split_th*rows) for the first section. 
//                                  the rest for the last section.
//  process: output is the maximum per pixel.
//===============================================================================
void Ev_yolact_combine(int num_mats, int x_size, int y_size, unsigned char ** mats_ptrs, unsigned char *out_mat, bool is_first_sec ,float split_th)
{
	
	ocl_ctl_combine_c_planar ctl;
	ctl.num_mats	= num_mats;
	ctl.size_x		= x_size;
	ctl.size_y      = y_size;
		
	ctl.split_th	= split_th;
	ctl.is_first_section	= is_first_sec;
	for (int k=0; k<num_mats; k++)
	{
		ctl.in_mats_ptrs[k] = mats_ptrs[k];	    // pointer for the output, planer format
		//LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL, "input  mat %d , at add %p \n",k, mats_ptrs[k] );
	}
    ctl.out_ptr      = out_mat;	    // pointer for the output, planer format
	//LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL, "output mat at add %p \n",out_mat);

#ifdef combine_c_CYCLES_MEAS	
    uint64_t start, end;
	start = evGetTimeInCycles();
#endif

    ctl.local_mem_ptr   = (unsigned char*) EVSS_CFG_VCCM_START;
    ctl.local_mem_size  = EVSS_CFG_VCCM_SIZE;

	void * args[] = {&ctl, out_mat};
	callOclManualKernel(OCL_KERNEL(ev_inu_ocl_combine_uchar_kernel), args);

#ifdef combine_c_CYCLES_MEAS
    float ev_clock_rate = CYCLES_PER_MS;
    end = evGetTimeInCycles();
	LOGG_PRINT(LOG_INFO_E, (ERRG_codeE)NULL, "Execution took %llu cycles, at clock rate:%d ,execution time=%f ms\n", (end - start),(int)ev_clock_rate,(float)(end - start)/ev_clock_rate);
#endif
} 
//===============================================================================

//} // extern "C"
    
