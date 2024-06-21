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
#include "inu_combine_c.h"

#define  OCL_bokeh_SIMD_WIDTH 32

#if ( __Xvec_guard_bit_option == 0 )
#define acc_convert_int16(acc)  acc
#endif

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

void evYieldThread(void);

#define AUX_RTC_LOW                    (0x104)
#define AUX_RTC_HIGH                   (0x105)

//// ***********************************************************************
#define      STEP_X 32
// ***********************************************************************
// ***********************************************************************

typedef struct
{
	global 	unsigned char 	* g_in_ptr[MAX_MATS_IN];
	global 	unsigned char	* g_out_ptr;
	
	local 	unsigned char	* in_ptr[MAX_MATS_IN];  
	local 	unsigned char	* out_ptr;	
	int     num_mats;
	
	int   	tile_num;
	int   	tile_vec_len;
	int   	tile_vec_start;
		
	event_t       load_event;
	event_t       store_event;
	
} ocl_combine_c_buffer_t;
// ***********************************************************************

// ***********************************************************************
void ocl_conbineC_load_buffer(local ocl_combine_c_buffer_t  * buffer,	int  start_input2load, int num_2load ,int tile_num)
{

    buffer->tile_vec_start		= start_input2load;
    buffer->tile_vec_len		= num_2load;
    buffer->tile_num	    	= tile_num;
	//printf("num_2load = %d \n",num_2load);

	buffer->load_event = async_work_group_copy(buffer->in_ptr[0],  &buffer->g_in_ptr[0][start_input2load], num_2load,   0);
	for (int k=1; k<buffer->num_mats; k++)
	{
	buffer->load_event = async_work_group_copy(buffer->in_ptr[k],  &buffer->g_in_ptr[k][start_input2load], num_2load, buffer->load_event);
	}	
}
// ************************************************************************
__attribute__(( reqd_work_group_size(1, 1, 1)))
__kernel void ocl_combineC_store_buffer(local ocl_combine_c_buffer_t  * buffer )
{
    int  num_samples2store = buffer->tile_vec_len;
	int  out_offset  = buffer->tile_vec_start;
    // printf("store: num_samples2store = %d \n", num_samples2store);		

	buffer->store_event = async_work_group_copy(&buffer->g_out_ptr[out_offset],  &buffer->out_ptr[0], num_samples2store,   0);	
}
// ***********************************************************************
__attribute__(( reqd_work_group_size(1, 1, 1)))
__kernel void ocl_combineC_process_buffer(local ocl_combine_c_buffer_t  * 	buffer)
{

	int 			tile_vec_len = buffer->tile_vec_len;
	local 	unsigned char * in_ptr[MAX_MATS_IN];
	int 	num_mats = buffer->num_mats;
 	for (int k=0; k<num_mats;k++) {
		in_ptr[k] = buffer->in_ptr[k];
	}
	local 	unsigned char  *out_ptr = buffer->out_ptr;

	for (int vec = 0; vec<tile_vec_len  ; vec = vec + 64)
	{

		uchar64  out_sum;

		// load 64 elements
        uchar64  v1 	= vload64(0, &in_ptr[0][vec]);
        uchar64  v2 	= vload64(0, &in_ptr[1][vec]);
		out_sum         = max(v1,v2);
		
		uchar64  v3;
		for (int k=2; k<num_mats; k++) {
			v3 			= vload64(0, &in_ptr[k][vec]);
			out_sum     = max(out_sum, v3);
		}
		#ifdef OCL_COMBINE_C_PRINT_EN_
		if(vec==0){
			printf(" v1       = %#v32hd\n",  v1.lo);
			printf(" v2       = %#v32hd\n",  v2.lo);
			printf(" v3       = %#v32hd\n",  v3.lo);
			printf(" out_sum  = %#v32hd\n",  out_sum.lo);
		}
		#endif


		vstore64(out_sum,0,&out_ptr[vec]);
	}
}
// ***********************************************************************
__attribute__(( reqd_work_group_size(1, 1, 1)))
kernel void ev_inu_ocl_combine_uchar_kernel(    global ocl_ctl_combine_c_planar * ctl,
										        global unsigned char 			* restrict g_dataOut )
{ 

	int num_mats 	= ctl->num_mats;
	int size_x 		= ctl->size_x;
	int size_y 		= ctl->size_y;
	
	
	float 	split_th 			= ctl->split_th;  
	bool	is_first_section 	= ctl->is_first_section;

	int sec_mat_len;	
	int sec_first_sample;
	if (is_first_section) {
	    sec_mat_len  = (int)((float)size_x*size_y * split_th);
		sec_first_sample = 0;
	}
	else {
		sec_first_sample = (int)((float)size_x*size_y * split_th);
	    sec_mat_len  = size_x*size_y - sec_first_sample;
	}
	
	
	int var_space_size = ctl->local_mem_size - (4*1024); 
	int max_tile_len   = var_space_size/(sizeof(char)*num_mats + sizeof(char));
	    max_tile_len   = max_tile_len & (~0x3f);
	int num_tiles   = ( sec_mat_len + max_tile_len-1)/max_tile_len;
	
  	local char  * local_mem_ptr 	= (local char *)ctl->local_mem_ptr;

	local unsigned char * in_ptr[MAX_MATS_IN];
	local unsigned char * out_ptr;
	
	for (int k=0; k<num_mats; k++)
	{
		in_ptr[k] 	   =  (local unsigned char  *)local_mem_ptr;  
		local_mem_ptr  +=  max_tile_len*sizeof(char);
	}
	out_ptr     	=  (local unsigned char  *	)local_mem_ptr;
	local_mem_ptr  +=   max_tile_len*sizeof(char);
	
#ifdef OCL_COMBINE_C_PRINT_EN_
	for (int k=0; k<num_mats; k++)
	{
		printf("INFO: global input[%d] at   = %p\n",  k, ctl->in_mats_ptrs[k]);
	}
	printf("INFO: global output at  = %p\n", ctl->out_ptr);
		
	printf("INFO: EVSS_CFG_VCCM_SIZE=%i \n", ctl->local_mem_size);
	printf("INFO: max_tile_len = %d  \n",max_tile_len);
	printf("INFO: sec_mat_len  = %d  \n",sec_mat_len);
	printf("INFO: num_tiles    = %d  \n",num_tiles);
	for (int k=0; k<num_mats; k++)
	{
		printf("INFO: local input[%d]  at   = %p\n",  k, in_ptr[k] );
	}
	printf("INFO: local output at   = %p\n",  out_ptr );


  	printf("INFO: end buffers allocation %p\n",  local_mem_ptr);
  	printf("INFO: vmem for stack and variables =%d \n",  ((int)ctl->local_mem_ptr+ ctl->local_mem_size-(int)local_mem_ptr));
#endif	

    local ocl_combine_c_buffer_t buffer;
	//  init buffers
	// --------------
	// global buffers
	for (int k=0; k<num_mats; k++) {
		buffer.g_in_ptr[k] =  (global unsigned char *)ctl->in_mats_ptrs[k];
	}
	buffer.g_out_ptr	   =  (global unsigned char *)ctl->out_ptr;
	
	// local ptrs
	for (int k=0; k<num_mats; k++) {
		buffer.in_ptr[k]    = in_ptr[k];  
	}
	buffer.out_ptr 		= out_ptr;
	buffer.num_mats     = num_mats;

	buffer.load_event	= -1;
	buffer.store_event  = -1;

	int num_remainder = sec_mat_len;

	for (int tile_num=0; tile_num<num_tiles; tile_num++)
	{
		int num_2load   = min(max_tile_len,num_remainder);
		int start_input2load = (tile_num)*max_tile_len + sec_first_sample;
		ocl_conbineC_load_buffer(&buffer, start_input2load,num_2load ,tile_num ); 
		
		num_remainder = num_remainder - num_2load;
		
		//wait to tile at curr_buff, to loading end
		wait_group_events(1, &buffer.load_event);
		// process tile
		ocl_combineC_process_buffer(&buffer);
		
		// store tile
		ocl_combineC_store_buffer(&buffer);
	
		// store wait
		wait_group_events(1, &buffer.store_event);

	}

}
// ***********************************************************************
