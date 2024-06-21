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
#include "inu_resize_f.h"

#define  OCL_bokeh_SIMD_WIDTH 32

#if ( __Xvec_guard_bit_option == 0 )
#define acc_convert_int16(acc)  acc
#endif

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

void evYieldThread(void);

#define AUX_RTC_LOW                    (0x104)
#define AUX_RTC_HIGH                   (0x105)
#define MAX_GAUSSIAN_WIDTH				1024

//// ***********************************************************************
#define      STEP_X 32
//#define      DBG_LINE   523
//#define      DBG_COL_S  416
// ***********************************************************************
#define RFRAC 14
#define FRAC_ONE (1 << RFRAC)
#define RMASK (FRAC_ONE - 1)
#define FRAC_FLOOR(x) (x >> RFRAC)
// ***********************************************************************

typedef struct
{
	global 	float 		* g_float_out_ptr; //float
	global 	half 		* g_fp16_out_ptr; //float

	local void          * in_pixels[4];   // 4 buffers used as input for resize per input channel
	local uchar 		 * out_pixel[2];	 // 2 output buffers after resize, per output channel
	
	int 	in_height;  		// input height	
 	int 	in_width;  		    // input width	
	
	int 	out_height;   
	int 	out_width;
	
	float 	priority_th;  
	int 	class_id;  
	int 	element_size;  
	
} ocl_resize_f_buffer_t;
// ***********************************************************************

#define STEP_XX 64


// ***********************************************************************
__attribute__(( reqd_work_group_size(1, 1, 1)))
__kernel void ocl_resize_float2uchar_proc(global ocl_ctl_resize_f_planar 	* ctl,
										  local  ocl_resize_f_buffer_t   	* buffer)
{

    //Get kernel parameters from the control structure
    int in_width     = buffer->in_width;
    int in_height    = buffer->in_height;
    int out_width    = buffer->out_width;
    int out_height   = buffer->out_height;

    float float_x_ratio = (float)(in_width )/out_width;
    float float_y_ratio = (float)(in_height)/out_height;
	
    float y_float_offset = ( float_y_ratio - 1)/2;   			

	// implementation with align_corners=false
	//printf("float_x_ratio =%.15f, float_y_ratio =%.15f \n",float_x_ratio , float_y_ratio);
	//printf("y_float_offset =%.15f \n",y_float_offset);
	
    //Local line buffers, that will be filled/emptied using async_work_group_copy
    local float  * rdLine_in[4];  
	for (int buf=0; buf<4; buf++)
	{
		rdLine_in[buf] = (local float*)buffer->in_pixels[buf];
	}

    local uchar  * wrLine_out[2];
	for (int buf=0; buf<2; buf++)
	{
		wrLine_out[buf] = (local unsigned char *)buffer->out_pixel[buf];
	}
	
   	// input output global pointers
    global float 	* restrict  g_ptr_in_ch;
	g_ptr_in_ch     = (global float * )ctl->in_ptr;

    global unsigned char 	* restrict  g_ptr_out_ch;
	g_ptr_out_ch       = (global unsigned char * )ctl->out_ptr;

	int first_in_row 	= 0;
    int first_out_row 	= 0;
	int last_out_row	= out_height;
  
 	int load_buff        = first_in_row & 0x3;
 	int load_buff_plus_1 = (first_in_row+1) & 0x3;
 
    //Read first two rows of image into input buffers for resize
    event_t e_rdLine[4]    ={0,0,0,0};
    event_t e_wrLine[2]    ={0,0};

	e_rdLine[load_buff]           = async_work_group_copy(rdLine_in[load_buff]         ,&g_ptr_in_ch[first_in_row*in_width]       , in_width, 0);
	e_rdLine[load_buff_plus_1]    = async_work_group_copy(rdLine_in[load_buff_plus_1]  ,&g_ptr_in_ch[(first_in_row+1)*in_width]   , in_width, 0);

 	wait_group_events(1, &e_rdLine[load_buff]);
 	wait_group_events(1, &e_rdLine[load_buff_plus_1]);

 	int last_loaded_in_row = (first_in_row+1);
	
    float16   vrx_first16 = (float16)( ( 0*float_x_ratio +(float_x_ratio-1)/2),
								       ( 1*float_x_ratio +(float_x_ratio-1)/2),
								       ( 2*float_x_ratio +(float_x_ratio-1)/2),
								       ( 3*float_x_ratio +(float_x_ratio-1)/2),
								       ( 4*float_x_ratio +(float_x_ratio-1)/2),
								       ( 5*float_x_ratio +(float_x_ratio-1)/2),
								       ( 6*float_x_ratio +(float_x_ratio-1)/2),
								       ( 7*float_x_ratio +(float_x_ratio-1)/2),
								       ( 8*float_x_ratio +(float_x_ratio-1)/2),
								       ( 9*float_x_ratio +(float_x_ratio-1)/2),
								       (10*float_x_ratio +(float_x_ratio-1)/2),
								       (11*float_x_ratio +(float_x_ratio-1)/2),
								       (12*float_x_ratio +(float_x_ratio-1)/2),
								       (13*float_x_ratio +(float_x_ratio-1)/2),
								       (14*float_x_ratio +(float_x_ratio-1)/2),
								       (15*float_x_ratio +(float_x_ratio-1)/2));
	#ifdef OCL_RESIZE_F_PRINT_EN_DEBUG_
		printf(" vrx_first16    = %#v16hf\n", vrx_first16);
	#endif
	
    int prev_bottom_buffer = load_buff;
    int curr_out_buf = 0;
	
	float  priority_th = buffer->priority_th;
	int    class_id    = buffer->class_id;

    // high rate rows loop
    for (int y_out = first_out_row ; y_out < last_out_row ; y_out++)  
    {
	    float y_float =  float_y_ratio*y_out + y_float_offset;
	          y_float =  max(y_float,(float)0);
		int   iy2     = (int)(y_float);

        int iy2_T_buff = iy2 & 3;  	   						// modulo 4
        int iy2_B_buff = min(iy2+1,in_height-1) & 3;       	// modulo 4

		// set pointers
        local float  * l0_in_pixel;
        local float  * l1_in_pixel;

        l0_in_pixel   = rdLine_in[iy2_T_buff];
        l1_in_pixel   = rdLine_in[iy2_B_buff];

		//duplicate border to eliminate check end line
		l0_in_pixel[in_width]  = l0_in_pixel[in_width-1];
		l1_in_pixel[in_width]  = l1_in_pixel[in_width-1];
		
	
		float beta0 = y_float - (float)iy2;
        float beta1 = 1       -  beta0;

		// wait read 
        if (prev_bottom_buffer!= iy2_B_buff) {
			prev_bottom_buffer = iy2_B_buff;
			wait_group_events(1, &e_rdLine[iy2_B_buff]);
#ifdef OCL_RESIZE_F_PRINT_EN_DEBUG_  
		printf(" wait_group_events, iy2_B_buff=%d\n", iy2_B_buff);			
#endif		
		}

		int in_line2load = iy2+2;
		if ( (in_line2load>last_loaded_in_row)&& (in_line2load<in_height) ){
			// read A1 & b1 lines
			last_loaded_in_row++;
			int load_buff = last_loaded_in_row&3;
			
			e_rdLine[load_buff] = async_work_group_copy(rdLine_in[load_buff],  &g_ptr_in_ch[  last_loaded_in_row*in_width], in_width, 0);
		}


		for (int x_out = 0; x_out < out_width; x_out+=64) {
           float16 q[4][4]; 		// first index 0:3, determain the 16 group, coef for: 0=tl,1=tr,2-bl,3,br,  
           int16   vix_left[4];     // indexes at the line. 0:3 determain the group of 16 indexes
				   
			for (int i=0; i<4; i++) {
		        float16 vrx = vrx_first16 + (float16)((float)(x_out+i*16)*float_x_ratio);
		                vrx = fmax(vrx,0);
		   
				float16 vix_left_f;
				float16 alpha0 = fract(vrx , &vix_left_f);
				float16 alpha1 = 1 - alpha0; 
		
				vix_left[i]  = convert_int16(vix_left_f);
				q[i][0] = (alpha1 * beta1); // top left
				q[i][1] = (alpha0 * beta1); // top rigth
				q[i][2] = (alpha1 * beta0); // bot left
				q[i][3] = (alpha0 * beta0); // bot right
				#ifdef OCL_RESIZE_F_PRINT_EN_DEBUG_
				if((y_out==0)&&(x_out==0))	{
					printf(" i=%d,vrx    	= %#v16hf\n", i,vrx);
					printf(" i=%d,vix_left_f= %#v16hf\n", i,vix_left_f);
					printf(" i=%d,vix_left[i]  = %#v16hd\n", i,vix_left[i]);
					printf(" i=%d,alpha0    = %#v16hf\n", i,alpha0);
					printf(" i=%d,alpha1    = %#v16hf\n", i,alpha1);
					printf(" i=%d,q[i][0]   = %#v16hf\n", i,q[i][0]);
					printf(" i=%d,q[i][1]   = %#v16hf\n", i,q[i][1]);
					printf(" \n\n\n");
				}
				#endif
		   }

			// read 4 vectors of 64 pixels from input for TL,TR,BL and BR
			// first 2 digits - 00=TL,01=TR,10,BL,11=BR, the last digit for number of 16 vec at the 64 vec
			//---------------------------------------------------------------------------------------------
			float16 i00_0 = vgather16( l0_in_pixel,        vix_left[0]);
			float16 i00_1 = vgather16( l0_in_pixel,        vix_left[1]);
			float16 i00_2 = vgather16( l0_in_pixel,        vix_left[2]);
			float16 i00_3 = vgather16( l0_in_pixel,        vix_left[3]);
																   
			float16 i01_0 = vgather16( &l0_in_pixel[1],    vix_left[0]);
			float16 i01_1 = vgather16( &l0_in_pixel[1],    vix_left[1]);
			float16 i01_2 = vgather16( &l0_in_pixel[1],    vix_left[2]);
			float16 i01_3 = vgather16( &l0_in_pixel[1],    vix_left[3]);
																   
			float16 i10_0 = vgather16( l1_in_pixel,        vix_left[0]);
			float16 i10_1 = vgather16( l1_in_pixel,        vix_left[1]);
			float16 i10_2 = vgather16( l1_in_pixel,        vix_left[2]);
			float16 i10_3 = vgather16( l1_in_pixel,        vix_left[3]);
																   
			float16 i11_0 = vgather16( &l1_in_pixel[1],    vix_left[0]);
			float16 i11_1 = vgather16( &l1_in_pixel[1],    vix_left[1]);
			float16 i11_2 = vgather16( &l1_in_pixel[1],    vix_left[2]);
			float16 i11_3 = vgather16( &l1_in_pixel[1],    vix_left[3]);

			//float16 i00_0 = (float16)(0.8f);
			float16 res_0 = i00_0 * q[0][0] + i01_0 * q[0][1] + i10_0 * q[0][2] + i11_0 * q[0][3];
			float16 res_1 = i00_1 * q[1][0] + i01_1 * q[1][1] + i10_1 * q[1][2] + i11_1 * q[1][3];
			float16 res_2 = i00_2 * q[2][0] + i01_2 * q[2][1] + i10_2 * q[2][2] + i11_2 * q[2][3];
			float16 res_3 = i00_3 * q[3][0] + i01_3 * q[3][1] + i10_3 * q[3][2] + i11_3 * q[3][3];

			#ifdef OCL_RESIZE_F_PRINT_EN_DEBUG_
				if((y_out==0)&&(x_out==0))	{
				printf(" res_0   = %#v16hf\n",res_0);			
				printf(" res_1   = %#v16hf\n",res_1);			
				printf(" res_2   = %#v16hf\n",res_2);			
				printf(" res_3   = %#v16hf\n",res_3);			
				}
			#endif
			
if(y_out<0) {
	printf("fffffff \n");
}

			int16 above_th0 = isgreater(res_0, priority_th);
			int16 above_th1 = isgreater(res_1, priority_th);
			int16 above_th2 = isgreater(res_2, priority_th);
			int16 above_th3 = isgreater(res_3, priority_th);

			char64 	tmp_out;
			uchar64 out_class;
			tmp_out.lo.lo = convert_char16(above_th0)*(char)class_id;
			tmp_out.lo.hi = convert_char16(above_th1)*(char)class_id;
			tmp_out.hi.lo = convert_char16(above_th2)*(char)class_id;
			tmp_out.hi.hi = convert_char16(above_th3)*(char)class_id;
			out_class.lo       = abs(tmp_out.lo);
			out_class.hi       = abs(tmp_out.hi);

			#ifdef OCL_RESIZE_F_PRINT_EN_DEBUG_			
			if((y_out==0)&&(x_out==0))	{
				printf(" res_0    	= %#v16hf\n",res_0);			
				printf(" above_th0  = %#v16hd\n",above_th0);			
				printf(" out_class  = %#v32hd\n",out_class.lo);			
				printf(" out_class  = %#v32hd\n",out_class.hi);			
				printf(" class_id   = %d\n",class_id);					
			}
			#endif
			vstore64( out_class, 0, &wrLine_out[curr_out_buf][x_out]);
		}	// end of for (int x_out = 0; x_out < out_width; x_out+=64)

		e_wrLine[curr_out_buf]     = async_work_group_copy(&g_ptr_out_ch[y_out*out_width],  wrLine_out[curr_out_buf],   out_width, 0);

		if (y_out>(first_out_row+1)){
			wait_group_events(1, &e_wrLine[1-curr_out_buf]);
		}

		// swap hr buffers
		curr_out_buf = 1-curr_out_buf;
	} // end of for (int y_out = first_out_row ; y_out < last_out_row ; y_out++)
	wait_group_events(1, &e_wrLine[1-curr_out_buf]);
	wait_group_events(1, &e_wrLine[curr_out_buf]);	
}
// ***********************************************************************
__attribute__(( reqd_work_group_size(1, 1, 1)))
__kernel void ocl_resize_float2uchar_proc2(global ocl_ctl_resize_f_planar 	* ctl,
										  local  ocl_resize_f_buffer_t   	* buffer)
{
    //Get kernel parameters from the control structure
    int in_width     = buffer->in_width;
    int in_height    = buffer->in_height;
    int out_width    = buffer->out_width;
    int out_height   = buffer->out_height;

    float float_x_ratio = (float)(in_width )/out_width;
    float float_y_ratio = (float)(in_height)/out_height;
	
    float y_float_offset = ( float_y_ratio - 1)/2;   			

	// implementation with align_corners=false
	//printf("float_x_ratio =%.15f, float_y_ratio =%.15f \n",float_x_ratio , float_y_ratio);
	//printf("y_float_offset =%.15f \n",y_float_offset);
	
    //Local line buffers, that will be filled/emptied using async_work_group_copy
    local float  * rdLine_in[4];  
	for (int buf=0; buf<4; buf++)
	{
		rdLine_in[buf] = (local float*)buffer->in_pixels[buf];
	}

    local uchar  * wrLine_out[2];
	for (int buf=0; buf<2; buf++)
	{
		wrLine_out[buf] = (local unsigned char *)buffer->out_pixel[buf];
	}
	
   	// input output global pointers
    global float 	* restrict  g_ptr_in_ch;
	g_ptr_in_ch     = (global float * )ctl->in_ptr;

    global unsigned char 	* restrict  g_ptr_out_ch;
	g_ptr_out_ch       = (global unsigned char * )ctl->out_ptr;


	int first_in_row, first_out_row, last_out_row;
  	int hr_row_threshold    = (int)(ctl->split_th *out_height);
  	bool first_section_flag = ctl->is_first_section;
  
    if (first_section_flag) { 		// first section
  		first_in_row = 0;  
  		first_out_row = 0;
  		last_out_row  = hr_row_threshold;
  	}
  	else {  // last section
  	    float y_float =  float_y_ratio*hr_row_threshold + y_float_offset;
  	          y_float =  max(y_float,(float)0);
  		first_in_row  = (int)(y_float); 
  		first_out_row =  hr_row_threshold;
  		last_out_row   =  out_height;
  	}
  
 	int load_buff        = first_in_row & 0x3;
 	int load_buff_plus_1 = (first_in_row+1) & 0x3;
 
    //Read first two rows of image into input buffers for resize
    event_t e_rdLine[4]    ={0,0,0,0};
    event_t e_wrLine[2]    ={0,0};

	e_rdLine[load_buff]           = async_work_group_copy(rdLine_in[load_buff]         ,&g_ptr_in_ch[first_in_row*in_width]       , in_width, 0);
	e_rdLine[load_buff_plus_1]    = async_work_group_copy(rdLine_in[load_buff_plus_1]  ,&g_ptr_in_ch[(first_in_row+1)*in_width]   , in_width, 0);

 	wait_group_events(1, &e_rdLine[load_buff]);
 	wait_group_events(1, &e_rdLine[load_buff_plus_1]);

 	int last_loaded_in_row = (first_in_row+1);
	
    half32   vrx_first32 = (half32)( ( 0*float_x_ratio +(float_x_ratio-1)/2),
								       ( 1*float_x_ratio +(float_x_ratio-1)/2),
								       ( 2*float_x_ratio +(float_x_ratio-1)/2),
								       ( 3*float_x_ratio +(float_x_ratio-1)/2),
								       ( 4*float_x_ratio +(float_x_ratio-1)/2),
								       ( 5*float_x_ratio +(float_x_ratio-1)/2),
								       ( 6*float_x_ratio +(float_x_ratio-1)/2),
								       ( 7*float_x_ratio +(float_x_ratio-1)/2),
								       ( 8*float_x_ratio +(float_x_ratio-1)/2),
								       ( 9*float_x_ratio +(float_x_ratio-1)/2),
								       (10*float_x_ratio +(float_x_ratio-1)/2),
								       (11*float_x_ratio +(float_x_ratio-1)/2),
								       (12*float_x_ratio +(float_x_ratio-1)/2),
								       (13*float_x_ratio +(float_x_ratio-1)/2),
								       (14*float_x_ratio +(float_x_ratio-1)/2),
								       (15*float_x_ratio +(float_x_ratio-1)/2),
                                       (16*float_x_ratio +(float_x_ratio-1)/2),
								       (17*float_x_ratio +(float_x_ratio-1)/2),
								       (18*float_x_ratio +(float_x_ratio-1)/2),
								       (19*float_x_ratio +(float_x_ratio-1)/2),
								       (20*float_x_ratio +(float_x_ratio-1)/2),
								       (21*float_x_ratio +(float_x_ratio-1)/2),
								       (22*float_x_ratio +(float_x_ratio-1)/2),
								       (23*float_x_ratio +(float_x_ratio-1)/2),
								       (24*float_x_ratio +(float_x_ratio-1)/2),
								       (25*float_x_ratio +(float_x_ratio-1)/2),
								       (26*float_x_ratio +(float_x_ratio-1)/2),
								       (27*float_x_ratio +(float_x_ratio-1)/2),
								       (28*float_x_ratio +(float_x_ratio-1)/2),
								       (29*float_x_ratio +(float_x_ratio-1)/2),
								       (30*float_x_ratio +(float_x_ratio-1)/2),
								       (31*float_x_ratio +(float_x_ratio-1)/2));
	#ifdef OCL_RESIZE_F_PRINT_EN_DEBUG_
		printf(" vrx_first16    = %#v16hf\n", vrx_first16);
	#endif
	
    int prev_bottom_buffer = load_buff;
    int curr_out_buf = 0;
	
	half  priority_th1  = (half)buffer->priority_th;
	int    class_id    = buffer->class_id;

    // high rate rows loop
    for (int y_out = first_out_row ; y_out < last_out_row ; y_out++)  
    {
	    float y_float =  float_y_ratio*y_out + y_float_offset;
	          y_float =  max(y_float,(float)0);
		int   iy2     = (int)(y_float);

        int iy2_T_buff = iy2 & 3;  	   						// modulo 4
        int iy2_B_buff = min(iy2+1,in_height-1) & 3;       	// modulo 4

		// set pointers
        local float  * l0_in_pixel;
        local float  * l1_in_pixel;

        l0_in_pixel   = rdLine_in[iy2_T_buff];
        l1_in_pixel   = rdLine_in[iy2_B_buff];

		//duplicate border to eliminate check end line
		l0_in_pixel[in_width]  = l0_in_pixel[in_width-1];
		l1_in_pixel[in_width]  = l1_in_pixel[in_width-1];
		
	
		half  beta0 = y_float - (half)iy2;
        half  beta1 = 1       -  beta0;

		// wait read 
        if (prev_bottom_buffer!= iy2_B_buff) {
			prev_bottom_buffer = iy2_B_buff;
			wait_group_events(1, &e_rdLine[iy2_B_buff]);
#ifdef OCL_RESIZE_F_PRINT_EN_DEBUG_  
		printf(" wait_group_events, iy2_B_buff=%d\n", iy2_B_buff);			
#endif		
		}

		int in_line2load = iy2+2;
		if ( (in_line2load>last_loaded_in_row)&& (in_line2load<in_height) ){
			// read A1 & b1 lines
			last_loaded_in_row++;
			int load_buff = last_loaded_in_row&3;
			
			e_rdLine[load_buff] = async_work_group_copy(rdLine_in[load_buff],  &g_ptr_in_ch[  last_loaded_in_row*in_width], in_width, 0);
		}


		for (int x_out = 0; x_out < out_width; x_out+=64) {
           half32  q[2][4]; 		// first index 0:2, determain the 32 group, coef for: 0=tl,1=tr,2-bl,3,br,  
           int16   vix_left[4];     // indexes at the line. 0:3 determain the group of 16 indexes
				   
			for (int i=0; i<2; i++) {
		        half32 vrx = vrx_first32 + (half32)((float)(x_out+i*32)*float_x_ratio);
		               vrx = fmax(vrx,0);
		   
				half16 vix_left_f_0 , vix_left_f_1;
				half32 alpha0;
				alpha0.lo = fract(vrx.lo , &vix_left_f_0);
				alpha0.hi = fract(vrx.hi , &vix_left_f_1);
				half32 alpha1 = (half32)1 - alpha0; 
		
				vix_left[2*i]     = convert_int16(vix_left_f_0);
				vix_left[2*i+1]   = convert_int16(vix_left_f_1);
				q[i][0] = (alpha1 * beta1); // top left
				q[i][1] = (alpha0 * beta1); // top rigth
				q[i][2] = (alpha1 * beta0); // bot left
				q[i][3] = (alpha0 * beta0); // bot right
				#ifdef OCL_RESIZE_F_PRINT_EN_DEBUG_
				if((y_out==0)&&(x_out==0))	{
					printf(" i=%d,vrx    	= %#v16hf\n", i,vrx);
					printf(" i=%d,vix_left_f= %#v16hf\n", i,vix_left_f);
					printf(" i=%d,vix_left[i]  = %#v16hd\n", i,vix_left[i]);
					printf(" i=%d,alpha0    = %#v16hf\n", i,alpha0);
					printf(" i=%d,alpha1    = %#v16hf\n", i,alpha1);
					printf(" i=%d,q[i][0]   = %#v16hf\n", i,q[i][0]);
					printf(" i=%d,q[i][1]   = %#v16hf\n", i,q[i][1]);
					printf(" \n\n\n");
				}
				#endif
		   }

			// read 4 vectors of 64 pixels from input for TL,TR,BL and BR
			// first 2 digits - 00=TL,01=TR,10,BL,11=BR, the last digit for number of 16 vec at the 64 vec
			//---------------------------------------------------------------------------------------------
			float16 i00_0 = vgather16( l0_in_pixel,        vix_left[0]);
			float16 i00_1 = vgather16( l0_in_pixel,        vix_left[1]);
			float16 i00_2 = vgather16( l0_in_pixel,        vix_left[2]);
			float16 i00_3 = vgather16( l0_in_pixel,        vix_left[3]);
																   
			float16 i01_0 = vgather16( &l0_in_pixel[1],    vix_left[0]);
			float16 i01_1 = vgather16( &l0_in_pixel[1],    vix_left[1]);
			float16 i01_2 = vgather16( &l0_in_pixel[1],    vix_left[2]);
			float16 i01_3 = vgather16( &l0_in_pixel[1],    vix_left[3]);
																   
			float16 i10_0 = vgather16( l1_in_pixel,        vix_left[0]);
			float16 i10_1 = vgather16( l1_in_pixel,        vix_left[1]);
			float16 i10_2 = vgather16( l1_in_pixel,        vix_left[2]);
			float16 i10_3 = vgather16( l1_in_pixel,        vix_left[3]);
																   
			float16 i11_0 = vgather16( &l1_in_pixel[1],    vix_left[0]);
			float16 i11_1 = vgather16( &l1_in_pixel[1],    vix_left[1]);
			float16 i11_2 = vgather16( &l1_in_pixel[1],    vix_left[2]);
			float16 i11_3 = vgather16( &l1_in_pixel[1],    vix_left[3]);

			half32 i00_lo, i00_hi;
			i00_lo.lo    = convert_half16(i00_0);
			i00_lo.hi    = convert_half16(i00_1);
			i00_hi.lo    = convert_half16(i00_2);
			i00_hi.hi    = convert_half16(i00_3);
			half32 i01_lo, i01_hi;
			i01_lo.lo    = convert_half16(i01_0);
			i01_lo.hi    = convert_half16(i01_1);
			i01_hi.lo    = convert_half16(i01_2);
			i01_hi.hi    = convert_half16(i01_3);
			half32 i10_lo, i10_hi;
			i10_lo.lo    = convert_half16(i10_0);
			i10_lo.hi    = convert_half16(i10_1);
			i10_hi.lo    = convert_half16(i10_2);
			i10_hi.hi    = convert_half16(i10_3);
			half32 i11_lo, i11_hi;
			i11_lo.lo    = convert_half16(i11_0);
			i11_lo.hi    = convert_half16(i11_1);
			i11_hi.lo    = convert_half16(i11_2);
			i11_hi.hi    = convert_half16(i11_3);

			half32 res_0 = i00_lo*q[0][0] + i01_lo*q[0][1] + i10_lo*q[0][2] + i11_lo*q[0][3];
			half32 res_1 = i00_hi*q[1][0] + i01_hi*q[1][1] + i10_hi*q[1][2] + i11_hi*q[1][3];

			#ifdef OCL_RESIZE_F_PRINT_EN_DEBUG_
				if((y_out==0)&&(x_out==0))	{
				printf(" res_0   = %#v16hf\n",res_0);			
				printf(" res_1   = %#v16hf\n",res_1);			
				}
			#endif
			
if(y_out<0) {
	printf("fffffff \n");
}

			short32 above_th0 = isgreater(res_0, priority_th1);
			short32 above_th1 = isgreater(res_1, priority_th1);

			char64 	tmp_out;
			uchar64 out_class;
			tmp_out.lo = convert_char32(above_th0)*(char)class_id;
			tmp_out.hi = convert_char32(above_th1)*(char)class_id;

			out_class.lo       = abs(tmp_out.lo);
			out_class.hi       = abs(tmp_out.hi);

			#ifdef OCL_RESIZE_F_PRINT_EN_DEBUG_			
			if((y_out==0)&&(x_out==0))	{
				printf(" res_0    	= %#v16hf\n",res_0);			
				printf(" above_th0  = %#v16hd\n",above_th0);			
				printf(" out_class  = %#v32hd\n",out_class.lo);			
				printf(" out_class  = %#v32hd\n",out_class.hi);			
				printf(" class_id   = %d\n",class_id);					
			}
			#endif
			vstore64( out_class, 0, &wrLine_out[curr_out_buf][x_out]);
		}	// end of for (int x_out = 0; x_out < out_width; x_out+=64)

		e_wrLine[curr_out_buf]     = async_work_group_copy(&g_ptr_out_ch[y_out*out_width],  wrLine_out[curr_out_buf],   out_width, 0);

		if (y_out>(first_out_row+1)){
			wait_group_events(1, &e_wrLine[1-curr_out_buf]);
		}

		// swap hr buffers
		curr_out_buf = 1-curr_out_buf;
	} // end of for (int y_out = first_out_row ; y_out < last_out_row ; y_out++)
	wait_group_events(1, &e_wrLine[1-curr_out_buf]);
	wait_group_events(1, &e_wrLine[curr_out_buf]);	

}
// ***********************************************************************
__attribute__(( reqd_work_group_size(1, 1, 1)))
__kernel void ocl_resize_half2uchar_proc2(global ocl_ctl_resize_f_planar 	* ctl,
										  local  ocl_resize_f_buffer_t   	* buffer)
{
    //Get kernel parameters from the control structure
    int in_width     = buffer->in_width;
    int in_height    = buffer->in_height;
    int out_width    = buffer->out_width;
    int out_height   = buffer->out_height;

    float float_x_ratio = (float)(in_width )/out_width;
    float float_y_ratio = (float)(in_height)/out_height;
	
    float y_float_offset = ( float_y_ratio - 1)/2;   			

	// implementation with align_corners=false
	//printf("float_x_ratio =%.15f, float_y_ratio =%.15f \n",float_x_ratio , float_y_ratio);
	//printf("y_float_offset =%.15f \n",y_float_offset);
	
    //Local line buffers, that will be filled/emptied using async_work_group_copy
    local half  * rdLine_in[4];  
	for (int buf=0; buf<4; buf++)
	{
		rdLine_in[buf] = (local half*)buffer->in_pixels[buf];
	}

    local uchar  * wrLine_out[2];
	for (int buf=0; buf<2; buf++)
	{
		wrLine_out[buf] = (local unsigned char *)buffer->out_pixel[buf];
	}
	
   	// input output global pointers
    global half 	* restrict  g_ptr_in_ch;
	g_ptr_in_ch     = (global half * )ctl->in_ptr;

    global unsigned char 	* restrict  g_ptr_out_ch;
	g_ptr_out_ch       = (global unsigned char * )ctl->out_ptr;


	int first_in_row, first_out_row, last_out_row;
  	int hr_row_threshold    = (int)(ctl->split_th *out_height);
  	bool first_section_flag = ctl->is_first_section;
  
    if (first_section_flag) { 		// first section
  		first_in_row = 0;  
  		first_out_row = 0;
  		last_out_row  = hr_row_threshold;
  	}
  	else {  // last section
  	    float y_float =  float_y_ratio*hr_row_threshold + y_float_offset;
  	          y_float =  max(y_float,(float)0);
  		first_in_row  = (int)(y_float); 
  		first_out_row =  hr_row_threshold;
  		last_out_row   =  out_height;
  	}
  
 	int load_buff        = first_in_row & 0x3;
 	int load_buff_plus_1 = (first_in_row+1) & 0x3;
 
    //Read first two rows of image into input buffers for resize
    event_t e_rdLine[4]    ={0,0,0,0};
    event_t e_wrLine[2]    ={0,0};

	e_rdLine[load_buff]           = async_work_group_copy(rdLine_in[load_buff]         ,&g_ptr_in_ch[first_in_row*in_width]       , in_width, 0);
	e_rdLine[load_buff_plus_1]    = async_work_group_copy(rdLine_in[load_buff_plus_1]  ,&g_ptr_in_ch[(first_in_row+1)*in_width]   , in_width, 0);

 	wait_group_events(1, &e_rdLine[load_buff]);
 	wait_group_events(1, &e_rdLine[load_buff_plus_1]);

 	int last_loaded_in_row = (first_in_row+1);
	
    half32   vrx_first32 = (half32)( ( 0*float_x_ratio +(float_x_ratio-1)/2),
								       ( 1*float_x_ratio +(float_x_ratio-1)/2),
								       ( 2*float_x_ratio +(float_x_ratio-1)/2),
								       ( 3*float_x_ratio +(float_x_ratio-1)/2),
								       ( 4*float_x_ratio +(float_x_ratio-1)/2),
								       ( 5*float_x_ratio +(float_x_ratio-1)/2),
								       ( 6*float_x_ratio +(float_x_ratio-1)/2),
								       ( 7*float_x_ratio +(float_x_ratio-1)/2),
								       ( 8*float_x_ratio +(float_x_ratio-1)/2),
								       ( 9*float_x_ratio +(float_x_ratio-1)/2),
								       (10*float_x_ratio +(float_x_ratio-1)/2),
								       (11*float_x_ratio +(float_x_ratio-1)/2),
								       (12*float_x_ratio +(float_x_ratio-1)/2),
								       (13*float_x_ratio +(float_x_ratio-1)/2),
								       (14*float_x_ratio +(float_x_ratio-1)/2),
								       (15*float_x_ratio +(float_x_ratio-1)/2),
                                       (16*float_x_ratio +(float_x_ratio-1)/2),
								       (17*float_x_ratio +(float_x_ratio-1)/2),
								       (18*float_x_ratio +(float_x_ratio-1)/2),
								       (19*float_x_ratio +(float_x_ratio-1)/2),
								       (20*float_x_ratio +(float_x_ratio-1)/2),
								       (21*float_x_ratio +(float_x_ratio-1)/2),
								       (22*float_x_ratio +(float_x_ratio-1)/2),
								       (23*float_x_ratio +(float_x_ratio-1)/2),
								       (24*float_x_ratio +(float_x_ratio-1)/2),
								       (25*float_x_ratio +(float_x_ratio-1)/2),
								       (26*float_x_ratio +(float_x_ratio-1)/2),
								       (27*float_x_ratio +(float_x_ratio-1)/2),
								       (28*float_x_ratio +(float_x_ratio-1)/2),
								       (29*float_x_ratio +(float_x_ratio-1)/2),
								       (30*float_x_ratio +(float_x_ratio-1)/2),
								       (31*float_x_ratio +(float_x_ratio-1)/2));
	#ifdef OCL_RESIZE_F_PRINT_EN_DEBUG_
		printf(" vrx_first16    = %#v16hf\n", vrx_first16);
	#endif
	
    int prev_bottom_buffer = load_buff;
    int curr_out_buf = 0;
	
	half  priority_th1  = (half)buffer->priority_th;
	int    class_id    = buffer->class_id;

    // high rate rows loop
    for (int y_out = first_out_row ; y_out < last_out_row ; y_out++)  
    {
	    float y_float =  float_y_ratio*y_out + y_float_offset;
	          y_float =  max(y_float,(float)0);
		int   iy2     = (int)(y_float);

        int iy2_T_buff = iy2 & 3;  	   						// modulo 4
        int iy2_B_buff = min(iy2+1,in_height-1) & 3;       	// modulo 4

		// set pointers
        local half  * l0_in_pixel;
        local half  * l1_in_pixel;

        l0_in_pixel   = rdLine_in[iy2_T_buff];
        l1_in_pixel   = rdLine_in[iy2_B_buff];

		//duplicate border to eliminate check end line
		l0_in_pixel[in_width]  = l0_in_pixel[in_width-1];
		l1_in_pixel[in_width]  = l1_in_pixel[in_width-1];
		
	
		half  beta0 = y_float - (half)iy2;
        half  beta1 = 1       -  beta0;

		// wait read 
        if (prev_bottom_buffer!= iy2_B_buff) {
			prev_bottom_buffer = iy2_B_buff;
			wait_group_events(1, &e_rdLine[iy2_B_buff]);
#ifdef OCL_RESIZE_F_PRINT_EN_DEBUG_  
		printf(" wait_group_events, iy2_B_buff=%d\n", iy2_B_buff);			
#endif		
		}

		int in_line2load = iy2+2;
		if ( (in_line2load>last_loaded_in_row)&& (in_line2load<in_height) ){
			// read A1 & b1 lines
			last_loaded_in_row++;
			int load_buff = last_loaded_in_row&3;
			
			e_rdLine[load_buff] = async_work_group_copy(rdLine_in[load_buff],  &g_ptr_in_ch[  last_loaded_in_row*in_width], in_width, 0);
		}


		for (int x_out = 0; x_out < out_width; x_out+=64) {
           half32  q[2][4]; 		// first index 0:2, determain the 32 group, coef for: 0=tl,1=tr,2-bl,3,br,  
           int16   vix_left[4];     // indexes at the line. 0:3 determain the group of 16 indexes
				   
			for (int i=0; i<2; i++) {
		        half32 vrx = vrx_first32 + (half32)((float)(x_out+i*32)*float_x_ratio);
		               vrx = fmax(vrx,0);
		   
				half16 vix_left_f_0 , vix_left_f_1;
				half32 alpha0;
				alpha0.lo = fract(vrx.lo , &vix_left_f_0);
				alpha0.hi = fract(vrx.hi , &vix_left_f_1);
				half32 alpha1 = (half32)1 - alpha0; 
		
				vix_left[2*i]     = convert_int16(vix_left_f_0);
				vix_left[2*i+1]   = convert_int16(vix_left_f_1);
				q[i][0] = (alpha1 * beta1); // top left
				q[i][1] = (alpha0 * beta1); // top rigth
				q[i][2] = (alpha1 * beta0); // bot left
				q[i][3] = (alpha0 * beta0); // bot right
				#ifdef OCL_RESIZE_F_PRINT_EN_DEBUG_
				if((y_out==0)&&(x_out==0))	{
					printf(" i=%d,vrx    	= %#v16hf\n", i,vrx);
					printf(" i=%d,vix_left_f= %#v16hf\n", i,vix_left_f);
					printf(" i=%d,vix_left[i]  = %#v16hd\n", i,vix_left[i]);
					printf(" i=%d,alpha0    = %#v16hf\n", i,alpha0);
					printf(" i=%d,alpha1    = %#v16hf\n", i,alpha1);
					printf(" i=%d,q[i][0]   = %#v16hf\n", i,q[i][0]);
					printf(" i=%d,q[i][1]   = %#v16hf\n", i,q[i][1]);
					printf(" \n\n\n");
				}
				#endif
		   }

			// read 4 vectors of 64 pixels from input for TL,TR,BL and BR
			// first 2 digits - 00=TL,01=TR,10,BL,11=BR, the last digit for number of 16 vec at the 64 vec
			//---------------------------------------------------------------------------------------------
			half32 i00_0 = vgather32( l0_in_pixel,        vix_left[0],        vix_left[1]);
			half32 i00_1 = vgather32( l0_in_pixel,        vix_left[2],        vix_left[3]);
																   
			half32 i01_0 = vgather32( &l0_in_pixel[1],    vix_left[0],        vix_left[1]);
			half32 i01_1 = vgather32( &l0_in_pixel[1],    vix_left[2],        vix_left[3]);
																   
			half32 i10_0 = vgather32( l1_in_pixel,        vix_left[0],        vix_left[1]);
			half32 i10_1 = vgather32( l1_in_pixel,        vix_left[2],        vix_left[3]);
																   
			half32 i11_0 = vgather32( &l1_in_pixel[1],    vix_left[0],        vix_left[1]);
			half32 i11_1 = vgather32( &l1_in_pixel[1],    vix_left[2],        vix_left[3]);

			half32 res_0 = i00_0*q[0][0] + i01_0*q[0][1] + i10_0*q[0][2] + i11_0*q[0][3];
			half32 res_1 = i00_1*q[1][0] + i01_1*q[1][1] + i10_1*q[1][2] + i11_1*q[1][3];

			#ifdef OCL_RESIZE_F_PRINT_EN_DEBUG_
				if((y_out==0)&&(x_out==0))	{
				printf(" res_0   = %#v16hf\n",res_0);			
				printf(" res_1   = %#v16hf\n",res_1);			
				}
			#endif
			
if(y_out<0) {
	printf("fffffff \n");
}

			short32 above_th0 = isgreater(res_0, priority_th1);
			short32 above_th1 = isgreater(res_1, priority_th1);

			char64 	tmp_out;
			uchar64 out_class;
			tmp_out.lo = convert_char32(above_th0)*(char)class_id;
			tmp_out.hi = convert_char32(above_th1)*(char)class_id;

			out_class.lo       = abs(tmp_out.lo);
			out_class.hi       = abs(tmp_out.hi);

			#ifdef OCL_RESIZE_F_PRINT_EN_DEBUG_			
			if((y_out==0)&&(x_out==0))	{
				printf(" res_0    	= %#v16hf\n",res_0);			
				printf(" above_th0  = %#v16hd\n",above_th0);			
				printf(" out_class  = %#v32hd\n",out_class.lo);			
				printf(" out_class  = %#v32hd\n",out_class.hi);			
				printf(" class_id   = %d\n",class_id);					
			}
			#endif
			vstore64( out_class, 0, &wrLine_out[curr_out_buf][x_out]);
		}	// end of for (int x_out = 0; x_out < out_width; x_out+=64)

		e_wrLine[curr_out_buf]     = async_work_group_copy(&g_ptr_out_ch[y_out*out_width],  wrLine_out[curr_out_buf],   out_width, 0);

		if (y_out>(first_out_row+1)){
			wait_group_events(1, &e_wrLine[1-curr_out_buf]);
		}

		// swap hr buffers
		curr_out_buf = 1-curr_out_buf;
	} // end of for (int y_out = first_out_row ; y_out < last_out_row ; y_out++)
	wait_group_events(1, &e_wrLine[1-curr_out_buf]);
	wait_group_events(1, &e_wrLine[curr_out_buf]);	

}
// ***********************************************************************
__attribute__(( reqd_work_group_size(1, 1, 1)))
kernel void ev_inu_ocl_resize_float_kernel(	    global ocl_ctl_resize_f_planar  	* ctl,
										        global unsigned char 			* restrict g_dataOut,
										        global float        			* restrict g_dataIn )
{

	int in_H = ctl->in_height;
	int in_W = ctl->in_width;
		
	int out_H = ctl->out_height;
	int out_W = ctl->out_width;
	int element_size = ctl->element_size;

  	local void  		 *	in_pixels[4];		
  	local unsigned char  *	out_pixel[2];		

  	local ocl_resize_f_buffer_t   buffer;       
  	local unsigned char * local_mem_ptr 	= (local unsigned char *)ctl->local_mem_ptr; 	
	int in_row_size       = (in_W +63)&(~0x3f);      
	int out_row_size      = (out_W+63)&(~0x3f);    
	
	for ( int buf = 0; buf < 4; buf ++)
	{
		in_pixels[buf] =  (local void  *)local_mem_ptr;
		local_mem_ptr 		+= in_row_size*sizeof(float);
	}

	for ( int buf = 0; buf < 2; buf ++)
	{
		out_pixel[buf] = local_mem_ptr;
		local_mem_ptr += out_row_size*sizeof(char);
	}

#ifdef OCL_RESIZE_F_PRINT_EN_DEBUG_
	printf("INFO: EVSS_CFG_VCCM_SIZE=%i \n", ctl->local_mem_size);
	printf("INFO: g_out_ptr  at %p\n", g_dataOut);
	printf("INFO: g_in_ptr   at %p\n", g_dataIn);
	printf("INFO: in size  [ H,W] = [%d , %d] \n", in_H, in_W);
	printf("INFO: out size [ H,W] = [%d , %d] \n", out_H,out_W);
	
	printf("INFO: vmem in_pixels[0]  at %p\n",in_pixels[0]);		
	printf("INFO: vmem in_pixels[1]  at %p\n",in_pixels[1]);		
	printf("INFO: vmem in_pixels[2]  at %p\n",in_pixels[2]);		
	printf("INFO: vmem in_pixels[3]  at %p\n",in_pixels[3]);	
		
	printf("INFO: vmem out_pixel[[0]  at %p\n",out_pixel[0]);	
	printf("INFO: vmem out_pixel[[1]  at %p\n",out_pixel[1]);	

  	printf("INFO: end buffers allocation %p\n",  local_mem_ptr);
  	printf("INFO: vmem for stack and variables =%d \n",  ((int)ctl->local_mem_ptr+ ctl->local_mem_size-(int)local_mem_ptr));
#endif

	// init buffer with the ptrs
	for ( int buf = 0; buf < 4; buf ++)  {
		buffer.in_pixels[buf] =  in_pixels[buf];
	}

	for ( int buf = 0; buf < 2; buf ++)  {
		buffer.out_pixel[buf] =  out_pixel[buf];
	}
	buffer.g_float_out_ptr = (global float * )g_dataIn;
	buffer.g_fp16_out_ptr  = (global half * )g_dataIn;

	buffer.in_height   = in_H;
	buffer.in_width    = in_W;
	buffer.out_height  = out_H;
	buffer.out_width   = out_W;
	buffer.priority_th   = ctl->priority_th;
	buffer.class_id   = ctl->class_id;
	buffer.element_size   = element_size;

	//ocl_resize_float2uchar_proc(ctl, &buffer);
	if (element_size == sizeof(float)) {
		ocl_resize_float2uchar_proc2(ctl, &buffer);
	}
	else if (element_size == sizeof(half)) {
		ocl_resize_half2uchar_proc2(ctl, &buffer);
	}
}
// ***********************************************************************
void print_en_ushorts(int tile_num ,int tile2print, int num_rows, int num_cols,local unsigned short * ptr)
{
	// print tile 
	// ---------
	if (tile_num==tile2print)
	{
		for(int line=0;line<num_rows;line++)
		{
			printf("line=%d\n",line);
			for(int c=0;c<num_cols;c++)
			{
				printf("%d,",ptr[line*num_cols+c]);
			}
			printf("\n");
		}
	}
}
// ***********************************************************************
void print_en_uchar(int tile_num ,int tile2print, int num_rows, int num_cols,local unsigned char * ptr)
{
	// print tile 
	// ---------
	if (tile_num==tile2print)
	{
		for(int line=0;line<num_rows;line++)
		{
			printf("line=%d\n",line);
			for(int c=0;c<num_cols;c++)
			{
				printf("%d,",ptr[line*num_cols+c]);
			}
			printf("\n");
		}
	}
}
// ***********************************************************************
