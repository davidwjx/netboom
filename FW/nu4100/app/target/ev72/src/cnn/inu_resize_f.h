/* Helper interface between OpenCL and C++ code */
#ifndef _EV_INU_RESIZE_F_H_
#define _EV_INU_RESIZE_F_H_
	
//#define  resize_f_CYCLES_MEAS 1
//#define  OCL_RESIZE_F_PRINT_EN_DEBUG 1 
#define  pwrtwo(x) (1 << (x))


#define CYCLES_PER_MS 850000


 // Corr1d control structure between the C function to the OCL function
typedef struct {
    void 	* in_ptr;  	    // pointer to the  input , planer format
    unsigned char 	* out_ptr;	    // pointer for the output, planer format

	int 	in_height;  		// input height	
 	int 	in_width;  		    // input width	
	int 	out_height;   
	int 	out_width;
	int     element_size;

	float 	priority_th;  
	int 	class_id;  

	float 	split_th;  
	bool	is_first_section;  
	
	void 	*local_mem_ptr; 	// pointer to (unsigned char*) EVSS_CFG_VCCM_START;
    int 	local_mem_size; 	// EVSS_CFG_VCCM_SIZE
} ocl_ctl_resize_f_planar;


void Ev_resize_float_mat(int in_x, int in_y, void *in_mat, int out_x, int out_y, unsigned char *out_mat, float thresh, int class_id, int element_size, bool is_first_sec ,float split_th);

#endif

