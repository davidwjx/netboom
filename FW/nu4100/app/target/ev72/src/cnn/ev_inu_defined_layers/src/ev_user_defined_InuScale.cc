/****************************************************************************
 *
 *   FileName: ev_user_defined_crop.cc
 *
 *   Author:  Arviv B.
 *
 *   Date: 
 *
 *   Description: 
 *   
 ****************************************************************************/

#include <ev_layer.h>
#include <math.h>
#include <stdlib.h>
#include <cstring>
#include <algorithm>



#include <ev_ocl.h>
#include "ev_layer_interface.h"
#include "ev_ocl_crop_interface.h"
#include "ev_ocl_InuScale_interface.h"



EVOCL_KERNEL_DECL(ev_ocl_InuScale_kernel);  // should be defined at \kernels

#define data_type short
extern short   inverse_LUT[];
extern "C" {
int Benny_evGetTimeInCycles(void) {
    return (int)getRTC();
}
}

int ev_ocl_InuScale(EvCnnDistributeUserDefined * data,int user_id)
{
    ocl_ctl_InuScale_t ctl;

    int16_t *output = (int16_t *)data->mapOut[0]->c_ptr;
    int16_t *input  = (int16_t *)data->mapIn[0]->c_ptr;
	ctl.dataIn  	= (short*)data->mapIn[0]->c_ptr;
    ctl.dataOut 	= (short*)data->mapOut[0]->c_ptr;
	
	ctl.IN_float_scale = data->mapIn[0]->scale;
    ctl.num_channels= data->mapIn[0]->num_maps;
    ctl.in_height 	= data->mapIn[0]->height;
    ctl.in_width 	= data->mapIn[0]->width;
	
	ctl.reduce2_LUT   	= inverse_LUT;
 		
    ctl.local_mem_ptr   = (unsigned char*) EVSS_CFG_VCCM_START;
    ctl.local_mem_size  = EVSS_CFG_VCCM_SIZE;

    void * args[] = {&ctl, output, input};
    callOclManualKernel(OCL_KERNEL(ev_ocl_InuScale_kernel), args);

    return EV_LAYER_OK;
}



extern "C" {
void evUserDefinedInuScale(EvCnnDistributeDataBase *info)
{

    int user_id = info->user_id; // Layer user id specified in ini file
    EvCnnDistributeUserDefined * layer_info = (EvCnnDistributeUserDefined *) info;




#ifdef InuScale_CYCLES_MEAS	
    static volatile int *start_debug_buffer_cc =(volatile int *)0x010008e0;   // (volatile int *)0x010008a8
    *start_debug_buffer_cc++ = 0xccccDDDD;
	uint64_t startCycles = getRTC();
	*start_debug_buffer_cc++ = (uint32_t)startCycles;
	//uint64_t startCycles = getRTC(); 
#endif    

    ev_ocl_InuScale(layer_info,user_id);

#ifdef InuScale_CYCLES_MEAS
    *start_debug_buffer_cc++ = 0xcccceeee;
	uint64_t EndCycles = getRTC();
	*start_debug_buffer_cc++ = (uint32_t)EndCycles;
	//*start_debug_buffer_cc++ = _lr(AUX_RTC_LOW);
	//float totalCycles = getRTC() - startCycles;
    //printf("InuScale[%d] , takes %f cycles \n\n\n", user_id,  totalCycles);
#endif	
	return;

}
}
    

