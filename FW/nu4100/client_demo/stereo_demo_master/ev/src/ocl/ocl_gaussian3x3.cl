/**
 * Copyright 2015-2018 Synopsys, Inc.
 * This software contains a version of the Khronos OpenVX sample implementation
 * that has been modified by Synopsys, Inc.  The software and the associated
 * documentation are proprietary to Synopsys, Inc., and may only be used in
 * accordance with the terms and conditions of a written license agreement
 * with Synopsys, Inc.
 *
 * Notwithstanding contrary terms in such license agreement, Licensee may
 * provide the binaries of the EV Runtime and Utilities Option to its
 * end-customer that purchase ICs that incorporate the Synopsys EV processor
 * core, subject to confidentiality terms no less restrictive than those
 * contained in the license agreement.
 * All other use, reproduction, or distribution of this software
 * and associated documentation is strictly prohibited.
 *
 * The original (unmodified) version of OpenVX sample implementation is
 * available at http://www.khronos.org and was licensed to Synopsys under
 * the following terms:
 *
 * Copyright (c) 2011-2016 The Khronos Group Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and/or associated documentation files (the
 * "Materials"), to deal in the Materials without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Materials, and to
 * permit persons to whom the Materials are furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Materials.
 *
 * MODIFICATIONS TO THIS FILE MAY MEAN IT NO LONGER ACCURATELY REFLECTS
 * KHRONOS STANDARDS. THE UNMODIFIED, NORMATIVE VERSIONS OF KHRONOS
 * SPECIFICATIONS AND HEADER INFORMATION ARE LOCATED AT
 *    https://www.khronos.org/registry/
 *
 * THE MATERIALS ARE PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * MATERIALS OR THE USE OR OTHER DEALINGS IN THE MATERIALS.
 */
/*----------------------------------------------------------------------*/
#include "ocl_kernel_requirements.h"
/*----------------------------------------------------------------------*/
//Vector size
#define STEP_X 32

/* Set the OpenCL work group size.
 * In this case, the kernel is manually vectorized, so the work group size is 1 */
__attribute__(( reqd_work_group_size(1, 1, 1)))

/*
 * Frame-based OpenCL kernel. Uses 4 input buffers and 2 output buffers,
 * to make communication with global memory faster by usage of
 * async_work_group_copy for reading, writing and processing simultaneously
 */
kernel void ocl_gaussian3x3( global uchar *src,
                             global uchar *dst,
                             global struct demo_ctl_t *ctl)
{
    //Get kernel parameters from the control structure
    int width = ctl->width;
    int height = ctl->height;
    int offset = ctl->offset;

    //Local line buffers, that will be filled/emptied using async_work_group_copy
    local uchar rdLine[4][MAX_WIDTH];
    local uchar wrLine[2][MAX_WIDTH];

    //Read first three lines of the image into input buffers
    event_t e_in = async_work_group_copy(rdLine[0], &src[0], width, 0);
    e_in = async_work_group_copy(rdLine[1], &src[width], width, e_in);
    e_in = async_work_group_copy(rdLine[2], &src[2*width], width, e_in);
    wait_group_events(1, &e_in);

    //Initialize event for writing from output buffer
    event_t e_out;

    //ID for output ring buffer
    int out_id = 0;

    //Gaussian blur procedure loop
    for (int y = offset; y < height - offset; ++y)
    {
        //IDs for input ring buffer
        int in_id = y % 4;
        int in_prev_id = (y - 1) % 4;
        int in_next_id = (y + 1) % 4;
        int in_next_next_id = (y + 2) % 4;

        /* Start reading new line from input picture into input buffer,
         * which will be used on the next iteration */
        if(y < height - 2*offset)
            e_in = async_work_group_copy(rdLine[in_next_next_id], &src[(y+2)*width], width, 0);

        //Pointers, that navigate through read and write buffers
        local uchar *rd_line_0 = rdLine[in_prev_id];
        local uchar *rd_line_1 = rdLine[in_id];
        local uchar *rd_line_2 = rdLine[in_next_id];

        local uchar *dst_start = wrLine[out_id] + offset;

        /* Process three input buffer's data
         * and store the result into the output buffer*/
        for(int x = offset; x < width - offset; x += STEP_X)
        {
            //Use shift operations instead of * and / to improve performance
            ushort32 vres = convert_ushort32( vload32( 0, rd_line_0 + x - offset ));
            vres +=         convert_ushort32( vload32( 0, rd_line_0 + x     )) << 1;
            vres +=         convert_ushort32( vload32( 0, rd_line_0 + x + offset ));

            vres +=         convert_ushort32( vload32( 0, rd_line_1 + x - offset )) << 1;
            vres +=         convert_ushort32( vload32( 0, rd_line_1 + x     )) << 2;
            vres +=         convert_ushort32( vload32( 0, rd_line_1 + x + offset )) << 1;

            vres +=         convert_ushort32( vload32( 0, rd_line_2 + x - offset ));
            vres +=         convert_ushort32( vload32( 0, rd_line_2 + x     )) << 1;
            vres +=         convert_ushort32( vload32( 0, rd_line_2 + x + offset ));

            vres >>= 4;

            vstore32( convert_uchar32( vres ), 0, dst_start);

            dst_start += STEP_X;
        }

        /* Wait until previous output done writing and will be free to use.
         * Start writing the output, that just got ready */
        if (y > offset) wait_group_events(1, &e_out);
        e_out = async_work_group_copy(&dst[y*width], wrLine[out_id], width - offset, 0);
        out_id = 1 - out_id;

        //Wait until next input line is ready for usage
        if(y < height - 2*offset) wait_group_events(1, &e_in);
    }

    //Wait for the last output to be written
    wait_group_events(1, &e_out);
}
