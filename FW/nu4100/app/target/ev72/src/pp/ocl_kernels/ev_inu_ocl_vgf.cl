/****************************************************************************
 *
 *   FileName: ev_inu_ocl_vgf.cl
 *
 *   Author:  Arviv B.
 *
 *   Date: 
 *
 *   Description: 
 *   
 ****************************************************************************/

//#include "ev_ocl_ctrl.h"
#include "../ev_ocl_vgf_interface.h"


#define  OCL_vgf_SIMD_WIDTH 32

// local ocl_InuVgf_buffer_t buffer[2] - buffer parameters per tile
typedef struct
{
	local short * input;						// pointer to the input tile
	local short * output;						// point to output point cloud after filtering 
	local uint  * tmp_index_mem_ptr;            // used to store ovelapping rows between w_strips
	      short   tile_num;						// used for debug
	      short   depth_height;
	      short   depth_width;
	      short   overlapping_x;				// number of overlapping columns at tile
	      short   overlapping_y;				// number of overlapping rows at tile
	      short   tile_size_x;					// num columns at tile
	      short   tile_size_y;					// num rows at tile
	      short   depth_src_row;				// start row at depth image to DMA to vmem
	      short   depth_src_col;				// start col at depth image to DMA to vmem
	      short   tile_depth_num_rows;			// number of rows to DMA from depth image
	      short   tile_depth_num_cols;			// number of columns to DMA from depth image 
	      short   vmem_depth_dst_row;           // row start at the vmem tile to DMA in the depth tile
	      short   vmem_depth_dst_col;			// col start at the vmem tile to DMA in the depth tile
	      short   vmem_num_cols2copy;			// number of overlapping columns to copy from the other buffer
	      short   vmem_num_rows2DMA;			// number of overlapping rows to DMA from tmp_index_mem
	      short   num_unique_outputs;			// number of uniqe outputs at the output tile buffer 
	      short   first_w_strip_flag;			// '1' for the first width strip
	      short   first_l_strap_flag;			// '1' for the first length strap
	      short   last_w_strip_flag;			// '1' for the lsrt width strip
	      short   last_l_strap_flag;			// '1' for the last length strap

	event_t       load_event_depth;
	event_t       load_event_indexes;
	event_t       store_event_results;
	event_t       store_event_indexes;

} ocl_InuVgf_buffer_t;

// ***********************************************************************
void ocl_vgf_init_buffer(
		local 	ocl_InuVgf_buffer_t*	buffer,
		global 	ocl_ctl_vgf_t* 	 		ctl,
		local 	short * input,			// input buffer at the vmem (double buffer)
		local 	short * output )        // output buffer at the vmem (double buffer)
{
	buffer->input = input;
#ifdef OCL_vgf_PRINT_EN_DEBUG
	printf("BUFFER at address %p: INIT input address %p\n", buffer, buffer->input);
#endif

	buffer->output = output;
#ifdef OCL_vgf_PRINT_EN_DEBUG
	printf("BUFFER at address %p: INIT output address %p\n", buffer, buffer->output);
#endif

    buffer->depth_height    = ctl->input_height;
    buffer->depth_width     = ctl->input_width;
	ocl_ctl_vgf_general_params_t vgf_gen_params = ctl->vgf_gen_params_ptr;
	
	buffer->tile_size_x   	= vgf_gen_params.tile_size_x;
	buffer->tile_size_y   	= vgf_gen_params.tile_size_y;
	buffer->overlapping_x   = vgf_gen_params.tile_overlapping_x;
	buffer->overlapping_y   = vgf_gen_params.tile_overlapping_y;

	buffer->load_event_depth   		= -1;
	buffer->load_event_indexes   	= -1;
	buffer->store_event_results   	= -1;
	buffer->store_event_indexes   	= -1;
}
// ***********************************************************************
void ocl_vgf_load_buffer(local ocl_InuVgf_buffer_t* buffer, global short* input, global uint * overlapping_rows,
                         int depth_src_row,  		int depth_src_col,
                         int tile_depth_num_rows,   int tile_depth_num_cols,
						 int vmem_depth_dst_row,    int vmem_depth_dst_col,  
						 int vmem_num_cols2copy,    int vmem_num_rows2DMA,
						 int first_w_strip_flag,    int first_l_strap_flag, 
						 int last_w_strip_flag,     int last_l_strap_flag , int tile_num)
{
	const int short_size  = sizeof(short);
	const int uint32_size = sizeof(int);

	buffer->depth_src_row = (short)depth_src_row;
	buffer->depth_src_col = (short)depth_src_col;
	buffer->tile_depth_num_rows = (short)tile_depth_num_rows;
	buffer->tile_depth_num_cols = (short)tile_depth_num_cols;
	buffer->vmem_depth_dst_row  = (short)vmem_depth_dst_row;
	buffer->vmem_depth_dst_col  = (short)vmem_depth_dst_col;
	buffer->vmem_num_cols2copy  = (short)vmem_num_cols2copy;
	buffer->vmem_num_rows2DMA   = (short)vmem_num_rows2DMA;
	buffer->first_w_strip_flag  = (short)first_w_strip_flag;
	buffer->first_l_strap_flag  = (short)first_l_strap_flag;
	buffer->last_w_strip_flag   = (short)last_w_strip_flag;
	buffer->last_l_strap_flag   = (short)last_l_strap_flag;
	buffer->tile_num   			= (short)tile_num;

    //  DMA in depth tile
	int dst_width  		= buffer->tile_depth_num_cols*2; 
	int src_width   	= buffer->tile_depth_num_cols;
	int upper_nearst_even_row = ((buffer->tile_depth_num_rows +1)/2)<<1;
	int dst_ptr_offset  = ((buffer->vmem_num_rows2DMA*2+upper_nearst_even_row)*buffer->tile_size_x + 
	                        buffer->vmem_num_cols2copy*2);	
	int src_ptr_offset  = (buffer->depth_src_row*buffer->depth_width + buffer->depth_src_col);
	int ch_data_size 	=  buffer->tile_depth_num_rows*buffer->tile_depth_num_cols;

	#ifdef OCL_vgf_PRINT_EN_DEBUG
	printf("vgf_load: tile_num=%d \n",tile_num);
	printf("vgf_load: dst_width=%d \n",dst_width);
	printf("vgf_load: upper_nearst_even_row=%d \n",upper_nearst_even_row);
	printf("vgf_load: dst_ptr_offset=%d \n",dst_ptr_offset);
	printf("vgf_load: ch_data_size=%d \n",ch_data_size);
	printf("vgf_load: dst=%p \n",buffer->input+dst_ptr_offset);
	printf("vgf_load: dst stride=%d \n",buffer->tile_size_x*2);
	printf("vgf_load: src_ptr_offset=%d \n",src_ptr_offset);
	printf("vgf_load: src_width=%d \n",src_width);
	printf("vgf_load: src=%p \n",input+dst_ptr_offset);
	printf("vgf_load: ch_data_size=%d \n",ch_data_size);
	#endif

	
	buffer->load_event_depth =
		async_work_group_copy2D(
				&buffer->input[dst_ptr_offset],				    // dst
				dst_width*short_size,							// dst width
				buffer->tile_size_x*2*short_size,				// dst stride
				&input[src_ptr_offset],							// src
				src_width*short_size, 							// src width
				buffer->depth_width*short_size,					// src stride
				ch_data_size*short_size,						// size
				0);
				
				
	// DMA indexes from previous w_strip		
	if (buffer->vmem_num_rows2DMA > 0){
		dst_width  		= buffer->tile_depth_num_cols; 
		src_width   	= buffer->tile_depth_num_cols;
		dst_ptr_offset  = buffer->vmem_num_cols2copy;	
		src_ptr_offset  = buffer->depth_src_col;
		ch_data_size 	= buffer->tile_depth_num_cols*buffer->overlapping_y;

		#ifdef OCL_vgf_PRINT_EN_DEBUG	
		printf("DMAin indexces: src=%p, src_width=%d \n",&overlapping_rows[src_ptr_offset],src_width );
		printf("DMAin indexces: dst=%p, dst_width=%d \n",&buffer->input[dst_ptr_offset*2] ,dst_width );
		#endif
	
		buffer->load_event_indexes =
			async_work_group_copy2D(
					(local uint *)(&buffer->input[dst_ptr_offset*2]),		// dst
					dst_width*uint32_size,									// dst width
					buffer->tile_size_x*uint32_size,		        		// dst stride
					(global uint *)(&overlapping_rows[src_ptr_offset]),  	// src
					src_width*uint32_size, 									// src width
					buffer->depth_width*uint32_size,						// src stride
					ch_data_size*uint32_size,								// size
					0);
	}				

}
// ***********************************************************************
void ocl_vgf_load_buffer_wait(local ocl_InuVgf_buffer_t* buffer)
{
	if (buffer->load_event_depth != -1){
		wait_group_events(1, &buffer->load_event_depth);
		buffer->load_event_depth = -1; 
	}
	
	if (buffer->load_event_indexes != -1){
		wait_group_events(1, &buffer->load_event_indexes);
		buffer->load_event_indexes = -1; 
	}
}
// ***********************************************************************
void ocl_vgf_copy_overlapping_cols(local ocl_InuVgf_buffer_t* dst_buffer,
                                   local ocl_InuVgf_buffer_t* src_buffer,
								   local short *  int_idx)
{
    short32  idx = vload32(0, int_idx);
	short32  valid_idx;
	ushort32 indexes;
	int   num_row,num_col,src_col,row , tile_size_x;
	int   src_idx_offset , full_iter_index , dst_idx_offset;
	short remainning_num_col;
	local ushort * src_ptr = (local ushort *)(&src_buffer->input[0]);
	local ushort * dst_ptr = (local ushort *)(&dst_buffer->input[0]);
	
	if (dst_buffer->vmem_num_cols2copy > 0) {
		num_row 	= (int)dst_buffer->tile_depth_num_rows+(int)dst_buffer->vmem_num_cols2copy;
		num_col 	= (int)src_buffer->overlapping_x;
		src_col 	= (int)src_buffer->tile_size_x - num_col;
		tile_size_x = (int)src_buffer->tile_size_x;
		
		for (row=0; row<num_row; row++)
		{// copy  num row of uint32 16 copies at one iteration 
			remainning_num_col = num_col;
			full_iter_index    = 0;
			while (remainning_num_col > 0 ){
			
				valid_idx  		= idx < (short)(2*remainning_num_col);
			    src_idx_offset 	= row*tile_size_x + src_col + full_iter_index*16;
			    dst_idx_offset 	= row*tile_size_x + full_iter_index*16;
				            
			    //load 16 indexces of uint as 32 ushorts  values from src_idx_offset (just for using mask)
                indexes         = vload32( 0, src_ptr + 2*src_idx_offset );		
				vstore32_mask( indexes, 0, dst_ptr+2*dst_idx_offset, convert_ushort32(valid_idx));
				
				full_iter_index++;
				remainning_num_col = max(0, remainning_num_col-16);
			}
		}
	}
}
// ***********************************************************************
void ocl_vgf_store_buffer(local ocl_InuVgf_buffer_t * buffer, global short* output,
                          global uint * overlapping_rows, int total_unique_output)
{
	// store the unique point cloud result from the tile
	if (buffer->num_unique_outputs > 0){
		// start output results DMA
		buffer->store_event_results =
				async_work_group_copy(
						output + 3*total_unique_output, 	// dst
						buffer->output, 					//src
						buffer->num_unique_outputs*3*sizeof(short),		// for x ,y and z
						0);
	}
	else
	{
		buffer->store_event_results = -1;
	}
	
	short idx_num_cols;
	short dst_first_col;
	short src_first_row , src_ptr_offset;
	short tile_width;
	// store the tile overlapping rows at overlapping_rows buffer for the next w strip
	if ( buffer->last_w_strip_flag !=1){
		short tile_size_x   = buffer->tile_size_x;
		short tile_size_y   = buffer->tile_size_y;
		short overlapping_x = buffer->overlapping_x;
		short overlapping_y = buffer->overlapping_y;
		if ( buffer->last_l_strap_flag ==1){
			idx_num_cols = buffer->tile_depth_num_cols + buffer->vmem_num_cols2copy;
			tile_width   = idx_num_cols;
		}
		else{
			idx_num_cols = tile_size_x - overlapping_x;
			tile_width   = tile_size_x;
		}
		
		if ( buffer->first_l_strap_flag ==1){
			dst_first_col = buffer->depth_src_col;
		}
		else{
			dst_first_col = buffer->depth_src_col - overlapping_x;
		}
		src_first_row = tile_size_y - overlapping_y;
	
	    src_ptr_offset = src_first_row*tile_size_x;

		#ifdef OCL_vgf_PRINT_EN_DEBUG
		printf("DMAout indexces: src=%p, src_width=%d \n",&buffer->input[src_ptr_offset*2],idx_num_cols );
		printf("DMAout indexces: dst=%p, dst_width=%d \n",&overlapping_rows[dst_first_col] ,idx_num_cols );
		#endif

		buffer->store_event_indexes =
			async_work_group_copy2D(
				(global uint *)(&overlapping_rows[dst_first_col]),		    // dst
				idx_num_cols*sizeof(int),									// dst width
				buffer->depth_width*sizeof(int),					        // dst stride
				(local uint *)(&buffer->input[src_ptr_offset*2]),			// src
				idx_num_cols*sizeof(int), 									// src width
				tile_size_x*sizeof(int),        							// src stride
				idx_num_cols*overlapping_y*sizeof(int),						// size
				0);		
	}
}
// ***********************************************************************
void ocl_vgf_store_buffer_wait(local ocl_InuVgf_buffer_t* buffer)
{
	if (buffer->store_event_results != -1){
		wait_group_events(1, &buffer->store_event_results);
		// Reset buffer storing state
		buffer->store_event_results = -1;
	}
	
	if (buffer->store_event_indexes != -1){
		wait_group_events(1, &buffer->store_event_indexes);
		// Reset buffer storing state
		buffer->store_event_indexes = -1;
	}
}
// ***********************************************************************
void ocl_vgf_process_buffer(ocl_InuVgf_buffer_t *buffer, 
                            local int * int_idx, 
							local short* short_idx,
							ocl_ctl_vgf_general_params_t *vgf_gen_params)
{
	int d_num_cols			= buffer->tile_depth_num_cols;
	int d_num_rows    		= buffer->tile_depth_num_rows;
	int tile_size_x  		= buffer->tile_size_x;

	int upper_nearst_even_row   = ((buffer->tile_depth_num_rows +1)/2)<<1;
    int src_depth_tile_offset0  = (buffer->vmem_num_rows2DMA*2+upper_nearst_even_row)*tile_size_x+buffer->vmem_num_cols2copy*2; // short
    int src_depth_tile_offset1  =  src_depth_tile_offset0 + d_num_cols; // short
	
    int idx_tile_offset0        = buffer->vmem_num_rows2DMA*tile_size_x+buffer->vmem_num_cols2copy;              // unsigned int
	int src_row_odd,src_row_pair , src_depth_row_offset , src_depth_offset, dst_idx_tile_row_offset, dst_idx_tile_offset;
	int remainning_num_col , full_iter_index;

	int16 in_offsets0 = (int16)( 0, 2, 4, 6, 8,10,12,14,16,18,20,22,24,26,28,30);
	int16 in_offsets1 = (int16)(32,34,36,38,40,42,44,46,48,50,52,54,56,58,60,62);
		
	short32 idx  = vload32(0, &short_idx[0]);
	
	short32 valid_depth_idx;
	short32 X ,Y ,Z , tmp_cmp;
	short32 min_b_x,min_b_y,min_b_z;
	short32 inv_voxel_size_x_Q15,inv_voxel_size_y_Q15,inv_voxel_size_z_Q15;
	short32 cc_x,cc_y;
	short32 inv_fc_x_Q23,inv_fc_y_Q23;
	short32 div_b_x,div_b_y,div_b_z;
	short32 pixel_row_index,pixel_col_index;
	short32 maxDepth = (short32)vgf_gen_params->MaxDepthMm;

	local short * input_depth = (local short *)buffer->input;
	local uint  * output_idx  = (local uint  *)buffer->input;
	local ushort * output_idx32  = (local ushort  *)buffer->input;
	acc40x32 	acc_res;
	short32 voxel_index_x,voxel_index_y,voxel_index_z;
	uint16   idx_low,idx_high;		
	ushort32 idx32_lsb,idx32_msb;		
    min_b_x = (short32)vgf_gen_params->min_b_x;
    min_b_y = (short32)vgf_gen_params->min_b_y;
    min_b_z = (short32)vgf_gen_params->min_b_z;

    inv_voxel_size_x_Q15 = (short32)vgf_gen_params->inv_voxel_size_x_Q15;
    inv_voxel_size_y_Q15 = (short32)vgf_gen_params->inv_voxel_size_y_Q15;
    inv_voxel_size_z_Q15 = (short32)vgf_gen_params->inv_voxel_size_z_Q15;

    div_b_x = (short32)vgf_gen_params->div_b_x;
    div_b_y = (short32)vgf_gen_params->div_b_y;
    div_b_z = (short32)vgf_gen_params->div_b_z;
	
    cc_x = (short32)vgf_gen_params->cc_x;
    cc_y = (short32)vgf_gen_params->cc_y;
    inv_fc_x_Q23 = (short32)vgf_gen_params->inv_fc_x_Q23;
    inv_fc_y_Q23 = (short32)vgf_gen_params->inv_fc_y_Q23;

	for (int src_row=0; src_row<d_num_rows; src_row++){
		src_row_odd 	= src_row & 1;
		src_row_pair 	= src_row>>1;

		if (src_row_odd==0) { //even row
			src_depth_row_offset = src_depth_tile_offset0 + src_row_pair*tile_size_x*2;
		}
		else { //odd row
			src_depth_row_offset = src_depth_tile_offset1 + src_row_pair*tile_size_x*2;
		}
		remainning_num_col = d_num_cols;
		dst_idx_tile_row_offset = idx_tile_offset0 + src_row*tile_size_x;
		pixel_row_index = (short32)(src_row+buffer->depth_src_row);
		
		full_iter_index    = 0;
		while (remainning_num_col > 0 ){

			valid_depth_idx = idx < (short)remainning_num_col;
		    src_depth_offset 	= src_depth_row_offset    + full_iter_index*OCL_vgf_SIMD_WIDTH;  // offset of short		
		    dst_idx_tile_offset = dst_idx_tile_row_offset + full_iter_index*OCL_vgf_SIMD_WIDTH;  // offset of unsigned int
		
			Z         = vload32( 0, &input_depth[src_depth_offset] );
			Z         = min(Z,maxDepth);
			tmp_cmp   = -(Z >  (short32)0);  			// indicate if Z>0
			pixel_col_index = (short32)(buffer->depth_src_col+ full_iter_index*OCL_vgf_SIMD_WIDTH);
			pixel_col_index = pixel_col_index + idx;
			
			X = pixel_col_index-cc_x;
			acc_mpy(&acc_res, X, inv_fc_x_Q23); //acc_res = (pixel_col_index-cc_x)*inv_fc_x_Q23

			X.lo =  convert_short16(acc_convert_int16(acc_lo(acc_res)) >> 10);
			X.hi =  convert_short16(acc_convert_int16(acc_hi(acc_res)) >> 10);			

            acc_mpy(&acc_res, X, Z);
			// Right shift 13 (the acc_res by 2^13), remain shifts = 0
			X.lo =  convert_short16(acc_convert_int16(acc_lo(acc_res)) >> 13);
			X.hi =  convert_short16(acc_convert_int16(acc_hi(acc_res)) >> 13);
			
            // calc point cloud axis y
			Y = pixel_row_index-cc_y;
			acc_mpy(&acc_res, Y, inv_fc_y_Q23);  //acc_res = (pixel_row_index-cc_y)*inv_fc_y_Q23

			// Right shift 10 (the acc_res by 2^10), remain shifts = 13
			Y.lo =  convert_short16(acc_convert_int16(acc_lo(acc_res)) >> 10);
			Y.hi =  convert_short16(acc_convert_int16(acc_hi(acc_res)) >> 10);

            acc_mpy(&acc_res, Y, Z);
			
			
			// Right shift 13 (the acc_res by 2^13), remain shifts = 0
			Y.lo =  convert_short16(acc_convert_int16(acc_lo(acc_res)) >> 13);
			Y.hi =  convert_short16(acc_convert_int16(acc_hi(acc_res)) >> 13);

			//calc voxels index per axis
			acc_mpy(&acc_res, X, inv_voxel_size_x_Q15);
			voxel_index_x.lo = convert_short16(acc_convert_int16(acc_lo(acc_res)) >> 15);
			voxel_index_x.hi = convert_short16(acc_convert_int16(acc_hi(acc_res)) >> 15);
			voxel_index_x    = (voxel_index_x - min_b_x)*tmp_cmp;

			acc_mpy(&acc_res, Y, inv_voxel_size_y_Q15);
			voxel_index_y.lo = convert_short16(acc_convert_int16(acc_lo(acc_res)) >> 15);
			voxel_index_y.hi = convert_short16(acc_convert_int16(acc_hi(acc_res)) >> 15);
			voxel_index_y    = (voxel_index_y - min_b_y)*tmp_cmp;

			acc_mpy(&acc_res, Z, inv_voxel_size_z_Q15);
			voxel_index_z.lo = convert_short16(acc_convert_int16(acc_lo(acc_res)) >> 15);
			voxel_index_z.hi = convert_short16(acc_convert_int16(acc_hi(acc_res)) >> 15);
			voxel_index_z    = (voxel_index_z - min_b_z)*tmp_cmp;		

			#ifdef OCL_vgf_PRINT_EN_DEBUG_d
			printf(" (Z >  (short32)0) = %#v32hi\n", tmp_cmp);
			printf(" Z[0:31] = %#v32hd\n", Z);
			printf(" X[0:31] = %#v32hd\n", X);
			printf(" Y[0:31] = %#v32hd\n", Y);
			printf(" voxel_index_x[0:31] = %#v32hx\n", voxel_index_x);
			printf(" voxel_index_y[0:31] = %#v32hx\n", voxel_index_y);
			printf(" voxel_index_z[0:31] = %#v32hx\n", voxel_index_z);
			#endif	
			
			idx_low	 = ((convert_uint16)(voxel_index_z.lo)*vgf_gen_params->div_b_y + convert_uint16(voxel_index_y.lo))<<11;
			idx_low	 = (idx_low + convert_uint16(voxel_index_x.lo));
			idx_high = ((convert_uint16)(voxel_index_z.hi)*vgf_gen_params->div_b_y + (convert_uint16)(voxel_index_y.hi))<<11;
			idx_high = (idx_high + convert_uint16(voxel_index_x.hi));

			#ifdef OCL_vgf_PRINT_EN_DEBUG_d
			printf(" idx_low[0:15]LSB  = %#v16hx\n", idx_low);
			printf(" idx_low[0:15]MSB  = %#v16hx\n", idx_low>>16);
			printf(" idx_high[0:15]LSB = %#v16hx\n", idx_high);
			printf(" idx_high[0:15]MSB = %#v16hx\n", idx_high>>16);
			printf(" valid_depth_idx[0:15] = %#v32hd\n", valid_depth_idx);
			#endif			
			
			// convert 2 x 16 uint indexes to 2x32 ushort - to used the mask at the vscatter
			idx32_lsb.lo = convert_ushort16(idx_low);
			idx32_msb.lo = convert_ushort16(idx_low>>16);
			idx32_lsb.hi = convert_ushort16(idx_high);
			idx32_msb.hi = convert_ushort16(idx_high>>16);
			
			vscatter32(idx32_lsb, &output_idx32[2*dst_idx_tile_offset]  , in_offsets0,in_offsets1 , valid_depth_idx);
			vscatter32(idx32_msb, &output_idx32[2*dst_idx_tile_offset+1], in_offsets0,in_offsets1 , valid_depth_idx);
			
			//if (remainning_num_col >= OCL_vgf_SIMD_WIDTH)
			//{
			//	//vscatter16(idx_low, &output_idx[dst_idx_tile_offset],    in_offsets0); // , valid_depth_idx.lo);
			//	//vscatter16(idx_high,&output_idx[dst_idx_tile_offset+16], in_offsets0); // , valid_depth_idx.hi);
			//	vstore16( idx_low,  0, &output_idx[dst_idx_tile_offset]);
			//	vstore16( idx_high, 0, &output_idx[dst_idx_tile_offset+16]);
			//}
			//else{
			//	// !!!!!!!!!!!!!!! meanwhile instead of  vscatter16 !!!!!!!!!!!!!!!!!!!
			//	for (int k1=0;k1<16;k1++)
			//	{
			//		if (valid_depth_idx.lo[k1] !=0)
			//		{
			//			output_idx[dst_idx_tile_offset+in_offsets0[k1]] = idx_low[k1];
			//		}
			//	}			
			//	for (int k2=0;k2<16;k2++)
			//	{
			//		if (valid_depth_idx.hi[k2] !=0)
			//		{
			//			output_idx[dst_idx_tile_offset+16+in_offsets0[k2]] = idx_high[k2];
			//		}
			//	}	
			//}			
			
			full_iter_index++;
			remainning_num_col = max(0, remainning_num_col-OCL_vgf_SIMD_WIDTH);
			}
	}
	
	
	// start invalidate duplicated indexes	
	//-------------------------------------
	
	int  tile_num_rows = d_num_rows +(int)buffer->vmem_num_rows2DMA;
	int  tile_num_cols =	d_num_cols + (int)buffer->vmem_num_cols2copy;
    uint voxel_index ;
	ushort index_x,index_y,index_z;
	short  win_size_x, win_size_y,zz;
	short  voxel_leaf_x_size = vgf_gen_params->voxel_leaf_x_size;
	short  voxel_leaf_y_size = vgf_gen_params->voxel_leaf_y_size;
	short  voxel_leaf_z_size = vgf_gen_params->voxel_leaf_z_size;
	short  lut_step_size     = vgf_gen_params->lut_step_size;
    int    start_win_col, last_win_col , win_first_col ; 
    int    start_win_row, last_win_row , index_offset;
	int16  diff_idx;
	uint16  indexes_vec , aa;
	int    num_overlapping_rows,num_overlapping_cols;
	short  num_unique_outputs=0;
	local short * local_pcl_out = (local short *)buffer->output;
	short3 pcl_point;
				
	if (buffer->last_w_strip_flag == 1)
	{
		num_overlapping_rows = 0;
	}
	else
	{
		num_overlapping_rows = buffer->overlapping_y;
	}
	
	if (buffer->last_l_strap_flag == 1)
	{
		num_overlapping_cols = 0;
	}
	else
	{
		num_overlapping_cols = buffer->overlapping_x;
	}

	for (int idx_row=0; idx_row<(tile_num_rows-num_overlapping_rows); idx_row++){
		for (int idx_col=0; idx_col<(tile_num_cols-num_overlapping_cols); idx_col++)
		{
			voxel_index = output_idx[idx_row*tile_size_x + idx_col];
			if (voxel_index>0)
			{
			    // output pcl from voxel index
				//-----------------------------
				index_x = (ushort)(voxel_index & 0x3ff);
				index_y = (ushort)((voxel_index>>11) & 0x3ff);
				index_z = (ushort)(voxel_index>>22);

				pcl_point[0] = (short)(( (index_x+min_b_x[0])*voxel_leaf_x_size + (voxel_leaf_x_size/2)));
				pcl_point[1] = (short)(( (index_y+min_b_y[0])*voxel_leaf_y_size + (voxel_leaf_y_size/2)));
				pcl_point[2] = (short)(( (index_z+min_b_z[0])*voxel_leaf_z_size + (voxel_leaf_z_size/2)));
				
				vstore3(pcl_point,num_unique_outputs,local_pcl_out);
				num_unique_outputs++;

				// determine the search window
				zz =  (short)(( ((index_z+min_b_z[0])*voxel_leaf_z_size)/lut_step_size));
				win_size_x = vgf_gen_params->lut_x[zz];
				win_size_y = vgf_gen_params->lut_y[zz];			
				short win_size_x_bw = win_size_x;
				
				#ifdef OCL_ADJUST_BW_SEARCH_WINDOW
				if ((win_size_x>=8) && (win_size_x<=11)){
					win_size_x_bw=7;
				}
				if ((win_size_x>=16) && (win_size_x<=18)){
					win_size_x_bw=15;
				}
				if (win_size_x==24){
					win_size_x_bw=23;
				}
				#endif

	            start_win_col = max(0,idx_col-win_size_x_bw);
	            last_win_col  = min(tile_num_cols-1,idx_col+win_size_x);
				start_win_row = idx_row;
	            last_win_row  = min(tile_num_rows-1,idx_row+win_size_y);
	
				win_first_col = idx_col+1;
				if (win_first_col >= tile_num_cols) {
					win_first_col = start_win_col;
					start_win_row = idx_row + 1;
				}
				
				#ifdef OCL_vgf_PRINT_EN_DEBUG_d
				if(voxel_index==-1)
				{
					printf(" tile_num = %d\n", buffer->tile_num);
					printf(" zz=%d , win_size_x=%d , win_size_y = %d\n", zz,win_size_x,win_size_y);
					printf(" serach rows= [%d,%d] , serach cols= [%d,%d] , win_first_col=%d\n", start_win_row, last_win_row,start_win_col,last_win_col,win_first_col);
				}
				#endif


				for (int win_row=start_win_row; win_row <= last_win_row; win_row++){
					int start_row_index = win_row*tile_size_x + win_first_col;
					int num_search_col = last_win_col - win_first_col + 1;
					for ( int col_iter_index=0 ; col_iter_index < num_search_col ; col_iter_index=col_iter_index+16) {
						index_offset = start_row_index + col_iter_index;
						indexes_vec  = vload16( 0, &output_idx[index_offset] );
						diff_idx = (indexes_vec!=voxel_index);
						uint16  zeros_16 = (uint16)0;
						vvmov_p(&zeros_16, indexes_vec, diff_idx);
						vstore16( zeros_16,  0, &output_idx[index_offset]);
				
						#ifdef OCL_vgf_PRINT_EN_DEBUG_d
						if(voxel_index==-1)
						{
							printf(" \n\n win_row= %d num_search_col=%d, col_iter_index=%d ,voxel_index=%d ,voxel_index=%x\n", win_row,num_search_col,col_iter_index, voxel_index, voxel_index);
							printf(" indexes_vec= %x\n", indexes_vec);
							printf(" indexes_vec[0:15]LSB  = %#v16hx\n", indexes_vec);
							printf(" indexes_vec[0:15]MSB  = %#v16hx\n", indexes_vec>>16);
							printf(" aa[0:15]LSB  = %#v16hx\n", aa);
							printf(" aa[0:15]MSB  = %#v16hx\n", aa>>16);
							printf(" diff_idx  = %#v16hx\n", diff_idx);
						}
						#endif
					} // for ( int col_iter_index=0 ; col_iter_index < num_search_col ; col_iter_index=col_iter_index+16)
					win_first_col = start_win_col;
				} // for (int win_row=start_win_row; win_row < last_win_row; win_row++)				
			} // if (voxel_idx>0)
		} 	// end of (int idx_col=0; idx_col<tile_num_cols; tile_num_cols++)
	} 		// end of (int idx_row=0; idx_row<tile_num_rows; idx_row++)
	
	buffer->num_unique_outputs = num_unique_outputs;
	#ifdef OCL_vgf_PRINT_EN_DEBUG_d
	printf(" num_unique_outputs  = %d\n", num_unique_outputs);
	#endif
}
//   ***********************************************************************
__attribute__(( reqd_work_group_size(1, 1, 1)))

kernel void ev_inu_ocl_vgf_kernel(	global ocl_ctl_vgf_t    * ctl,
                    				global short 			* restrict out,
									global short 			* restrict in ,
									global unsigned int		* restrict overlapping_rows
									)
{
  	local short *output [2];
  	local short *input  [2];
  	local ocl_InuVgf_buffer_t buffer[2];             // new struct  
  	local short * local_mem_ptr = (local short*)ctl->local_mem_ptr;
  	//int local_mem_size = (ctl->local_mem_size - 4096) >> 1;  // can be used to dynamic setting the tile_size_x
	ocl_ctl_vgf_general_params_t vgf_gen_params= ctl->vgf_gen_params_ptr;
  	int tile_size_x    = (int)vgf_gen_params.tile_size_x;
  	int tile_size_y    = (int)vgf_gen_params.tile_size_y;
  	int buf;
  	int rows_tile,cols_tile,input_tile_size,output_tile_size;
  	int total_unique_output	= 0;
  	int first_loop 			= 1;

  #ifdef OCL_vgf_PRINT_EN_DEBUG
  	printf("INFO: tile_size_x=%i \n",tile_size_x);
  	printf("INFO: tile_size_y=%i \n",tile_size_y);
  #endif
  	
  	rows_tile = min(tile_size_y, ctl->input_height);  		// num rows at tile
  	cols_tile = min(tile_size_x, ctl->input_width);  		// num cols at tile
  	input_tile_size  = (rows_tile+1) * cols_tile*2;			// the input is shorts, but we used it to indexes too (index unsigned int)
  															// the +1 at the row, to resereve memory for indexes in case of num rows odd
  	output_tile_size = rows_tile * cols_tile*3;             // each output is 3 shorts
 
  	for ( buf = 0; buf < 2; buf ++)
  	{                                     
  		input[buf] = local_mem_ptr;
  		local_mem_ptr += input_tile_size;
  		output[buf] =  local_mem_ptr;
  		local_mem_ptr += output_tile_size;
  	}
  #ifdef OCL_vgf_PRINT_EN_DEBUG
  	printf("INFO: EVSS_CFG_VCCM_SIZE=%i \n",ctl->local_mem_size);
  	//printf("INFO: local_mem_size=%i \n",local_mem_size);
  	printf("INFO: in  at %p\n", in);
  	printf("INFO: out at %p\n", out);
  	printf("INFO: overlapping_rows at %p\n", overlapping_rows);
  	printf("INFO: buffer[0].input at %p\n",  input[0]);
  	printf("INFO: buffer[0].output at %p\n", output[0]);
  	printf("INFO: buffer[1].input at %p\n",  input[1]);
  	printf("INFO: buffer[1].output at %p\n", output[1]);
  #endif
  
  	ocl_vgf_init_buffer(&buffer[0], ctl, input[0], output[0] );
  	ocl_vgf_init_buffer(&buffer[1], ctl, input[1], output[1] );
  		
  	local int   int_idx[OCL_vgf_SIMD_WIDTH];  			// OCL_vgf_SIMD_WIDTH=32
  	local short short_idx[OCL_vgf_SIMD_WIDTH];
  	
    int overlapping_x 			= (int)vgf_gen_params.tile_overlapping_x;
    int overlapping_y 			= (int)vgf_gen_params.tile_overlapping_y; 
    int  depth_src_row;        	// holds the row number of the depth image to DMA into the vmem
    int  depth_src_col;        	// holds the col number of the depth image to DMA into the vmem
  	int  tile_depth_num_rows;	// holds number of rows to DMA from depth image to VMEM
  	int  tile_depth_num_cols;	// holds number of cols to DMA from depth image to VMEM
  	int  vmem_depth_dst_row;	// holds the row number at the vmem buffer to DMA in the depth 
  	int  vmem_depth_dst_col;	// holds the col number at the vmem buffer to DMA in the depth 
  	int  vmem_num_cols2copy;	// holds the number of overlapping indexes cols from previous tile at the same w strip, to copy to current buffer
  	int  vmem_num_rows2DMA;	    // holds the number of overlapping indexes rows from previous w strip to copy to VMEM (from tmp_index_mem_ptr)
  	int  first_w_strip_flag;	// indicator for all tiles at the first w strip	    
  	int  last_w_strip_flag;	    // indicator for all tiles at the last w strip
  	int  first_l_strap_flag;	// indicator for all tiles at the first l strap
  	int  last_l_strap_flag;	    // indicator for all tiles at the last l strap
  	int  curr_vmem_buf = 0;
  	int  depth_rows_remaining 	= ctl->input_height;    // H;
  	int  depth_cols_remaining 	= ctl->input_width;     // W;
	int  tile_num=-1;			// used for debug
	
  	// Init map offsets for gather/scatter
  	for (int lane = 0; lane < OCL_vgf_SIMD_WIDTH; lane++)
  	{
  	    int_idx[lane]   =  (int)lane;
  		short_idx[lane] =  (short)lane;
  	}
  
    // start load the first tile
  	depth_src_row = 0;
  	depth_src_col = 0;
  	tile_depth_num_rows =  min( tile_size_y , depth_rows_remaining);
  	tile_depth_num_cols =  min( tile_size_x , depth_cols_remaining);
  	vmem_depth_dst_row  = 0;
  	vmem_depth_dst_col  = 0;
  	vmem_num_cols2copy  = 0; // first l_strap
  	vmem_num_rows2DMA   = 0; // first w_strip
  	first_w_strip_flag  = 1;
  	first_l_strap_flag  = 1;
  	last_w_strip_flag	= 0; 
  	last_l_strap_flag	= 0;
  	if (tile_depth_num_rows == ctl->input_height){
  		last_w_strip_flag = 1;
		}

  	if ( tile_depth_num_cols == ctl->input_width){
  		last_l_strap_flag = 1;
		}
  
    tile_num++;
  	ocl_vgf_load_buffer(&buffer[curr_vmem_buf], in, overlapping_rows, depth_src_row, depth_src_col,
  	                    tile_depth_num_rows,tile_depth_num_cols,vmem_depth_dst_row, 
  						vmem_depth_dst_col,vmem_num_cols2copy,vmem_num_rows2DMA,
  						first_w_strip_flag,first_l_strap_flag,last_w_strip_flag,last_l_strap_flag , tile_num); 
  		
  	depth_rows_remaining  = max(0,depth_rows_remaining-tile_depth_num_rows);
  	depth_cols_remaining  = max(0,depth_cols_remaining-tile_depth_num_cols);
  	first_l_strap_flag = 0;
  	
  	// loops over all tiles in w_strip and over all l_strap
	while (depth_rows_remaining>0)	{
	#ifdef OCL_vgf_PRINT_EN_DEBUG
	printf("vgf_kernel:depth_rows_remaining=%d \n",depth_rows_remaining);
	#endif
		if(first_loop == 1){
			first_loop = 0;
		}
		else{
			// start of new w_strip, so the first tile is first_l_strap_flag
			first_w_strip_flag 		= 0;
			depth_src_row 			= depth_src_row + tile_depth_num_rows;
			tile_depth_num_rows 	= min( tile_size_y-overlapping_y , depth_rows_remaining);
			depth_rows_remaining  	= max( depth_rows_remaining - tile_depth_num_rows , 0);
			vmem_depth_dst_row      = overlapping_y ;
			depth_cols_remaining 	= ctl->input_width;
			first_l_strap_flag 		= 1;
			vmem_num_rows2DMA 		= overlapping_y;
		}
		
		while (depth_cols_remaining>0){
			#ifdef OCL_vgf_PRINT_EN_DEBUG
				printf("vgf_kernel:depth_cols_remaining=%d , curr_vmem_buf=%d \n",depth_cols_remaining , curr_vmem_buf);		
			#endif
			
			// --- copy overlapping columns indexes from the other buffer ---
			//                              dst buffer          , src buffer
			ocl_vgf_copy_overlapping_cols(&buffer[curr_vmem_buf],&buffer[1-curr_vmem_buf],short_idx);   // input src buffer  
			
			// start DMA for the following  tile
			if (first_l_strap_flag==1){
				depth_src_col = 0;
				tile_depth_num_cols = min( tile_size_x , depth_cols_remaining);
				vmem_depth_dst_col  = 1;
				vmem_num_cols2copy  = 0; // first l_strap
			}
			else{
			    depth_src_col = depth_src_col + tile_depth_num_cols;
				tile_depth_num_cols = min( tile_size_x-overlapping_x , depth_cols_remaining);
				vmem_depth_dst_col  = overlapping_x;
				vmem_num_cols2copy  = overlapping_x; // first l_strap
			}
	        last_w_strip_flag=0; 
			last_l_strap_flag=0;
			if (depth_src_row + tile_depth_num_rows == (ctl->input_height)){
				last_w_strip_flag = 1;
			}        
			if ( depth_src_col + tile_depth_num_cols == (ctl->input_width)){
				last_l_strap_flag = 1;
			}

            tile_num++;
			ocl_vgf_load_buffer(&buffer[1-curr_vmem_buf], in, overlapping_rows, depth_src_row, depth_src_col,
							    tile_depth_num_rows,tile_depth_num_cols,vmem_depth_dst_row, 
						        vmem_depth_dst_col,vmem_num_cols2copy,vmem_num_rows2DMA,
						        first_w_strip_flag,first_l_strap_flag,last_w_strip_flag,last_l_strap_flag, tile_num); 

			first_l_strap_flag = 0;
			depth_cols_remaining  = max( depth_cols_remaining - tile_depth_num_cols , 0);

			// verify that vmem_input_buff[curr_vmem_buf] buffer loaded - DMA  finished
			ocl_vgf_load_buffer_wait(&buffer[curr_vmem_buf]);
		
			ocl_vgf_store_buffer_wait(&buffer[curr_vmem_buf]);

			#ifdef OCL_vgf_PRINT_EN_DEBUG_d
			//-- print input buffer loop depth only, for debug ---------
			if(0) 	{
				if (buffer[curr_vmem_buf].tile_num==16) {
					int trow,tcol,vidx,voffset;
					int num_row = buffer[curr_vmem_buf].tile_depth_num_rows;
					int num_col = buffer[curr_vmem_buf].tile_depth_num_cols;
					int src_row = buffer[curr_vmem_buf].depth_src_row;
					int src_col = buffer[curr_vmem_buf].depth_src_col;
					
					short * vptr= buffer[curr_vmem_buf].input;
					voffset = (tile_size_x*tile_size_y) + buffer[curr_vmem_buf].vmem_num_cols2copy*2;
					printf("vptr=%p ,&vptr[offset]=%p\n",vptr,&vptr[voffset]);
					printf("src_row=%d ,src_col=%d,num_row=%d ,num_col=%d\n",src_row,src_col,num_row,num_col);
					
					for (trow=0; trow<num_row;trow++) 	{
						for (tcol=0; tcol<num_col;tcol++) 	{
							vidx = (trow>>1)*tile_size_x*2+tcol + (trow&1)*num_col;
							printf("row=%d,col=%d, data=%d ,in=%d\n",src_row+trow,src_col+tcol,vptr[vidx+voffset] , in[(src_row+trow)*ctl->input_width +(src_col+tcol)]);
						}
					}	
				}
			}	
			#endif		

			// --- process curr_vmem_buf buffer ---
			ocl_vgf_process_buffer(&buffer[curr_vmem_buf], int_idx , short_idx , &vgf_gen_params);

			#ifdef OCL_vgf_PRINT_EN_DEBUG_d
			// --- print input buffer loop after processing with/without invalidation ---
			if (0) 	{ 
				printf("process_tile_num==%d , curr_vmem_buf=%d \n",buffer[curr_vmem_buf].tile_num,curr_vmem_buf);
				if (buffer[curr_vmem_buf].tile_num==46)  { // start from zero
					int trow,tcol,vidx;
					int tile_size_x = buffer[curr_vmem_buf].tile_size_x;
					int tile_size_y = buffer[curr_vmem_buf].tile_size_y;
					int start_row   = buffer[curr_vmem_buf].depth_src_row - buffer[curr_vmem_buf].vmem_num_rows2DMA;
					int start_col   = buffer[curr_vmem_buf].depth_src_col - buffer[curr_vmem_buf].vmem_num_cols2copy;
					uint * idex_ptr= (uint *)buffer[curr_vmem_buf].input;

					printf("---->start_row=%d,start_col=%d \n",start_row,start_col);
					for (trow=0; trow<tile_size_y;trow++) {
						for (tcol=0; tcol<tile_size_x;tcol++)	{
							vidx = trow*tile_size_x+tcol;
							printf("row=%d,col=%d, add = %p , data=%d \n",trow+start_row,tcol+start_col, &idex_ptr[vidx] ,idex_ptr[vidx]);
						}
					}	
				}
			}
			#endif

			// -- initiate DMA to output results & overlapping rows from curr_vmem_buf --
			ocl_vgf_store_buffer(&buffer[curr_vmem_buf], out , overlapping_rows , total_unique_output);
			
			total_unique_output = total_unique_output + buffer[curr_vmem_buf].num_unique_outputs;
			#ifdef OCL_vgf_PRINT_EN_DEBUG
			printf("total_unique_output=%d, num_unique_outputs=%d \n",total_unique_output,buffer[curr_vmem_buf].num_unique_outputs);
			#endif
		    
	        // swap vmem_buffers
			curr_vmem_buf = 1-curr_vmem_buf;
		}  	// end of while (depth_cols_remaining>0)
	}		// end of while (depth_rows_remaining>0)
	
	// --  last tile ---------
	// copy overlapping columns indexes from the other buffer
	//                              dst buffer          , src buffer
	ocl_vgf_copy_overlapping_cols(&buffer[curr_vmem_buf],&buffer[1-curr_vmem_buf],short_idx);   // input src buffer  

	ocl_vgf_load_buffer_wait(&buffer[curr_vmem_buf]);	

	ocl_vgf_store_buffer_wait(&buffer[curr_vmem_buf]);

	ocl_vgf_process_buffer(&buffer[curr_vmem_buf], int_idx , short_idx , &vgf_gen_params);
			
	ocl_vgf_store_buffer(&buffer[curr_vmem_buf], out , overlapping_rows , total_unique_output);
			
	total_unique_output = total_unique_output + buffer[curr_vmem_buf].num_unique_outputs;

	// verify store DMA  finished
	ocl_vgf_store_buffer_wait(&buffer[1- curr_vmem_buf]);
	ocl_vgf_store_buffer_wait(&buffer[curr_vmem_buf]);
	
    // inform the calling func about 	total_unique_output
	ctl->total_unique_output = total_unique_output;
	#ifdef OCL_vgf_PRINT_EN_DEBUG
	printf("total_unique_output=%d, num_unique_outputs=%d \n",total_unique_output,buffer[curr_vmem_buf].num_unique_outputs);
	#endif
	//ctl->status =??;
}


