/* Helper interface between OpenCL and C++ code */
#ifndef _EV_Crop_OCL_INTERFACE_H_
#define _EV_Crop_OCL_INTERFACE_H_
	
#define LUT_STEP_SIZE 					128
#define MAX_X  							10000   // at [mm]
#define MAX_Y							10000   // at [mm]

// VGF general ctl parameters structure that calculated at init
typedef struct {
	short   		inv_fc_x_Q23;
	short   		inv_fc_y_Q23;
	short   		cc_x;
	short   		cc_y;
	short   		voxel_leaf_x_size;
	short   		voxel_leaf_y_size;
	short   		voxel_leaf_z_size;
	short   		inv_voxel_size_x_Q15;
	short   		inv_voxel_size_y_Q15;
	short   		inv_voxel_size_z_Q15;
	short   		min_b_x;
	short   		min_b_y;
	short   		min_b_z;
	short   		div_b_x;
	short   		div_b_y;
	short   		div_b_z;
	short   		lut_step_size;
	short   		tile_size_x;
	short   		tile_size_y;
	short   		tile_overlapping_x;
	short   		tile_overlapping_y;
	short  			MaxDepthMm;
	unsigned short  lut_x[MAX_X/LUT_STEP_SIZE];
	unsigned short  lut_y[MAX_Y/LUT_STEP_SIZE];
} ocl_ctl_vgf_general_params_t;


 // Vgf control structure between the C function to the OCL function
typedef struct {
    short 	* dataIn;  	// pointer to the input depth image
    short 	* dataOut;	// pointer for the point cloud outputs, each point is x y and z
    //input
	int 	input_width;  	// input depth image width	
	int 	input_height;  	// input depth image height	
	
	ocl_ctl_vgf_general_params_t vgf_gen_params_ptr;

    //output
	int 	max_pc_outputs;  		
	
	void 	*local_mem_ptr; 	// pointer to (unsigned char*) EVSS_CFG_VCCM_START;
    int 	local_mem_size; 	// EVSS_CFG_VCCM_SIZE
    int 	status; 			// kernel return status
    int 	total_unique_output; 			// kernel return status
    int 	stack_size;
} ocl_ctl_vgf_t;

#endif

