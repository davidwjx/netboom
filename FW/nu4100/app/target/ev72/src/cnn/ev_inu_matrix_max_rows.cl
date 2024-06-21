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
#include "inu_matrix_max_rows.h"

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
	global half  * g_in_matrix;
	global float * g_out_res;
	
	local half          * l_in_mat;  
	local float 		* out_res;	
	int   max_tile_num_rows;

	int 	in_height;  		// input height	
 	int 	in_width;  		    // input width	
 	int 	in_buffers_spacing;  
	
	int 	tile_num;
	int     first_pixel_of_tile;
	int     num_pixels_at_tile;
	int     num_rows_at_tile;
	
	event_t       load_event;
	event_t       store_event;	
} ocl_mat_max_rows_buffer_t;
// ***********************************************************************
void ocl_mat_max_rows_load_buffer(local ocl_mat_max_rows_buffer_t  * buffer,	int  start_pixel2load, int num_pixels2load, int num_rows2load ,int tile_num)
{

    buffer->first_pixel_of_tile	= start_pixel2load;
    buffer->num_pixels_at_tile	= num_pixels2load;
	buffer->num_rows_at_tile    =  num_rows2load;
    buffer->tile_num			= tile_num;
	
	int in_width 			= buffer->in_width;
    int in_buffers_spacing 	= buffer->in_buffers_spacing;
	//buffer->load_event = async_work_group_copy(&buffer->l_in_mat[0],  &buffer->g_in_matrix[start_pixel2load], num_pixels2load,   0);	

//printf( "tile=%d, start=%d, num2load=%d ,in_buffers_spacing=%d \n",tile_num,start_pixel2load,num_pixels2load,in_buffers_spacing);
//printf( "buffer->l_in_mat @ %p \n",buffer->l_in_mat );
//printf( "in_width =%d \n",in_width );
//printf( "&buffer->g_in_matrix[start_pixel2load] @ %p \n",&buffer->g_in_matrix[start_pixel2load] );

	buffer->load_event =  async_work_group_copy2D(
												   buffer->l_in_mat,					    		// dst
												   in_width*sizeof(half),							// dst width
												   in_buffers_spacing*sizeof(half),	    		    // dst stride
												   &buffer->g_in_matrix[start_pixel2load],	   		// src
												   num_pixels2load*sizeof(half), 					// src width
												   num_pixels2load*sizeof(half),	       			// src stride
												   num_pixels2load*sizeof(half),	// size
												   0);


#ifdef OCL_MAT_MAX_ROWS_PRINT_EN_
	if (buffer->tile_num ==0) {
	half32 tt0 = vload32(0, &buffer->l_in_mat[0]);
	printf(" tt0 = %#v16hf\n",  tt0.lo);
	}	
#endif	
}

// ***********************************************************************
__attribute__(( reqd_work_group_size(1, 1, 1)))
__kernel void ocl_mat_max_rows_process_buffer(local ocl_mat_max_rows_buffer_t  * buffer)
{

	//Get kernel parameters from the control structure
	int in_width 	= buffer->in_width;
	int in_buffers_spacing = buffer->in_buffers_spacing;
	int num_rows_at_tile   = buffer->num_rows_at_tile;
		
    int16  pixel_offsets_lo = (int16)( 	( 0*in_buffers_spacing),
										( 1*in_buffers_spacing),
										( 2*in_buffers_spacing),
										( 3*in_buffers_spacing),
										( 4*in_buffers_spacing),
										( 5*in_buffers_spacing),
										( 6*in_buffers_spacing),
										( 7*in_buffers_spacing),
										( 8*in_buffers_spacing),
										( 9*in_buffers_spacing),
										(10*in_buffers_spacing),
										(11*in_buffers_spacing),
										(12*in_buffers_spacing),
										(13*in_buffers_spacing),
										(14*in_buffers_spacing),
										(15*in_buffers_spacing));
	int16  pixel_offsets_hi = pixel_offsets_lo + (int16)(16*in_buffers_spacing);
	local half  * local_in_ptr = (local half *) buffer->l_in_mat;
	for (int row=0; row<num_rows_at_tile; row+=32) {
		half32  max_vec 		= (half32)0;
		int 	in_row_offset 	= row*in_buffers_spacing;
		
		for(int w=1; w<in_width;w++)
		{	
				half32 rows_data = vgather32( &local_in_ptr[in_row_offset +w] ,  pixel_offsets_lo , pixel_offsets_hi);

				max_vec.lo = max(max_vec.lo,rows_data.lo);
				max_vec.hi = max(max_vec.hi,rows_data.hi);
		} // w
		float16  max_lo = convert_float16(max_vec.lo);
		float16  max_hi = convert_float16(max_vec.hi);
		vstore16(max_lo, 0,(local float *)&buffer->out_res[row]);
		vstore16(max_hi, 1,(local float *)&buffer->out_res[row]);

	} // row
}
// ***********************************************************************
__attribute__(( reqd_work_group_size(1, 1, 1)))
__kernel void ocl_mat_max_rows_store_buffer(local ocl_mat_max_rows_buffer_t  * buffer)
{
    int  num_pixels2store = buffer->num_rows_at_tile;
	int  out_offset       = buffer->tile_num  * buffer->max_tile_num_rows;   
	buffer->store_event = async_work_group_copy(&buffer->g_out_res[out_offset],  &buffer->out_res[0], num_pixels2store,   0);	

#ifdef OCL_MAT_MAX_ROWS_PRINT_EN_
	if (buffer->tile_num==0){
		float16 tt0 = vload16(0, &buffer->out_res[0]);
	
		printf(" out_offset=%d \n",  out_offset);
		printf(" num_pixels2store=%d \n",  num_pixels2store);
		printf(" tt0        = %#v16hf\n",  tt0);
	}			
#endif
}
// ***********************************************************************
__attribute__(( reqd_work_group_size(1, 1, 1)))
kernel void ev_inu_ocl_matrix_max_rows_kernel(  global ocl_ctl_matrix_max_rows  	* ctl,
										        global float 					* restrict g_dataOut,
										        global float        			* restrict g_in_matrix)
{

	//float 	split_th 			= ctl->split_th;  
	//bool	is_first_section 	= ctl->is_first_section;

	int in_H 		= ctl->in_height;
	int in_W 		= ctl->in_width;

	int in_buffers_spacing = ((in_W + 31)&(~31)) + 1;

	int var_space_size = ctl->local_mem_size - (4*1024); 
	int max_tile_num_rows  = var_space_size/(sizeof(half)*in_buffers_spacing + sizeof(float));
	    max_tile_num_rows  = min(max_tile_num_rows,in_H);
	int num_tiles = ( in_H + max_tile_num_rows-1)/max_tile_num_rows;
	int max_pixels_at_tile   = max_tile_num_rows*in_W;


  	local half   *	in_ptr;	 
  	local float  *	out_ptr;                     // output interleaved
  	local unsigned char  * local_mem_ptr 	= (local unsigned char *)ctl->local_mem_ptr;
	int   local_input_buffer_size = (max_tile_num_rows*in_buffers_spacing + 1)&(~1);

	in_ptr      	=  (local half  *	)local_mem_ptr;  
	local_mem_ptr  +=  local_input_buffer_size*sizeof(half);
	out_ptr 	    =  (local float  *	)local_mem_ptr;
	local_mem_ptr  +=  max_tile_num_rows*sizeof(float);
	
#ifdef OCL_MAT_MAX_ROWS_PRINT_EN_
	printf("INFO: EVSS_CFG_VCCM_SIZE=%i \n", ctl->local_mem_size);
	printf("INFO: g_out_ptr      at %p\n", g_dataOut);
	printf("INFO: g_in_matrix   at %p\n", g_in_matrix);

	printf("INFO: in matrix size  [H,W] = [%d ,%d] \n", in_H, in_W); 

	printf("INFO: var_space_size = %d  \n", var_space_size );
	printf("INFO: max_tile_num_rows  = %d  \n", max_tile_num_rows );
	printf("INFO: in_buffers_spacing = %d  \n", in_buffers_spacing );
	printf("INFO: num_tiles      = %d  \n", num_tiles );
	
	printf("INFO: vmem in_ptr      at %p\n",in_ptr);		
 	printf("INFO: vmem out_ptr     at %p\n",out_ptr);		
  	printf("INFO: end buffers allocation %p\n",  local_mem_ptr);
  	printf("INFO: vmem for stack and variables =%d \n",  ((int)ctl->local_mem_ptr+ ctl->local_mem_size-(int)local_mem_ptr));
#endif

    local ocl_mat_max_rows_buffer_t buffer;
	//  init buffers
	// --------------
	// global buffers
	buffer.g_in_matrix	    = (global half  * )g_in_matrix;
	buffer.g_out_res	    = (global float * )g_dataOut;

	buffer.l_in_mat =  in_ptr;  
	buffer.out_res 	=  out_ptr;
	
	buffer.max_tile_num_rows=  max_tile_num_rows;
	buffer.in_height=  in_H;
	buffer.in_width =  in_W;
	buffer.in_buffers_spacing = in_buffers_spacing;
	buffer.load_event	   = -1;
	buffer.store_event     = -1;
	
	int num_remainder_rows   = in_H;
		
	for (int tile_num=0; tile_num<num_tiles; tile_num++) {
		int num_rows2load   = min(max_tile_num_rows , num_remainder_rows);
		int num_pixels2load   = num_rows2load*in_W;
		int start_pixel2load = (tile_num)*max_pixels_at_tile;

		ocl_mat_max_rows_load_buffer(&buffer,	start_pixel2load,num_pixels2load,num_rows2load ,tile_num ); 
		num_remainder_rows   = num_remainder_rows - num_rows2load;

		//wait to tile at curr_buff, to loading end
		wait_group_events(1, &buffer.load_event);

		// process tile
		ocl_mat_max_rows_process_buffer(&buffer);

		// store tile
		ocl_mat_max_rows_store_buffer(&buffer);
	
		// store wait
		wait_group_events(1, &buffer.store_event);
	}
}
// ***********************************************************************
