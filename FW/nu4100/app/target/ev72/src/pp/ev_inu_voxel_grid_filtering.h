/* Helper interface between calling function to the VGF code*/
#ifndef _EV_Inu_VGF_INTERFACE_H_
#define _EV_Inu_VGF_INTERFACE_H_

// init return value
#define	VGF_SUCCESS						0
#define VGF_FAILURE 					-1
#define VGF_ERROR_INVALID_PARAMETERS	-2
	
#define LUT_STEP_SIZE 					128
#define MIN_X 							-10000  // at [mm]
#define MAX_X  							10000   // at [mm]
#define MIN_Y							-10000  // at [mm]
#define MAX_Y							10000   // at [mm]
#define MIN_Z							0       // at [mm]
#define MAX_Z							10000   // at [mm]

#define MIN_DEPTH						350     // at [mm]
#define VGF_TILE_SIZE					110

//#define Vgf_PRINT_2FILE1_EN 			
//#define Vgf_PRINT_2FILE2_EN 			
//#define Vgf_PRINT_EN_CC_DEBUG 			
//#define Pcl_PRINT_EN_CC_DEBUG 			
#define  EVSS_CFG_OCL
//#define  OCL_ADJUST_BW_SEARCH_WINDOW
#define Vgf_CYCLES_MEAS

 // Vgf init parameters structure
typedef struct {
	int 	voxel_leaf_x_size;  		// voxel size in [mm]
	int 	voxel_leaf_y_size;  		// voxel size in [mm]
	int 	voxel_leaf_z_size;  		// voxel size in [mm]

    float  	focal_point_x;		
    float  	focal_point_y;		
    float  	center_point_x;		
    float  	center_point_y;		
	
	int     MaxDepthMm;
} vgf_init_params_t;


 // Vgf control structure per call interface to the calling function
typedef struct {
    unsigned short 	* dataIn;  	// pointer to the input depth image
    short 			* dataOut;	// pointer for the point cloud outputs, each point is x y and z
    unsigned int 	* overlapping_rows_buffer;	// pointer for the buffer that holds the voxel indexes of the overlapping rows between w strips
    //input
	int 			input_width;  	// input depth image width	
	int 			input_height;  	// input depth image height	
	int 			vgf_flag; 		// 0- for pcl output, 1-for pcl_output with VGF

    //output
	int 			max_pc_outputs;  		
} vgf_dynamic_params_t;


#endif

