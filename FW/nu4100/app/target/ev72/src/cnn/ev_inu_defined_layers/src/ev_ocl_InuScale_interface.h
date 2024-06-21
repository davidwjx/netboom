/* Helper interface between OpenCL and C++ code */
#ifndef _EV_InuScale_OCL_INTERFACE_H_
#define _EV_InuScale_OCL_INTERFACE_H_

//#define  OCL_InuScale_PRINT_EN_DEBUG
//#define  InuScale_PRINT_EN_CC_DEBUG
//#define  InuScale_CYCLES_MEAS


// argmax kernel control structure
typedef struct {
    short * dataIn;
    short * dataOut;
	
	short * reduce2_LUT;    // pointer to the round(2^14./sqrt(t)) LUT
	
	float  IN_float_scale;	// scale_out /scale_in;
	int num_channels;  		// number of channels
	int in_height;    		// out_xsize*out_ysize
	int in_width;      		// out_xsize*out_ysize
	 
    void *local_mem_ptr; 	// pointer to (unsigned char*) EVSS_CFG_VCCM_START;
    int local_mem_size; 	// EVSS_CFG_VCCM_SIZE
    int status; 			// kernel return status
    int stack_size;
} ocl_ctl_InuScale_t;

#endif
