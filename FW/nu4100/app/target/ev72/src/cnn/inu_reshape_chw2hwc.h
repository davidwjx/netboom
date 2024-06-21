/* Helper interface between OpenCL and C++ code */
#ifndef _EV_INU_RESIZE_PLANAR_H_
#define _EV_INU_RESIZE_PLANAR_H_
	
//#define  reshape_CYCLES_MEAS 1
//#define  OCL_reshape_PRINT_EN_DEBUG 1 
#define DO_SCALING

#define  pwrtwo(x) (1 << (x))


#define CYCLES_PER_MS 850000


 // Corr1d control structure between the C function to the OCL function
typedef struct {
    short 	* in_ptr;  	    // pointer to the  input , planer format
    short 	* out_ptr;	    // pointer for the output, planer format
	
	short   * tanh_Q10_lut;

	int 	in_height;  		// input height	
 	int 	in_width;  		    // input width	
	int 	in_channels;  		// num input channels
	
	int     one_over_scale_factor;
	int     one_over_scale_shift_bits;

	float 	split_th;  
	bool	is_first_section;  
	
	void 	*local_mem_ptr; 	// pointer to (unsigned char*) EVSS_CFG_VCCM_START;
    int 	local_mem_size; 	// EVSS_CFG_VCCM_SIZE
} ocl_ctl_reshape_chw2hwc;


void inu_reshape_chw2hwc(short * in_ptr, short * out_ptr, int in_H, int  in_W, int in_ch ,float scale,bool do_tanh, bool is_first_sec ,float split_th);

#endif

