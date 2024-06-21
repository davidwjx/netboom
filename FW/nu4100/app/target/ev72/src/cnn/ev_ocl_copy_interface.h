/* Helper interface between OpenCL and C++ code */
#ifndef _EV_COPY_OCL_INTERFACE_H_
#define _EV_COPY_OCL_INTERFACE_H_
	
//#define  copy_CYCLES_MEAS 1
//#define  OCL_copy_PRINT_EN_DEBUG 1 

 // isp_pre_proc control structure between the C function to the OCL function
typedef struct {
    unsigned char 	* dataIn;  	// pointer to the input tensor
    unsigned char	* dataOut;	// pointer for the output tensor

	int 	size;  	            // size at uchar
	
	void 	*local_mem_ptr; 	// pointer to (unsigned char*) EVSS_CFG_VCCM_START;
    int 	local_mem_size; 	// EVSS_CFG_VCCM_SIZE
    int 	status; 			// kernel return status
    int 	stack_size;
} ocl_ctl_copy_t;

void copy_kernel(unsigned char* src, unsigned char* dst, int size);

#endif

