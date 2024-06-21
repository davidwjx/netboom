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
#include "inu_softmax.h"

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
	global 	short 			* g_out_ptr;
	
	local 	short       	* exp_lut_ptr;  
	local 	short        	* in_ptr;  
	local 	short 			* out_ptr;	

	int   	in_vec_len;
	int   	num_groups;
	int   	num_classes_plus1;
	
	int   	tile_num;
	int   	tile_vec_len;
	int   	tile_vec_start;
    int     num_groups_at_tile;	
    int     max_groups_at_tile;	
		
	event_t       load_event;
	event_t       store_event;
} ocl_softmax_buffer_t;

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
void ocl_softmax_load_buffer(local ocl_softmax_buffer_t  * buffer,	int  start_input2load, int num_groups2load ,int tile_num)
{

    int     num_inputs2load 	= num_groups2load*buffer->num_classes_plus1;
    buffer->tile_vec_start		= start_input2load;
    buffer->num_groups_at_tile	= num_groups2load;
    buffer->tile_vec_len		= num_inputs2load;
    buffer->tile_num	    	= tile_num;

 	//global short * g_short_ptr  = (global short *)buffer->g_in_ptr;
	//buffer->load_event = async_work_group_copy((local short *)buffer->in_ptr,  &g_short_ptr[start_input2load], num_inputs2load,   0);
	buffer->load_event = async_work_group_copy(buffer->in_ptr,  &buffer->g_in_ptr[start_input2load], num_inputs2load,   0);
	
}
// ************************************************************************
__attribute__(( reqd_work_group_size(1, 1, 1)))
__kernel void ocl_softmax_store_buffer(local ocl_softmax_buffer_t  * buffer )
{
    int  num_samples2store = buffer->tile_vec_len;
	int  out_offset        = buffer->tile_vec_start;
    // printf("store: num_samples2store = %d \n", num_samples2store);		

	buffer->store_event = async_work_group_copy(&buffer->g_out_ptr[out_offset],  &buffer->out_ptr[0], num_samples2store,   0);	
}
//===============================================================================
static
void scale_fit(float scale, short abs_bits, short *int_scale, short *frac_bits)
{
    if (fabs(1.f - scale) < FLT_EPSILON) {
        *int_scale = (1 << abs_bits) - 1;
        *frac_bits = abs_bits;
        return;
    }

    int exp_val;
    *int_scale = frexp(scale, &exp_val) * (1 << abs_bits);
    *frac_bits = abs_bits - exp_val;
}	
// ***********************************************************************
__attribute__(( reqd_work_group_size(1, 1, 1)))
__kernel void ocl_softmax_process_buffer(local ocl_softmax_buffer_t  * 	buffer,
										        int 						scale_factor , 
												int 						scale_shift_bits)
{

	int num_groups 			= buffer->num_groups_at_tile;
	int num_classes_plus1 	= buffer->num_classes_plus1;
	//int tile_vec_len 		= buffer->tile_vec_len;
	local 	short  * in_ptr = buffer->in_ptr;
	local 	short  *out_ptr = buffer->out_ptr;

	int num_full_32classes_iter = num_classes_plus1>>5;
	int num_partial_32classes_iter = num_classes_plus1&0x1f;
	short32 indx = (short32)( 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15,
							 16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31);
	short32 mask = indx<(short32)num_partial_32classes_iter;
								 
	#ifdef OCL_softmax_PRINT_EN_DEBUG_
    printf(" num_full_32classes_iter = %d\n",  num_full_32classes_iter);
    printf(" num_partial_32classes_iter = %d\n",  num_partial_32classes_iter);
    printf(" mask = %#v32hd\n",  mask);
	#endif
	
	for (int group = 0; group<num_groups  ; group++)
	{
	
		// find the max class val
		int start = group*num_classes_plus1;
		short32 max_vec = (short)(-32768);
		short32 v , d;
		short32 exp_vec;
		for (int class_iter=0; class_iter<num_full_32classes_iter; class_iter++)
		{
            v 		= vload32(class_iter, &in_ptr[start]);
            max_vec = max(v, max_vec);
			#ifdef OCL_softmax_PRINT_EN_DEBUG_
			if(group==0){
				printf(" v       = %#v32hd\n",  v);
				printf(" max_vec = %#v32hd\n",  max_vec);
			}
			#endif
		}
				v = vload32(num_full_32classes_iter, &in_ptr[start]);
		        v = v & mask;
        max_vec = max(v, max_vec);
        short max_val = (short) reduc_max(max_vec);

		#ifdef OCL_softmax_PRINT_EN_DEBUG_
		if(group==0){
			printf(" v       = %#v32hd\n",  v);
			printf(" max_vec = %#v32hd\n",  max_vec);
			printf(" max_val = %d\n",  max_val);
		}
		#endif


		// substruct max_val + find exponent and exp valuestrucet exp  + sum all classes exp at group
		acc_type sum = acc_init(0);
		for (int class_iter=0; class_iter<num_full_32classes_iter; class_iter++)
		{
			v 		= vload32(class_iter, &in_ptr[start]);
            d 		= v - (short32)max_val;		
			
			// scale input to lut resolution
			int16 dala16_lo = (convert_int16(d.lo) * (int16)scale_factor)>> scale_shift_bits; 
			int16 dala16_hi = (convert_int16(d.hi) * (int16)scale_factor)>> scale_shift_bits;	

		    d.lo            =  convert_short16(dala16_lo);
		    d.hi            =  convert_short16(dala16_hi);
		    d 		= d + (short32)(EXP_LUT_SHIFT);
		    d 		= clamp(d , 0, EXP_LUT_SIZE-1);
			exp_vec = vgather32(buffer->exp_lut_ptr, convert_int16(d.lo),convert_int16(d.hi));
			acc_mac(&sum, exp_vec, (short32) 1); // sum(exp(v[i] - max))
			vstore32(exp_vec,class_iter,&out_ptr[start]);

			#ifdef OCL_softmax_PRINT_EN_DEBUG_
			if(group==0){
				printf(" v       = %#v32hd\n",  v);
				printf(" d       = %#v32hd\n",  d);
				printf(" exp_vec = %#v32hd\n",  exp_vec);
			}
			#endif
		}
		v 		= vload32(num_full_32classes_iter, &in_ptr[start]);
        d 		= v - (short32)max_val;		
		
		// scale input to lut resolution
		int16 dala16_lo = (convert_int16(d.lo) * (int16)scale_factor)>> scale_shift_bits; 
		int16 dala16_hi = (convert_int16(d.hi) * (int16)scale_factor)>> scale_shift_bits;	

		d.lo            =  convert_short16(dala16_lo);
		d.hi            =  convert_short16(dala16_hi);
		d 		= d + (short32)(EXP_LUT_SHIFT);
		d 		= clamp(d , 0, EXP_LUT_SIZE-1);
		exp_vec = vgather32(buffer->exp_lut_ptr, convert_int16(d.lo),convert_int16(d.hi));
		exp_vec = exp_vec & mask;
		acc_mac(&sum, exp_vec, (short32) 1); // sum(exp(v[i] - max))
		vstore32(exp_vec,num_full_32classes_iter,&out_ptr[start]);

		#ifdef OCL_softmax_PRINT_EN_DEBUG_
		if(group==0){
			printf(" v       = %#v32hd\n",  v);
			printf(" d       = %#v32hd\n",  d);
			printf(" exp_vec = %#v32hd\n",  exp_vec);
		}
		#endif

       uint sum_value = reduc_add(acc_convert(acc_lo(sum))) + reduc_add(acc_convert(acc_hi(sum)));

       // calc norm factor
		short norm_factor =1;
		short norm_shift_bits=2;
		float scale_f = (float)(1<<Q_OUT)/sum_value;
		scale_fit(scale_f, 15, &norm_factor, &norm_shift_bits);

		#ifdef OCL_softmax_PRINT_EN_DEBUG_	
		if (group==0){
			printf(" sum_value = %d\n",sum_value);
			printf(" scale_f = %f\n",scale_f);
			printf(" norm_factor = %d\n",norm_factor);
			printf(" norm_shift_bits = %d\n",norm_shift_bits);
		}			
		#endif
		
	// norm output
	for (int class_iter=0; class_iter<=num_full_32classes_iter; class_iter++)
		{
			v 		= vload32(class_iter, &out_ptr[start]);
			int16 dala16_lo = (convert_int16(v.lo) * (int16)norm_factor); 
			int16 dala16_hi = (convert_int16(v.hi) * (int16)norm_factor);	
			      dala16_lo = (dala16_lo + (1<<(norm_shift_bits-1))) >> norm_shift_bits; 
			      dala16_hi = (dala16_hi + (1<<(norm_shift_bits-1))) >> norm_shift_bits;	
		    d.lo            =  convert_short16(dala16_lo);
		    d.hi            =  convert_short16(dala16_hi);
			vstore32(d,class_iter,&out_ptr[start]);
			#ifdef OCL_softmax_PRINT_EN_DEBUG_	
			if (group==0){
				printf(" v      = %#v32hd\n",v);
				printf(" output = %#v32hd\n",d);
			}
			#endif		
		}	
	}
}
// ***********************************************************************
__attribute__(( reqd_work_group_size(1, 1, 1)))
__attribute__(( reqd_work_group_size(1, 1, 1)))
kernel void ev_inu_ocl_softmax_kernel(	global ocl_ctl_softmax  	* ctl,
										global short 			    * restrict g_dataOut,
										global short 			    * restrict g_dataIn )
{

  	float 	split_th 			= ctl->split_th;  
	bool	is_first_section 	= ctl->is_first_section;
	int num_groups 			= ctl->num_groups;
	int num_classes_plus1 	= ctl->num_classes_plus1;
	
	//split the group among the cores
		int first_sec_num_groups = (int)(split_th*num_groups);	
		int sec_len ,sec_num_groups;	
	int sec_first_sample;
	if (is_first_section) {
		sec_first_sample = 0;
		sec_num_groups = first_sec_num_groups;
	    sec_len  = sec_num_groups * num_classes_plus1;
	}
	else {
		sec_first_sample = first_sec_num_groups * num_classes_plus1;
		sec_num_groups = num_groups - first_sec_num_groups;
	    sec_len  = sec_num_groups * num_classes_plus1;
	}

  	local short  *	in_ptr;	     	
  	local short  *	out_ptr;     

	int var_space_size = ctl->local_mem_size - (4*1024) - (EXP_LUT_SIZE*sizeof(short)); 
	int max_tile_len   = var_space_size/(sizeof(short) + sizeof(short)); 	// input @ short and output @ short.
	    max_tile_len   = max_tile_len & (~0x3f);
	int max_groups_at_tile = max_tile_len/num_classes_plus1;
	
  	local short  * local_mem_ptr 	= (local short *)ctl->local_mem_ptr;

	// load exp LUT to start of VMEM
	local short *  l_exp_lut = (local short *)ctl->local_mem_ptr;
	event_t load_lut_event = 0;
	load_lut_event = async_work_group_copy((local short *)ctl->local_mem_ptr,  (global short *)ctl->exp_lut_ptr , EXP_LUT_SIZE ,   0);
	wait_group_events(1, &load_lut_event);
	local_mem_ptr  +=  EXP_LUT_SIZE;

	in_ptr 			=  (local short  *	)local_mem_ptr;  
	local_mem_ptr  +=  max_tile_len;
	out_ptr     	=  (local short  *	)local_mem_ptr;
	local_mem_ptr  +=   max_tile_len;

	int num_tiles   = ( sec_num_groups + max_groups_at_tile-1)/max_groups_at_tile;

#ifdef OCL_softmax_PRINT_EN_DEBUG_
	printf("INFO: global input at   = %p\n", g_dataIn);
	printf("INFO: global output at  = %p\n", g_dataOut);
	printf("INFO: local logistic_lut at  = %p\n", l_exp_lut);
		
	printf("INFO: EVSS_CFG_VCCM_SIZE=%i \n", ctl->local_mem_size);
	printf("INFO: max_tile_len = %d  \n",max_tile_len);
	printf("INFO: num_tiles    = %d  \n",num_tiles);
	printf("INFO: vmem in_ptr   at %p\n", in_ptr);		
	printf("INFO: vmem out_ptr  at %p\n", out_ptr);		
	printf("INFO: max_groups_at_tile %d\n", max_groups_at_tile);		

  	printf("INFO: end buffers allocation %p\n",  local_mem_ptr);
  	printf("INFO: vmem for stack and variables =%d \n",  ((int)ctl->local_mem_ptr+ ctl->local_mem_size-(int)local_mem_ptr));
#endif

    local ocl_softmax_buffer_t buffer;
	//  init buffers
	// --------------
	// global buffers
	buffer.g_in_ptr	    = (global short  * )g_dataIn;
	buffer.g_out_ptr	= (global short * )g_dataOut;
	buffer.in_ptr	    = in_ptr;  
	buffer.out_ptr 		= out_ptr;
	buffer.in_vec_len   = sec_len;
	buffer.num_groups   = sec_num_groups;
	buffer.num_classes_plus1   = num_classes_plus1;
	buffer.max_groups_at_tile = max_groups_at_tile;
    buffer.exp_lut_ptr  =  l_exp_lut;
	buffer.load_event	= -1;
	buffer.store_event  = -1;
	
    int scale_factor     = ctl->scale_factor;
    int scale_shift_bits = ctl->scale_shift_bits;

	int num_remainder_groups = sec_num_groups;

	for (int tile_num=0; tile_num<num_tiles; tile_num++)
	{
	
		int num_groups2load   = min(max_groups_at_tile,num_remainder_groups);
		int start_input2load = (tile_num)*max_groups_at_tile*num_classes_plus1 +  sec_first_sample;
		ocl_softmax_load_buffer(&buffer, start_input2load,num_groups2load ,tile_num ); 
		num_remainder_groups = num_remainder_groups - num_groups2load;
		
		//wait to tile at curr_buff, to loading end
		wait_group_events(1, &buffer.load_event);
		
		// process tile
		ocl_softmax_process_buffer(&buffer , scale_factor , scale_shift_bits);
		
		// store tile
		ocl_softmax_store_buffer(&buffer);
	
		// store wait
		wait_group_events(1, &buffer.store_event);

	}

    
}

// ***********************************************************************
