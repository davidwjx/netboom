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
#include "inu_reshape_chw2hwc.h"

#define  OCL_bokeh_SIMD_WIDTH 32

#if ( __Xvec_guard_bit_option == 0 )
#define acc_convert_int16(acc)  acc
#endif

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

void evYieldThread(void);

#define AUX_RTC_LOW                    (0x104)
#define AUX_RTC_HIGH                   (0x105)

#define MAX_IN_CHANNELS					32
#define OUTPUT_Q    					10
#define TANH_LUT_SIZE                   2049

// ***********************************************************************
//#define      DBG_LINE   523
//#define      DBG_COL_S  416
// ***********************************************************************

typedef struct
{
	global short * g_in_ptr;
	global short * g_out_ptr;
	
	local short          * tanh_lut;  
	local short          * in_ptr[MAX_IN_CHANNELS];  
	local short 		 * out_ptr;	
	int   in_buffers_spacing;
	int   max_in_C_per_iter;
	int   channels_iter;          // iteration number
	int   num_channels_at_tile;
	
	int 	tile_num;
	int 	in_height;  		// input height	
 	int 	in_width;  		    // input width	
	int 	in_channels;  		// num input channels	
	
	int     first_pixel_at_ch;
	int     num_pixels_at_tile;
	int     max_num_pixels_per_ch;
	
	event_t       load_event;
	event_t       store_event;
} ocl_reshape_buffer_t;
// ***********************************************************************
void ocl_reshape_load_buffer(local ocl_reshape_buffer_t  * buffer,	int  start_pixel2load, int num_pixels2load ,int tile_num)
{

    buffer->first_pixel_at_ch	= start_pixel2load;
    buffer->num_pixels_at_tile	= num_pixels2load;
    buffer->tile_num	= tile_num;
	int     num_in_channels = buffer->num_channels_at_tile;
	int     in_ch_stride    = buffer->in_height * buffer->in_width;
	int     local_channel_spacing = buffer->in_buffers_spacing;
	int     src_offset   = (buffer->channels_iter*buffer->max_in_C_per_iter*in_ch_stride) + start_pixel2load;

	//------------------------------------------------------------------
	buffer->load_event =  async_work_group_copy2D(
												   buffer->in_ptr[0],					    		// dst
												   num_pixels2load*sizeof(short),					// dst width
												   local_channel_spacing*sizeof(short),			    // dst stride
												   &buffer->g_in_ptr[src_offset],		    		// src
												   num_pixels2load*sizeof(short), 					// src width
												   in_ch_stride*sizeof(short),	        			// src stride
												   num_pixels2load*num_in_channels*sizeof(short),	// size
												   0);


	//--------------------------------------------------------------------
	//for (int ch=0; ch<num_in_channels; ch++)
	//{
	//	int gin_offset     = in_ch_stride*ch + start_pixel2load;
    //    // printf("load: ch=%d , num_pixels2load = %d \n", ch, num_pixels2load);		
	//	buffer->load_event = async_work_group_copy(buffer->in_ptr[ch],  &buffer->g_in_ptr[gin_offset], num_pixels2load,   buffer->load_event);
	//}
}
// ************************************************************************
__attribute__(( reqd_work_group_size(1, 1, 1)))
__kernel void ocl_reshape_chw2hwc_process_buffer(local ocl_reshape_buffer_t  * buffer,
										        int one_over_scale_factor , int one_over_scale_shift_bits)
{

	int num_ch    			= buffer->max_in_C_per_iter;
	int in_buffers_spacing 	= buffer->in_buffers_spacing;
 
    int16   vix[2];  //indexes for vgather32
	short32  ch_idx = (short32)( 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31);
	vix[0]  = (int16)( 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15)*in_buffers_spacing;
	vix[1]  = (int16)(16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31)*in_buffers_spacing;
	local short * in_ptr0 = buffer->in_ptr[0];
	local short * out_ptr = buffer->out_ptr;
	short32  valid_mask;
#ifdef DO_SCALING
	int factor = one_over_scale_factor;
	int num_shifts = one_over_scale_shift_bits - OUTPUT_Q;
#endif
    int num_vec32_over_ch = (num_ch + 31)>>5;

	for (int pixel_num=0; pixel_num<buffer->num_pixels_at_tile; pixel_num++)
	{
		int num_remainning_ch = num_ch;
		for (int vec_num=0; vec_num<num_vec32_over_ch; vec_num++)
		{
		    valid_mask    = ch_idx < (short32)(num_remainning_ch);
			num_remainning_ch = num_remainning_ch - 32;
			int strat_ch  = (vec_num*32);
			short32 data  = vgather32(&in_ptr0[strat_ch*in_buffers_spacing+pixel_num], vix[0], vix[1] );	
#ifdef DO_SCALING	
			// scale data
			int16 data16_lo = (convert_int16(data.lo) * (int16)factor) >> num_shifts;
			int16 data16_hi = (convert_int16(data.hi) * (int16)factor) >> num_shifts;
			data.lo         = convert_short16( data16_lo);
			data.hi         = convert_short16( data16_hi);

			//if ((buffer->tile_num == 0) && (pixel_num==0))
			//{
			//  //printf(" data16_lo = %#v16hd\n",  data16_lo);
			//  //printf(" data16_hi = %#v16hd\n",  data16_hi);
			//  printf(" data = %#v32hd\n",  data);
			//}
#endif			 
			vstore32_mask( data,0, (local short *)&out_ptr[strat_ch +pixel_num*num_ch] , convert_short32(valid_mask) );
		
		}
	}
}
// ***********************************************************************
__attribute__(( reqd_work_group_size(1, 1, 1)))
__kernel void ocl_reshape_store_buffer(local ocl_reshape_buffer_t  * buffer )
{
    int  dst_stride  = buffer->in_channels;
	int  out_offset  = buffer->tile_num * buffer->max_num_pixels_per_ch * buffer->in_channels + \
	                   buffer->channels_iter*buffer->max_in_C_per_iter;
	int  num_pixels2store = buffer->num_channels_at_tile*buffer->num_pixels_at_tile;

	//------------------------------------------------------------------
	buffer->store_event =  async_work_group_copy2D(
												   &buffer->g_out_ptr[out_offset],		    		// dst
												   buffer->num_channels_at_tile*sizeof(short),		// dst width
												   dst_stride*sizeof(short),			    		// dst stride
												   buffer->out_ptr,		    					// src
												   buffer->num_channels_at_tile*sizeof(short), 		// src width
												   buffer->max_in_C_per_iter*sizeof(short),		// src stride
												   num_pixels2store*sizeof(short),	// size
												   0);

//if ( buffer->tile_num == 0) 
//{
//  short32 data = vload32( 0, (local short *)buffer->out_ptr );
//  short32 data1 = vload32( 1, (local short *)buffer->out_ptr );
//  printf("store: data = %#v32hd\n",  data);
//  printf("store: data1 = %#v32hd\n",  data1);
//}

	//--------------------------------------------------------------------
    //int  num_pixels2store = buffer->num_pixels_at_tile * buffer->in_channels;
	//int  out_offset  = buffer->tile_num * buffer->max_num_pixels_per_ch * buffer->in_channels;
    // printf("store: num_pixels2store = %d \n", num_pixels2store);		

	//buffer->store_event = async_work_group_copy(&buffer->g_out_ptr[out_offset],  &buffer->out_ptr[0], num_pixels2store,   0);	
}
// ***********************************************************************
__attribute__(( reqd_work_group_size(1, 1, 1)))
kernel void ev_inu_ocl_reshape_chw2hwc_scale_kernel(	global ocl_ctl_reshape_chw2hwc  	* ctl,
										                global short 			            * restrict g_dataOut,
										                global short 			            * restrict g_dataIn )
{
	float 	split_th 			= ctl->split_th;  
	bool	is_first_section 	= ctl->is_first_section;

	int in_H = ctl->in_height;
	int in_W = ctl->in_width;
	int in_C = ctl->in_channels;

	int sec_num_channels;	
	int first_sec_num_channels = (int)((float)in_C * split_th +31)&(~31);
	    first_sec_num_channels = min(in_C ,first_sec_num_channels);
	int channels_iter_offset;  
	
	if (is_first_section) {
	    sec_num_channels = first_sec_num_channels;
	    channels_iter_offset = 0;  
	}
	else {
	    sec_num_channels = in_C - first_sec_num_channels;
	    channels_iter_offset = (first_sec_num_channels + MAX_IN_CHANNELS-1)/MAX_IN_CHANNELS;  
	}
	int num_ch_iter = ( sec_num_channels + MAX_IN_CHANNELS-1)/MAX_IN_CHANNELS;
		
  	local short  *	in_ptr[MAX_IN_CHANNELS];	 // each ptr for a channel	
  	local short  *	out_ptr;     // output interleaved

	int max_in_C_per_iter = min(sec_num_channels ,MAX_IN_CHANNELS) ;
	
	int var_space_size = ctl->local_mem_size - (4*1024) -(TANH_LUT_SIZE*sizeof(short));
	int buffers_spacing;
	if (max_in_C_per_iter > 0) {
		buffers_spacing = var_space_size/(2*max_in_C_per_iter*sizeof(short)); 	//2 for input and output , 2 for double buffers, in_C num of buffers
	}
	else {
		buffers_spacing = var_space_size/(2*1*sizeof(short)); 	//2 for input and output , 2 for double buffers, in_C num of buffers
	}

	buffers_spacing = (buffers_spacing&(~0x1f)) + 1;  // in shorts

	int max_num_pixels_per_ch = min((buffers_spacing)&(~0x1f),in_W*in_H);
  	local unsigned char  * local_mem_ptr 	= (local unsigned char *)ctl->local_mem_ptr;

	// load tanh LUT to start of VMEM
	//event_t load_lut_event = 0;
	//local short  *	l_tanh_lut = (local short *)ctl->local_mem_ptr;
	//load_lut_event = async_work_group_copy((local short *)ctl->local_mem_ptr,  (global short *)ctl->tanh_Q10_lut , TANH_LUT_SIZE ,   0);
	local_mem_ptr  +=  TANH_LUT_SIZE*sizeof(short);
	
	
	
	for( int input_ch =0; input_ch<max_in_C_per_iter; input_ch++)
	{
		in_ptr[input_ch] 	=  (local short  *	)local_mem_ptr;  
		local_mem_ptr  +=  buffers_spacing*sizeof(short);
	}
	out_ptr 	=  (local short  *	)local_mem_ptr;
	local_mem_ptr  +=   buffers_spacing*sizeof(short)*max_in_C_per_iter;

	int in_ch_stride =  in_H*in_W;
	int num_tiles = ( in_ch_stride + max_num_pixels_per_ch-1)/max_num_pixels_per_ch;

#ifdef OCL_reshape_PRINT_EN_DEBUG
	printf("INFO: global input at   = %p\n", g_dataIn);
	printf("INFO: global output at  = %p\n", g_dataOut);
	
	printf("INFO: EVSS_CFG_VCCM_SIZE=%i \n", ctl->local_mem_size);
	printf("INFO: buffers_spacing  = %d\n", buffers_spacing);
	printf("INFO: max_num_pixels_per_ch = %d\n",max_num_pixels_per_ch);
	printf("INFO: max_in_C_per_iter     = %d\n",max_in_C_per_iter);
	printf("INFO: sec_num_channels      = %d\n",sec_num_channels);
	printf("INFO: num_tiles             = %d\n",num_tiles);
	printf("INFO: num_ch_iter           = %d\n",num_ch_iter);
	printf("INFO: channels_iter_offset  = %d\n",channels_iter_offset);
	
	for( int input_ch =0; input_ch<max_in_C_per_iter; input_ch++)
	{
		printf("INFO: vmem in_ptr[%d]   at %p\n",input_ch, in_ptr[input_ch]);		
	}		
	printf("INFO: vmem out_ptr     at %p\n",out_ptr);		
  	printf("INFO: end buffers allocation %p\n",  local_mem_ptr);
  	printf("INFO: vmem for stack and variables =%d \n",  ((int)ctl->local_mem_ptr+ ctl->local_mem_size-(int)local_mem_ptr));
#endif

    local ocl_reshape_buffer_t buffer;
	//  init buffers
	// --------------
	// global buffers
	buffer.g_in_ptr	    = (global short * )ctl->in_ptr;
	buffer.g_out_ptr	= (global short * )ctl->out_ptr;
	
	for( int input_ch =0; input_ch<max_in_C_per_iter; input_ch++)
	{
		buffer.in_ptr[input_ch] = in_ptr[input_ch];  
	}
	buffer.out_ptr 		= out_ptr;
	buffer.in_buffers_spacing = buffers_spacing;
	buffer.max_num_pixels_per_ch = max_num_pixels_per_ch;
	buffer.max_in_C_per_iter     = max_in_C_per_iter;

	buffer.in_width    	= ctl->in_width;
	buffer.in_height     	= ctl->in_height;
	buffer.in_channels     = ctl->in_channels;	
	buffer.load_event		= -1;
	buffer.store_event     = -1;

	int curr_buff = 0;
	global short *g_in_ptr;
	global short *g_out_ptr;
	g_in_ptr  		= g_dataIn;
	g_out_ptr    	= g_dataOut;
	
    int one_over_scale_factor     = ctl->one_over_scale_factor;
    int one_over_scale_shift_bits = ctl->one_over_scale_shift_bits;

	for (int ch_iter=0; ch_iter<num_ch_iter; ch_iter++)
	{
		buffer.channels_iter = ch_iter  + channels_iter_offset;
		buffer.num_channels_at_tile = min(max_in_C_per_iter, sec_num_channels - ch_iter*max_in_C_per_iter);
		int num_remainder_pixels = in_ch_stride;
		
		for (int tile_num=0; tile_num<num_tiles; tile_num++)
		{		
			int	num_pixels2load   = min(max_num_pixels_per_ch,num_remainder_pixels);
			int	start_pixel2load = (tile_num)*max_num_pixels_per_ch;
			ocl_reshape_load_buffer(&buffer,	start_pixel2load,num_pixels2load ,tile_num ); 
			num_remainder_pixels = num_remainder_pixels - num_pixels2load;
			
			//wait to tile at curr_buff, to loading end
			wait_group_events(1, &buffer.load_event);
			
			// process tile
			ocl_reshape_chw2hwc_process_buffer(&buffer ,one_over_scale_factor , one_over_scale_shift_bits);
			
			// store tile
			ocl_reshape_store_buffer(&buffer);
	
			// store wait
			wait_group_events(1, &buffer.store_event);
		
			//sawp buffers
			curr_buff = 1- curr_buff;
		}
	}
   
}
// ************************************************************************
__attribute__(( reqd_work_group_size(1, 1, 1)))
__kernel void ocl_reshape_chw2hwc_tanh_process_buffer(local ocl_reshape_buffer_t  * buffer,
										        int one_over_scale_factor , int one_over_scale_shift_bits)
{

	int num_ch    			= buffer->max_in_C_per_iter;
	int in_buffers_spacing 	= buffer->in_buffers_spacing;
 
    int16   vix[2];  //indexes for vgather32
	short32  ch_idx = (short32)( 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31);
	vix[0]  = (int16)( 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15)*in_buffers_spacing;
	vix[1]  = (int16)(16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31)*in_buffers_spacing;
	local short * in_ptr0 = buffer->in_ptr[0];
	local short * out_ptr = buffer->out_ptr;
	short32  valid_mask;
#ifdef DO_SCALING
	int factor = one_over_scale_factor;
	int num_shifts = one_over_scale_shift_bits - OUTPUT_Q;
#endif
    int num_vec32_over_ch = (num_ch + 31)>>5;

	for (int pixel_num=0; pixel_num<buffer->num_pixels_at_tile; pixel_num++)
	{
		int num_remainning_ch = num_ch;
		for (int vec_num=0; vec_num<num_vec32_over_ch; vec_num++)
		{
		    valid_mask    = ch_idx < (short32)(num_remainning_ch);
			num_remainning_ch = num_remainning_ch - 32;
			int strat_ch  = (vec_num*32);
			short32 data  = vgather32(&in_ptr0[strat_ch*in_buffers_spacing+pixel_num], vix[0], vix[1] );	
#ifdef DO_SCALING	
			// scale data
			// convert_int16(data.lo) * (int16)factor) >> (num_shifts - output at Q10
			// The tanh LUT is  at step size 2*4/2^11 -> 4/1024, 
			int16 dala16_lo = (convert_int16(data.lo) * (int16)factor) >> (num_shifts+2); 
			int16 dala16_hi = (convert_int16(data.hi) * (int16)factor) >> (num_shifts+2); 
			      dala16_lo = dala16_lo + (int16)(TANH_LUT_SIZE>>1);
			      dala16_hi = dala16_hi + (int16)(TANH_LUT_SIZE>>1);
			dala16_lo       = clamp(dala16_lo , 0, TANH_LUT_SIZE-1);
			dala16_hi       = clamp(dala16_hi , 0, TANH_LUT_SIZE-1);
			data            = vgather32(buffer->tanh_lut, dala16_lo, dala16_hi);
			//if ((buffer->tile_num == 0) && (pixel_num==0))
			//{
			//  printf(" after tanh: dala16_lo = %#v16hd\n",  dala16_lo);
			//  printf(" after tanh: dala16_hi = %#v16hd\n",  dala16_hi);
			//  printf(" after tanh: data = %#v32hd\n",  data);
			//}
#endif			 
			vstore32_mask( data,0, (local short *)&out_ptr[strat_ch +pixel_num*num_ch] , convert_short32(valid_mask) );
		
		}
	}
}

// ***********************************************************************
__attribute__(( reqd_work_group_size(1, 1, 1)))
kernel void ev_inu_ocl_reshape_chw2hwc_scale_tanh_kernel(	global ocl_ctl_reshape_chw2hwc  	* ctl,
										                    global short 			            * restrict g_dataOut,
										                    global short 			            * restrict g_dataIn )
{
	float 	split_th 			= ctl->split_th;  
	bool	is_first_section 	= ctl->is_first_section;

	int in_H = ctl->in_height;
	int in_W = ctl->in_width;
	int in_C = ctl->in_channels;
		
	int sec_num_channels;	
	int first_sec_num_channels = (int)((float)in_C * split_th +31)&(~31);
	    first_sec_num_channels = min(in_C ,first_sec_num_channels);
	int channels_iter_offset;  
	
	if (is_first_section) {
	    sec_num_channels = first_sec_num_channels;
	    channels_iter_offset = 0;  
	}
	else {
	    sec_num_channels = in_C - first_sec_num_channels;
	    channels_iter_offset = (first_sec_num_channels + MAX_IN_CHANNELS-1)/MAX_IN_CHANNELS;  
	}
	int num_ch_iter = ( sec_num_channels + MAX_IN_CHANNELS-1)/MAX_IN_CHANNELS;
		
  	local short  *	in_ptr[MAX_IN_CHANNELS];	 // each ptr for a channel	
  	local short  *	out_ptr;     // output interleaved

	int max_in_C_per_iter = min(sec_num_channels ,MAX_IN_CHANNELS) ;

	int var_space_size = ctl->local_mem_size - (4*1024) -(TANH_LUT_SIZE*sizeof(short)); 
	
	int buffers_spacing;
	if (max_in_C_per_iter > 0) {
		buffers_spacing = var_space_size/(2*max_in_C_per_iter*sizeof(short)); 	//2 for input and output , 2 for double buffers, in_C num of buffers
	}
	else {
		buffers_spacing = var_space_size/(2*1*sizeof(short)); 	//2 for input and output , 2 for double buffers, in_C num of buffers
	}
	
	buffers_spacing = (buffers_spacing&(~0x1f)) + 1;  // in shorts
	
	int max_num_pixels_per_ch = min((buffers_spacing )&(~0x1f),in_W*in_H);
  	local unsigned char  * local_mem_ptr 	= (local unsigned char *)ctl->local_mem_ptr;

	// load tanh LUT to start of VMEM
	event_t load_lut_event = 0;
	local short  *	l_tanh_lut = (local short *)ctl->local_mem_ptr;
	load_lut_event = async_work_group_copy((local short *)ctl->local_mem_ptr,  (global short *)ctl->tanh_Q10_lut , TANH_LUT_SIZE ,   0);
	wait_group_events(1, &load_lut_event);
	local_mem_ptr  +=  TANH_LUT_SIZE*sizeof(short);
	
	for( int input_ch =0; input_ch<max_in_C_per_iter; input_ch++)
	{
		in_ptr[input_ch] 	=  (local short  *	)local_mem_ptr;  
		local_mem_ptr  +=  buffers_spacing*sizeof(short);
	}
	out_ptr     	=  (local short  *	)local_mem_ptr;
	local_mem_ptr  +=   buffers_spacing*sizeof(short)*max_in_C_per_iter;
	int in_ch_stride =  in_H*in_W;
	int num_tiles   = ( in_ch_stride + max_num_pixels_per_ch-1)/max_num_pixels_per_ch;

#ifdef OCL_reshape_PRINT_EN_DEBUG
	printf("INFO: global input at   = %p\n", g_dataIn);
	printf("INFO: global output at  = %p\n", g_dataOut);
	printf("INFO: local tanh_lut at  = %p\n", l_tanh_lut);
		
	printf("INFO: EVSS_CFG_VCCM_SIZE=%i \n", ctl->local_mem_size);
	printf("INFO: buffers_spacing  = %d\n", buffers_spacing);
	printf("INFO: max_num_pixels_per_ch = %d\n",max_num_pixels_per_ch);
	printf("INFO: max_in_C_per_iter     = %d\n",max_in_C_per_iter);
	printf("INFO: sec_num_channels      = %d\n",sec_num_channels);
	printf("INFO: num_tiles             = %d\n",num_tiles);
	printf("INFO: num_ch_iter           = %d\n",num_ch_iter);
	printf("INFO: channels_iter_offset  = %d\n",channels_iter_offset);
	
	for( int input_ch =0; input_ch<max_in_C_per_iter; input_ch++)
	{
		printf("INFO: vmem in_ptr[%d]   at %p\n",input_ch, in_ptr[input_ch]);		
	}		
	printf("INFO: vmem out_ptr     at %p\n",out_ptr);		

  	printf("INFO: end buffers allocation %p\n",  local_mem_ptr);
  	printf("INFO: vmem for stack and variables =%d \n",  ((int)ctl->local_mem_ptr+ ctl->local_mem_size-(int)local_mem_ptr));
#endif

    local ocl_reshape_buffer_t buffer;
	//  init buffers
	// --------------
	// global buffers
	buffer.g_in_ptr	    = (global short * )ctl->in_ptr;
	buffer.g_out_ptr	= (global short * )ctl->out_ptr;
	
	for( int input_ch   = 0; input_ch<max_in_C_per_iter; input_ch++)
	{
		buffer.in_ptr[input_ch] = in_ptr[input_ch];  
	}
	buffer.out_ptr 		= out_ptr;
	buffer.in_buffers_spacing    = buffers_spacing;
	buffer.max_num_pixels_per_ch = max_num_pixels_per_ch;
	buffer.max_in_C_per_iter     = max_in_C_per_iter;
    buffer.tanh_lut        =  l_tanh_lut;

	buffer.in_width    		= ctl->in_width;
	buffer.in_height     	= ctl->in_height;
	buffer.in_channels     	= ctl->in_channels;		
	buffer.load_event		= -1;
	buffer.store_event     	= -1;

	int curr_buff = 0;
	global short *g_in_ptr;
	global short *g_out_ptr;
	g_in_ptr  		= g_dataIn;
	g_out_ptr    	= g_dataOut;
	
    int one_over_scale_factor     = ctl->one_over_scale_factor;
    int one_over_scale_shift_bits = ctl->one_over_scale_shift_bits;


		
	for (int ch_iter=0; ch_iter<num_ch_iter; ch_iter++)
	{
		buffer.channels_iter = ch_iter + channels_iter_offset;
		buffer.num_channels_at_tile = min(max_in_C_per_iter, sec_num_channels - ch_iter*max_in_C_per_iter);
		int num_remainder_pixels = in_ch_stride;

		for (int tile_num=0; tile_num<num_tiles; tile_num++)
		{
		
			int num_pixels2load   = min(max_num_pixels_per_ch,num_remainder_pixels);
			int start_pixel2load = (tile_num)*max_num_pixels_per_ch;
			ocl_reshape_load_buffer(&buffer,	start_pixel2load,num_pixels2load ,tile_num ); 
			num_remainder_pixels = num_remainder_pixels - num_pixels2load;
			
			//wait to tile at curr_buff, to loading end
			wait_group_events(1, &buffer.load_event);
			
			// process tile
			ocl_reshape_chw2hwc_tanh_process_buffer(&buffer ,one_over_scale_factor , one_over_scale_shift_bits);
			
			// store tile
			ocl_reshape_store_buffer(&buffer);
	
			// store wait
			wait_group_events(1, &buffer.store_event);
			
			//sawp buffers
			curr_buff = 1- curr_buff;
		}
	}
    
}