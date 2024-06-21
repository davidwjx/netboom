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
#include "inu_convert2float.h"

#if ( __Xvec_guard_bit_option == 0 )
#define acc_convert_int16(acc)  acc
#endif

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

void evYieldThread(void);

#define AUX_RTC_LOW                    (0x104)
#define AUX_RTC_HIGH                   (0x105)

#define MAX_IN_CHANNELS					32


// ***********************************************************************
//#define      DBG_LINE   523
//#define      DBG_COL_S  416
// ***********************************************************************

typedef struct
{
	global 	short 			* g_in_ptr;
	global 	float 			* g_out_ptr; //float
	global 	half 			* g_fp16_out_ptr; //float
	local 	short        	* in_ptr;  
	local 	float 			* out_ptr;	
	local 	half 			* out_fp16_ptr;	
    
	bool    float_output;
	int   	in_vec_len;
	float  	scaler;
	
	int   	tile_num;
	int   	tile_vec_len;
	int   	tile_vec_start;
		
	event_t       load_event;
	event_t       store_event;
} ocl_convert2f_buffer_t;

#if ( __Xvec_guard_bit_option == 2 )
#define acc_type acc40x32
#define acc_convert(acc) acc_convert_int16((acc))
#define acc_init(val) acc_init_acc40x32((val))
#else
#define acc_type acc32x32
#define acc_convert(acc) (acc)
#define acc_init(val) acc_init_acc32x32((val))
#endif
// ***********************************************************************
void ocl_convert2f_load_buffer(local ocl_convert2f_buffer_t  * buffer,	int  start_input2load, int num_2load ,int tile_num)
{

    buffer->tile_vec_start		= start_input2load;
    buffer->tile_vec_len		= num_2load;
    buffer->tile_num	    	= tile_num;

 	//global short * g_short_ptr  = (global short *)buffer->g_in_ptr;
	buffer->load_event = async_work_group_copy(buffer->in_ptr,  &buffer->g_in_ptr[start_input2load], num_2load,   0);
	
}
// ************************************************************************
__attribute__(( reqd_work_group_size(1, 1, 1)))
__kernel void ocl_convert2f_store_buffer(local ocl_convert2f_buffer_t  * buffer )
{
    int  num_samples2store = buffer->tile_vec_len;
	int  out_offset  = buffer->tile_vec_start;
    // printf("store: num_samples2store = %d \n", num_samples2store);		

	if (buffer->float_output ==1) {
		buffer->store_event = async_work_group_copy((global float * )&buffer->g_out_ptr[out_offset],  &buffer->out_ptr[0], num_samples2store,   0);	
	}
	else { // output at half format
		buffer->store_event = async_work_group_copy((global half * )&buffer->g_fp16_out_ptr[out_offset],  &buffer->out_fp16_ptr[0], num_samples2store,   0);	
	}
}
// ***********************************************************************
__attribute__(( reqd_work_group_size(1, 1, 1)))
__kernel void ocl_convert2f_process_buffer(local ocl_convert2f_buffer_t  * 	buffer)
{

	int 			tile_vec_len = buffer->tile_vec_len;
	local 	short * in_ptr 	= buffer->in_ptr;
	local 	float *	out_ptr = buffer->out_ptr;
	local 	half  *	out_fp16_ptr = buffer->out_fp16_ptr;

	float scaler_val = buffer->scaler;
	float16 scaler_vec16 = (float16)scaler_val;
	
	for (int vec = 0; vec<tile_vec_len  ; vec = vec + 64)
	{
		// load 64 elements
        short32  v1 	= vload32(0, &in_ptr[vec]);
        short32  v2 	= vload32(1, &in_ptr[vec]);

		if (buffer->float_output ==1) { //output at float format

			float16 v1_lf   = convert_float16(v1.lo);
			float16 v1_hf   = convert_float16(v1.hi);
			float16 v2_lf   = convert_float16(v2.lo);
			float16 v2_hf   = convert_float16(v2.hi);
	
			float16 out1_lf   = v1_lf*scaler_vec16;
			float16 out1_hf   = v1_hf*scaler_vec16;
			float16 out2_lf   = v2_lf*scaler_vec16;
			float16 out2_hf   = v2_hf*scaler_vec16;
	
			#ifdef OCL_convert2f_PRINT_EN_DEBUG_
			if(vec==0){
				printf(" scaler_val= %f\n",  scaler_val);
				printf(" v1       = %#v32hd\n",  v1);
				printf(" v1_lf    = %#v16hf\n",  v1_lf);
				printf(" v1_hf    = %#v16hf\n",  v1_hf);
				printf(" out1_lf    = %#v16hf\n", out1_lf);
				printf(" out1_hf    = %#v16hf\n", out1_hf);
			}
			#endif
	
	
			vstore16(out1_lf,0,(local float *)&out_ptr[vec]);
			vstore16(out1_hf,1,(local float *)&out_ptr[vec]);
			vstore16(out2_lf,2,(local float *)&out_ptr[vec]);
			vstore16(out2_hf,3,(local float *)&out_ptr[vec]);
		}
		else { //output at half format

			half32  v1_f    = convert_half32(v1);
			half32  v2_f    = convert_half32(v2);
	
			half32 	out1_f   = v1_f*(half32)scaler_val;
			half32 	out2_f   = v2_f*(half32)scaler_val;
	
			vstore32(out1_f,0,(local half *)&out_fp16_ptr[vec]);
			vstore32(out2_f,1,(local half *)&out_fp16_ptr[vec]);
		}
	}
}
// ***********************************************************************
__attribute__(( reqd_work_group_size(1, 1, 1)))
__attribute__(( reqd_work_group_size(1, 1, 1)))
kernel void ev_inu_ocl_convert2f_kernel(	global OCL_conver2f_PRINT_EN_DEBUG  * ctl,
										    global float 			    		* restrict g_dataOut,
										    global short 			    		* restrict g_dataIn )
{


  	float 	split_th 			= ctl->split_th;  
	bool	is_first_section 	= ctl->is_first_section;

	int sec_mat_len;	
	int sec_first_sample;
	if (is_first_section) {
	    sec_mat_len  = (int)((float)ctl->in_vec_len * split_th);
		sec_first_sample = 0;
	}
	else {
		sec_first_sample = (int)((float)ctl->in_vec_len * split_th);
	    sec_mat_len  = ctl->in_vec_len - sec_first_sample;
	}

	//printf("ocl: sec_first_sample=%d, sec_mat_len=%d \n",sec_first_sample,sec_mat_len);
	
	int in_vec_len 			= sec_mat_len;
		
  	local short  *	in_ptr;	     	
  	local float  *	out_ptr;     

	int var_space_size = ctl->local_mem_size - (4*1024); 
	int max_tile_len   = var_space_size/(sizeof(short) + sizeof(float)); 	// input @ short and output @ short.
	    max_tile_len   = max_tile_len & (~0x3f);
	int num_tiles   = ( in_vec_len + max_tile_len-1)/max_tile_len;
	
  	local char  * local_mem_ptr 	= (local char *)ctl->local_mem_ptr;

	in_ptr 			=  (local short  *	)local_mem_ptr;  
	local_mem_ptr  +=  max_tile_len*sizeof(short);
	out_ptr     	=  (local float  *	)local_mem_ptr;
	local_mem_ptr  +=   max_tile_len*sizeof(float);

#ifdef OCL_convert2f_PRINT_EN_DEBUG_
	printf("INFO: global input at   = %p\n", g_dataIn);
	printf("INFO: global output at  = %p\n", g_dataOut);
		
	printf("INFO: EVSS_CFG_VCCM_SIZE=%i \n", ctl->local_mem_size);
	printf("INFO: max_tile_len = %d  \n",max_tile_len);
	printf("INFO: num_tiles    = %d  \n",num_tiles);
	printf("INFO: in_vec_len   = %d  \n",in_vec_len);
	printf("INFO: vmem in_ptr   at %p\n", in_ptr);		
	printf("INFO: vmem out_ptr  at %p\n", out_ptr);		

  	printf("INFO: end buffers allocation %p\n",  local_mem_ptr);
  	printf("INFO: vmem for stack and variables =%d \n",  ((int)ctl->local_mem_ptr+ ctl->local_mem_size-(int)local_mem_ptr));
#endif

    local ocl_convert2f_buffer_t buffer;
	//  init buffers
	// --------------
	// global buffers
	buffer.g_in_ptr	    = (global short * )g_dataIn;
	buffer.g_out_ptr	= (global float * )g_dataOut;
	buffer.g_fp16_out_ptr = (global half *)g_dataOut;
	buffer.in_ptr	    = in_ptr;  
	buffer.out_ptr 		= out_ptr;
	buffer.out_fp16_ptr = (local half *)out_ptr;
	buffer.in_vec_len   = in_vec_len;
	buffer.scaler       = ctl->scaler;
	buffer.float_output = ctl->float_output;
	buffer.load_event	= -1;
	buffer.store_event  = -1;
	
	int num_remainder = in_vec_len;

	for (int tile_num=0; tile_num<num_tiles; tile_num++)
	{
	
		int num_2load   = min(max_tile_len,num_remainder);
		int start_input2load = (tile_num)*max_tile_len + sec_first_sample;
		ocl_convert2f_load_buffer(&buffer, start_input2load,num_2load ,tile_num ); 
		num_remainder = num_remainder - num_2load;
		
		//wait to tile at curr_buff, to loading end
		wait_group_events(1, &buffer.load_event);
		
		// process tile
		ocl_convert2f_process_buffer(&buffer);
		
		// store tile
		ocl_convert2f_store_buffer(&buffer);
	
		// store wait
		wait_group_events(1, &buffer.store_event);

	}

    
}

// ***********************************************************************
