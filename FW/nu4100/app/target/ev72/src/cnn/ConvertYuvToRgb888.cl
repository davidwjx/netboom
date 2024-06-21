/****************************************************************************
 *
 *   FileName: ConvertVYUY16ToRgb888.cl
 *
 *   Author:  Noam Porat
 *
 *   Date: June 2020
 *
 *   Description: Converting YUV422 (YUYVYUYV 8bits each Y ,U or V = every 4byted is 2 pixels) to RGB888 (splitted)
 *   
 ****************************************************************************/

/*----------------------------------------------------------------------*/
#include "ConvertYuvToRgb888.h"
/*----------------------------------------------------------------------*/

/* Set the OpenCL work group size.
 * In this case, the kernel is manually vectorized, so the work group size is 1 */
__attribute__(( reqd_work_group_size(1, 1, 1)))

/****************************************************************************
*
*  Function Name: ocl_convertYUV2RGB
*
*  Description: 
*
*  Inputs:
*
*  Outputs:  RGB image splited to 3 seperate outputs (R, G and B)
*
*  Returns: 
*
*  Context: Converting YUV422 (YUYVYUYV 8bits each Y ,U or V = every 4byted is 2 pixels) to RGB888 (splitted)
*			This kernel is used when no scaledown is needed. in this case the kernel also split the output R, G and B into 3 different output pointers
*
****************************************************************************/

kernel void ocl_convertYUV2RGB( global unsigned char * restrict src,
                          global unsigned char  * restrict rgb_buffer,  //This argument is not used in this function
                          global struct convertYUV2RGB_ctl_t * restrict ctl)
{   
 	static int T[3][3] = { { 8192, 0, 11452 },{ 8192, -2803, -5833 },{ 8192, 14440, 0 } };
	global uchar *split_ptr0=NULL, *split_ptr1=NULL, *split_ptr2=NULL;
	global uint *vyuyP; //YUV source image pointer
   uint16 u1, v1, y1, y2, vres;
   int16 temp1, temp2, temp3, r, g, b, r1, g1, b1;
   uchar32 bb, rr, gg;
   uchar32 mask = (uchar32)(0,16,1,17,2,18,3,19,4,20,5,21,6,22,7,23,8,24,9,25,10,26,11,27,12,28,13,29,14,30,15,31);
   //Initialize event for writing from output buffer    
   event_t e_out, e_in;
   int in_id	= 0;
   int out_id	= 0;
   
   int row, num_cycles, num_src_lines[2], total_num_lines, tileSizeInLines;
   
   //Local double buffers, that will be filled/emptied using async_work_group_copy
   local uint		rdLine[2][MAX_WIDTH>>2];
   local uchar	wrLine[2][3][MAX_WIDTH>>1];

   //pointers to input and output buffers
	local uint		*rd_line_0;
   local uchar	*dst_start0;
   local uchar	*dst_start1;
   local uchar	*dst_start2;
   
   //Get kernel parameters from the control structure
   int width					= ctl->processW;//bytes
   int images_width			= ctl->images_width;
   int processH				= ctl->processH;
	int scaler_frame_pixels		= ctl->scaler_frame_pixels>>1;
   
   int offset = (scaler_frame_pixels>>1);//scaler_frame_pixels is the num of pixels to remove from start and end of each row. every int is 2 pixels so we divide by 2
   int widthInDoublePixels	= (width>>2);

	if (ctl->yuv_swap)
   {
		split_ptr0 = (global uchar *)ctl->yuv_split[0];
		split_ptr1 = (global uchar *)ctl->yuv_split[1];
		split_ptr2 = (global uchar *)ctl->yuv_split[2];
	}
	else
   {
		split_ptr0 = (global uchar *)ctl->yuv_split[2];
		split_ptr1 = (global uchar *)ctl->yuv_split[1];
		split_ptr2 = (global uchar *)ctl->yuv_split[0];
	}
  
   
//	   unsigned int *ptr = (unsigned int*)0x01001000;   
//	   *ptr++ = 0xdeadbeaf;   

//	   long long start_dma, end_dma; 
   total_num_lines = processH;
	//Calculate the number of lines in a tile
   tileSizeInLines = MAX_WIDTH/width;
	//load first tile from DDR
   num_src_lines[in_id] = min ( tileSizeInLines, total_num_lines );
   e_in = async_work_group_copy(rdLine[in_id], (global uint *)(src), num_src_lines[in_id]*widthInDoublePixels, 0);
   total_num_lines -= num_src_lines[in_id];
	//update sourse pointer
   vyuyP = (global uint *)(src + (num_src_lines[in_id] * width));
	
	//Calculate the number of iterations in main loop
   num_cycles = (processH + (tileSizeInLines - 1)) / tileSizeInLines;
   
	for (int i = 0; i < num_cycles; i++)
	{
//	      start_dma = getRTC();
      wait_group_events(1, &e_in);
//	      end_dma = getRTC();  
//	      *ptr++ = (end_dma - start_dma) | 0x20000000;   
      
      //get next tile from DDR (while processing current tile)
      num_src_lines[1-in_id] = min ( tileSizeInLines, total_num_lines );
      if (num_src_lines[1-in_id] > 0)
      {
			//if there are more lines to transfer
         e_in = async_work_group_copy(rdLine[1-in_id], vyuyP, num_src_lines[1-in_id]*widthInDoublePixels, 0);
         total_num_lines -= num_src_lines[1-in_id];
         vyuyP += (num_src_lines[1-in_id]*widthInDoublePixels);
      }
      //process tile
   	for (row = 0; row < num_src_lines[in_id]; row++)
      {
         rd_line_0   = &rdLine[in_id][row*widthInDoublePixels];
         dst_start0	= &wrLine[out_id][0][row*images_width];
         dst_start1	= &wrLine[out_id][1][row*images_width];
         dst_start2	= &wrLine[out_id][2][row*images_width];

         for(int x = offset; x < (widthInDoublePixels - offset) ; x += STEP_X)
         {
            //load 16 int values from input buffer
            vres  = vload16( 0, rd_line_0 + x );
				//parser Y U and V (each int is YUYV)

			y1 = vres & 0xff;
			y1 = y1;
			u1 = (vres>>8) & 0xff;
			u1 = u1 - 128;
			y2 = (vres>>16) & 0xff;
			y2 = y2;
			v1 = (vres>>24) & 0xff;
			v1 = v1 - 128;
            

            temp1 =  (convert_int16)(T[0][1] * u1 + T[0][2] * v1);
            temp2 =  (convert_int16)(T[1][1] * u1 + T[1][2] * v1);
            temp3 =  (convert_int16)(T[2][1] * u1 + T[2][2] * v1);

            r	= ((convert_int16)(T[0][0] * y1) + temp1)>>13;
            g	= ((convert_int16)(T[1][0] * y1) + temp2)>>13;
            b 	= ((convert_int16)(T[2][0] * y1) + temp3)>>13;

            r 	= clamp(r, 0, 255);
            g 	= clamp(g, 0, 255);
            b 	= clamp(b, 0, 255);

            r1 	= ((convert_int16)(T[0][0] * y2) + temp1)>>13;
            g1 	= ((convert_int16)(T[1][0] * y2) + temp2)>>13;
            b1 	= ((convert_int16)(T[2][0] * y2) + temp3)>>13;

            r1	= clamp(r1, 0, 255);
            g1	= clamp(g1, 0, 255);
            b1	= clamp(b1, 0, 255);
            //merge pixels
            bb = shuffle2(convert_uchar16(b) ,convert_uchar16(b1), mask);
            vstore32( bb, 0, dst_start0);
            gg = shuffle2(convert_uchar16(g) ,convert_uchar16(g1), mask);
            vstore32( gg, 0, dst_start1);
            rr = shuffle2(convert_uchar16(r) ,convert_uchar16(r1), mask);
            vstore32( rr, 0, dst_start2);
            //update local dst pointers
            dst_start0 += 32;
            dst_start1 += 32;
            dst_start2 += 32;
         }
   	}
      
      // Wait until previous output done writing and will be free to use.        
      if (i > 0) wait_group_events(1, &e_out);
      // Start writing the output, that just got ready        
      e_out = async_work_group_copy(&split_ptr0[i*tileSizeInLines*images_width], wrLine[out_id][0], num_src_lines[in_id]*images_width, 0);
      e_out = async_work_group_copy(&split_ptr1[i*tileSizeInLines*images_width], wrLine[out_id][1], num_src_lines[in_id]*images_width, e_out);
      e_out = async_work_group_copy(&split_ptr2[i*tileSizeInLines*images_width], wrLine[out_id][2], num_src_lines[in_id]*images_width, e_out);
      
      //Update pointers
      in_id	= 1 - in_id;
      out_id	= 1 - out_id;
	}
   //Wait for the last output to be written    
   wait_group_events(1, &e_out);
}


/****************************************************************************
*
*  Function Name: ocl_convertYUV2RGB_NoSplit
*
*  Description: 
*
*  Inputs:
*
*  Outputs: RGB image in a 888format
*
*  Returns: 
*
*  Context: Converting YUV422 (YUYVYUYV 8bits each Y ,U or V = every 4byted is 2 pixels) to RGB888 (splitted)
*			This kernel is used when a scaledown is needed and will take place after this conversion. 
*
****************************************************************************/

kernel void ocl_convertYUV2RGB_NoSplit( global unsigned char * restrict src,
                                global unsigned char  * restrict rgb_buffer,  
                                global struct convertYUV2RGB_ctl_t * restrict ctl)
{   
	static int T[3][3] = { { 8192, 0, 11452 },{ 8192, -2803, -5833 },{ 8192, 14440, 0 } };
	global uint *vyuyP; //YUV source image pointer
   uint16 u1, v1, y1, y2, vres;
   int16 temp1, temp2, temp3, r, g, b, r1, g1, b1;
	int16 voffset_R_out_0, voffset_R_out_0_2, voffset_G_out_0, voffset_B_out_0, voffset_G_out_0_2, voffset_B_out_0_2;
   uchar32 bb, rr, gg;
   uchar32 mask = (uchar32)(0,16,1,17,2,18,3,19,4,20,5,21,6,22,7,23,8,24,9,25,10,26,11,27,12,28,13,29,14,30,15,31);
   //Initialize events DMAs transfers    
   event_t e_out, e_in;
   int in_id	= 0;
   int out_id	= 0;
   int row, num_cycles, num_src_lines[2], total_num_lines, tileSizeInLines;

   //Local double buffers, that will be filled/emptied using async_work_group_copy
   local uint		rdLine[2][MAX_WIDTH>>2];
   local uchar	wrLine[2][(MAX_WIDTH>>1)*3 + 32*3];

   //pointer to output buffer
   local uchar *dst_start0;
   
   //Get kernel parameters from the control structure
   int width          	= ctl->processW; //Bytes
   int processH        	= ctl->processH;
   int scaler_frame_pixels	= ctl->scaler_frame_pixels>>1;
   
   int offset = (scaler_frame_pixels>>1);//scaler_frame_pixels is the num of pixels to remove from start and end of each row. every int is 2 pixels so we divide by 2
   int widthInDoublePixels	= (width>>2);

   
//	   unsigned int *ptr = (unsigned int*)0x01001000;   
//	   *ptr++ = 0xdeadbeaf;   
   total_num_lines = processH;
	//Calculate the number of lines in a tile
   tileSizeInLines = MAX_WIDTH/width;
   //RGB offsets for scattering output
   for(int k = 0; k < 16; k++)
   {
      voffset_R_out_0[k] = k*3;
   }
	voffset_G_out_0		= voffset_R_out_0 + 1;
	voffset_B_out_0 		= voffset_G_out_0 + 1;
   voffset_R_out_0_2 	= voffset_R_out_0 + 48;
   voffset_G_out_0_2 	= voffset_G_out_0 + 48;
   voffset_B_out_0_2 	= voffset_B_out_0 + 48;

//	   long long start_dma, end_dma; 
	
	//load first tile from DDR
   num_src_lines[in_id] = min ( tileSizeInLines, total_num_lines );
   e_in = async_work_group_copy(rdLine[in_id], (global uint *)(src), num_src_lines[in_id]*widthInDoublePixels, 0);
   total_num_lines -= num_src_lines[in_id];
	//update sourse pointer
   vyuyP = (global uint *)(src + (num_src_lines[in_id] * width));
	
	//Calculate the number of iterations in main loop
   num_cycles = (processH + (tileSizeInLines - 1)) / tileSizeInLines;
   
	for (int i = 0; i < num_cycles; i++)
	{
//	      start_dma = getRTC();
      wait_group_events(1, &e_in);
//	      end_dma = getRTC();  
//	      *ptr++ = (end_dma - start_dma) | 0x20000000;   
      
      //get next tile from DDR (while processing current tile)
      num_src_lines[1-in_id] = min ( tileSizeInLines, total_num_lines );
      if (num_src_lines[1-in_id] > 0)
      {
         e_in = async_work_group_copy(rdLine[1-in_id], vyuyP, num_src_lines[1-in_id]*widthInDoublePixels, 0);
         total_num_lines -= num_src_lines[1-in_id];
         vyuyP += (num_src_lines[1-in_id]*(width>>2));
      }
      //process tile
   	for (row = 0; row < num_src_lines[in_id]; row++)
      {
         local uint *rd_line_0 = &rdLine[in_id][row*widthInDoublePixels];
         dst_start0 = &wrLine[out_id][row*(width>>1)*3];

         for(int x = offset; x < (widthInDoublePixels - offset) ; x += STEP_X)
         {
            //Use shift operations instead of * and / to improve performance
            vres  = vload16( 0, rd_line_0 + x );

            y1 = vres & 0xff;
            y1 = y1;
            u1 = (vres>>8) & 0xff;
            u1 = u1 - 128;
            y2 = (vres>>16) & 0xff;
            y2 = y2;
            v1 = (vres>>24) & 0xff;
            v1 = v1 - 128;

            temp1 =  (convert_int16)(T[0][1] * u1 + T[0][2] * v1);
            temp2 =  (convert_int16)(T[1][1] * u1 + T[1][2] * v1);
            temp3 =  (convert_int16)(T[2][1] * u1 + T[2][2] * v1);

            r 	= ((convert_int16)(T[0][0] * y1) + temp1)>>13;
            g 	= ((convert_int16)(T[1][0] * y1) + temp2)>>13;
            b 	= ((convert_int16)(T[2][0] * y1) + temp3)>>13;

            r 	= clamp(r, 0, 255);
            g 	= clamp(g, 0, 255);
            b 	= clamp(b, 0, 255);

            r1 	= ((convert_int16)(T[0][0] * y2) + temp1)>>13;
            g1 	= ((convert_int16)(T[1][0] * y2) + temp2)>>13;
            b1 	= ((convert_int16)(T[2][0] * y2) + temp3)>>13;

            r1	= clamp(r1, 0, 255);
            g1	= clamp(g1, 0, 255);
            b1	= clamp(b1, 0, 255);
            //merge pixels
            bb = shuffle2(convert_uchar16(b) ,convert_uchar16(b1), mask);
            gg = shuffle2(convert_uchar16(g) ,convert_uchar16(g1), mask);
            rr = shuffle2(convert_uchar16(r) ,convert_uchar16(r1), mask);

            vscatter32(rr, dst_start0, voffset_R_out_0,voffset_R_out_0_2);
            vscatter32(gg, dst_start0, voffset_G_out_0,voffset_G_out_0_2);
            vscatter32(bb, dst_start0, voffset_B_out_0,voffset_B_out_0_2);

            //update local dst pointer
            dst_start0 += 32*3;
         }
   	}
      // Wait until previous output done writing and will be free to use.        
      if (i > 0) wait_group_events(1, &e_out);
      // Start writing the output, that just got ready        
      e_out = async_work_group_copy(&rgb_buffer[i*tileSizeInLines*(width>>1)*3], wrLine[out_id], num_src_lines[in_id]*(width>>1)*3, 0);
      //Update pointers
      in_id	= 1 - in_id;
      out_id 	= 1 - out_id;
	}
   //Wait for the last output to be written    
   wait_group_events(1, &e_out);
}



