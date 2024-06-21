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
#include "inu_logistic.h"

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
	global 	void 			* g_in_ptr;
	global 	float 			* g_out_ptr;
	
	local 	float       	* logistic_lut_ptr;  
	local 	void        	* in_ptr;  
	local 	float 			* out_ptr;	

	int   	tile_num;
	int   	in_vec_len;
	int   	tile_vec_len;
	int   	tile_vec_start;
    int     max_tile_vec_len;	
	int     element_size;
		
	event_t       load_event;
	event_t       store_event;
} ocl_logistic_buffer_t;
// ***********************************************************************
void ocl_logistic_load_buffer(local ocl_logistic_buffer_t  * buffer,	int  start_input2load, int num_inputs2load ,int tile_num)
{

    buffer->tile_vec_start	= start_input2load;
    buffer->tile_vec_len	= num_inputs2load;
    buffer->tile_num	    = tile_num;

    // printf("load: ch=%d , num_inputs2load = %d \n", ch, num_inputs2load);
	if (buffer->element_size == 2)
	{
		global short * g_short_ptr  = (global short *)buffer->g_in_ptr;
		buffer->load_event = async_work_group_copy((local short *)buffer->in_ptr,  &g_short_ptr[start_input2load], num_inputs2load,   0);
	}
	else if(buffer->element_size == 1)
	{
		global char * g_char_ptr  = (global char *)buffer->g_in_ptr;
		buffer->load_event = async_work_group_copy((local char *)buffer->in_ptr,  &g_char_ptr[start_input2load], num_inputs2load,   0);
	}
	
}
// ************************************************************************
__attribute__(( reqd_work_group_size(1, 1, 1)))
__kernel void ocl_logistic_store_buffer(local ocl_logistic_buffer_t  * buffer )
{
    int  num_samples2store = buffer->tile_vec_len;
	int  out_offset  = buffer->tile_num * buffer->max_tile_vec_len;
    // printf("store: num_samples2store = %d \n", num_samples2store);		

	buffer->store_event = async_work_group_copy(&buffer->g_out_ptr[out_offset],  &buffer->out_ptr[0], num_samples2store,   0);	
}
// ***********************************************************************
__attribute__(( reqd_work_group_size(1, 1, 1)))
__kernel void ocl_logistic_process_short_buffer(local ocl_logistic_buffer_t  * 	buffer,
										        int 						scale_factor , 
												int 						scale_shift_bits)
{

	local short * in_ptr  = (local short *)buffer->in_ptr;
	local float * out_ptr = buffer->out_ptr;
		
	int factor 		= scale_factor;
	int num_shifts 	= scale_shift_bits;
    int tile_len    = buffer->tile_vec_len;

	
	for (int in_num = 0; in_num<tile_len  ; in_num += 32)
	{
		short32 data  = vload32( 0, &in_ptr[in_num] );	

		// scale data and calc lut indexes
		// The logistic LUT is  at step size max_int/2^10 -> 6/1024, lut size=2049

		int16 dala16_lo = (convert_int16(data.lo) * (int16)factor) >> num_shifts; 
		int16 dala16_hi = (convert_int16(data.hi) * (int16)factor) >> num_shifts; 
		      dala16_lo = dala16_lo + (int16)(LOGISTIC_LUT_SIZE>>1);
		      dala16_hi = dala16_hi + (int16)(LOGISTIC_LUT_SIZE>>1);
#ifdef OCL_logistic_PRINT_EN_DEBUG_
		if ((in_num == 0) && (buffer->tile_num==0))
		{
			printf(" data = %#v32hd\n",  data);
			printf(" dala16_lo = %#v16hd\n",  dala16_lo);
			printf(" dala16_hi = %#v16hd\n",  dala16_hi);
		}
#endif		
		dala16_lo       = clamp(dala16_lo , 0, LOGISTIC_LUT_SIZE-1);
		dala16_hi       = clamp(dala16_hi , 0, LOGISTIC_LUT_SIZE-1);
		float16 out_f_lo = vgather16(buffer->logistic_lut_ptr, dala16_lo);
		float16 out_f_hi = vgather16(buffer->logistic_lut_ptr, dala16_hi);
		
#ifdef OCL_logistic_PRINT_EN_DEBUG_
		if ((in_num == 0) && (buffer->tile_num==0))
		{
			printf(" data = %#v32hd\n",  data);
			printf(" out_f_lo = %#v16hf\n",  out_f_lo);
			printf(" out_f_hi = %#v16hf\n",  out_f_hi);
		}
#endif	 
		vstore16( out_f_lo,0, (local float *)&out_ptr[in_num]);
		vstore16( out_f_hi,1, (local float *)&out_ptr[in_num]);
	}
}
// ***********************************************************************
__attribute__(( reqd_work_group_size(1, 1, 1)))
__kernel void ocl_logistic_process_char_buffer(local ocl_logistic_buffer_t  * 	buffer,
										        int 						scale_factor , 
												int 						scale_shift_bits)
{

	local char  * in_ptr  = (local char *)buffer->in_ptr;
	local float * out_ptr = buffer->out_ptr;
		
	int factor 		= scale_factor;
	int num_shifts 	= scale_shift_bits;
    int tile_len    = buffer->tile_vec_len;

	
	for (int in_num = 0; in_num<tile_len  ; in_num += 64)
	{
		char64 data  = vload64( 0, &in_ptr[in_num] );	

		// scale data and calc lut indexes
		// The logistic LUT is  at step size max_int/2^10 -> 6/1024, lut size=2049

		int16 dala16_ll = (convert_int16(data.lo.lo) * (int16)factor) >> num_shifts; 
		int16 dala16_lh = (convert_int16(data.lo.hi) * (int16)factor) >> num_shifts; 
		int16 dala16_hl = (convert_int16(data.hi.lo) * (int16)factor) >> num_shifts; 
		int16 dala16_hh = (convert_int16(data.hi.hi) * (int16)factor) >> num_shifts; 
		      dala16_ll = dala16_ll + (int16)(LOGISTIC_LUT_SIZE>>1);
		      dala16_lh = dala16_lh + (int16)(LOGISTIC_LUT_SIZE>>1);
		      dala16_hl = dala16_hl + (int16)(LOGISTIC_LUT_SIZE>>1);
		      dala16_hh = dala16_hh + (int16)(LOGISTIC_LUT_SIZE>>1);
#ifdef OCL_logistic_PRINT_EN_DEBUG_
		if ((in_num == 0) && (buffer->tile_num==0))
		{
			printf(" data = %#v32hd\n",  data);
			printf(" dala16_lo = %#v16hd\n",  dala16_lo);
			printf(" dala16_hi = %#v16hd\n",  dala16_hi);
		}
#endif		
		dala16_ll       = clamp(dala16_ll , 0, LOGISTIC_LUT_SIZE-1);
		dala16_lh       = clamp(dala16_lh , 0, LOGISTIC_LUT_SIZE-1);
		dala16_hl       = clamp(dala16_hl , 0, LOGISTIC_LUT_SIZE-1);
		dala16_hh       = clamp(dala16_hh , 0, LOGISTIC_LUT_SIZE-1);
		float16 out_f_ll = vgather16(buffer->logistic_lut_ptr, dala16_ll);
		float16 out_f_lh = vgather16(buffer->logistic_lut_ptr, dala16_lh);
		float16 out_f_hl = vgather16(buffer->logistic_lut_ptr, dala16_hl);
		float16 out_f_hh = vgather16(buffer->logistic_lut_ptr, dala16_hh);
		
#ifdef OCL_logistic_PRINT_EN_DEBUG_
		if ((in_num == 0) && (buffer->tile_num==0))
		{
			printf(" data = %#v32hd\n",  data);
			printf(" out_f_lo = %#v16hf\n",  out_f_lo);
			printf(" out_f_hi = %#v16hf\n",  out_f_hi);
		}
#endif	 
		vstore16( out_f_ll,0, (local float *)&out_ptr[in_num]);
		vstore16( out_f_lh,1, (local float *)&out_ptr[in_num]);
		vstore16( out_f_hl,2, (local float *)&out_ptr[in_num]);
		vstore16( out_f_hh,3, (local float *)&out_ptr[in_num]);
	}
}
// ***********************************************************************
__attribute__(( reqd_work_group_size(1, 1, 1)))
kernel void ev_inu_ocl_logistic_kernel(	global ocl_ctl_logistic  	* ctl,
										global float 			    * restrict g_dataOut,
										global void 			    * restrict g_dataIn )
{

	int in_len = ctl->in_vec_len;
		
  	local void   *	in_ptr;	     // each ptr for a channel	
  	local float  *	out_ptr;     // output interleaved

	int var_space_size = ctl->local_mem_size - (4*1024) -(LOGISTIC_LUT_SIZE*sizeof(float)); 
	int element_size   = ctl->element_size;
	int max_tile_len   = var_space_size/(element_size + sizeof(float)); 	// input @ short/char and output @ float.
	    max_tile_len   = max_tile_len & (~0x3f);
	
  	local void  * local_mem_ptr 	= (local void *)ctl->local_mem_ptr;

	// load tanh LUT to start of VMEM
	local float *  l_logistic_lut = (local float *)ctl->local_mem_ptr;
	event_t load_lut_event = 0;
	load_lut_event = async_work_group_copy((local float *)ctl->local_mem_ptr,  (global float *)ctl->logistic_lut_ptr , LOGISTIC_LUT_SIZE ,   0);
	wait_group_events(1, &load_lut_event);
	local_mem_ptr  +=  LOGISTIC_LUT_SIZE*sizeof(float);
	
	in_ptr 			=  (local void  *	)local_mem_ptr;  
	local_mem_ptr  +=  max_tile_len * element_size;
	out_ptr     	=  (local float  *	)local_mem_ptr;
	local_mem_ptr  +=   max_tile_len*sizeof(float);

#ifdef OCL_logistic_PRINT_EN_DEBUG_
	printf("INFO: global input at   = %p\n", g_dataIn);
	printf("INFO: global output at  = %p\n", g_dataOut);
	printf("INFO: local logistic_lut at  = %p\n", l_logistic_lut);
		
	printf("INFO: EVSS_CFG_VCCM_SIZE=%i \n", ctl->local_mem_size);
	printf("INFO: max_tile_len = %d ,element_size =%d \n",max_tile_len, element_size);
	printf("INFO: vmem in_ptr   at %p\n", in_ptr);		
	printf("INFO: vmem out_ptr  at %p\n", out_ptr);		

  	printf("INFO: end buffers allocation %p\n",  local_mem_ptr);
  	printf("INFO: vmem for stack and variables =%d \n",  ((int)ctl->local_mem_ptr+ ctl->local_mem_size-(int)local_mem_ptr));
#endif

    local ocl_logistic_buffer_t buffer;
	//  init buffers
	// --------------
	// global buffers
	buffer.g_in_ptr	    = (global void  * )g_dataIn;
	buffer.g_out_ptr	= (global float * )g_dataOut;
	buffer.in_ptr	    = in_ptr;  
	buffer.out_ptr 		= out_ptr;
	buffer.in_vec_len   = in_len;
	buffer.max_tile_vec_len = max_tile_len;
	buffer.element_size = element_size;
    buffer.logistic_lut_ptr  =  l_logistic_lut;
	buffer.load_event	   = -1;
	buffer.store_event     = -1;

	int num_tiles   = ( in_len + max_tile_len-1)/max_tile_len;

    int scale_factor     = ctl->scale_factor;
    int scale_shift_bits = ctl->scale_shift_bits;

	int num_remainder_inputs = in_len;

	for (int tile_num=0; tile_num<num_tiles; tile_num++)
	{
	
		int num_inputs2load   = min(max_tile_len,num_remainder_inputs);
		int start_input2load = (tile_num)*max_tile_len;
		ocl_logistic_load_buffer(&buffer,	start_input2load,num_inputs2load ,tile_num ); 
		num_remainder_inputs = num_remainder_inputs - num_inputs2load;
		
		//wait to tile at curr_buff, to loading end
		wait_group_events(1, &buffer.load_event);
		
		// process tile
		if (element_size ==1)
			{
			ocl_logistic_process_char_buffer(&buffer , scale_factor , scale_shift_bits);
			}
		else if (element_size==2)
			{
			ocl_logistic_process_short_buffer(&buffer , scale_factor , scale_shift_bits);
			}
		
		// store tile
		ocl_logistic_store_buffer(&buffer);
	
		// store wait
		wait_group_events(1, &buffer.store_event);

	}

    
}