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
#include "../ev_ocl_corr1d_interface.h"

#define  OCL_corr1d_SIMD_WIDTH 32


// local ocl_InuCorr1d_buffer_t buffer[2] - buffer parameters per tile
typedef struct
{
	local short * inputA;						// pointer to the input tile
	local short * inputB;						// pointer to the input tile
	local short * output;						// point to output point cloud after filtering 
	      int     tile_num;						// used for debug

		  int 	  input_width;  				// input tensor width	
		  int 	  input_height;  				// input tensor height	
		  int 	  input_num_maps; 				// input tensor num maps
	      int     tile_row_at_input;            // first row of tile from the input tensor
	      int     num_rows_at_tile;
	      int 	  num_of_shifts;
	      int 	  stride;
	      int     scale;
	      int     scale_bits;

	event_t       load_eventA;
	event_t       load_eventB;
	event_t       store_event;

} ocl_InuCorr1d_buffer_t;
// ***********************************************************************
// ***********************************************************************
void ocl_corr1d_init_buffer(
		local 	ocl_InuCorr1d_buffer_t *	buffer,
		global 	ocl_ctl_corr1d_t* 	 		ctl,
		local 	short * inputA,			
		local 	short * inputB,			
		local 	short * output )        
{
	buffer->inputA = inputA;
	buffer->inputB = inputB;
	buffer->output = output;

    buffer->input_height    = ctl->input_height;
    buffer->input_width     = ctl->input_width;
    buffer->input_num_maps  = ctl->input_num_maps;
    buffer->num_of_shifts   = ctl->num_of_shifts;
    buffer->stride          = ctl->stride;
    buffer->scale           = ctl->scale;
    buffer->scale_bits      = ctl->scale_bits;

	buffer->load_eventA		= -1;
	buffer->load_eventB		= -1;
	buffer->store_event   	= -1;
}
// ***********************************************************************
// ***********************************************************************
void ocl_corr1d_load_buffer(local ocl_InuCorr1d_buffer_t* buffer, global short* inputA, global short* inputB,
                            int tile_row_at_input,  	 	     int num_rows_at_tile,  int tile_num)
{
	const int short_size  = sizeof(short);
	buffer->tile_num   			= tile_num;
	buffer->tile_row_at_input	= tile_row_at_input;
	buffer->num_rows_at_tile    = num_rows_at_tile;

    //  DMA in tile
	int data_size 	    = num_rows_at_tile*buffer->input_width*buffer->input_num_maps;
	int dst_width  		= num_rows_at_tile*buffer->input_width; 
	int dst_stride 		= dst_width; 
	int src_width   	= buffer->input_width * num_rows_at_tile;
	int src_stride      = buffer->input_width * buffer->input_height;
	int src_ptr_offset  = buffer->input_width * tile_row_at_input;

	#ifdef OCL_corr1d_PRINT_EN_DEBUG_
	printf("corr1d_load: tile_num=%d \n",tile_num);
	printf("corr1d_load: dst_width=%d \n",dst_width);
	printf("corr1d_load: tile_row_at_input=%d \n",tile_row_at_input);
    printf("corr1d_load: dstA=%p \n",buffer->inputA);
	printf("corr1d_load: dst_stride=%d \n",dst_stride);
	printf("corr1d_load: data_size=%d \n",data_size);
	
	printf("corr1d_load: src_ptr_offset=%d \n",src_ptr_offset);
	printf("corr1d_load: src_width=%d \n",src_width);
	printf("corr1d_load: srcA=%p \n",inputA+src_ptr_offset);
    printf("corr1d_load: dstB=%p \n",buffer->inputB);
	#endif

    //load inputA	
	buffer->load_eventA =
		async_work_group_copy2D(
				&buffer->inputA[0],							    // dst
				dst_width*short_size,							// dst width
				dst_stride*short_size,				            // dst stride
				&inputA[src_ptr_offset],						// src
				src_width*short_size, 							// src width
				src_stride*short_size,	        				// src stride
				data_size*short_size,	    					// size
				0);


	#ifdef OCL_corr1d_PRINT_EN_DEBUG_
	printf("corr1d_load: srcB=%p \n",inputB+src_ptr_offset);
    printf("corr1d_load: dstB=%p \n",buffer->inputB);
	#endif

				
    //load inputB	
	buffer->load_eventB =
		async_work_group_copy2D(
				&buffer->inputB[0],							    // dst
				dst_width*short_size,							// dst width
				dst_stride*short_size,				            // dst stride
				&inputB[src_ptr_offset],						// src
				src_width*short_size, 							// src width
				src_stride*short_size,	        				// src stride
				data_size*short_size,	    					// size
				0);
}
// ***********************************************************************
void ocl_corr1d_load_buffer_wait(local ocl_InuCorr1d_buffer_t* buffer)
{
	if (buffer->load_eventA != -1){
		wait_group_events(1, &buffer->load_eventA);
		buffer->load_eventA = -1; 
	}
	
	if (buffer->load_eventB != -1){
		wait_group_events(1, &buffer->load_eventB);
		buffer->load_eventB = -1; 
	}
}
// ***********************************************************************
// ***********************************************************************
void ocl_corr1d_store_buffer(local ocl_InuCorr1d_buffer_t * buffer, global short* output )
{
    int   num_out_maps  = (int)(buffer->num_of_shifts/buffer->stride);
	int   data_size     = buffer->num_rows_at_tile*buffer->input_width*num_out_maps;
	int   dst_offset    = buffer->input_width*buffer->tile_row_at_input;
	int   dst_width  	= buffer->num_rows_at_tile*buffer->input_width; 
	int   dst_stride 	= buffer->input_width*buffer->input_height; 

	int   src_width   	= buffer->num_rows_at_tile*buffer->input_width;
	int   src_stride    = src_width;

	#ifdef OCL_corr1d_PRINT_EN_DEBUG_
	printf("corr1d_store: tile_num=%d \n",buffer->tile_num);
	printf("corr1d_store: tile_row_at_input=%d \n",buffer->tile_row_at_input);
	printf("corr1d_store: num_out_maps=%d \n",num_out_maps);
	printf("corr1d_store: src_width=%d \n",src_width);
	printf("corr1d_store: src_stride=%d \n",src_stride);
	printf("corr1d_store: data_size=%d \n",data_size);
    printf("corr1d_store: src=%p \n",buffer->output);
	
	printf("corr1d_store: dst_stride=%d \n",dst_stride);
	printf("corr1d_store: dst_width=%d \n",dst_width);
    printf("corr1d_store: dst =%p \n",&output[dst_offset]);
	#endif


	buffer->store_event =
		async_work_group_copy2D(
			&output[dst_offset],	 	    					// dst
			dst_width*sizeof(short),							// dst width
			dst_stride*sizeof(short),					        // dst stride
			&buffer->output[0],						    		// src
			src_width*sizeof(short), 							// src width
			src_stride*sizeof(short),        					// src stride
			data_size*sizeof(short),							// size
			0);		
	
}
// ***********************************************************************
void ocl_corr1d_store_buffer_wait(local ocl_InuCorr1d_buffer_t* buffer)
{
	if (buffer->store_event != -1){
		wait_group_events(1, &buffer->store_event);
		// Reset buffer storing state
		buffer->store_event = -1;
	}
}
// ***********************************************************************
#define NUM_SHIFTS 9 //8
#define Debug_Tile 0
#define Debug_Line 0
#define Debug_Col  33
// ***********************************************************************
void ocl_corr1d_process_buffer(ocl_InuCorr1d_buffer_t *buffer, 
                               local int * int_idx, 
							   local short* short_idx)
{
	int num_out_maps		= (buffer->num_of_shifts/buffer->stride);
    int remainning_num_col;
	short32 idx  = vload32(0, &short_idx[0]);
	short32 zeros32 = (short32)0;
	short32 valid_mask = idx < (short)32;
    short32 data_inA,data_inB, res;
    acc40x32 	acc_res;
	
	local short * inputA  = (local short *)buffer->inputA;
	local short * inputB  = (local short *)buffer->inputB;
	local short * output  = (local short *)buffer->output;
    
	#ifdef OCL_corr1d_PRINT_EN_DEBUG
	printf("corr1d_process: tile_num=%d \n",buffer->tile_num);
	//printf(" idx[0:31] = %#v32hd\n", idx);	
    printf("corr1d_process: &output[0]=%p \n",&output[0]);
    printf("corr1d_process: num out maps =%d \n",num_out_maps);
	#endif
	
	for (int r=0;r<buffer->num_rows_at_tile;r++)
	{
	    int r_offset = r*buffer->input_width;
		for (int c=0; c<num_out_maps;c++)
		{
		    int c_offset =  c*buffer->input_width*buffer->num_rows_at_tile;
			//vstore32_mask( zeros32,  0, &output[r_offset+c_offset] , valid_mask);
			vstore32( zeros32,  0, &output[r_offset+c_offset]);
		}
	}


   for (int shift_iter=0; shift_iter<num_out_maps; shift_iter++)
	{
		int dst_map_offset = shift_iter*buffer->input_width*buffer->num_rows_at_tile;
		for (int line=0; line<buffer->num_rows_at_tile; line++)
		{
			int src_line_offset = line*buffer->input_width;
			for (int col=shift_iter; col< buffer->input_width; col+=32)
			{   remainning_num_col = buffer->input_width-col;
			    acc_res = acc_init_acc40x32(0);
				for (int ch=0; ch<buffer->input_num_maps; ch++)
				{
					data_inA     = vload32( 0, &inputA[src_line_offset+col + ch*buffer->input_width*buffer->num_rows_at_tile] );
					data_inB     = vload32( 0, &inputB[src_line_offset+col-shift_iter + ch*buffer->input_width*buffer->num_rows_at_tile] );
					#ifdef OCL_corr1d_PRINT_EN_DEBUG_
						if (line==Debug_Line && col==Debug_Col && buffer->tile_num==Debug_Tile)
						{
						printf(" data_inA[0:31] = %#v32hd\n", data_inA);
						printf(" data_inB[0:31] = %#v32hd\n", data_inB);
						}			
					#endif		
					acc_mac(&acc_res, data_inA, data_inB);
				} // end loop for src maps
   
				valid_mask = idx < (short32)(remainning_num_col);
				
			    res.lo = convert_short16_sat(acc_convert_int16(acc_lo(acc_res)) >> NUM_SHIFTS);			
			    res.hi = convert_short16_sat(acc_convert_int16(acc_hi(acc_res)) >> NUM_SHIFTS);
				#ifdef OCL_corr1d_PRINT_EN_DEBUG_
					if (line==Debug_Line && col==Debug_Col && buffer->tile_num==Debug_Tile)
					{
					printf(" res[0:31] = %#v32hd\n", res);
					printf(" res[0:31] = %#v32hx\n", res);
					}
				#endif
				acc_mpy(&acc_res, res, (short32)buffer->scale);
			    res.lo = convert_short16(acc_convert_int16(acc_lo(acc_res)) >> (buffer->scale_bits-NUM_SHIFTS));
			    res.hi = convert_short16(acc_convert_int16(acc_hi(acc_res)) >> (buffer->scale_bits-NUM_SHIFTS));

				vstore32_mask( res,  0, &output[dst_map_offset+ src_line_offset + col] , valid_mask);
				#ifdef OCL_corr1d_PRINT_EN_DEBUG_
					if (line==Debug_Line && col==Debug_Col && buffer->tile_num==Debug_Tile)
					{
					printf(" valid_mask[0:31] = %#v32hd\n", valid_mask);
					//printf(" (short32)buffer->scale) = %#v32hd\n", (short32)buffer->scale);
					//printf(" buffer->scale_bits = %#d\n", buffer->scale_bits);
					printf(" res[0:31] = %#v32hd\n", res);
					}
				#endif
			}    // end loop of segments at row
		} // end loop on rows
	} // end loop of shifts
}
//   ***********************************************************************
// ***********************************************************************
__attribute__(( reqd_work_group_size(1, 1, 1)))

kernel void ev_inu_ocl_corr1d_kernel(	global ocl_ctl_corr1d_t * ctl,
										global short 			* restrict out,
										global short 			* restrict inA ,
										global short 			* restrict inB 	)
{
  	local short *output  [2];
  	local short *inputA  [2];
  	local short *inputB  [2];
  	local ocl_InuCorr1d_buffer_t buffer[2];             // new struct  
 	int  buf;
    int  tile_num=-1;			// used for debug
	int  tile_row_at_input=0;   // first row of tile from input
	int  curr_vmem_buf = 0;

	
  	local short * local_mem_ptr = (local short*)ctl->local_mem_ptr; 	
	int local_mem_size = (ctl->local_mem_size - 4096) >> 1;  //  4096 for stack, double_buffer
	
	int input_row_with_depth_size = ctl->input_num_maps*ctl->input_width;                 // for inputA & B, size of shorts
	int output_row_with_depth_size= (ctl->num_of_shifts/ctl->stride)*ctl->input_width;    // for output , sizeof(short);
    int max_num_rows_at_tile = local_mem_size/((input_row_with_depth_size*2+output_row_with_depth_size)*sizeof(short));
	int num_of_tiles         = (ctl->input_height + max_num_rows_at_tile-1)/max_num_rows_at_tile;

  #ifdef OCL_corr1d_PRINT_EN_DEBUG
	printf("INFO: EVSS_CFG_VCCM_SIZE=%i \n",ctl->local_mem_size);
	printf("INFO: local_mem_ptr=%p \n",local_mem_ptr);
	printf("INFO: local_mem_size=%i \n",local_mem_size);
	printf("INFO: inA at %p\n", inA);
	printf("INFO: inB at %p\n", inB);
	printf("INFO: out at %p\n", out);
	printf("INFO: ctl->input_num_maps  = %d\n", ctl->input_num_maps);
	printf("INFO: ctl->input_width  = %d\n", ctl->input_width);
	printf("INFO: ctl->num_of_shifts  = %d\n", ctl->num_of_shifts);
	printf("INFO: input_row_with_depth_size  = %d\n", input_row_with_depth_size);
	printf("INFO: output_row_with_depth_size = %d\n", output_row_with_depth_size);
	printf("INFO: max_num_rows_at_tile= %d\n",  max_num_rows_at_tile);	
	printf("INFO: num_of_tiles= %d\n",  num_of_tiles);	
  #endif	

 	for ( buf = 0; buf < 2; buf ++)
 	{                                     
 		inputA[buf] = local_mem_ptr;
 		local_mem_ptr += input_row_with_depth_size*max_num_rows_at_tile;
 		inputB[buf] = local_mem_ptr;
 		local_mem_ptr += input_row_with_depth_size*max_num_rows_at_tile;
 		output[buf] =  local_mem_ptr;
 		local_mem_ptr += output_row_with_depth_size*max_num_rows_at_tile;
 	}
  #ifdef OCL_corr1d_PRINT_EN_DEBUG
  	printf("INFO: buffer[0].inputA at %p\n", inputA[0]);
  	printf("INFO: buffer[0].inputB at %p\n", inputB[0]);
  	printf("INFO: buffer[0].output at %p\n", output[0]);
  	printf("INFO: buffer[1].inputA at %p\n", inputA[1]);
  	printf("INFO: buffer[1].inputB at %p\n", inputB[1]);
  	printf("INFO: buffer[1].output at %p\n", output[1]);
  #endif

	ocl_corr1d_init_buffer(&buffer[0], ctl, inputA[0],inputB[0], output[0] );
	ocl_corr1d_init_buffer(&buffer[1], ctl, inputA[1],inputB[1], output[1] );
	

    local int   int_idx[OCL_corr1d_SIMD_WIDTH];  			// OCL_corr1d_SIMD_WIDTH=32
    local short short_idx[OCL_corr1d_SIMD_WIDTH];
    // Init map offsets for gather/scatter
    for (int lane = 0; lane < OCL_corr1d_SIMD_WIDTH; lane++)
    {
        int_idx[lane]   =  (int)lane;
    	short_idx[lane] =  (short)lane;
    }

	int rows_remaining 	= ctl->input_height;    // H;
	
	// load first tile
	int num_rows_at_tile = min(max_num_rows_at_tile, rows_remaining);  		// num rows at tile
    tile_num++;
  	ocl_corr1d_load_buffer(&buffer[curr_vmem_buf], inA,inB,tile_row_at_input, num_rows_at_tile, tile_num); 
	tile_row_at_input = tile_row_at_input + num_rows_at_tile;
	rows_remaining    = max(0, rows_remaining-num_rows_at_tile);

  	// loops over all tiles, each tile is num of rows
	for (int tile_cntr=0; tile_cntr<num_of_tiles; tile_cntr++)	{

	   #ifdef OCL_corr1d_PRINT_EN_DEBUG
	   printf("tile_cntr= %d, corr1d_kernel:rows_remaining=%d \n",tile_cntr,rows_remaining);
	   #endif

	   // load next tile
       if (tile_cntr < (num_of_tiles-1))
	   {
		  num_rows_at_tile = min(max_num_rows_at_tile, rows_remaining);
		  tile_num++;
		  ocl_corr1d_load_buffer(&buffer[1-curr_vmem_buf], inA,inB,tile_row_at_input, num_rows_at_tile, tile_num); 
		  tile_row_at_input = tile_row_at_input + num_rows_at_tile;
		  rows_remaining    = max(0, rows_remaining-num_rows_at_tile);
	   }
	   
	   
	   // verify that buffer loaded - DMA  finished
	   ocl_corr1d_load_buffer_wait(&buffer[curr_vmem_buf]);
	   
	   if (tile_cntr>1)
	        ocl_corr1d_store_buffer_wait(&buffer[curr_vmem_buf]);
	   
	   
	   // --- process curr_vmem_buf buffer ---
	   ocl_corr1d_process_buffer(&buffer[curr_vmem_buf], int_idx , short_idx );

	   // -- initiate DMA to output results & overlapping rows from curr_vmem_buf --
	   ocl_corr1d_store_buffer(&buffer[curr_vmem_buf], out );
	   
	   // swap vmem_buffers
	   curr_vmem_buf = 1-curr_vmem_buf;

	}  // end for loop 

	// --  last tile ---------
	// verify store DMA  finished
	ocl_corr1d_store_buffer_wait(&buffer[1- curr_vmem_buf]);
	ocl_corr1d_store_buffer_wait(&buffer[curr_vmem_buf]);
	
 	//ctl->status =??;
}


