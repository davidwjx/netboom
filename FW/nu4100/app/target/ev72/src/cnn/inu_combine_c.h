/* Helper interface between OpenCL and C++ code */
#ifndef _EV_INU_COMBINE_H_
#define _EV_INU_COMBINE_H_
	
//#define  combine_c_CYCLES_MEAS 1
//#define  OCL_COMBINE_C_PRINT_EN 1 
#define  pwrtwo(x) (1 << (x))

#define MAX_MATS_IN   10
#define CYCLES_PER_MS 850000


 // Corr1d control structure between the C function to the OCL function
typedef struct {
    unsigned char   * in_mats_ptrs[MAX_MATS_IN];  	    // pointer to the  input , planer format
    unsigned char 	* out_ptr;	    // pointer for the output, planer format

 	int 	num_mats;  		
 	int 	size_x;  		
	int 	size_y;   

	float 	split_th;  
	bool	is_first_section;  
	
	void 	*local_mem_ptr; 	// pointer to (unsigned char*) EVSS_CFG_VCCM_START;
    int 	local_mem_size; 	// EVSS_CFG_VCCM_SIZE
} ocl_ctl_combine_c_planar;

void Ev_yolact_combine(int num_mats, int x_size, int y_size, unsigned char ** mats_ptrs, unsigned char *out_mat, bool is_first_sec ,float split_th);

#endif

