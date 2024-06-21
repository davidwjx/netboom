/* Helper interface between OpenCL and C++ code */
#ifndef _EV_INU_PROTO_MASK_H_
#define _EV_INU_PROTO_MASK_H_
	
//#define  proto_mask_CYCLES_MEAS 1
//#define  OCL_PROTO_MASK_PRINT_EN 1 
#define  pwrtwo(x) (1 << (x))

#define SIGMOID_LUT_SIZE          2049
#define SIGMOID_MIN_X           	-6
#define SIGMOID_MAX_X           	 6
#define SIGMOID_LUT_RESOLUTION  	((float)(1<<11)/(SIGMOID_MAX_X-SIGMOID_MIN_X))  // 1/resolution
#define CYCLES_PER_MS 850000

#define DO_LOGISTIC 1

 // Corr1d control structure between the C function to the OCL function
typedef struct {
    float 	* in_proto_ptr;  	    
    float 	* in_mask_ptr;  	    
    float 	* out_res_ptr;  	    
    __fp16 	* sigmoid_lut;  	    

	int 	in_height;  		// input height	
 	int 	in_width;  		    // input width	
	int 	in_channels;   

	int 	num_elements;  

	float 	split_th;  
	bool	is_first_section;  
	
	void 	*local_mem_ptr; 	// pointer to (unsigned char*) EVSS_CFG_VCCM_START;
    int 	local_mem_size; 	// EVSS_CFG_VCCM_SIZE
} ocl_ctl_proto_mask_mul;


void Ev_multiply_mask(int width, int hight, int channels, int num_elements, float *proto_data,  float *mask_data, float *mask_proto_result, bool is_first_section, float split_th);

#endif

