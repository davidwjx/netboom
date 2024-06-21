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

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <algorithm>
#include <evss/kdisp.h>
#include <inu_utils.h>
#include "inu_app_pipe.h"
#include "ev_ocl_copy_interface.h"	
#include "log.h"

//Declaration of OpenCL kernel
EVOCL_KERNEL_DECL(ev_inu_ocl_copy_kernel);

//===============================================================================
// inputs:
//      	*input_blob  - pointer to input blob, data (unsigned char).
//      	*output_blob - pointer to output blob (unsigned char).
//      	size        - size of input/output data [char].
// outputs:
//      	copy the input blob 
//===============================================================================
void copy_kernel(unsigned char *src ,unsigned char *dst ,int size)
{
	
	ocl_ctl_copy_t ctl;
#ifdef copy_CYCLES_MEAS	
    uint64_t start, end;
	start = evGetTimeInCycles();
#endif
	ctl.dataIn  	= (unsigned char *)src;
	ctl.dataOut 	= (unsigned char *)dst;

	ctl.size     = (int )size;

	ctl.local_mem_ptr   = (unsigned char*) EVSS_CFG_VCCM_START;
	ctl.local_mem_size  = EVSS_CFG_VCCM_SIZE;
	
	void * args[] = {&ctl, dst, src};
	inuCallOcl(OCL_KERNEL(ev_inu_ocl_copy_kernel), args);

#ifdef copy_CYCLES_MEAS
    #define CYCLES_PER_MS 820000
    float ev_clock_rate = CYCLES_PER_MS;
    end = evGetTimeInCycles();
	printf("Execution took %llu cycles, at clock rate:%d ,execution time=%f ms\n", (end - start),(int)ev_clock_rate,(float)(end - start)/ev_clock_rate);

#endif
} 
//===============================================================================

//} // extern "C"
    
