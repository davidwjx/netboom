/* Helper interface between OpenCL and C++ code */
#ifndef _EV_INU_CONVERT2FLOAT_H_
#define _EV_INU_CONVERT2FLOAT_H_
	
//#define  softmax_CYCLES_MEAS 1
//#define  OCL_convert2f_PRINT_EN_DEBUG 1 

#define  pwrtwo(x) (1 << (x))
#define CYCLES_PER_MS 864000


 // Corr1d control structure between the C function to the OCL function
typedef struct {
    short 	* in_ptr;  	    // pointer to the  input , planer format
    void 	* out_ptr;	    // pointer for the output, planer format
	int 	in_vec_len;  
	bool    float_output;   // output format:1=float format, 0=half format 
	
	float   scaler;

	float 	split_th;  
	bool	is_first_section;  
	
	void 	*local_mem_ptr; 	// pointer to (unsigned char*) EVSS_CFG_VCCM_START;
    int 	local_mem_size; 	// EVSS_CFG_VCCM_SIZE
} OCL_conver2f_PRINT_EN_DEBUG;


void inu_convert2float(short * in_ptr, void * out_ptr, int num_elemnts ,float scaler, bool float_output, bool is_first_sec ,float split_th);
#endif

