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
#include "inu_resize_f.h"	

//Declaration of OpenCL kernel
EVOCL_KERNEL_DECL(ev_inu_ocl_resize_float_kernel);

//===============================================================================
//  inputs: 1) int in_x  - number of columns of the input matix.
//          2) int in+y  - number of rows of the input matrix.
//          3) void *in_mat - ponter to input matrix (of type float or half).
//          4) int out_x - number of columns of the output matix (after resize).
//          5) int out_y - number of rows of the output matix (after resize).
//          6) unsigned char *out_mat - pointer to the output matrix of uchar.
//          7) loat thresh - the treshold to compare each pixel after the resize.
//          8) int class_id- the class id fot each pixel above the trshold.
//          8) int element_size- element size of each element at the input matrix, 2 for half, 4 for float.
//          9) bool is_first_sec  - used for split the function to the two cores. (true for the first section (part) of the function.
//                                  false for the second  (last) section
//         10) float split_th     - used for split the function to the two cores. floor(split_th*rows) for the first section. 
//                                  the rest for the last section.
//  process: resize the input map of size (in_y,in_x) to size (out_y,out_x), the input format can be float or half, 
//           each pixel (after the resize) above the treshold ,gets the class_id.
//===============================================================================
void Ev_resize_float_mat(int in_x, int in_y, void *in_mat, int out_x, int out_y, unsigned char *out_mat, \
                        float thresh, int class_id, int element_size, bool is_first_sec ,float split_th)
{
	
	ocl_ctl_resize_f_planar ctl;
	ctl.in_ptr		= in_mat;
	ctl.out_ptr		= out_mat;
	
	ctl.in_height	= in_y;
	ctl.in_width	= in_x;
	
	ctl.out_height	= out_y;
	ctl.out_width	= out_x;
	ctl.priority_th	= thresh;
	ctl.class_id	= class_id;
	ctl.element_size	= element_size;
	
	ctl.split_th	= split_th;
	ctl.is_first_section	= is_first_sec;


	LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL, "input  size [h,w] = [ %d, %d] \n",in_y, in_x);
	LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL, "output size [h,w] = [ %d, %d] \n",out_y, out_x);
	LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL, "thresh = %f, class_id = %d, element_size=%d\n", thresh,class_id , element_size);
	LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL, "Split: is_first_sec=%d , split_th=%f\n", is_first_sec, split_th);

#ifdef resize_f_CYCLES_MEAS	
    uint64_t start, end;
	start = evGetTimeInCycles();
#endif

    ctl.local_mem_ptr   = (unsigned char*) EVSS_CFG_VCCM_START;
    ctl.local_mem_size  = EVSS_CFG_VCCM_SIZE;

	void * args[] = {&ctl, out_mat, 
	                       in_mat};
	callOclManualKernel(OCL_KERNEL(ev_inu_ocl_resize_float_kernel), args);

#ifdef resize_f_CYCLES_MEAS
    float ev_clock_rate = CYCLES_PER_MS;
    end = evGetTimeInCycles();
	LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL, "Execution took %llu cycles, at clock rate:%d ,execution time=%f ms\n", (end - start),(int)ev_clock_rate,(float)(end - start)/ev_clock_rate);
#endif
} 
//===============================================================================

//} // extern "C"
    
