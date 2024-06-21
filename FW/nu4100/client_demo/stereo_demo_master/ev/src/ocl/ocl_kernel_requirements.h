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

#ifndef OCL_KERNEL_REQUIREMENTS_H_
#define OCL_KERNEL_REQUIREMENTS_H_

    //Maximum possible image width allowed by OCL user kernel
    #define MAX_WIDTH 2048

    /* Control information for the OCL kernel.
     * In general it is a good idea to pass pointers
     * to the kernel control structures, rather than passing them as arguments.
     * This can save a few cycles when calling the openCL kernel */
    struct demo_ctl_t
    {
        int width;
        int height;
        int offset;
    };


#endif
