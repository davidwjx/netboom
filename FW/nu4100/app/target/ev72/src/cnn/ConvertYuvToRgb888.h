/****************************************************************************
 *
 *   FileName: ConvertVYUY16ToRgb888.h
 *
 *   Author:  Noam Porat
 *
 *   Date: June 2020
 *
 *   Description: 
 *   
 ****************************************************************************/


#ifndef ConvertVYUY16ToRgb888_H_
#define ConvertVYUY16ToRgb888_H_

#define getRTC evGetTimeInCycles
unsigned long long  getRTC();

void	evDataCacheInvalidateLines(void *start, void *end);

//	kernel void my_evDataCacheInvalidateLines() {
//	   unsigned long X = GetRTC();
//	}


//Vector size
#define STEP_X 16 //16 int = 64B

//Maximum possible image width allowed by OCL user kernel
#define MAX_WIDTH 25600 //1280x2x10 lines

/* Control information for the OCL kernel.
* In general it is a good idea to pass pointers
* to the kernel control structures, rather than passing them as arguments.
* This can save a few cycles when calling the openCL kernel */
struct convertYUV2RGB_ctl_t
{
   int processW;
   int processH;
   int scaler_frame_pixels;
   int images_width;
   bool yuv_swap; // swap the channels
   unsigned char **yuv_split;
};

#endif
