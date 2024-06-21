/* Helper interface between OpenCL and C++ code */
#ifndef _EV_INU_RESIZE_PLANAR_H_
#define _EV_INU_RESIZE_PLANAR_H_
	
//#define  logistic_CYCLES_MEAS 1
//#define  OCL_logistic_PRINT_EN_DEBUG 1 

#define  pwrtwo(x) (1 << (x))
#define CYCLES_PER_MS 864000

#define  LOGISTIC_MAX_INT           6
#define  LOGISTIC_LUT_RESOLUTION  	((float)(1<<10)/LOGISTIC_MAX_INT)
#define  LOGISTIC_LUT_SIZE 			((1 << 11)+1)

 // Corr1d control structure between the C function to the OCL function
typedef struct {
    void 	* in_ptr;  	    // pointer to the  input , planer format
    float 	* out_ptr;	    // pointer for the output, planer format
	
	float   * logistic_lut_ptr;

	int 	in_vec_len;  
	int     element_size;
	
	int     scale_factor;
	int     scale_shift_bits;

	float 	split_th;  
	bool	is_first_section;  
	
	void 	*local_mem_ptr; 	// pointer to (unsigned char*) EVSS_CFG_VCCM_START;
    int 	local_mem_size; 	// EVSS_CFG_VCCM_SIZE
} ocl_ctl_logistic;


void inu_logistic(void * in_ptr, float * out_ptr, int in_vec_length ,float scale,int element_size, bool is_first_sec ,float split_th);

#endif

