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


#define  OCL_pcl_SIMD_WIDTH 32
#define get_timer0() _lr(0x21)

// local ocl_InuPcl_buffer_t buffer[2] - buffer parameters per tile
typedef struct
{
	local short * input;						// pointer to the input tile
	local short * output;						// point to output point cloud after filtering 
	      short   tile_num;						// used for debug
	      short   depth_height;
	      short   depth_width;
	      short   tile_num_rows;				// num lines/rows at current tile
	      short   row_offset;					// the line offset from the image
	      short   num_outputs;					// number of point cloud point at the tile (differ than zero) 

	event_t       load_event_depth;
	event_t       store_event_results;

} ocl_InuPcl_buffer_t;

// ***********************************************************************
void ocl_pcl_init_buffer(
		local 	ocl_InuPcl_buffer_t*	buffer,
		global 	ocl_ctl_vgf_t* 	 		ctl,
		local 	short * input,			// input buffer at the vmem (double buffer)
		local 	short * output )        // output buffer at the vmem (double buffer)
{
	buffer->input = input;
#ifdef OCL_Pcl_PRINT_EN_DEBUG
	printf("BUFFER at address %p: INIT input address %p\n", buffer, buffer->input);
#endif

	buffer->output = output;
#ifdef OCL_Pcl_PRINT_EN_DEBUG
	printf("BUFFER at address %p: INIT output address %p\n", buffer, buffer->output);
#endif

    buffer->depth_height    = ctl->input_height;
    buffer->depth_width     = ctl->input_width;

	buffer->load_event_depth   		= -1;
	buffer->store_event_results   	= -1;
}
// ***********************************************************************
void ocl_pcl_load_buffer(local ocl_InuPcl_buffer_t* buffer, global short* input, 
                         int rows_at_tile,  		        int row_offset,     int tile_num)
{
	const int short_size  		= sizeof(short);

	buffer->tile_num_rows 		= (short)rows_at_tile;
	buffer->row_offset    		= (short)row_offset;
	buffer->tile_num   			= (short)tile_num;
	buffer->num_outputs			=  0;

    //  DMA in depth tile
	int dst_width  		= buffer->depth_width; 
	int src_width   	= buffer->depth_width;
	int src_ptr_offset  = row_offset*buffer->depth_width;
	int ch_data_size 	= rows_at_tile*buffer->depth_width;

	#ifdef OCL_Pcl_PRINT_EN_DEBUG
	printf("vgf_load: dst_width=%d \n",dst_width);
	printf("vgf_load: src_width=%d \n",src_width);
	printf("vgf_load: src_ptr_offset=%d \n",src_ptr_offset);
	printf("vgf_load: src=%p \n",input+src_ptr_offset);
	printf("vgf_load: ch_data_size=%d \n",ch_data_size);
	#endif

	
	buffer->load_event_depth =
		async_work_group_copy2D(
				&buffer->input[0],				    			// dst
				dst_width*short_size,							// dst width
				dst_width*short_size,							// dst stride
				&input[src_ptr_offset],							// src
				src_width*short_size, 							// src width
				src_width*short_size,							// src stride
				ch_data_size*short_size,						// size
				0);
}
// ***********************************************************************
void ocl_pcl_load_buffer_wait(local ocl_InuPcl_buffer_t* buffer)
{
	//if (buffer->load_event_depth != -1){
		wait_group_events(1, &buffer->load_event_depth);
	//	buffer->load_event_depth = -1; 
	//}
}
// ***********************************************************************
void ocl_pcl_store_buffer(local ocl_InuPcl_buffer_t * buffer, global short* output,
                          int total_unique_output)
{
	// store the unique point cloud result from the tile
	if (buffer->num_outputs > 0){
		// start output results DMA
		buffer->store_event_results =
				async_work_group_copy(
						output + 3*total_unique_output, 	// dst
						buffer->output, 					//src
						buffer->num_outputs*3*sizeof(short),		// for x ,y and z
						0);
	}
}
// ***********************************************************************
void ocl_pcl_store_buffer_wait(local ocl_InuPcl_buffer_t* buffer)
{
	if (buffer->store_event_results != -1){
		wait_group_events(1, &buffer->store_event_results);
		// Reset buffer storing state
		buffer->store_event_results = -1;
	}
}
// ***********************************************************************
void ocl_pcl_process_buffer(ocl_InuPcl_buffer_t *buffer, 
                            local int * int_idx, 
							local short* short_idx,
							ocl_ctl_vgf_general_params_t *vgf_gen_params)
{
	local short * input_depth = (local short *)buffer->input;
	local short * output_pcl  = (local short *)buffer->output;

	int d_num_cols			= buffer->depth_width;
	int d_num_rows    		= buffer->tile_num_rows;

	int src_depth_row_offset , src_depth_offset;
	int remainning_num_col , full_iter_index;

	short32 idx  	  = vload32(0, &short_idx[0]);

		
	//short32 valid_depth_idx;
	short32 X ,Y ,Z , tmp_cmp;
	short32 cc_x,cc_y;
	short32 inv_fc_x_Q23,inv_fc_y_Q23;
	short32 pixel_row_index,pixel_col_index;
	int num_outputs  = 0;

	acc40x32 	acc_res;
	
	short32 maxDepth = (short32)vgf_gen_params->MaxDepthMm;
    cc_x = (short32)vgf_gen_params->cc_x;
    cc_y = (short32)vgf_gen_params->cc_y;
    inv_fc_x_Q23 = (short32)vgf_gen_params->inv_fc_x_Q23;
    inv_fc_y_Q23 = (short32)vgf_gen_params->inv_fc_y_Q23;


	for (int src_row=0; src_row<d_num_rows; src_row++){

		src_depth_row_offset 	= src_row*d_num_cols;
		remainning_num_col 		= d_num_cols;
				
		full_iter_index    = 0;
		while (remainning_num_col > 0 ){

			//valid_depth_idx = idx < (short)remainning_num_col;
		    src_depth_offset 	= src_depth_row_offset    + full_iter_index*OCL_pcl_SIMD_WIDTH;  // offset of short		
		
			Z         = vload32( 0, &input_depth[src_depth_offset] );
			Z         = min(Z,maxDepth);
			tmp_cmp   = -(Z >  (short32)0);  			// indicate if Z>0
			pixel_col_index = (short32)(full_iter_index*OCL_pcl_SIMD_WIDTH) + idx;
			
			X = pixel_col_index-cc_x;
			acc_mpy(&acc_res, X, inv_fc_x_Q23); //acc_res = (pixel_col_index-cc_x)*inv_fc_x_Q23

			X.lo =  convert_short16(acc_convert_int16(acc_lo(acc_res)) >> 10);
			X.hi =  convert_short16(acc_convert_int16(acc_hi(acc_res)) >> 10);			

            acc_mpy(&acc_res, X, Z);
			// Right shift 13 (the acc_res by 2^13), remain shifts = 0
			X.lo =  convert_short16(acc_convert_int16(acc_lo(acc_res)) >> 13);
			X.hi =  convert_short16(acc_convert_int16(acc_hi(acc_res)) >> 13);
			
            // calc point cloud axis y
			pixel_row_index = buffer->row_offset + src_row;
			Y = pixel_row_index-cc_y;
			acc_mpy(&acc_res, Y, inv_fc_y_Q23);  //acc_res = (pixel_row_index-cc_y)*inv_fc_y_Q23

			// Right shift 10 (the acc_res by 2^10), remain shifts = 13
			Y.lo =  convert_short16(acc_convert_int16(acc_lo(acc_res)) >> 10);
			Y.hi =  convert_short16(acc_convert_int16(acc_hi(acc_res)) >> 10);

            acc_mpy(&acc_res, Y, Z);
			
			
			// Right shift 13 (the acc_res by 2^13), remain shifts = 0
			Y.lo =  convert_short16(acc_convert_int16(acc_lo(acc_res)) >> 13);
			Y.hi =  convert_short16(acc_convert_int16(acc_hi(acc_res)) >> 13);


			#ifdef OCL_Pcl_PRINT_EN_DEBUG_d
			printf(" (Z >  (short32)0) = %#v32hi\n", tmp_cmp);
			printf(" Z[0:31] = %#v32hd\n", Z);
			printf(" X[0:31] = %#v32hd\n", X);
			printf(" Y[0:31] = %#v32hd\n", Y);
			//printf(" valid_depth_idx[0:15] = %#v32hd\n", valid_depth_idx);
			#endif	
			
		#if 0  // without zeros filtering 
			int16 io_offsets0 = ((int16)3)*vload16(0, &int_idx[0 ]);
			int16 io_offsets1 = ((int16)3)*vload16(0, &int_idx[16]);		
			//int16 io_offsets0 = (int16)(0 , 3, 6, 9,12,15,18,21,24,27,30,33,36,39,42,45);
    	    //int16 io_offsets1 = (int16)(48,51,54,57,60,63,66,69,72,75,78,81,84,87,90,93);
						
			vscatter32(X, &output_pcl[3*num_outputs + 0]  , io_offsets0,io_offsets1);	// , valid_depth_idx);
			vscatter32(Y, &output_pcl[3*num_outputs + 1]  , io_offsets0,io_offsets1);	// , valid_depth_idx);
			vscatter32(Z, &output_pcl[3*num_outputs + 2]  , io_offsets0,io_offsets1); 	//, valid_depth_idx);
			num_outputs = num_outputs+OCL_pcl_SIMD_WIDTH;
			
			// dummy delay for the process that the output will be correct
			short tmp=0;
			for(int k=0;k<3000;k++)
			{
				tmp=tmp+1;
			}
			output_pcl[3*num_outputs]=tmp;

        #else	// with zeros filtering 
		    short32 predicate_partial_sum = scan_excl_add(tmp_cmp);
            int16 pred__partial_sum_h = ((int16)3)*convert_int16(predicate_partial_sum.hi);
            int16 pred__partial_sum_l = ((int16)3)*convert_int16(predicate_partial_sum.lo);	
			
			vscatter32(X, &output_pcl[3*num_outputs + 0]  , pred__partial_sum_l,pred__partial_sum_h);	// , valid_depth_idx);
			vscatter32(Y, &output_pcl[3*num_outputs + 1]  , pred__partial_sum_l,pred__partial_sum_h);	// , valid_depth_idx);
			vscatter32(Z, &output_pcl[3*num_outputs + 2]  , pred__partial_sum_l,pred__partial_sum_h); 	//, valid_depth_idx);
			
			//num_outputs = num_outputs + reduc_add(tmp_cmp);
			num_outputs = num_outputs+predicate_partial_sum[31];
			if (tmp_cmp[31] != 0)
				num_outputs++;
	   #endif
			
			full_iter_index++;
			remainning_num_col = max(0, remainning_num_col-OCL_pcl_SIMD_WIDTH);
						
			} // while (remainning_num_col > 0 )
	} // for (int src_row=0; src_row<d_num_rows; src_row++)
		
	buffer->num_outputs = (short)num_outputs;
	#ifdef OCL_Pcl_PRINT_EN_DEBUG_d
	printf(" num_outputs  = %d\n", num_outputs);
	#endif
}
//   ***********************************************************************
__attribute__(( reqd_work_group_size(1, 1, 1)))

kernel void ev_inu_ocl_pcl_kernel(	global ocl_ctl_vgf_t    * ctl,
                    				global short 			* restrict out,
									global short 			* restrict in 
									)
{
  	local short *output [2];
  	local short *input  [2];
  	local ocl_InuPcl_buffer_t buffer[2];             // new struct  
  	local short * local_mem_ptr = (local short*)ctl->local_mem_ptr;
  	int local_mem_size = (ctl->local_mem_size - 4096) >> 1;  	// can be used to dynamic setting the tile_size_x
																// the >>1 for double buffer
	int max_lines_tile = (local_mem_size-OCL_pcl_SIMD_WIDTH*3*sizeof(short)) / ((ctl->input_width)*sizeof(short)*(1+3));  // 1 for input, 3 for output
	
	ocl_ctl_vgf_general_params_t vgf_gen_params= ctl->vgf_gen_params_ptr;
  	int buf;

  	int rows_at_tile,cols_at_tile,input_tile_size,output_tile_size;
  	int total_output	= 0;
  	
  	rows_at_tile = min(max_lines_tile, ctl->input_height);  	// num rows at tile
  	cols_at_tile = ctl->input_width;  							// num cols at tile
  	input_tile_size  = rows_at_tile * cols_at_tile*1;		    // the input is shorts
  	output_tile_size = rows_at_tile * cols_at_tile*3+OCL_pcl_SIMD_WIDTH*3;           // each output is 3 shorts
 
  	for ( buf = 0; buf < 2; buf ++)
  	{                                     
  		input[buf] = local_mem_ptr;
  		local_mem_ptr += input_tile_size;
  		output[buf] =  local_mem_ptr;
  		local_mem_ptr += output_tile_size;
  	}
  #ifdef OCL_Pcl_PRINT_EN_DEBUG
  	printf("INFO: EVSS_CFG_VCCM_SIZE=%i \n",ctl->local_mem_size);
  	printf("INFO: local_mem_size=%i \n",local_mem_size);
  	printf("INFO: in  at %p\n", in);
  	printf("INFO: out at %p\n", out);
  	printf("INFO: buffer[0].input at %p\n",  input[0]);
  	printf("INFO: buffer[0].output at %p\n", output[0]);
  	printf("INFO: buffer[1].input at %p\n",  input[1]);
  	printf("INFO: buffer[1].output at %p\n", output[1]);
  #endif
  
  	ocl_pcl_init_buffer(&buffer[0], ctl, input[0], output[0] );
  	ocl_pcl_init_buffer(&buffer[1], ctl, input[1], output[1] );
  		
  	local int   int_idx[OCL_pcl_SIMD_WIDTH];  			// OCL_pcl_SIMD_WIDTH=32
  	local short short_idx[OCL_pcl_SIMD_WIDTH];
  	
  	int  curr_vmem_buf 		= 0;
  	int  row_offset 		= 0;
  	int  rows_remaining 	= ctl->input_height;    // H;
	int  tile_num			=-1;					// used for debug
	
  	// Init map offsets for gather/scatter
  	for (int lane = 0; lane < OCL_pcl_SIMD_WIDTH; lane++)
  	{
  	    int_idx[lane]   =  (int)lane;
  		short_idx[lane] =  (short)lane;
  	}
  
    // start load the first tile 
    tile_num++;
  	ocl_pcl_load_buffer(&buffer[curr_vmem_buf], in, rows_at_tile,row_offset,tile_num); 
  	rows_remaining        = max(0,rows_remaining-rows_at_tile);
	row_offset			  = row_offset + rows_at_tile;
	  	
  	// loops over all rows at the image 
	//-------------------------------------
	while (rows_remaining>0)	{
	#ifdef OCL_Pcl_PRINT_EN_DEBUG
	printf("vgf_kernel:rows_remaining=%d \n",rows_remaining);
	#endif
		
		
		// start DMA for the following  tile
		
		void evWaitAllStuEvents();
		evWaitAllStuEvents();
		
        tile_num++;
		rows_at_tile = min(max_lines_tile, rows_remaining);
		ocl_pcl_load_buffer(&buffer[1-curr_vmem_buf], in, rows_at_tile,row_offset,tile_num);
		rows_remaining        = max(0,rows_remaining-rows_at_tile);
		row_offset			  = row_offset + rows_at_tile;
		
		// verify that vmem_input_buff[curr_vmem_buf] buffer loaded - DMA  finished
		//ocl_pcl_load_buffer_wait(&buffer[curr_vmem_buf]);
		//ocl_pcl_store_buffer_wait(&buffer[curr_vmem_buf]);
	
		// --- process curr_vmem_buf buffer ---
		ocl_pcl_process_buffer(&buffer[curr_vmem_buf], int_idx , short_idx , &vgf_gen_params);

		// -- initiate DMA to output results & overlapping rows from curr_vmem_buf --
		ocl_pcl_store_buffer(&buffer[curr_vmem_buf], out , total_output);
		total_output = total_output + buffer[curr_vmem_buf].num_outputs;
		#ifdef OCL_Pcl_PRINT_EN_DEBUG
		printf("total_output=%d, num_outputs=%d \n",total_output,buffer[curr_vmem_buf].num_outputs);
		#endif
		    
	    // swap vmem_buffers
		curr_vmem_buf = 1-curr_vmem_buf;
		
	}		// end of while (rows_remaining>0)
	
	// --  last tile ---------

	//ocl_pcl_load_buffer_wait(&buffer[curr_vmem_buf]);
	//ocl_pcl_store_buffer_wait(&buffer[curr_vmem_buf]);
	
	evWaitAllStuEvents();
	ocl_pcl_process_buffer(&buffer[curr_vmem_buf], int_idx , short_idx , &vgf_gen_params);
			
	ocl_pcl_store_buffer(&buffer[curr_vmem_buf], out , total_output);
	total_output = total_output + buffer[curr_vmem_buf].num_outputs;

	// verify store DMA  finished
	ocl_pcl_store_buffer_wait(&buffer[1- curr_vmem_buf]);
	ocl_pcl_store_buffer_wait(&buffer[curr_vmem_buf]);
	
	//evWaitAllStuEvents();
	
    // inform the calling func about 	total_output
	ctl->total_unique_output = total_output;
	#ifdef OCL_Pcl_PRINT_EN_DEBUG
	printf("total_output=%d, num_outputs=%d \n",total_output,buffer[curr_vmem_buf].num_outputs);
	#endif
}


