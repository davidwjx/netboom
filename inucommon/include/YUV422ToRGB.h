#pragma once

#include "CommonAlgo.h"

namespace InuCommon
{
    ////////////////////////////////////////////////////////////////////////
    /// \brief YUV422 to RGB converter
    ///
    /// Class:
    ///
    /// Role: Cnvert input buffer, YUV422 encoded to RGB (or BGR)
    ///
    /// Inherits:
    ///
    /// Responsibilities:
    ///      1. Translate input image in YUV422 format to RGB.
    ///
    /// Comments:
    ///      1. Selects algorithm according to iOptimizationLevel
    ///
    ////////////////////////////////////////////////////////////////////////

    /// \brief Extended YUV422 to RGB converter
    ///
    /// \param[out] oResultBuffer   Pointer to output data location.
    ///                             Buffer should be big enaugh to hold 4 * mWith * mHeight data
    ///                             (output format is RGBA or BGRA, each component is 8 bits in size)
    /// \param[in] iYImage          YUV422 image to convert
    /// \param iNotInUse            Unused
    /// \param[in] mHeight          Heigh, in pixels
    /// \param[in] mWidth           Width, in pixels
    /// \param[in] stride           Input image stride. Distance between input image lines, in bytes
    /// \param[in] iOptimizationLevel   Desired optimization level.
    /// \param[in] swapOutput       Selects either RGB or BGR output component order
    /// \param[in] iSwapInput       If true then in the input data each pixel is compound of V,Y,U,V otherwise it's Y,V,Y,U
    /// \return bool                true for success, false if some error
    COMMONALGO_API bool YUV422ToRGB_AlgoExt(
        unsigned char* oFirstBuffer,
        unsigned char* oSecondBuffer,

        unsigned char *iYImage,

        unsigned int mHeight,
        unsigned int mWidth,
        unsigned int stride,

        int iOptimizationLevel,

        bool swapOutput,

        bool iSwapInput,
        bool i16bpp,    // if false then 24 bpp

        bool omp = true // use OpenMP
    );
}
