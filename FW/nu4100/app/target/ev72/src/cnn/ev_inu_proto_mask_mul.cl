/****************************************************************************
 *
 *   FileName: ev_inu_resize_planar.cl
 *
 *   Author:  Arviv B.
 *
 *   Date: 
 *
 *   Description:  
 *   
 ****************************************************************************/

#include "stu.h"
#include "inu_proto_mask_mul.h"

#if ( __Xvec_guard_bit_option == 0 )
#define acc_convert_int16(acc)  acc
#endif

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

#define AUX_RTC_LOW                    (0x104)
#define AUX_RTC_HIGH                   (0x105)
#define MAX_GAUSSIAN_WIDTH				1024

#define MAX_IN_CHANNELS					64

//// ***********************************************************************
#define      STEP_X 32
//#define      DBG_LINE   523
//#define      DBG_COL_S  416
// ***********************************************************************

typedef struct
{
	global float * g_in_proto;
	global float * g_in_mask;
	global float * g_out_res;
	
	local half          * l_sigmoid_lut;  
	local float         * in_proto;  
	local float         * in_mask;  
	local float 		* out_res;	
	int   max_num_pixels_per_ch;

	int 	in_height;  		// input height	
 	int 	in_width;  		    // input width	
	int 	in_channels;  		// num input channels	
	int     num_elements;
	
	int 	tile_num;
	int     first_pixel_at_ch;
	int     num_pixels_at_tile;
	
	event_t       load_event;
	event_t       store_event;	
} ocl_proto_mask_buffer_t;
// ***********************************************************************
void ocl_proto_load_buffer(local ocl_proto_mask_buffer_t  * buffer,	int  start_pixel2load, int num_pixels2load ,int tile_num)
{

    buffer->first_pixel_at_ch	= start_pixel2load;
    buffer->num_pixels_at_tile	= num_pixels2load;
    buffer->tile_num			= tile_num;
	int     num_in_channels 	= buffer->in_channels;

	int gin_offset     = num_in_channels*start_pixel2load;										   
	buffer->load_event =  async_work_group_copy2D(
												   buffer->in_proto,					    		// dst
												   num_in_channels*sizeof(float),					// dst width
												   (num_in_channels+1)*sizeof(float),			    // dst stride
												   &buffer->g_in_proto[gin_offset],		    		// src
												   num_pixels2load*sizeof(float), 					// src width
												   num_pixels2load*sizeof(float),	       			// src stride
												   num_pixels2load*num_in_channels*sizeof(float),	// size
												   0);
#ifdef OCL_PROTO_MASK_PRINT_EN_
	if (buffer->tile_num ==0) {
	float16 tt0 = vload16(0, &buffer->in_proto[0]);
	float16 tt5 = vload16(0, &buffer->in_proto[5*33]);
	printf(" tt0 = %#v16hf\n",  tt0);
	printf(" tt5 = %#v16hf\n",  tt5);
	}	
#endif	
}

// ***********************************************************************
__attribute__(( reqd_work_group_size(1, 1, 1)))
__kernel void ocl_proto_process_buffer(local ocl_proto_mask_buffer_t  * buffer)
{

	//Get kernel parameters from the control structure
	int in_channels 	= buffer->in_channels;
	int num_elements	= buffer->num_elements;
	int num_pixels_at_tile   = buffer->num_pixels_at_tile;
	int pixel_spacing 	= in_channels+1;

    int16  pixel_offsets_lo = (int16)( 	( 0*pixel_spacing),
										( 1*pixel_spacing),
										( 2*pixel_spacing),
										( 3*pixel_spacing),
										( 4*pixel_spacing),
										( 5*pixel_spacing),
										( 6*pixel_spacing),
										( 7*pixel_spacing),
										( 8*pixel_spacing),
										( 9*pixel_spacing),
										(10*pixel_spacing),
										(11*pixel_spacing),
										(12*pixel_spacing),
										(13*pixel_spacing),
										(14*pixel_spacing),
										(15*pixel_spacing));
	int16  pixel_offsets_hi = pixel_offsets_lo + (int16)(16*pixel_spacing);

    int16  out_offsets_lo = (int16)( 	( 0*num_elements),
										( 1*num_elements),
										( 2*num_elements),
										( 3*num_elements),
										( 4*num_elements),
										( 5*num_elements),
										( 6*num_elements),
										( 7*num_elements),
										( 8*num_elements),
										( 9*num_elements),
										(10*num_elements),
										(11*num_elements),
										(12*num_elements),
										(13*num_elements),
										(14*num_elements),
										(15*num_elements));
	int16  out_offsets_hi = out_offsets_lo + (int16)(16*num_elements);
	
	local float  * proto_in_ptr = (local float *) buffer->in_proto;
	for (int pixel=0; pixel<num_pixels_at_tile; pixel+=32)
	{	
		int proto_pixel_offset = pixel*pixel_spacing;
		for(int elem=0; elem<num_elements;elem++)
		{
			half32 sum = 0;
			for (int ch=0; ch<in_channels; ch++)
			{
				int proto_in_offset = proto_pixel_offset + ch;
				float16 proto_data0 = vgather16( &proto_in_ptr[proto_in_offset] ,  pixel_offsets_lo);
				float16 proto_data1 = vgather16( &proto_in_ptr[proto_in_offset] ,  pixel_offsets_hi);
				half32 proto_data;
				proto_data.lo = convert_half16(proto_data0);
				proto_data.hi = convert_half16(proto_data1);

				half   mask_data   = (half)(buffer->in_mask[elem*in_channels + ch]);
				sum     +=  proto_data*mask_data;
			}
#ifdef DO_LOGISTIC
			// clamp sum into range  min_x to max_x
			sum.lo  = clamp(sum.lo,(half16)SIGMOID_MIN_X,(half16)SIGMOID_MAX_X);
			sum.hi  = clamp(sum.hi,(half16)SIGMOID_MIN_X,(half16)SIGMOID_MAX_X);
			
			// do sigmoid , logistic regration with LUT
			sum = (sum - (half32)SIGMOID_MIN_X)*(half32)SIGMOID_LUT_RESOLUTION;
	
			int16 lut_indx_lo = convert_int16(sum.lo);
			int16 lut_indx_hi = convert_int16(sum.hi);

			half32   lut_out = vgather32(buffer->l_sigmoid_lut, lut_indx_lo,lut_indx_hi);
			vscatter16(convert_float16(lut_out.lo), (local float *)&buffer->out_res[pixel*num_elements + elem],out_offsets_lo);
			vscatter16(convert_float16(lut_out.hi), (local float *)&buffer->out_res[pixel*num_elements + elem],out_offsets_hi);
#else
			float16  sum_lo = convert_float16(sum.lo);
			float16  sum_hi = convert_float16(sum.hi);
			vscatter16(sum_lo, (local float *)&buffer->out_res[pixel*num_elements + elem],out_offsets_lo);
			vscatter16(sum_hi, (local float *)&buffer->out_res[pixel*num_elements + elem],out_offsets_hi);
#endif
		}		
	}
}
// ***********************************************************************
__attribute__(( reqd_work_group_size(1, 1, 1)))
__kernel void ocl_proto_store_buffer(local ocl_proto_mask_buffer_t  * buffer)
{
    int  num_pixels2store = buffer->num_pixels_at_tile * buffer->num_elements;
	int  out_offset       = buffer->first_pixel_at_ch  * buffer->num_elements;   

#ifdef OCL_PROTO_MASK_PRINT_EN_
	if (buffer->tile_num==0){
		float16 tt0 = vload16(0, &buffer->out_res[0]);
		float16 tt1 = vload16(1, &buffer->out_res[0]);
	
		printf(" out_offset=%d \n",  out_offset);
		printf(" num_pixels2store=%d \n",  num_pixels2store);
		printf(" tt0        = %#v16hf\n",  tt0);
		printf(" tt1        = %#v16hf\n",  tt1);
	}			
#endif
	buffer->store_event = async_work_group_copy(&buffer->g_out_res[out_offset],  &buffer->out_res[0], num_pixels2store,   0);	
}
// ***********************************************************************
__attribute__(( reqd_work_group_size(1, 1, 1)))
kernel void ev_inu_ocl_proto_mask_mul_kernel(   global ocl_ctl_proto_mask_mul  	* ctl,
										        global float 					* restrict g_dataOut,
										        global float        			* restrict g_proto_data ,
										        global float        			* restrict g_mask_data )
{

	//float 	split_th 			= ctl->split_th;  
	//bool	is_first_section 	= ctl->is_first_section;

	int in_H 		= ctl->in_height;
	int in_W 		= ctl->in_width;
	int in_C 		= ctl->in_channels;
	int num_elem 	= ctl->num_elements;

	int var_space_size = ctl->local_mem_size - (4*1024) -(SIGMOID_LUT_SIZE*sizeof(half)); 
	
	
	// assumption, in_C <=64  not big number , usually=32
	int pixel_spacing       = in_C+1;
	int max_elements_per_ch = (var_space_size - num_elem*in_C*sizeof(float))/(pixel_spacing*sizeof(float) + num_elem*sizeof(float));

	// proto at HWC, mask at HW and output at HWC
	int in_ch_size =  in_H*in_W;
	int max_num_pixels_per_ch = min(max_elements_per_ch,in_ch_size);
	    max_num_pixels_per_ch = max_num_pixels_per_ch & (~0x1f);
	int num_tiles = ( in_ch_size + max_num_pixels_per_ch-1)/max_num_pixels_per_ch;

  	local float  *	in_ptr;	 
  	local float  *	out_ptr;                     // output interleaved
  	local float  *	mask_in_ptr;              
  	local unsigned char  * local_mem_ptr 	= (local unsigned char *)ctl->local_mem_ptr;

	// load sigmoid LUT at VMEM start -bring the LUT at float format, to tmp mem and convert it to half into the buffer
	//------------------------------------------------------------------------------------------------------------------
	local half  *	l_lut = (local half *)ctl->local_mem_ptr;
	local_mem_ptr  +=  ((SIGMOID_LUT_SIZE+1)&(~1))*sizeof(half);
	
#ifdef DO_LOGISTIC
	//local float  *	l_float_lut = (local float *)ctl->local_mem_ptr;
	event_t load_lut_event = 0;
	load_lut_event = async_work_group_copy(l_lut,  (global half *)ctl->sigmoid_lut , SIGMOID_LUT_SIZE ,   0);
	wait_group_events(1, &load_lut_event);
#endif

	in_ptr      	=  (local float  *	)local_mem_ptr;  
	local_mem_ptr  +=  max_num_pixels_per_ch*pixel_spacing*sizeof(float);
	mask_in_ptr	    =  (local float  *	)local_mem_ptr;
	local_mem_ptr  +=  in_C*num_elem*sizeof(float);
	out_ptr 	    =  (local float  *	)local_mem_ptr;
	local_mem_ptr  +=  max_num_pixels_per_ch*num_elem*sizeof(float);
	
#ifdef OCL_PROTO_MASK_PRINT_EN_
	printf("INFO: EVSS_CFG_VCCM_SIZE=%i \n", ctl->local_mem_size);
	printf("INFO: g_out_ptr      at %p\n", g_dataOut);
	printf("INFO: g_proto_data   at %p\n", g_proto_data);
	printf("INFO: g_mask_data    at %p\n", g_mask_data);
	printf("INFO: proto size  [H,W,C] = [%d ,%d,%d] \n", in_H, in_W,in_C); 
	printf("INFO: mask  size  [H,W]   = [%d ,%d] \n", num_elem,in_C);

	printf("INFO: var_space_size = %d  \n", var_space_size );
	printf("INFO: max_num_pixels_per_ch  = %d  \n", max_num_pixels_per_ch );
	printf("INFO: num_tiles              = %d  \n", num_tiles );
	
	printf("INFO: vmem in_ptr      at %p\n",in_ptr);		
	printf("INFO: vmem mask_in_ptr at %p\n",mask_in_ptr);		
 	printf("INFO: vmem out_ptr     at %p\n",out_ptr);		
  	printf("INFO: end buffers allocation %p\n",  local_mem_ptr);
  	printf("INFO: vmem for stack and variables =%d \n",  ((int)ctl->local_mem_ptr+ ctl->local_mem_size-(int)local_mem_ptr));
#endif

    local ocl_proto_mask_buffer_t buffer;
	//  init buffers
	// --------------
	// global buffers
	buffer.g_in_proto	    = (global float * )g_proto_data;
	buffer.g_in_mask	    = (global float * )g_mask_data;
	buffer.g_out_res	    = (global float * )g_dataOut;
	buffer.l_sigmoid_lut    = (local  half  * )l_lut;

	buffer.in_proto =  in_ptr;  
	buffer.in_mask 	=  mask_in_ptr;
	buffer.out_res 	=  out_ptr;
	
	buffer.max_num_pixels_per_ch=  max_num_pixels_per_ch;
	buffer.in_height=  in_H;
	buffer.in_width =  in_W;
	buffer.in_channels=  in_C;
	buffer.num_elements=  num_elem;
	buffer.load_event		= -1;
	buffer.store_event     = -1;


	// load mask matrix once
	event_t load_mask_event = 0;
	load_mask_event = async_work_group_copy(mask_in_ptr, g_mask_data , num_elem*in_C ,   0);
	wait_group_events(1, &load_mask_event);
	
	int num_remainder_pixels = in_ch_size;
		
	for (int tile_num=0; tile_num<num_tiles; tile_num++) {

		int num_pixels2load   = min(max_num_pixels_per_ch,num_remainder_pixels);
		int start_pixel2load = (tile_num)*max_num_pixels_per_ch;
		ocl_proto_load_buffer(&buffer,	start_pixel2load,num_pixels2load ,tile_num ); 
		num_remainder_pixels = num_remainder_pixels - num_pixels2load;

		//wait to tile at curr_buff, to loading end
		wait_group_events(1, &buffer.load_event);

		// process tile
		ocl_proto_process_buffer(&buffer);

		// store tile
		ocl_proto_store_buffer(&buffer);
	
		// store wait
		wait_group_events(1, &buffer.store_event);
	}
}
// ***********************************************************************
