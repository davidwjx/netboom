/****************************************************************************
 *
 *   FileName: ev_inu_voxel_grid_filtering.cc
 *
 *   Author:  Arviv B.
 *
 *   Date: 
 *
 *   Description: 
 *   
 ****************************************************************************/

//#include <ev_layer.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <algorithm>
//#include <VX/lower/vx_lower.h>
#include <evss/kdisp.h>

#include "log.h"

#include <inu_utils.h>
#include "ev_inu_voxel_grid_filtering.h"
#include "ev_ocl_vgf_interface.h"


//extern "C" {
		
ocl_ctl_vgf_general_params_t 	vgf_gen_params;
int16_t   min_x  = MIN_X;   // at [mm]
int16_t   max_x  = MAX_X;   // at [mm]
int16_t   min_y  = MIN_Y;   // at [mm]
int16_t   max_y  = MAX_Y;   // at [mm]
int16_t   min_z  = MIN_Z;   // at [mm]
int16_t   max_z  = MAX_Z;   // at [mm]

//Declaration of OpenCL kernel
EVOCL_KERNEL_DECL(ev_inu_ocl_vgf_kernel);
EVOCL_KERNEL_DECL(ev_inu_ocl_pcl_kernel);

// The init function calculate parameters that should calculated once and set them at static variables.
// (like the LUTs)  and check the voxel leaf size, return status
// 
int evInuVoxelGridFiltering_init(vgf_init_params_t *vgf_init_p)
{
	uint16_t 	dx,dy,dz;
	int     	rt = VGF_SUCCESS;
	float 		tmp_lut_x,tmp_lut_y;
	int   		lut_x_size  =  MAX_X/LUT_STEP_SIZE;
	int   		lut_y_size  =  MAX_Y/LUT_STEP_SIZE;
	int   		k;
	
	vgf_gen_params.cc_x = uint16_t(vgf_init_p->center_point_x+0.5);
	vgf_gen_params.cc_y = uint16_t(vgf_init_p->center_point_y+0.5);
	vgf_gen_params.inv_fc_x_Q23 = uint16_t((1<<23)/vgf_init_p->focal_point_x);
	vgf_gen_params.inv_fc_y_Q23 = uint16_t((1<<23)/vgf_init_p->focal_point_y);
	vgf_gen_params.voxel_leaf_x_size = uint16_t(vgf_init_p->voxel_leaf_x_size);
	vgf_gen_params.voxel_leaf_y_size = uint16_t(vgf_init_p->voxel_leaf_y_size);
	vgf_gen_params.voxel_leaf_z_size = uint16_t(vgf_init_p->voxel_leaf_z_size);
	vgf_gen_params.inv_voxel_size_x_Q15 = uint16_t((1<<15)/vgf_init_p->voxel_leaf_x_size);
	vgf_gen_params.inv_voxel_size_y_Q15 = uint16_t((1<<15)/vgf_init_p->voxel_leaf_y_size);
	vgf_gen_params.inv_voxel_size_z_Q15 = uint16_t((1<<15)/vgf_init_p->voxel_leaf_z_size);
	vgf_gen_params.MaxDepthMm           = int16_t(vgf_init_p->MaxDepthMm);
	
	
	// check if voxel_leaf_size is in range
	dx = uint16_t(((max_x-min_x)*vgf_gen_params.inv_voxel_size_x_Q15)>>15);
	dy = uint16_t(((max_y-min_y)*vgf_gen_params.inv_voxel_size_y_Q15)>>15);
	dz = uint16_t(((max_z-min_z)*vgf_gen_params.inv_voxel_size_z_Q15)>>15);
	if (dx >= (1<<11)) rt = VGF_ERROR_INVALID_PARAMETERS;
	if (dy >= (1<<11)) rt = VGF_ERROR_INVALID_PARAMETERS;
	if (dz >= (1<<11)) rt = VGF_ERROR_INVALID_PARAMETERS;
	#ifdef Vgf_PRINT_EN_CC_DEBUG 
	if (rt==VGF_ERROR_INVALID_PARAMETERS)
	{
		printf("\Error: voxel size too small \n");
	}
	#endif
	
	//calc the min_b per axis and set the div_b per axis
	vgf_gen_params.min_b_x = int16_t(min_x*vgf_gen_params.inv_voxel_size_x_Q15>>15);
	vgf_gen_params.min_b_y = int16_t(min_y*vgf_gen_params.inv_voxel_size_y_Q15>>15);
	vgf_gen_params.min_b_z = int16_t(min_z*vgf_gen_params.inv_voxel_size_z_Q15>>15);
	vgf_gen_params.div_b_x = int16_t(1<<11); 
	vgf_gen_params.div_b_y = int16_t(1<<11); 
	vgf_gen_params.div_b_z = int16_t(1<<10);
	vgf_gen_params.lut_step_size = int16_t(LUT_STEP_SIZE);
	
	
	// calculat lut_x
	tmp_lut_x   =  vgf_gen_params.voxel_leaf_x_size * vgf_init_p->focal_point_x/LUT_STEP_SIZE;
	for(k=1;k<=lut_x_size;k++)
	{                             
		vgf_gen_params.lut_x[k-1] = uint16_t(float(tmp_lut_x/k)+.999999)+1+3;
	}
    vgf_gen_params.lut_x[0] = vgf_gen_params.lut_x[2];
	vgf_gen_params.lut_x[1] = vgf_gen_params.lut_x[2];
	// calculat lut_y
	tmp_lut_y   =  vgf_gen_params.voxel_leaf_y_size * vgf_init_p->focal_point_y/LUT_STEP_SIZE;
	for(k=1;k<=lut_y_size;k++)
	{                             
		vgf_gen_params.lut_y[k-1] = uint16_t(float(tmp_lut_y/k)+.999999)+1+3;
	}
    vgf_gen_params.lut_y[0] = vgf_gen_params.lut_y[2];
	vgf_gen_params.lut_y[1] = vgf_gen_params.lut_y[2];
	
	vgf_gen_params.tile_size_x = VGF_TILE_SIZE;
	vgf_gen_params.tile_size_y = VGF_TILE_SIZE;
	tmp_lut_x = (float)(vgf_init_p->voxel_leaf_x_size * vgf_init_p->focal_point_x)/MIN_DEPTH;
	tmp_lut_y = (float)(vgf_init_p->voxel_leaf_y_size * vgf_init_p->focal_point_y)/MIN_DEPTH;
	vgf_gen_params.tile_overlapping_x = (int16_t)(tmp_lut_x);
	vgf_gen_params.tile_overlapping_y = (int16_t)(tmp_lut_y);
	return(rt);
} // evInuVoxelGridFiltering_init


//===============================================================================
#ifdef EVSS_CFG_OCL
int ev_inu_vgf_ocl(vgf_dynamic_params_t *vgf_info)
{ 
    ocl_ctl_vgf_t ctl;
#ifdef Vgf_PRINT_EN_CC_DEBUG    
	if ((int )vgf_info->vgf_flag == 0) {
		printf("ocl PCL \n");
	}
	else {
		printf("ocl VGF \n");
	}
#endif	
    short *output   = (short *)vgf_info->dataOut;
    short *input    = (short *)vgf_info->dataIn;
	unsigned int * overlapping_rows = (unsigned int *)vgf_info->overlapping_rows_buffer; // used as temp buffer for overlapping rows
	ctl.dataIn  	= (short *)vgf_info->dataIn;
    ctl.dataOut 	= (short *)vgf_info->dataOut;

    ctl.input_width   = (int )vgf_info->input_width;
    ctl.input_height  = (int )vgf_info->input_height;
    ctl.max_pc_outputs= (int )vgf_info->max_pc_outputs;
	
	ctl.vgf_gen_params_ptr = vgf_gen_params;

    ctl.local_mem_ptr   = (unsigned char*) EVSS_CFG_VCCM_START;
    ctl.local_mem_size  = EVSS_CFG_VCCM_SIZE;
	
	void * args[] = {&ctl, output, input , overlapping_rows};
	if ((int )vgf_info->vgf_flag == 0) {
		inuCallOcl(OCL_KERNEL(ev_inu_ocl_pcl_kernel), args);
	}
	else {
		inuCallOcl(OCL_KERNEL(ev_inu_ocl_vgf_kernel), args);
	}

	return(ctl.total_unique_output);  
}

#else
int ev_inu_vgf_scalar(vgf_dynamic_params_t *vgf_info)
{
   //volatile uint32_t test_value = 0;
   //volatile uint32_t val1,val2 = 0;
  short *bottom_data = (short *)vgf_info->dataIn;
  short *top_data    = (short *)vgf_info->dataOut;

  int input_width     = vgf_info->input_width;			// X dimension of a map
  int input_height    = vgf_info->input_height;			// Y dimension of a map
  //int max_pc_outputs  = vgf_info->max_pc_outputs;			
  int num_outputs     = 0;
  short X,Y,Z;
  int   idx_out,idx_in,out_idx_offset=input_height*input_width;
  uint16_t index_x,index_y,index_z;
  uint32_t voxel_idx,* idx_out_ptr;
  short win_size_x, win_size_y , win_first_col ;
  int   win_idx_in;
  int   start_win_col, last_win_col;
  int   start_win_row, last_win_row;
  #ifdef Vgf_PRINT_EN_CC_DEBUG  
  printf("input_width= %d,   input_height=%d   \n", input_width, input_height );
  #endif 

  #ifdef Vgf_PRINT_2FILE1_EN
  FILE * fid1;
  fid1 =  fopen("idex.txt", "w");  
  #endif
  #ifdef Vgf_PRINT_2FILE2_EN
  FILE * fid2;
  fid2 =  fopen("idex_after_invalid.txt", "w");  
  #endif  
  // -- convert depth input to point cloud and to voxel index --
  // write the voxel index at output at the same location as the depth
  // assumption output buffer size =  (input_width*input_height)*3 shorts
  idx_out_ptr = (uint32_t *)(&top_data[out_idx_offset]);

  for (int row = 0; row < input_height; row++) 
  { 
	for (int col = 0; col < input_width; col++) 
	{
		idx_in  = (row*input_width) + col;
		Z = bottom_data[idx_in];
		if (Z >0)
		{
			X = short(float((col - vgf_gen_params.cc_x)*Z*(vgf_gen_params.inv_fc_x_Q23>>5))/float(1<<(23-5)));
			Y = short(float((row - vgf_gen_params.cc_y)*Z*(vgf_gen_params.inv_fc_y_Q23>>5))/float(1<<(23-5)));

			
			index_x = uint16_t((X*vgf_gen_params.inv_voxel_size_x_Q15 >>15) - vgf_gen_params.min_b_x);
			index_y = uint16_t((Y*vgf_gen_params.inv_voxel_size_y_Q15 >>15) - vgf_gen_params.min_b_y);
			index_z = uint16_t((Z*vgf_gen_params.inv_voxel_size_z_Q15 >>15) - vgf_gen_params.min_b_z);
			
			voxel_idx = uint32_t(index_x) + uint32_t(index_y)*vgf_gen_params.div_b_x + uint32_t(index_z)*vgf_gen_params.div_b_x*vgf_gen_params.div_b_y;
		}
		else{
			voxel_idx = 0;
		}
		idx_out_ptr[idx_in] = voxel_idx ;
	}
  }

  // print loop - for indexes
  //-------------------------
  #ifdef Vgf_PRINT_2FILE1_EN  
  for (int row = 0; row < input_height; row++) 
  { 
	for (int col = 0; col < input_width; col++) 
	{
	   idx_in  = (row*input_width) + col;
	   fprintf(fid1, "%10x  ",idx_out_ptr[idx_in]);
	}
	fprintf(fid1, "\n");
  }
  fclose(fid1);
  #endif


 

  // -- start invalided duplicate indexes --
  for (int row = 0; row < input_height; row++) { 
	for (int col = 0; col < input_width; col++) {
		
		idx_in  = (row*input_width) + col;
		voxel_idx   = idx_out_ptr[idx_in];
		if (voxel_idx>0) {
			// detrmaine the search window size
			index_z = voxel_idx>>22;
			Z = short(( ((index_z+vgf_gen_params.min_b_z)*vgf_gen_params.voxel_leaf_z_size)/vgf_gen_params.lut_step_size));
			win_size_x = vgf_gen_params.lut_x[Z];
			win_size_y = vgf_gen_params.lut_y[Z];
			
			//start_win_col= max( 0 , col-win_size_x);
			if (col-win_size_x < 0)  start_win_col =0;
			else                     start_win_col= col-win_size_x;
			//last_win_col = min(input_width , col+win_size_x);
			if (input_width < col+win_size_x)  last_win_col = input_width-1;
			else                               last_win_col = col+win_size_x;

			start_win_row = row;
			//last_win_row  = min(input_height, row+win_size_y);
			if (input_height < row+win_size_y) last_win_row = input_height-1;
			else                               last_win_row = row+win_size_y;

			win_first_col = col+1;
			if (win_first_col >= input_width) {
				win_first_col = start_win_col;
				start_win_row = row + 1;
			}
			
			for (int win_row=start_win_row; win_row <= last_win_row; win_row++){
				for (int win_col=win_first_col; win_col <= last_win_col; win_col++){
					win_idx_in = (win_row*input_width) + win_col;
					if (voxel_idx == idx_out_ptr[win_idx_in] ){
						idx_out_ptr[win_idx_in] = 0;
					}
				} // for (int win_col=win_first_col; win_col < last_win_col; win_col++)
				win_first_col = start_win_col;
			} // for (int win_row=start_win_row; win_row < last_win_row; win_row++)
		} // if (voxel_idx>0)
		
	} // for (int col = 0; col < input_width; col++)
  } //  for (int row = 0; row < input_height; row++)

// print loop - after invalidation
//---------------------------------
  #ifdef Vgf_PRINT_2FILE2_EN  
  for (int row = 0; row < input_height; row++) 
  { 
	for (int col = 0; col < input_width; col++) 
	{
	   idx_in  = (row*input_width) + col;
	   fprintf(fid2, "%10x  ",idx_out_ptr[idx_in]);
	}
	fprintf(fid2, "\n");
  }
  fclose(fid2);
  #endif

  
  // start output uniqe indexes
  for (int row = 0; row < input_height; row++) { 
	for (int col = 0; col < input_width; col++) {
		idx_in  = (row*input_width) + col;
		voxel_idx   = idx_out_ptr[idx_in];
		if (voxel_idx>0) {
			// detrmaine the search window size
			index_x = uint16_t(voxel_idx & 0x3ff);
			index_y = uint16_t((voxel_idx>>11) & 0x3ff);
			index_z = voxel_idx>>22;
			X = short(( (index_x+vgf_gen_params.min_b_x)*vgf_gen_params.voxel_leaf_x_size + (vgf_gen_params.voxel_leaf_x_size/2)));
			Y = short(( (index_y+vgf_gen_params.min_b_y)*vgf_gen_params.voxel_leaf_y_size + (vgf_gen_params.voxel_leaf_y_size/2)));
			Z = short(( (index_z+vgf_gen_params.min_b_z)*vgf_gen_params.voxel_leaf_z_size + (vgf_gen_params.voxel_leaf_z_size/2)));
			
			idx_out=3*num_outputs;
			top_data[idx_out  ]=X;	
			top_data[idx_out+1]=Y;	
			top_data[idx_out+2]=Z;	
			num_outputs++;
		} // if (voxel_idx>0)
		
	} // for (int col = 0; col < input_width; col++)
  } //  for (int row = 0; row < input_height; row++)
  #ifdef Vgf_PRINT_EN_CC_DEBUG  
  printf("VGF_num_optputs =%d  \n", num_outputs );
  #endif  
  
  return(num_outputs);
}
#endif
//===============================================================================
	
int evInuVoxelGridFiltering(vgf_dynamic_params_t *vgf_info)
{
	int num_outputs;
	
#ifdef EVSS_CFG_OCL    
    if ((int )vgf_info->vgf_flag == 0) {
		LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL,"ocl PCL \n");
	}
	else {
		LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL,"ocl VGF \n");
	}
#else
	LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL,"scalar VGF \n");
#endif	

	
#ifdef Vgf_CYCLES_MEAS	
    uint64_t start, end;
	start = getRTC();
#endif

#ifdef EVSS_CFG_OCL
    num_outputs = ev_inu_vgf_ocl(vgf_info);
#else
	num_outputs = ev_inu_vgf_scalar(vgf_info);
#endif	

#ifdef Vgf_CYCLES_MEAS	
    end = getRTC();
    //printf("Execution took %llu cycles\n", end - start);
	LOGG_PRINT(LOG_INFO_E, (ERRG_codeE)NULL,"Execution took %llu cycles\n", end - start);
#endif

	return(num_outputs);

} // evInuVoxelGridFiltering

//===============================================================================

//} // extern "C"
    
