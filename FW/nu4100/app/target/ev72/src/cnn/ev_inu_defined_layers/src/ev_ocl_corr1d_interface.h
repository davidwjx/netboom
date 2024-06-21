/* Helper interface between OpenCL and C++ code */
#ifndef _EV_MyCorr1d_OCL_INTERFACE_H_
#define _EV_MyCorr1d_OCL_INTERFACE_H_
	
//#define corr1d_CYCLES_MEAS 1
//#define  OCL_corr1d_PRINT_EN_DEBUG 1 
 // Corr1d control structure between the C function to the OCL function
typedef struct {
    short 	* dataInA;  	// pointer to the input tensorA
    short 	* dataInB;  	// pointer to the input tensorB
    short 	* dataOut;	    // pointer for the corrd1d outputs

    //input
	int 	input_width;  	// input depth image width	
	int 	input_height;  	// input depth image height	
	int 	input_num_maps; 

	float 	fscaleA;
	float 	fscaleB;
    float 	fscaleC;	
	
	int     scale;
	int     scale_bits;
	
	int 	num_of_shifts;
	int 	stride;
	
	void 	*local_mem_ptr; 	// pointer to (unsigned char*) EVSS_CFG_VCCM_START;
    int 	local_mem_size; 	// EVSS_CFG_VCCM_SIZE
    int 	status; 			// kernel return status
    int 	stack_size;
} ocl_ctl_corr1d_t;

#endif

