/****************************************************************************
 *
 *   FileName: ev_ocl_InuScale.cl
 *
 *   Author:  Arviv B.
 *
 *   Date: 
 *
 *   Description: 
 *   
 ****************************************************************************/

#include "ev_ocl_ctrl.h"
#include "../ev_ocl_InuScale_interface.h"

#define  OCL_InuScale_SIMD_WIDTH 32
#define  N_LUT                   10
#define  Q_LUT                   14
#define  REDUCE2_LUT_SIZE (2*(1<<N_LUT)*1/2)         // in bytes

int Benny_evGetTimeInCycles(void);

//#define OCL_InuScale_CSM_PRINT_EN 1

#ifdef OCL_InuScale_CSM_PRINT_EN 
static volatile int *start_debug_buffer =(volatile int *)0x01000900;   // (volatile int *)0x010008a8
static volatile int *end_debug_buffer   =(volatile int *)(0x01000900+((2*1024+60*94)*sizeof(int)));
static volatile int *ocl_debug_ptr      =(volatile int *)0x01000900;

void ocl_debug_to_csm (unsigned int value)
{
        if(ocl_debug_ptr >= end_debug_buffer) {
          //ocl_debug_ptr  = start_debug_buffer;
        }
		else {
			*ocl_debug_ptr = value;
			ocl_debug_ptr++;
			*ocl_debug_ptr = 0xEEEEDDDD;
		}
}

#endif

// local ocl_InuScalel_buffer_t buffer[2]
typedef struct
{
	local short * input;
	local short * output;
    local short * local_reduce2_LUT_ptr;
	local int   * local_partial_sum_ptr;

	event_t load_event; 
	event_t store_event;

	int in_height;
	int in_width;
	int num_channels;
	int out_ch_offset; // hold the ch offset
	int num_out_channels;   // holds number of rows to transfer
} ocl_InuScalel_buffer_t;

// ***********************************************************************
void ocl_InuScale_init_buffer(
		local ocl_InuScalel_buffer_t* buffer,
		global ocl_ctl_InuScale_t* ctl,
		local short * input, local short * output,
		local short * l_reduce2_LUT_ptr,
        local int  * local_partial_sum_ptr	)
{
	buffer->input = input;
	buffer->output = output;
    buffer->local_reduce2_LUT_ptr =   l_reduce2_LUT_ptr;
	buffer->local_partial_sum_ptr =   local_partial_sum_ptr;

	buffer->in_width  			  = ctl->in_width;
	buffer->in_height  			  = ctl->in_height;
	buffer->num_channels		  = ctl->num_channels;

	buffer->store_event   = -1;
	buffer->out_ch_offset = 0;

}
// ***********************************************************************
void ocl_InuScale_load_LUT(local ocl_InuScalel_buffer_t* buffer, global short* g_lut_ptr, int lut_size_in_bytes)
{	
	// load from CSM to VMEM
	buffer->load_event =
        	async_work_group_copy(
					buffer->local_reduce2_LUT_ptr,         			// dst
					g_lut_ptr,                                      //src
					lut_size_in_bytes/2,	                        // size in shorts
					0);
}
// ***********************************************************************

void ocl_InuScale_load_buffer(local ocl_InuScalel_buffer_t* buffer, global short* input, int out_feature_offset, int num_out_channels)
{
	int size_shorts	= buffer->in_width*buffer->in_height*num_out_channels;
	int stride 		= buffer->in_width*buffer->in_height;
	
	buffer->load_event =
			async_work_group_copy(
					buffer->input,   							// dst
					&input[stride*out_feature_offset],  		//src
					size_shorts,	                            // size
					0);

	buffer->out_ch_offset = out_feature_offset;
	buffer->num_out_channels = num_out_channels;
}
// ***********************************************************************

void ocl_InuScale_load_buffer_wait(local ocl_InuScalel_buffer_t* buffer)
{
	if (buffer->load_event != -1){
		wait_group_events(1, &buffer->load_event);
		buffer->load_event = -1; // Reset buffer loading state
	}
	else
	{
		return;
	}
}
// ***********************************************************************
void ocl_InuScale_store_buffer(local ocl_InuScalel_buffer_t* buffer, global short* output)
{
	int size_shorts	= buffer->in_width*buffer->in_height*buffer->num_out_channels;
	int stride 		= buffer->in_width*buffer->in_height;
	
	if(buffer->out_ch_offset >= 0){
					
	buffer->store_event =
			async_work_group_copy(
					&output[buffer->out_ch_offset*stride], 				    // dst
					buffer->output,                                 		//src
					size_shorts,	                                  		// size
					0);						
	}
	else
	{
		buffer->store_event = -1;
	}
}
// ***********************************************************************

void ocl_InuScale_store_buffer_wait(local ocl_InuScalel_buffer_t* buffer)
{
	if (buffer->store_event != -1){
		wait_group_events(1, &buffer->store_event);
		// Reset buffer storing state
		buffer->store_event = -1;
	}
	else
	{
		return;
	}
}

// ***********************************************************************
void ocl_InuScale_process_16bit1(
		local short *output,
		local short *input,
		local short *reduce2_LUT,
		local int  *local_partial_sum_ptr,
		int in_height,
		int in_width,
		int num_channels,
        int num_out_channels,
		int out_ch_offset,
		local int * io_offsets,
		local short * map_idx
		)
{

	short32 idx  = vload32(0, &map_idx[0]);
	short32 feature_idx;

    int num_full_simd_width_iter    =  ((in_height*in_width)/OCL_InuScale_SIMD_WIDTH);
    int partial_simd_width_size     =   (in_height*in_width) - (num_full_simd_width_iter*OCL_InuScale_SIMD_WIDTH);
    local short * local_norm_fact_ptr = (local short *)local_partial_sum_ptr;

	short32		const_32_32   =     (short32) (32);
	short32		const_32_N_LUT =    (short32) N_LUT;
	short32     y;
	int16       partial_sum_l,partial_sum_h;
	int16       acc_vec_l,acc_vec_h;
	acc40x32    sq_acc; 
	acc40x16    sq_acc_l,sq_acc_h; 
	//acc40x32    acc_norm_sample; 
	short32     norm_val,temp_norm_val,norm_fact,lut_indexes,norm_shifts;
	acc40x16    acc0, acc1;
	int k;

   if (out_ch_offset==0)
   {
	   // ****************************** reset the local_partial_sum  buffer *************
	   int reset_inter_16 = in_width*in_height>>4;
	   int reset_inter_16_partial = in_width*in_height - (reset_inter_16<<4);
	   
	   int16		const_16_0   =     (int16) (0);
	   for (k=0;k<(reset_inter_16);k++)
	   {
			vstore16(const_16_0,0, &local_partial_sum_ptr[k*16]); 
	   }
		for (k=0;k<(reset_inter_16_partial);k++)
	   {
			local_partial_sum_ptr[reset_inter_16*16 +k]=0; 
	   }
   }

	// loop over full simd at a feature map including the partial 
	for (int full_iter_index=0; full_iter_index<(num_full_simd_width_iter+1) ; full_iter_index++)
	{
		feature_idx = 	idx < (short)OCL_InuScale_SIMD_WIDTH;           // initialization
		if ( full_iter_index == num_full_simd_width_iter )
		{   // partial simd width iteration
			feature_idx = idx < (short)partial_simd_width_size;
			if (partial_simd_width_size==0) break;
		}	

        // load 16 accumulator with the partial sum - int16 	
		partial_sum_l  = vload16(0, &local_partial_sum_ptr[full_iter_index*OCL_InuScale_SIMD_WIDTH]); 
		partial_sum_h  = vload16(0, &local_partial_sum_ptr[full_iter_index*OCL_InuScale_SIMD_WIDTH +16]); 

		// loop over the channels - to calc sum of square samples
       sq_acc = acc_init_acc40x32((short32) (0));
	   for (int map_index=0; map_index < num_out_channels ; map_index++)
		{ 
			int io_offset = map_index*(in_height*in_width) + full_iter_index*OCL_InuScale_SIMD_WIDTH;
			y         = vload32(0, &input[io_offset]);		
			acc_mac(&sq_acc, y, y);		// y without scaling	
		} // for (int map_index=0; map_index < num_out_channels ; map_index++)


		acc_vec_l = acc_convert_int16(acc_lo(sq_acc));
		acc_vec_h = acc_convert_int16(acc_hi(sq_acc));

        acc0 = acc_init_acc40x16((int16) (0));
		acc1 = acc_init_acc40x16((int16) (0));		
		acc_add(&acc0, acc_vec_l, partial_sum_l);
		acc_add(&acc1, acc_vec_h, partial_sum_h);


		// if not last channels tile			
		if ((out_ch_offset+num_out_channels)!=num_channels)
		{
			// store results at the local_partial_sum_ptr
			vstore16(acc_convert_int16(acc0),0, &local_partial_sum_ptr[full_iter_index*OCL_InuScale_SIMD_WIDTH]); 
			vstore16(acc_convert_int16(acc1),0, &local_partial_sum_ptr[full_iter_index*OCL_InuScale_SIMD_WIDTH+16]); 			
		
		}
		else
		{  // last channels tile
			
		
        // 1 / sqrt	
			acc_sqrt(&acc0);
			acc_sqrt(&acc1);
	
			// calculate norm_factor   - we want to find min N where sum_sq_out < 2^(N)
			// pay attention that the sqrt(sq_acc) is without scaling & the sample after that without scaling
			norm_shifts.lo        = convert_short16(clz(acc_convert_int16_sat(acc0)));             
			norm_shifts.hi        = convert_short16(clz(acc_convert_int16_sat(acc1)));
		
			// we want to find min N where sum_sq < 2^(N)
			norm_val = const_32_32 - norm_shifts;  				    // N = minimum number of number bits that contain the number
			temp_norm_val = norm_val - const_32_N_LUT;     			// temp_norm_val=(N-N_LUT)
		
			lut_indexes.lo =  convert_short16(acc_convert_int16(acc0) >> temp_norm_val.lo);
			lut_indexes.hi =  convert_short16(acc_convert_int16(acc1) >> temp_norm_val.hi);
		
			//lut_indexes = sum_sq>>(temp_norm_val);              //lut_index=floor(sum_sq_out/2^(N-Nlut));
			lut_indexes =lut_indexes - (short32)((1<<N_LUT)/2); //lut_index=floor(sum_sq_out/2^(N-Nlut)) - LutSize/2;

			//norm_factor = LUT(lut_index)/2^N;
			norm_fact= vgather32(&reduce2_LUT[0], convert_int16(lut_indexes.lo), convert_int16(lut_indexes.hi), feature_idx);
			norm_fact= norm_fact>>norm_val;
		
			// store results OF norm_fact at the local_norm_fact_ptr AS SHORTS
			vstore32(norm_fact,0, &local_norm_fact_ptr[full_iter_index*OCL_InuScale_SIMD_WIDTH]); 
		}
		
	}

}
// ***********************************************************************
void ocl_InuScale_process_16bit(
		local short *output,
		local short *input,
		local int   *local_partial_sum_ptr,
		int in_height,
		int in_width,
		int num_channels,
        int num_out_channels,
		int out_ch_offset,
		local int * io_offsets,
		local short * map_idx
		)	
{

	int16 io_offsets0 = vload16(0, &io_offsets[0 ]);
	int16 io_offsets1 = vload16(0, &io_offsets[16]);

	short32 idx  = vload32(0, &map_idx[0]);
	short32 feature_idx;

    int num_full_simd_width_iter    =  ((in_height*in_width)/OCL_InuScale_SIMD_WIDTH);
    int partial_simd_width_size     =   (in_height*in_width) - (num_full_simd_width_iter*OCL_InuScale_SIMD_WIDTH);
	local short * local_norm_fact_ptr = (local short *)local_partial_sum_ptr;
	
	short32     y,out_norm;
	acc40x32    sq_acc; 
	acc40x32    acc_norm_sample; 
	short32     norm_val,temp_norm_val,norm_fact,lut_indexes,norm_shifts;
	acc40x16    acc0, acc1;


	// loop over full simd at a feature map including the partial 
	for (int full_iter_index=0; full_iter_index<(num_full_simd_width_iter+1) ; full_iter_index++)
	{
		feature_idx = 	idx < (short)OCL_InuScale_SIMD_WIDTH;           // initialization
		if ( full_iter_index == num_full_simd_width_iter )
		{   // partial simd width iteration
			feature_idx = idx < (short)partial_simd_width_size;
			if (partial_simd_width_size==0) break;
		}		
	
        // norm_fact = 2^Q_LUT/sqrt(..) without scaling
		norm_fact         = vload32(0, &local_norm_fact_ptr[full_iter_index*OCL_InuScale_SIMD_WIDTH]);
		
		// loop over the channels - for normalization (sample*norm_fact/2^Q_LUT;)
		for (int map_index=0; map_index < num_out_channels ; map_index++)
		{
		    int io_offset = map_index*(in_height*in_width) + full_iter_index*OCL_InuScale_SIMD_WIDTH;
			y         = vload32(0, &input[io_offset]);
		
			// out_norm  = y*norm_fact>>(Q_LUT-12);	 - output scale 4095
			acc_mpy(&acc_norm_sample,y, norm_fact);
	        out_norm.lo =  convert_short16(acc_convert_int16(acc_lo(acc_norm_sample)) >> (Q_LUT-12));
	        out_norm.hi =  convert_short16(acc_convert_int16(acc_hi(acc_norm_sample)) >> (Q_LUT-12));
			vscatter32(out_norm, &output[io_offset], io_offsets0, io_offsets1, feature_idx);			
		}		

	}

}
// ***********************************************************************
void ocl_InuScale_process_buffer(ocl_InuScalel_buffer_t *buffer, local int * io_offsets, local short* map_idx)
{
	ocl_InuScale_process_16bit(
			buffer->output,
			buffer->input,
			buffer->local_partial_sum_ptr,
			buffer->in_height,
			buffer->in_width,
			buffer->num_channels,
			buffer->num_out_channels,
			buffer->out_ch_offset,
			io_offsets,
			map_idx);
}
// ***********************************************************************
void ocl_InuScale_process_buffer1(ocl_InuScalel_buffer_t *buffer, local int * io_offsets, local short* map_idx)
{
	ocl_InuScale_process_16bit1(
			buffer->output,
			buffer->input,
			buffer->local_reduce2_LUT_ptr,
			buffer->local_partial_sum_ptr,
			buffer->in_height,
			buffer->in_width,
			buffer->num_channels,
			buffer->num_out_channels,
			buffer->out_ch_offset,
			io_offsets,
			map_idx);
}
// ***********************************************************************
__attribute__(( reqd_work_group_size(1, 1, 1)))


kernel void ev_ocl_InuScale_kernel(	global ocl_ctl_InuScale_t  * ctl,
                    				global short * restrict out,
									global short * restrict in
									)
{
	local short *output [2];
	local short *input  [2];

	local int   io_offsets[OCL_InuScale_SIMD_WIDTH]; // OCL_InuScale_SIMD_WIDTH=32
	local short map_idx[OCL_InuScale_SIMD_WIDTH];

	int 	z = 0, channels;
	int 	buf;
	local   ocl_InuScalel_buffer_t buffer[2];             // new struct  
	int     channels_remaining = ctl->num_channels;       // num ch

    int     partial_sum_buffer_size = (ctl->in_height*ctl->in_width*sizeof(int) +OCL_InuScale_SIMD_WIDTH*sizeof(int));
	int local_mem_size = (ctl->local_mem_size - 4096 - REDUCE2_LUT_SIZE - partial_sum_buffer_size) >> 2;
	
	int max_channels_tile = local_mem_size / ((ctl->in_width)*(ctl->in_height)*sizeof(short));  

	
	// The INPUT & OUTPUT are at dimension of shorts
	max_channels_tile = min(max_channels_tile, ctl->num_channels);  		   // max_channels_tile = num channels at tile
	int input_tile_size_inshort  = max_channels_tile * ctl->in_width*ctl->in_height;
	int output_tile_size_inshort = max_channels_tile * ctl->in_width*ctl->in_height;


    // ctl->reduce2_LUT pointer to the round(2^14./sqrt(t)) LUT
	local short * local_reduce2_LUT_ptr = (local short*)ctl->local_mem_ptr;
	local int   * local_partial_sum_ptr = (local int *)local_reduce2_LUT_ptr + (REDUCE2_LUT_SIZE/sizeof(int));   // the first 2*768 are for reduce2 LUT
	local short * local_mem_ptr = (local short*)local_partial_sum_ptr + (partial_sum_buffer_size/sizeof(short));   
	global short * global_lut_ptr = (global short*)ctl->reduce2_LUT;          
	
	for ( buf = 0; buf < 2; buf ++)
	{                                     
		input[buf] = local_mem_ptr;
		local_mem_ptr += input_tile_size_inshort;
		output[buf] =  local_mem_ptr;
		local_mem_ptr += output_tile_size_inshort;
	}
	
	buf = 1; 
	ocl_InuScale_init_buffer(&buffer[0],ctl,input[0],output[0],local_reduce2_LUT_ptr,local_partial_sum_ptr);
	ocl_InuScale_init_buffer(&buffer[1],ctl,input[1],output[1],local_reduce2_LUT_ptr,local_partial_sum_ptr);

#ifdef OCL_InuScale_CSM_PRINT_EN
	ocl_debug_to_csm (0xbebebebe);
	ocl_debug_to_csm ((int)in);
	ocl_debug_to_csm ((int)out);
	ocl_debug_to_csm (local_mem_size);
	ocl_debug_to_csm (ctl->in_width);
	ocl_debug_to_csm (ctl->num_channels);
	ocl_debug_to_csm (ctl->in_height);
	ocl_debug_to_csm (max_channels_tile);
	ocl_debug_to_csm (input_tile_size_inshort);
	ocl_debug_to_csm (partial_sum_buffer_size);
	ocl_debug_to_csm ((int)buffer[0].input);
	ocl_debug_to_csm ((int)buffer[0].output);
	ocl_debug_to_csm ((int)buffer[1].input);
	ocl_debug_to_csm ((int)buffer[1].output);
#endif


    // load reduce2 LUT to VDSP mem
	ocl_InuScale_load_LUT(&buffer[1-buf],global_lut_ptr, REDUCE2_LUT_SIZE); // REDUCE2_LUT_SIZE in bytes

	channels = min(channels_remaining, max_channels_tile);               
	ocl_InuScale_load_buffer(&buffer[buf], in, z, channels); // z holds the out_maps_offset, channels=number of ch to load

		
	// Init map offsets for gather/scatter
	for (int lane = 0, in_feature_offset = 0, out_feature_offset = 0; lane < OCL_InuScale_SIMD_WIDTH; lane++, in_feature_offset += 1, out_feature_offset += 1)
	{
		io_offsets[lane] = out_feature_offset; // int
		map_idx[lane] = lane;                  // short
	}

    ocl_InuScale_load_buffer_wait(&buffer[1-buf]);      //wait for LUT loading
	channels_remaining -= channels;  
	z += channels;


    // loop for calculating the normalize factor
	while  (channels_remaining > 0)
	{
		channels = min(channels_remaining, max_channels_tile);

		ocl_InuScale_load_buffer(&buffer[1 - buf], in, z, channels);

		channels_remaining -= channels;
		z += channels;

		ocl_InuScale_load_buffer_wait(&buffer[buf]);
		ocl_InuScale_process_buffer1(&buffer[buf], io_offsets, map_idx); 

		buf = 1 - buf;
	}
	ocl_InuScale_load_buffer_wait(&buffer[buf]);
	ocl_InuScale_process_buffer1(&buffer[buf], io_offsets, map_idx);


   // start loopp for normalization
   // *******************************

	z = 0;
	buf=1;
	channels_remaining = ctl->num_channels;       // num ch

    channels = min(channels_remaining, max_channels_tile);               
	ocl_InuScale_load_buffer(&buffer[buf], in, z, channels); // z holds the out_maps_offset, channels=number of ch to load
	channels_remaining -= channels;  
	z += channels;

	while  (channels_remaining > 0)
	{
		channels = min(channels_remaining, max_channels_tile);
		ocl_InuScale_load_buffer(&buffer[1 - buf], in, z, channels);
		channels_remaining -= channels;
		z += channels;

		ocl_InuScale_store_buffer_wait(&buffer[buf]);
		ocl_InuScale_load_buffer_wait(&buffer[buf]);
		ocl_InuScale_process_buffer(&buffer[buf], io_offsets, map_idx); 
		ocl_InuScale_store_buffer(&buffer[buf], out);

		buf = 1 - buf;
	}
	ocl_InuScale_load_buffer_wait(&buffer[buf]);
	ocl_InuScale_store_buffer_wait(&buffer[buf]);
	ocl_InuScale_process_buffer(&buffer[buf], io_offsets, map_idx);
	ocl_InuScale_store_buffer(&buffer[buf], out);
	ocl_InuScale_store_buffer_wait(&buffer[buf]);

}


