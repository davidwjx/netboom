/* Helper interface between OpenCL and C++ code */
#ifndef _EV_Crop_OCL_INTERFACE_H_
#define _EV_Crop_OCL_INTERFACE_H_

//#define  OCL_Crop_PRINT_EN_DEBUG
//#define  Crop_PRINT_EN_CC_DEBUG


 // Crop kernel control structure
typedef struct {
    short * dataIn;
    short * dataOut;
    //input
	int input_width;  		
	int input_height;  		
	int input_num_maps;  	
    //output
	int output_width;  		
	int output_height;  		
	int output_num_maps; 
    
	int offset;
	int axis;

    float  IOfscale;		// scale_out /scale_in;

    void *local_mem_ptr; 	// pointer to (unsigned char*) EVSS_CFG_VCCM_START;
    int local_mem_size; 	// EVSS_CFG_VCCM_SIZE
    int status; 			// kernel return status
    int stack_size;
} ocl_ctl_Crop_t;

#endif

