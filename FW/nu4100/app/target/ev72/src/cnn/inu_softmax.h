/* Helper interface between OpenCL and C++ code */
#ifndef _EV_INU_SOFTMAX_H_
#define _EV_INU_SOFTMAX_H_
	
//#define  softmax_CYCLES_MEAS 1
//#define  OCL_softmax_PRINT_EN_DEBUG 1 

#define  pwrtwo(x) (1 << (x))
#define CYCLES_PER_MS 864000

#define  Q_IN    				10
#define  Q_OUT    				10

#define  EXP_MIN_INT           	-6
#define  EXP_MAX_INT           	0
#define  EXP_LUT_RESOLUTION  	((float)(1<<10)/(EXP_MAX_INT-EXP_MIN_INT))  // 1/resolution
#define  EXP_LUT_SHIFT       	(int)(abs(EXP_MIN_INT)*EXP_LUT_RESOLUTION)
#define  EXP_LUT_SIZE 			((1 << 10)+1)

 // Corr1d control structure between the C function to the OCL function
typedef struct {
    short 	* in_ptr;  	    // pointer to the  input , planer format
    short 	* out_ptr;	    // pointer for the output, planer format
	
	short   * exp_lut_ptr;

	int 	num_groups;  
	int     num_classes_plus1;
	
	int     scale_factor;
	int     scale_shift_bits;

	float 	split_th;  
	bool	is_first_section;  
	
	void 	*local_mem_ptr; 	// pointer to (unsigned char*) EVSS_CFG_VCCM_START;
    int 	local_mem_size; 	// EVSS_CFG_VCCM_SIZE
} ocl_ctl_softmax;


void inu_softmax(short * in_ptr, short * out_ptr, int num_groups ,int num_classes_plus1, bool is_first_sec ,float split_th);
#endif

