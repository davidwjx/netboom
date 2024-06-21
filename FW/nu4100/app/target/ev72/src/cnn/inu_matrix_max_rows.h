/* Helper interface between OpenCL and C++ code */
#ifndef _EV_INU_MATRIX_MAX_H_
#define _EV_INU_MATRIX_MAX_H_
	
//#define  matrix_max_rows_CYCLES_MEAS 1
//#define  OCL_MAT_MAX_ROWS_PRINT_EN 1 

#define CYCLES_PER_MS 850000

 // Corr1d control structure between the C function to the OCL function
typedef struct {
    __fp16 	* in_matrix_ptr;  	    
    float 	* out_res_ptr;  	    

	int 	in_height;  		// input height	
 	int 	in_width;  		    // input width	

	float 	split_th;  
	bool	is_first_section;  
	
	void 	*local_mem_ptr; 	// pointer to (unsigned char*) EVSS_CFG_VCCM_START;
    int 	local_mem_size; 	// EVSS_CFG_VCCM_SIZE
} ocl_ctl_matrix_max_rows;

void EV_matrix_rows_max_values(__fp16 *input_mat, int rows, int cols, float *max_values , bool is_first_section, float split_th);

#endif

