/*************************************************************************/
/**                                                                     **/
/** Copyright (C) 2017 Synopsys, Inc.                                   **/
/** All Rights Reserved.                                                **/
/**                                                                     **/
/** This Synopsys software and all associated documentation are         **/
/** proprietary to Synopsys, Inc. and may only be used pursuant to the  **/
/** terms and conditions of a written license agreement with Synopsys,  **/
/** Inc. All other use, reproduction, modification, or distribution of  **/
/** this Synopsys software or the associated documentation is strictly  **/
/** prohibited.                                                         **/
/**                                                                     **/
/*************************************************************************/

#include <ev_layer.h>
#include <math.h>
#include <stdlib.h>
#include <cstring>
#include <algorithm>
#include <evss/kdisp.h>
#include <ev_ocl.h>
#include "ev_layer_interface.h"
#include "ev_ocl_corr1d_interface.h"

#define clamp(v, vmin, vmax) std::max(std::min((v), (vmax)), (vmin))
#define FLT_EPSILON 1.19209290e-07F

#ifdef EVSS_CFG_HAS_OCL
//Declaration of OpenCL kernel
EVOCL_KERNEL_DECL(ev_inu_ocl_corr1d_kernel);
//----------------------------------------------------------------------
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
/*----------------------------------------------------------------------*/
ev_layer_status_e ev_ocl_correlate(EvCnnDistributeUserDefined *data)
{
	ocl_ctl_corr1d_t ctl;
    short *inputA   = (short*) data->mapIn[0]->c_ptr;
    short *inputB   = (short*) data->mapIn[1]->c_ptr;
    short *output   = (short*) data->mapOut[0]->c_ptr;
	ctl.dataInA  	= (short*) data->mapIn[0]->c_ptr;
	ctl.dataInB  	= (short*) data->mapIn[1]->c_ptr;
    ctl.dataOut 	= (short*) data->mapOut[0]->c_ptr;

    ctl.input_width     = data->mapIn[0]->width;
    ctl.input_height    = data->mapIn[0]->height;
    ctl.input_num_maps  = data->mapIn[0]->num_maps;

    ctl.fscaleA    = (int )data->scaleIn[0];
    ctl.fscaleB    = (int )data->scaleIn[1];
    ctl.fscaleC    = (int )data->scaleOut[0];
    ctl.num_of_shifts    = data->params.pEvCaffeCorr1dParameter.num_of_shifts;
    ctl.stride           = data->params.pEvCaffeCorr1dParameter.stride;

    ctl.local_mem_ptr   = (unsigned char*) EVSS_CFG_VCCM_START;
    ctl.local_mem_size  = EVSS_CFG_VCCM_SIZE;
	//printf("local_mem_size =%d  \n", EVSS_CFG_VCCM_SIZE );
  

    int rescale;
    int rescale_out_bits;
    scale_fit(ctl.fscaleC / (ctl.fscaleA*ctl.fscaleB), 15, rescale, rescale_out_bits);
	ctl.scale  = rescale;
	ctl.scale_bits = rescale_out_bits; 
 
	void * args[] = {&ctl, output, inputA , inputB};
	callOclManualKernel(OCL_KERNEL(ev_inu_ocl_corr1d_kernel), args);

	return EV_LAYER_OK;
}

#else
ev_layer_status_e ev_my_scalar_corr1d(EvCnnDistributeUserDefined *data)
{

    EV_ALWAYS(data->numIn == 2);
    EV_ALWAYS(data->numOut == 1);


    int c1 = data->mapIn[0]->num_maps;
    int h1 = data->mapIn[0]->height;
    int w1 = data->mapIn[0]->width;
	
    int c2 = data->mapOut[0]->num_maps;
    int h2 = data->mapOut[0]->height;
    int w2 = data->mapOut[0]->width;

    //int max_displacement = 32;

    short* dataA = (short*) data->mapIn[0]->c_ptr;
    short* dataB = (short*) data->mapIn[1]->c_ptr;
    short* dataC = (short*) data->mapOut[0]->c_ptr;
	
	//data->params.pEvCaffeCorr1dParameter.num_of_shifts;  // should change to pEvCaffeMyCorrelationParameter after define it in caffe.proto
	//data->params.pEvCaffeCorr1dParameter.stride;

	float fscaleA = data->scaleIn[0];
	float fscaleB = data->scaleIn[1];
    float fscaleC = data->scaleOut[0];
	float fscale  = fscaleC / (fscaleA * fscaleB);

    int out_total_len = h2 * w2 * c2;
    memset(dataC, 0, out_total_len * sizeof (short));
    int sum_ch=0;

	for (int c=0; c<c2; c++) // c is the displacement to the right
	{
		for (int y=0; y<h2; y++)
		{
			for (int x=c; x<w2; x++)
			{
				sum_ch=0;
				for (int ch=0; ch<c1; ch++)
				{
					sum_ch += dataA[ch*h1*w1+ y*w1+x]*dataB[ch*h1*w1+ y*w1+x-c];
				}
				dataC[c*h2*w2 + y*w2 + x ] = (short)((float)sum_ch*fscale);
			}
		}
	}

    return EV_LAYER_OK;
}
#endif

extern "C" {

void evUserDefinedCorr1d(EvCnnDistributeDataBase *info)
{

    ev_layer_status_e status;

    EvCnnDistributeUserDefined * layer_info = (EvCnnDistributeUserDefined *) info;
#ifdef EVSS_CFG_HAS_OCL
    status = ev_ocl_correlate(layer_info); 
    //if (status != EV_LAYER_OK)
#else
    ev_my_scalar_corr1d(layer_info);
#endif
    return;

}

} // extern "C"
