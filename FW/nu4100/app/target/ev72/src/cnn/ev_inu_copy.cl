/****************************************************************************
 *
 *   FileName: ev_inu_copy.cl
 *
 *   Author:  Arviv B.
 *
 *   Date: 
 *
 *   Description: 
 *   
 ****************************************************************************/

//#include "ev_ocl_ctrl.h"
#include "ev_ocl_copy_interface.h"
#include "stu.h"

// local ocl_copy_buffer_t buffer[2] - buffer parameters per tile
typedef struct
{
	local unsigned char * input;						// pointer to the input tile
	local unsigned char * output;						// point to output point cloud after filtering 
	      int     tile_num;						// used for debug
		  int 	  tile_size;    		        // 
		  int 	  offset;

	event_t       load_event;
	event_t       store_event;

} ocl_copy_buffer_t;
// ***********************************************************************
// ***********************************************************************
void ocl_copy_init_buffer(
		local 	ocl_copy_buffer_t *	buffer,
		global 	ocl_ctl_copy_t    *	ctl,
		local 	unsigned char * input,			
		local 	unsigned char * output )        
{
	buffer->input = input;
	buffer->output = output;

    buffer->tile_size       = ctl->size;

	buffer->load_event 		= -1;
	buffer->store_event   	= -1;
}
// ***********************************************************************
// ***********************************************************************
void ocl_copy_load_buffer(local ocl_copy_buffer_t* buffer,  global unsigned char* g_input,
						int tile_offset,  int num_chars_at_tile ,  int tile_num)
{
	buffer->tile_num   			= tile_num;
	buffer->offset   	        = tile_offset;
	buffer->tile_size           = num_chars_at_tile;


	#ifdef OCL_copy_PRINT_EN_DEBUG_
	printf("load: tile_num=%d \n",tile_num);
	printf("load: input_width=%d \n",input_width);
	printf("load: tile_offset=%d \n",tile_offset);
	printf("load: data_size=%d \n",data_size);
	#endif

    //load input	
	buffer->load_event = async_work_group_copy(&buffer->input[0],				    // dst
											   &g_input[tile_offset],			    // src
											   num_chars_at_tile,	                // size
											   0);	
	
}
// ***********************************************************************
void ocl_copy_load_buffer_wait(local ocl_copy_buffer_t* buffer)
{
	while (evEventIsBusy((ev_ocl_event_t)buffer->load_event));
}
// ***********************************************************************
// ***********************************************************************
void ocl_copy_store_buffer(local ocl_copy_buffer_t * buffer, global unsigned char* g_output )
{
	int   data_size     = buffer->tile_size;
	int   dst_offset    = buffer->offset;


	#ifdef OCL_copy_PRINT_EN_DEBUG_
	printf("store: tile_num=%d \n",buffer->tile_num);
	printf("store: tile_offset=%d \n",dst_offset);
	printf("store: data_size=%d \n",data_size);
	#endif

    //load input	
	buffer->store_event = async_work_group_copy(&g_output[dst_offset],	 	    					// dst
											    &buffer->output[0],						    		// src
											    data_size,	    					            // size
											    0);	
	
}
// ***********************************************************************
void ocl_copy_store_buffer_wait(local ocl_copy_buffer_t* buffer)
{
	while (evEventIsBusy((ev_ocl_event_t)buffer->store_event));
}
// ***********************************************************************
#define Debug_Tile 0
//// ***********************************************************************
// ***********************************************************************
__attribute__(( reqd_work_group_size(1, 1, 1)))

kernel void ev_inu_ocl_copy_kernel(global ocl_ctl_copy_t 	* ctl,
										    global unsigned char    		* restrict g_out,
											global unsigned char 			* restrict g_in 	)
{
  	local unsigned char *output  [3];
  	local unsigned char *input   [3];
  	local ocl_copy_buffer_t buffer[3];             // new struct  
 	int  buf;
    int  tile_num=-1;			// used for debug
	int  tile_offset=0;  		// first char of tile from input

	local unsigned char * local_mem_ptr = (local unsigned char*)ctl->local_mem_ptr; 	
	int   local_mem_size  = (ctl->local_mem_size - 4096);  //  4096 for stack
	
	int io_tile_size =  (local_mem_size/3)&(~0x7f);    	// align to 128, , triple
	int num_of_tiles         = (ctl->size + io_tile_size-1)/io_tile_size;	// round up

  #ifdef OCL_copy_PRINT_EN_DEBUG
	printf("INFO: EVSS_CFG_VCCM_SIZE=%i \n",ctl->local_mem_size);
	printf("INFO: local_mem_ptr=%p \n",local_mem_ptr);
	printf("INFO: local_mem_size=%i \n",local_mem_size);
	printf("INFO: g_in at %p\n", g_in);
	printf("INFO: g_out at %p\n", g_out);
	printf("INFO: ctl->size  = %d\n", ctl->size);
	printf("INFO: io_tile_size  = %d\n", io_tile_size);
	printf("INFO: num_of_tiles= %d\n",  num_of_tiles);	
  #endif	

 	for ( buf = 0; buf < 3; buf ++)
 	{                                     
 		input[buf]  = (local unsigned char  *)local_mem_ptr;
 		output[buf] = (local unsigned char  *)local_mem_ptr;
 		local_mem_ptr += io_tile_size;
 	}
	
  #ifdef OCL_copy_PRINT_EN_DEBUG
  	printf("INFO: buffer[0].input at %p\n", input[0]);
  	printf("INFO: buffer[0].output at %p\n", output[0]);
  	printf("INFO: buffer[1].input at %p\n", input[1]);
  	printf("INFO: buffer[1].output at %p\n", output[1]);
  	printf("INFO: buffer[2].input at %p\n", input[2]);
  	printf("INFO: buffer[2].output at %p\n", output[2]);
  #endif

	ocl_copy_init_buffer(&buffer[0], ctl, input[0], output[0] );
	ocl_copy_init_buffer(&buffer[1], ctl, input[1], output[1] );
	ocl_copy_init_buffer(&buffer[2], ctl, input[2], output[2] );
	
	int chars_remaining 	= ctl->size;   
	
	// load first tile
	int  curr_vmem_buf = 0;
	int  load_wait_vmem_buf;
	int  store_wait_vmem_buf;
	int num_chars_at_tile = min(io_tile_size, chars_remaining);  		// num rows at tile
    tile_num++;
  	ocl_copy_load_buffer(&buffer[curr_vmem_buf], g_in, tile_offset ,num_chars_at_tile , tile_num); 
	tile_offset = tile_offset + num_chars_at_tile;
	chars_remaining     = max(0, chars_remaining-num_chars_at_tile);
	load_wait_vmem_buf  = curr_vmem_buf;
	store_wait_vmem_buf = curr_vmem_buf;
	curr_vmem_buf= 1;

  	// loops over all tiles, each tile is num of rows
	for (int tile_cntr=0; tile_cntr<num_of_tiles; tile_cntr++)	{

	   #ifdef OCL_copy_PRINT_EN_DEBUG_
	   printf("tile_cntr= %d, copy_kernel:chars_remaining=%d \n",tile_cntr,chars_remaining);
	   #endif

	   if (tile_cntr>1)
	   {
	        ocl_copy_store_buffer_wait(&buffer[store_wait_vmem_buf]);
			store_wait_vmem_buf = load_wait_vmem_buf;
	   }
 

	   // load next tile
       if (tile_cntr < (num_of_tiles-1))
	   {
		  num_chars_at_tile = min(io_tile_size, chars_remaining);
		  tile_num++;
		  ocl_copy_load_buffer(&buffer[curr_vmem_buf], g_in,tile_offset, num_chars_at_tile, tile_num); 
		  tile_offset = tile_offset + num_chars_at_tile;
		  chars_remaining    = max(0, chars_remaining-num_chars_at_tile);
	   }
	   
	   // verify that buffer loaded - DMA  finished
	   ocl_copy_load_buffer_wait(&buffer[load_wait_vmem_buf]);
	   
	   // -- initiate DMA to output results & overlapping rows from curr_vmem_buf --
	   ocl_copy_store_buffer(&buffer[load_wait_vmem_buf], g_out );
	   
	   // swap vmem_buffers
	   load_wait_vmem_buf  = curr_vmem_buf;
	   curr_vmem_buf++;
	   if (curr_vmem_buf==3) curr_vmem_buf=0;  //% modulo 3

	}  // end for loop 

	// --  last tile ---------
	// verify store DMA  finished
	ocl_copy_store_buffer_wait(&buffer[store_wait_vmem_buf]);
	store_wait_vmem_buf = load_wait_vmem_buf;
	ocl_copy_store_buffer_wait(&buffer[store_wait_vmem_buf]);
	
}


