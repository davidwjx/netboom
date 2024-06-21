// Define this to '1' in order to use FP SSE code (and FP native code as well)
#define SSE_FP (0)

#if defined(WIN32)
#define USE_STREAMING_STORE (1)
#endif

// Define this to make use of old code.
#define OLD_IMPLEMENTATION (0)

// Select the calculation precision (enable one of the following)
#define SHIFT (10)
//#define SHIFT (8)

#if defined(_OPENMP)
    #include <omp.h>
#endif

#include <iostream>
#include <algorithm>

#if defined(__ARM_NEON__) || defined(__aarch64__)
    // SSE2 macros for Android/ARM/NEON
    #include <SSE2NEON.h>
#elif defined(_WIN32)
    // SSE2 intrinsics for Visual Studio
    #include <intrin.h>
#elif defined(__linux__) && !defined(__arm__)
    // SSE2 intrinsics for Linux
    #include <x86intrin.h>
#elif defined(__linux__)
    //#error No SSE2 Intrinsics defined.
#endif

#include "CommonAlgo.h"
#include "TiffFile.h"
#include "RgbShufflers.h"

#define NOMINMAX
#include "OSUtilities.h"
#undef NOMINMAX

#include "YUV422ToRGB.h"

static bool omp;

namespace InuCommon
{
#define ROUND (1ll<<(SHIFT - 1))
    
    // These values are used only by YUV422ToRGB_Naive_f/YUV422ToRGB_SSE_f
#define Y_CONST (1.1640625f)
#define BU_CONST (2.015625f)
#define GU_CONST (-0.390625f)
#define GV_CONST (-0.8125f)
#define RV_CONST (1.59765625f)

#define CONST_16 (16.0f)
#define CONST_128 (128.0f)


    constexpr double CSC_MATRIX_0_0_VALUE{ 1.000000 };
    constexpr double CSC_MATRIX_0_1_VALUE{ 0.000000 };
    constexpr double CSC_MATRIX_0_2_VALUE{ 1.406250 };
    constexpr double CSC_MATRIX_1_0_VALUE{ 1.000000 };
    constexpr double CSC_MATRIX_1_1_VALUE{ -0.343750 };
    constexpr double CSC_MATRIX_1_2_VALUE{ -0.703125 };
    constexpr double CSC_MATRIX_2_0_VALUE{ 1.000000 };
    constexpr double CSC_MATRIX_2_1_VALUE{ 1.765625 };
    constexpr double CSC_MATRIX_2_2_VALUE{ 0.000000 };

    constexpr int CSC_MATRIX_0_0 = int(CSC_MATRIX_0_0_VALUE * (1ll << SHIFT));
    constexpr int CSC_MATRIX_0_1 = int(CSC_MATRIX_0_1_VALUE * (1ll << SHIFT));
    constexpr int CSC_MATRIX_0_2 = int(CSC_MATRIX_0_2_VALUE * (1ll << SHIFT));
    constexpr int CSC_MATRIX_1_0 = int(CSC_MATRIX_1_0_VALUE * (1ll << SHIFT));
    constexpr int CSC_MATRIX_1_1 = int(CSC_MATRIX_1_1_VALUE * (1ll << SHIFT));
    constexpr int CSC_MATRIX_1_2 = int(CSC_MATRIX_1_2_VALUE * (1ll << SHIFT));
    constexpr int CSC_MATRIX_2_0 = int(CSC_MATRIX_2_0_VALUE * (1ll << SHIFT));
    constexpr int CSC_MATRIX_2_1 = int(CSC_MATRIX_2_1_VALUE * (1ll << SHIFT));
    constexpr int CSC_MATRIX_2_2 = int(CSC_MATRIX_2_2_VALUE * (1ll << SHIFT));


#if OLD_IMPLEMENTATION
    void YUV422ToRGB_Naive_i_Old(
        unsigned char *_pOutData,
        unsigned short *_pInData,
        const unsigned int width,
        const unsigned int height,
        const unsigned int inputStride, // unused in code
        bool swapOutput
    )
    {
        unsigned int imageSize = width * height;

        unsigned char *pOutData = (unsigned char *)_pOutData;
        const unsigned char *pInData = (unsigned char *)_pInData;

        for (int i = 0; i < int(sizeof(_pInData[0])*imageSize / sizeof(pInData[0])); i += 4 * sizeof(pInData[0]))
        {
            unsigned char v = *pInData++;
            unsigned char y1 = *pInData++;
            unsigned char u = *pInData++;
            unsigned char y = *pInData++;

            unsigned char v1 = v;
            unsigned char u1 = u;

            int c = y - 16;
            int d = u - 128;
            int e = v - 128;

            int c1 = y1 - 16;
            int d1 = u1 - 128;
            int e1 = v1 - 128;

            int b = (298 * c + 516 * d + 128) >> 8;               // B
            b = InuCommon::COsUtilities::minNoBranch(b, 255);

            int g = (298 * c - 100 * d - 208 * e + 128) >> 8;     // G
            g = InuCommon::COsUtilities::minNoBranch(g, 255);

            int r = (298 * c + 409 * e + 128) >> 8;               // R
            r = InuCommon::COsUtilities::minNoBranch(r, 255);

            if (swapOutput)
            {
                InuCommon::CRgbaShuffler::InsertNaive(pOutData,
                    (unsigned char)(InuCommon::COsUtilities::maxNoBranch(r, 0)),
                    (unsigned char)(InuCommon::COsUtilities::maxNoBranch(g, 0)),
                    (unsigned char)(InuCommon::COsUtilities::maxNoBranch(b, 0)),
                    InuCommon::ALPHA_TRANSPARENT
                );
            }
            else // output is webcam register or rectified
            {
                InuCommon::CBgraShuffler::InsertNaive(pOutData,
                    (unsigned char)(InuCommon::COsUtilities::maxNoBranch(r, 0)),
                    (unsigned char)(InuCommon::COsUtilities::maxNoBranch(g, 0)),
                    (unsigned char)(InuCommon::COsUtilities::maxNoBranch(b, 0)),
                    InuCommon::ALPHA_TRANSPARENT
                );
            }

            b = (298 * c1 + 516 * d1 + 128) >> 8;               // B
            b = InuCommon::COsUtilities::minNoBranch(b, 255);

            g = (298 * c1 - 100 * d1 - 208 * e1 + 128) >> 8;     // G
            g = InuCommon::COsUtilities::minNoBranch(g, 255);

            r = (298 * c1 + 409 * e1 + 128) >> 8;               // R
            r = InuCommon::COsUtilities::minNoBranch(r, 255);

            if (swapOutput)
            {
                InuCommon::CRgbaShuffler::InsertNaive(pOutData,
                    (unsigned char)(InuCommon::COsUtilities::maxNoBranch(r, 0)),
                    (unsigned char)(InuCommon::COsUtilities::maxNoBranch(g, 0)),
                    (unsigned char)(InuCommon::COsUtilities::maxNoBranch(b, 0)),
                    InuCommon::ALPHA_TRANSPARENT
                );
            }
            else // output is webcam register or rectified
            {
                InuCommon::CBgraShuffler::InsertNaive(pOutData,
                    (unsigned char)(InuCommon::COsUtilities::maxNoBranch(r, 0)),
                    (unsigned char)(InuCommon::COsUtilities::maxNoBranch(g, 0)),
                    (unsigned char)(InuCommon::COsUtilities::maxNoBranch(b, 0)),
                    InuCommon::ALPHA_TRANSPARENT
                );
            }
        }
    }
#endif

    template <class T>
    void CalcRGB_Naive_i(
        const unsigned short y0,
        const unsigned short u0,
        const unsigned short v0,
        const unsigned short y1,
        unsigned char*& pOutData
    )
    {
        double y0f = y0;
        double y1f = y1;

        double u0f = u0 - 128.0;
        double v0f = v0 - 128.0;

        double fr = CSC_MATRIX_0_0_VALUE * y0f + CSC_MATRIX_0_1_VALUE * u0f + CSC_MATRIX_0_2_VALUE * v0f + 0.5;
        double fg = CSC_MATRIX_1_0_VALUE * y0f + CSC_MATRIX_1_1_VALUE * u0f + CSC_MATRIX_1_2_VALUE * v0f + 0.5;
        double fb = CSC_MATRIX_2_0_VALUE * y0f + CSC_MATRIX_2_1_VALUE * u0f + CSC_MATRIX_2_2_VALUE * v0f + 0.5;

        int r = std::max(std::min(int(fr), 255), 0);
        int g = std::max(std::min(int(fg), 255), 0);
        int b = std::max(std::min(int(fb), 255), 0);

        T::InsertNaive(pOutData, r, g, b, InuCommon::ALPHA_TRANSPARENT);

        fr = CSC_MATRIX_0_0_VALUE * y1f + CSC_MATRIX_0_1_VALUE * u0f + CSC_MATRIX_0_2_VALUE * v0f + 0.5;
        fg = CSC_MATRIX_1_0_VALUE * y1f + CSC_MATRIX_1_1_VALUE * u0f + CSC_MATRIX_1_2_VALUE * v0f + 0.5;
        fb = CSC_MATRIX_2_0_VALUE * y1f + CSC_MATRIX_2_1_VALUE * u0f + CSC_MATRIX_2_2_VALUE * v0f + 0.5;

        r = std::max(std::min(int(fr), 255), 0);
        g = std::max(std::min(int(fg), 255), 0);
        b = std::max(std::min(int(fb), 255), 0);

        T::InsertNaive(pOutData, r, g, b, InuCommon::ALPHA_TRANSPARENT);
    }

    template <class T>
    void CalcRGB_Naive_16bpp_i(
        unsigned char *pOutFirstData,
        unsigned char *pOutSecondData,
        const unsigned char *pInData,
        const unsigned int width,
        const unsigned int height,
        const unsigned int inputStride,
        const unsigned int uOffset,
        const unsigned int vOffset,
        const unsigned int yOffset0,
        const unsigned int yOffset1
    )
    {
        for (unsigned int row = 0; row < height; row++)
        {
            // NOTE: Divided by 2 here as each iteration consumes 2 input pixels
            for (unsigned int column = 0; column < width / 2; column++, pInData += 4)
            {
                unsigned char y0 = pInData[yOffset0];
                unsigned char u0 = pInData[uOffset];
                unsigned char v0 = pInData[vOffset];
                unsigned char y1 = pInData[yOffset1];

                CalcRGB_Naive_i<T>(y0, u0, v0, y1, pOutFirstData);
            }

            if (!pOutSecondData)
            {
                continue;
            }

            // NOTE: Divided by 2 here as each iteration consumes 2 input pixels
            for (unsigned int column = width / 2; column < width; column++, pInData += 4)
            {
                unsigned char y0 = pInData[yOffset0];
                unsigned char u0 = pInData[uOffset];
                unsigned char v0 = pInData[vOffset];
                unsigned char y1 = pInData[yOffset1];

                CalcRGB_Naive_i<T>(y0, u0, v0, y1, pOutSecondData);
            }
        }
    }

    void YUV422ToRGB_Naive_i(
        unsigned char *_pOutFirstData,
        unsigned char *_pOutSecondData,
        unsigned short *_pInData,
        const unsigned int width,
        const unsigned int height,
        const unsigned int inputStride,
        const bool swapOutput,
        const bool swapInput
    )
    {
        const int uOffset = swapInput ? 1 : 2;
        const int vOffset = swapInput ? 3 : 0;
        const int yOffset0 = swapInput ? 0 : 3;
        const int yOffset1 = swapInput ? 2 : 1;

        if (swapOutput)
        {
            CalcRGB_Naive_16bpp_i<InuCommon::CRgbaShuffler>(
                _pOutFirstData,
                _pOutSecondData,
                (unsigned char *)_pInData,
                width,
                height,
                inputStride,
                uOffset,
                vOffset,
                yOffset0,
                yOffset1
            );
        }
        else
        {
            CalcRGB_Naive_16bpp_i<InuCommon::CBgraShuffler>(
                _pOutFirstData,
                _pOutSecondData,
                (unsigned char *)_pInData,
                width,
                height,
                inputStride,
                uOffset,
                vOffset,
                yOffset0,
                yOffset1
            );
        }
    }

    inline void ExtractYUV(
        const unsigned char* pInData,
        unsigned short& y0,
        unsigned short& y1,
        unsigned short& u,
        unsigned short& v
    )
    {
       // yyyyyyyy vvvv00yy 00vvvvvv
        v = pInData[2];
        v <<= 4;
        v = v | (pInData[1] >> 4);

        y0 = pInData[1] & 0x3;
        y0 <<= 8;
        y0 |= pInData[0] ;

        // yyyyyyyy uuuu00yy 00uuuuuuuu
        u = pInData[5];
        u <<= 4;
        u = u | (pInData[4] >> 4);

        y1 = pInData[4] & 0x3;
        y1 <<= 8;
        y1 |= pInData[3];
    }

    template <class T>
    void CalcRGB_Naive_24bpp_i(
        unsigned char *pOutFirstData,
        unsigned char *pOutSecondData,
        const unsigned char *pInData,
        const unsigned int width,
        const unsigned int height,
        const unsigned int inputStride
    )
    {
        unsigned short y0, y1, u, v;

        for (unsigned int j = 0; j < height; j++)
        {
            // Read the first line
            for (unsigned int i = 0; i < width / 2 ; i++)
            {
                ExtractYUV(pInData, y0, y1, u, v);
                CalcRGB_Naive_i<T>(y0, u, v, y1, pOutFirstData);
                pInData += 6;
            }

            if (!pOutSecondData )
            {
                continue;
            }

            for (unsigned int i = 0; i < width / 2; i++)
            {
                ExtractYUV(pInData, y0, y1, u, v);
                CalcRGB_Naive_i<T>(y0, u, v, y1, pOutSecondData);
                pInData += 6;
            }
        }
    }

    void YUV422ToRGB_24bpp_Naive_i(
        unsigned char *_pOutFirstData,
        unsigned char *_pOutSecondData,
        unsigned char *_pInData,
        const unsigned int width,
        const unsigned int height,
        const unsigned int inputStride,
        const bool swapOutput,
        const bool swapInput
    )
    {
        if (swapOutput)
        {
            CalcRGB_Naive_24bpp_i<InuCommon::CRgbaShuffler>(
                _pOutFirstData,
                _pOutSecondData,
                _pInData,
                width,
                height,
                inputStride
             );
        }
        else
        {
            CalcRGB_Naive_24bpp_i<InuCommon::CBgraShuffler>(
                _pOutFirstData,
                _pOutSecondData,
                _pInData,
                width,
                height,
                inputStride
            );
        }
    }

    void YUV422ToRGB_Naive_f(
        unsigned char *_pOutData,
        unsigned short *_pInData,
        const unsigned int width,
        const unsigned int height,
        const unsigned int inputStride,
        const bool swapOutput,
        const bool swapInput
    )
    {
        unsigned int imageSize = height * width;

        unsigned char *pOutData = (unsigned char *)_pOutData;
        const unsigned char *pInData = (unsigned char *)_pInData;

#if defined(WIN32)
        // otherwise does not match....
        unsigned int currentControlFp = 0;
        _controlfp_s(&currentControlFp, _RC_DOWN, _MCW_RC); // set rounding mode to truncate
#endif

        int uOffset = swapInput ? 3 : 2;
        int vOffset = swapInput ? 1 : 0;
        int yOffset0 = swapInput ? 0 : 3;
        int yOffset1 = swapInput ? 2 : 1;

        if (swapOutput)
        {
            for (int i = 0; i < int(sizeof(_pInData[0])*imageSize / sizeof(pInData[0])); i += 4 * sizeof(pInData[0]))
            {
                unsigned char v0 = pInData[i + vOffset];
                unsigned char u0 = pInData[i + uOffset];
                unsigned char y0 = pInData[i + yOffset0];

                unsigned char y1 = pInData[i + yOffset1];
                unsigned char v1 = v0;
                unsigned char u1 = u0;

                float _y0 = float(y0) - CONST_16;
                float _u0 = float(u0) - CONST_128;
                float _v0 = float(v0) - CONST_128;

                float b = Y_CONST * _y0 + BU_CONST * _u0;     // B
                float g = Y_CONST * _y0 + GU_CONST * _u0 + GV_CONST * _v0;     // G
                float r = Y_CONST * _y0 + RV_CONST * _v0;     // R

                b = std::min(b, 255.0f);
                g = std::min(g, 255.0f);
                r = std::min(r, 255.0f);

                b = std::max(b, 0.0f);
                g = std::max(g, 0.0f);
                r = std::max(r, 0.0f);

                unsigned char *outAddr = &pOutData[2 * i];

                InuCommon::CRgbaShuffler::InsertNaive(outAddr,
                    (unsigned char)r,
                    (unsigned char)g,
                    (unsigned char)b,
                    InuCommon::ALPHA_TRANSPARENT
                );

                float _y1 = (float)(int)(y1)-CONST_16;
                float _u1 = (float)(int)(u1)-CONST_128;
                float _v1 = (float)(int)(v1)-CONST_128;

                b = Y_CONST * _y1 + BU_CONST * _u1;     // B
                g = Y_CONST * _y1 + GU_CONST * _u1 + GV_CONST * _v1;     // G
                r = Y_CONST * _y1 + RV_CONST * _v1;     // R

                b = std::min(b, 255.0f);
                g = std::min(g, 255.0f);
                r = std::min(r, 255.0f);

                b = std::max(b, 0.0f);
                g = std::max(g, 0.0f);
                r = std::max(r, 0.0f);

                outAddr = &pOutData[2 * i + 4];

                InuCommon::CRgbaShuffler::InsertNaive(outAddr,
                    (unsigned char)r,
                    (unsigned char)g,
                    (unsigned char)b,
                    InuCommon::ALPHA_TRANSPARENT
                );
            }
        }
        else
        {
            for (int i = 0; i < int(sizeof(_pInData[0])*imageSize / sizeof(pInData[0])); i += 4 * sizeof(pInData[0]))
            {
                unsigned char v0 = pInData[i + vOffset];
                unsigned char u0 = pInData[i + uOffset];
                unsigned char y0 = pInData[i + yOffset0];

                unsigned char y1 = pInData[i + yOffset1];
                unsigned char v1 = v0;
                unsigned char u1 = u0;

                float _y0 = float(y0) - CONST_16;
                float _u0 = float(u0) - CONST_128;
                float _v0 = float(v0) - CONST_128;

                float b = Y_CONST * _y0 + BU_CONST * _u0;     // B
                float g = Y_CONST * _y0 + GU_CONST * _u0 + GV_CONST * _v0;     // G
                float r = Y_CONST * _y0 + RV_CONST * _v0;     // R

                b = std::min(b, 255.0f);
                g = std::min(g, 255.0f);
                r = std::min(r, 255.0f);

                b = std::max(b, 0.0f);
                g = std::max(g, 0.0f);
                r = std::max(r, 0.0f);

                //unsigned char *outAddr = &pOutData[2 * i];

                InuCommon::CBgraShuffler::InsertNaive(pOutData,
                    (unsigned char)r,
                    (unsigned char)g,
                    (unsigned char)b,
                    InuCommon::ALPHA_TRANSPARENT
                );

                float _y1 = (float)(int)(y1)-CONST_16;
                float _u1 = (float)(int)(u1)-CONST_128;
                float _v1 = (float)(int)(v1)-CONST_128;

                b = Y_CONST * _y1 + BU_CONST * _u1;     // B
                g = Y_CONST * _y1 + GU_CONST * _u1 + GV_CONST * _v1;     // G
                r = Y_CONST * _y1 + RV_CONST * _v1;     // R

                b = std::min(b, 255.0f);
                g = std::min(g, 255.0f);
                r = std::min(r, 255.0f);

                b = std::max(b, 0.0f);
                g = std::max(g, 0.0f);
                r = std::max(r, 0.0f);

                //outAddr = &pOutData[2 * i + 4];

                InuCommon::CBgraShuffler::InsertNaive(pOutData,
                    (unsigned char)r,
                    (unsigned char)g,
                    (unsigned char)b,
                    InuCommon::ALPHA_TRANSPARENT
                );
            }
        }
    }

#if !defined(__arm__) || defined(__ARM_NEON__)
    /// <summary>
    /// 
    /// </summary>
    /// <remark>May optionaly use OpenMP</ramerk>
    /// <param name="_pInData"></param>
    /// <param name="_pOutData"></param>
    /// <param name="imageSize"></param>
    /// <param name="swapOutput"></param>
    void YUV422ToRGB_SSE_f(
        unsigned short *_pInData,
        unsigned char *_pOutData,
        const unsigned int width,
        const unsigned int height,
        const unsigned int inputStride,
        const bool swapOutput
    )
    { 
        unsigned int imageSize = width * height;

        static const __m128i ZERO = _mm_setzero_si128();

        //                                          Y1             U            Y0             V
        static const __m128 G_CONST0 = _mm_set_ps(Y_CONST, GU_CONST, Y_CONST, GV_CONST);
        static const __m128 BR_CONST0 = _mm_set_ps(Y_CONST, BU_CONST, Y_CONST, RV_CONST);

        static const __m128 SUB_CONST = _mm_set_ps(CONST_16, CONST_128, CONST_16, CONST_128);

        __m128i * pInData = (__m128i *)_pInData;
        __m128i *pOutData = (__m128i *)_pOutData;

        // Both "legs" of the "if" are identical, less output byte placment

#pragma omp parallel if(omp)
        {
            int tid;
            int cnt = (int(sizeof(_pInData[0]) * imageSize / sizeof(__m128i)));

#if defined(_OPENMP)
            if (omp)
            {
                int N = omp_get_num_threads();
                cnt /= N;
                tid = omp_get_thread_num();
            }
#else
            tid = 0;
#endif

#if defined(WIN32)
            unsigned int currentControlFp = 0;
            _controlfp_s(&currentControlFp, _RC_DOWN, _MCW_RC); // set rounding mode to truncate
#endif

            if (swapOutput)
            {
                for (int i = tid*cnt; i < (tid + 1)*cnt; ++i)
                {
                    __m128i yuv;

#if USE_STREAMING_STORE
                    yuv = _mm_stream_load_si128(pInData + i); // { V0, Y0, U0, Y1,   V1, Y2, U1, Y3,   V2, Y5, U2, Y6,   V3, Y6, U3, Y7 } -- 16 vales, 8 bitseach
#else
                    yuv = _mm_loadu_si128(pInData + i); // { V0, Y0, U0, Y1,   V1, Y2, U1, Y3,   V2, Y5, U2, Y6,   V3, Y6, U3, Y7 } -- 16 vales, 8 bitseach
#endif

#if defined(WIN32)
                    __m128i yuvLow = _mm_unpacklo_epi8(yuv, ZERO); // -- 16 bits each

                    __m128 y0y1_f = _mm_cvtepi32_ps(_mm_unpacklo_epi16(yuvLow, ZERO)); // {V0, Y0, U0, Y1}
                    y0y1_f = _mm_sub_ps(y0y1_f, SUB_CONST);

                    __m128 b0 = _mm_dp_ps(y0y1_f, BR_CONST0, 0x64);
                    __m128 g0 = _mm_dp_ps(y0y1_f, G_CONST0, 0x72);
                    __m128 r0 = _mm_dp_ps(y0y1_f, BR_CONST0, 0x31);

                    __m128i rgb0 = _mm_cvtps_epi32(_mm_or_ps(g0, _mm_or_ps(b0, r0)));

                    __m128 b1 = _mm_dp_ps(y0y1_f, BR_CONST0, 0xc4);
                    __m128 g1 = _mm_dp_ps(y0y1_f, G_CONST0, 0xd2);
                    __m128 r1 = _mm_dp_ps(y0y1_f, BR_CONST0, 0x91);

                    __m128i rgb1 = _mm_cvtps_epi32(_mm_or_ps(g1, _mm_or_ps(b1, r1)));

                    __m128 y2y3_f = _mm_cvtepi32_ps(_mm_unpackhi_epi16(yuvLow, ZERO)); //  {V1, Y2, U1, Y3}
                    y2y3_f = _mm_sub_ps(y2y3_f, SUB_CONST);

                    __m128 b2 = _mm_dp_ps(y2y3_f, BR_CONST0, 0x64);
                    __m128 g2 = _mm_dp_ps(y2y3_f, G_CONST0, 0x72);
                    __m128 r2 = _mm_dp_ps(y2y3_f, BR_CONST0, 0x31);

                    __m128i rgb2 = _mm_cvtps_epi32(_mm_or_ps(g2, _mm_or_ps(b2, r2)));

                    __m128 b3 = _mm_dp_ps(y2y3_f, BR_CONST0, 0xc4);
                    __m128 g3 = _mm_dp_ps(y2y3_f, G_CONST0, 0xd2);
                    __m128 r3 = _mm_dp_ps(y2y3_f, BR_CONST0, 0x91);

                    __m128i rgb3 = _mm_cvtps_epi32(_mm_or_ps(g3, _mm_or_ps(b3, r3)));

                    __m128i rgb01 = _mm_packs_epi32(rgb0, rgb1);
                    __m128i rgb23 = _mm_packs_epi32(rgb2, rgb3);

                    __m128i rgb0123 = _mm_packus_epi16(rgb01, rgb23);

                    // TODO: InuCommon::ALPHA_TRANSPARENT
#if USE_STREAMING_STORE
                    _mm_stream_si128(pOutData + (2 * i) + 0, rgb0123);
#else
                    _mm_store_si128(pOutData + (2 * i) + 0, rgb0123);
#endif

                    __m128i yuvHigh = _mm_unpackhi_epi8(yuv, ZERO);

                    __m128 y4y5_f = _mm_cvtepi32_ps(_mm_unpacklo_epi16(yuvHigh, ZERO)); // {V2, Y5, U2, Y6}
                    y4y5_f = _mm_sub_ps(y4y5_f, SUB_CONST);

                    __m128 b4 = _mm_dp_ps(y4y5_f, BR_CONST0, 0x64);
                    __m128 g4 = _mm_dp_ps(y4y5_f, G_CONST0, 0x72);
                    __m128 r4 = _mm_dp_ps(y4y5_f, BR_CONST0, 0x31);

                    __m128i rgb4 = _mm_cvtps_epi32(_mm_or_ps(g4, _mm_or_ps(b4, r4)));

                    __m128 b5 = _mm_dp_ps(y4y5_f, BR_CONST0, 0xc4);
                    __m128 g5 = _mm_dp_ps(y4y5_f, G_CONST0, 0xd2);
                    __m128 r5 = _mm_dp_ps(y4y5_f, BR_CONST0, 0x91);

                    __m128i rgb5 = _mm_cvtps_epi32(_mm_add_ps(g5, _mm_add_ps(b5, r5)));


                    __m128 y6y7_f = _mm_cvtepi32_ps(_mm_unpackhi_epi16(yuvHigh, ZERO)); // {V3, Y6, U3, Y7};
                    y6y7_f = _mm_sub_ps(y6y7_f, SUB_CONST);

                    __m128 b6 = _mm_dp_ps(y6y7_f, BR_CONST0, 0x64);
                    __m128 g6 = _mm_dp_ps(y6y7_f, G_CONST0, 0x72);
                    __m128 r6 = _mm_dp_ps(y6y7_f, BR_CONST0, 0x31);

                    __m128i rgb6 = _mm_cvtps_epi32(_mm_or_ps(g6, _mm_or_ps(b6, r6)));

                    __m128 b7 = _mm_dp_ps(y6y7_f, BR_CONST0, 0xc4);
                    __m128 g7 = _mm_dp_ps(y6y7_f, G_CONST0, 0xd2);
                    __m128 r7 = _mm_dp_ps(y6y7_f, BR_CONST0, 0x91);

                    __m128i rgb7 = _mm_cvtps_epi32(_mm_or_ps(g7, _mm_or_ps(b7, r7)));

                    // pack ...


                    __m128i rgb45 = _mm_packs_epi32(rgb4, rgb5);
                    __m128i rgb67 = _mm_packs_epi32(rgb6, rgb7);

                    __m128i rgb4567 = _mm_packus_epi16(rgb45, rgb67);

                    // TODO: InuCommon::ALPHA_TRANSPARENT
#if USE_STREAMING_STORE
                    _mm_stream_si128(pOutData + (2ll * i) + 1, rgb4567);
#else
                    _mm_store_si128(pOutData + (2ll * i) + 1, rgb4567);
#endif
#endif
                }
            }
            else
            {
                for (int i = tid*cnt; i < (tid + 1)*cnt; ++i)
                {
                    __m128i yuv;

#if USE_STREAMING_STORE
                    yuv = _mm_stream_load_si128(pInData + i); // { V0, Y0, U0, Y1,   V1, Y2, U1, Y3,   V2, Y5, U2, Y6,   V3, Y6, U3, Y7 } -- 16 vales, 8 bitseach
#else
                    yuv = _mm_loadu_si128(pInData + i); // { V0, Y0, U0, Y1,   V1, Y2, U1, Y3,   V2, Y5, U2, Y6,   V3, Y6, U3, Y7 } -- 16 vales, 8 bitseach
#endif

#if defined(WIN32)
                    __m128i yuvLow = _mm_unpacklo_epi8(yuv, ZERO); // -- 16 bits each

                    __m128 y0y1_f = _mm_cvtepi32_ps(_mm_unpacklo_epi16(yuvLow, ZERO)); // {V0, Y0, U0, Y1}
                    y0y1_f = _mm_sub_ps(y0y1_f, SUB_CONST);

                    __m128 b0 = _mm_dp_ps(y0y1_f, BR_CONST0, 0x61);
                    __m128 g0 = _mm_dp_ps(y0y1_f, G_CONST0, 0x72);
                    __m128 r0 = _mm_dp_ps(y0y1_f, BR_CONST0, 0x34);

                    __m128i rgb0 = _mm_cvtps_epi32(_mm_or_ps(g0, _mm_or_ps(b0, r0)));

                    __m128 b1 = _mm_dp_ps(y0y1_f, BR_CONST0, 0xc1);
                    __m128 g1 = _mm_dp_ps(y0y1_f, G_CONST0, 0xd2);
                    __m128 r1 = _mm_dp_ps(y0y1_f, BR_CONST0, 0x94);

                    __m128i rgb1 = _mm_cvtps_epi32(_mm_or_ps(g1, _mm_or_ps(b1, r1)));

                    __m128 y2y3_f = _mm_cvtepi32_ps(_mm_unpackhi_epi16(yuvLow, ZERO)); //  {V1, Y2, U1, Y3}
                    y2y3_f = _mm_sub_ps(y2y3_f, SUB_CONST);

                    __m128 b2 = _mm_dp_ps(y2y3_f, BR_CONST0, 0x61);
                    __m128 g2 = _mm_dp_ps(y2y3_f, G_CONST0, 0x72);
                    __m128 r2 = _mm_dp_ps(y2y3_f, BR_CONST0, 0x34);

                    __m128i rgb2 = _mm_cvtps_epi32(_mm_or_ps(g2, _mm_or_ps(b2, r2)));

                    __m128 b3 = _mm_dp_ps(y2y3_f, BR_CONST0, 0xc1);
                    __m128 g3 = _mm_dp_ps(y2y3_f, G_CONST0, 0xd2);
                    __m128 r3 = _mm_dp_ps(y2y3_f, BR_CONST0, 0x94);

                    __m128i rgb3 = _mm_cvtps_epi32(_mm_or_ps(g3, _mm_or_ps(b3, r3)));

                    __m128i rgb01 = _mm_packs_epi32(rgb0, rgb1);
                    __m128i rgb23 = _mm_packs_epi32(rgb2, rgb3);

                    __m128i rgb0123 = _mm_packus_epi16(rgb01, rgb23);

                    // TODO: InuCommon::ALPHA_TRANSPARENT
#if USE_STREAMING_STORE
                    _mm_stream_si128(pOutData + (2ll * i) + 0, rgb0123);
#else
                    _mm_store_si128(pOutData + (2ll * i) + 0, rgb0123);
#endif

                    __m128i yuvHigh = _mm_unpackhi_epi8(yuv, ZERO);

                    __m128 y4y5_f = _mm_cvtepi32_ps(_mm_unpacklo_epi16(yuvHigh, ZERO)); // {V2, Y5, U2, Y6}
                    y4y5_f = _mm_sub_ps(y4y5_f, SUB_CONST);

                    __m128 b4 = _mm_dp_ps(y4y5_f, BR_CONST0, 0x61);
                    __m128 g4 = _mm_dp_ps(y4y5_f, G_CONST0, 0x72);
                    __m128 r4 = _mm_dp_ps(y4y5_f, BR_CONST0, 0x34);

                    __m128i rgb4 = _mm_cvtps_epi32(_mm_or_ps(g4, _mm_or_ps(b4, r4)));

                    __m128 b5 = _mm_dp_ps(y4y5_f, BR_CONST0, 0xc1);
                    __m128 g5 = _mm_dp_ps(y4y5_f, G_CONST0, 0xd2);
                    __m128 r5 = _mm_dp_ps(y4y5_f, BR_CONST0, 0x94);

                    __m128i rgb5 = _mm_cvtps_epi32(_mm_add_ps(g5, _mm_add_ps(b5, r5)));


                    __m128 y6y7_f = _mm_cvtepi32_ps(_mm_unpackhi_epi16(yuvHigh, ZERO)); // {V3, Y6, U3, Y7};
                    y6y7_f = _mm_sub_ps(y6y7_f, SUB_CONST);

                    __m128 b6 = _mm_dp_ps(y6y7_f, BR_CONST0, 0x61);
                    __m128 g6 = _mm_dp_ps(y6y7_f, G_CONST0, 0x72);
                    __m128 r6 = _mm_dp_ps(y6y7_f, BR_CONST0, 0x34);

                    __m128i rgb6 = _mm_cvtps_epi32(_mm_or_ps(g6, _mm_or_ps(b6, r6)));

                    __m128 b7 = _mm_dp_ps(y6y7_f, BR_CONST0, 0xc1);
                    __m128 g7 = _mm_dp_ps(y6y7_f, G_CONST0, 0xd2);
                    __m128 r7 = _mm_dp_ps(y6y7_f, BR_CONST0, 0x94);

                    __m128i rgb7 = _mm_cvtps_epi32(_mm_or_ps(g7, _mm_or_ps(b7, r7)));

                    // pack ...


                    __m128i rgb45 = _mm_packs_epi32(rgb4, rgb5);
                    __m128i rgb67 = _mm_packs_epi32(rgb6, rgb7);

                    __m128i rgb4567 = _mm_packus_epi16(rgb45, rgb67);

                    // TODO: InuCommon::ALPHA_TRANSPARENT
#if USE_STREAMING_STORE
                    _mm_stream_si128(pOutData + (2ll * i) + 1, rgb4567);
#else
                    _mm_store_si128(pOutData + (2ll * i) + 1, rgb4567);
#endif
#endif
                }
            }
        }
    }
#endif

#if !defined(__arm__) || defined(__ARM_NEON__)
    /// <summary>
    /// Swapped-input (Y0, V, Y1, U) to RGB
    /// </summary>
    /// <remark>new</remark>
    /// <remark>May optionaly use OpenMP</ramerk>
    /// <param name="_pInData"></param>
    /// <param name="_pOutData"></param>
    /// <param name="imageSize"></param>
    /// <param name="swapOutput"></param>
    void YUV422ToRGB_SSE_i(
        unsigned short *_pInData,
        unsigned char *_pOutData,
        const unsigned int width,
        const unsigned int height,
        const unsigned int inputStride,
        const unsigned int outputStride,
        const bool swapOutput
    )
    { 
        unsigned int imageSize = width * height;

        const __m128i * pInData = (__m128i *)_pInData;
        __m128i *pOutData = (__m128i *)_pOutData;

        __m128i CSC_MATRIX_x_0;
        __m128i CSC_MATRIX_x_1;
        __m128i CSC_MATRIX_x_2;

        const __m128i OFFSET = _mm_set_epi32(128, 0, 128, 0);

        // TODO: Handle swapped output
        if (swapOutput)
        {
            // Shuffle output data to match required output format

            CSC_MATRIX_x_0 = _mm_set_epi32(0, CSC_MATRIX_2_0, CSC_MATRIX_1_0, CSC_MATRIX_0_0);
            CSC_MATRIX_x_1 = _mm_set_epi32(0, CSC_MATRIX_2_1, CSC_MATRIX_1_1, CSC_MATRIX_0_1);
            CSC_MATRIX_x_2 = _mm_set_epi32(0, CSC_MATRIX_2_2, CSC_MATRIX_1_2, CSC_MATRIX_0_2);
        }
        else
        {
            // No shuffle output data

            CSC_MATRIX_x_0 = _mm_set_epi32(0, CSC_MATRIX_0_0, CSC_MATRIX_1_0, CSC_MATRIX_2_0);
            CSC_MATRIX_x_1 = _mm_set_epi32(0, CSC_MATRIX_0_1, CSC_MATRIX_1_1, CSC_MATRIX_2_1);
            CSC_MATRIX_x_2 = _mm_set_epi32(0, CSC_MATRIX_0_2, CSC_MATRIX_1_2, CSC_MATRIX_2_2);
        }

        const __m128i alphaRound = _mm_set_epi32(InuCommon::ALPHA_TRANSPARENT << SHIFT, ROUND, ROUND, ROUND);

        // 8 16-bit items, each representing 2 input pixels
        constexpr int SF { sizeof(__m128i) / sizeof(_pInData[0]) };

#pragma omp parallel for if(omp)
        for (int row = 0; row < int(height); row++)
        {
            const int ii0 = long(row) * inputStride / (SF * 2);
            const __m128i* pRowStartIn = &pInData[ii0];

            const int oi0 = long(row) * outputStride / (SF * 2);
            __m128i* pRowStartOut = &pOutData[oi0];

            for (int column = 0; column < int(width / SF); column++, pRowStartOut += 2)
            {
                // 16 values, 8 bits each (from lsb). These will yield 8 pixels.
                // { Y0, U0, Y1, V0,  Y2, U1, Y3, V1,  Y4, U2, Y5, V2,  Y6, U3, Y7, V3 }
                __m128i yuv = _mm_loadu_si128(&pRowStartIn[column]);


                // 16 bits each (from lsb) { Y0, U0, Y1, V0,  Y2, U1, Y3, V1 }
                __m128i yuvLow = _mm_unpacklo_epi8(yuv, _mm_setzero_si128());

                // low

                // 32 bits each (from lsb) { Y0, U0, Y1, V0, }
                __m128i a = _mm_unpacklo_epi16(yuvLow, _mm_setzero_si128());
                a = _mm_sub_epi32(a, OFFSET);

                __m128i u0u1 = _mm_shuffle_epi32(a, _MM_SHUFFLE(1, 1, 1, 1)); // pick u0
                __m128i v0v1 = _mm_shuffle_epi32(a, _MM_SHUFFLE(3, 3, 3, 3)); // pick v0

                u0u1 = _mm_mullo_epi32(u0u1, CSC_MATRIX_x_1);
                v0v1 = _mm_mullo_epi32(v0v1, CSC_MATRIX_x_2);

                __m128i sumLow01 = _mm_add_epi32(u0u1, v0v1);

                // b - g - r - 0
                __m128i y0 = _mm_shuffle_epi32(a, _MM_SHUFFLE(0, 0, 0, 0));
                y0 = _mm_mullo_epi32(y0, CSC_MATRIX_x_0);

                __m128i r0g0b0 = _mm_add_epi32(sumLow01, y0);
                r0g0b0 = _mm_add_epi32(r0g0b0, alphaRound);
                r0g0b0 = _mm_srai_epi32(r0g0b0, SHIFT);

                // b - g - r - 0
                __m128i y1 = _mm_shuffle_epi32(a, _MM_SHUFFLE(2, 2, 2, 2));
                y1 = _mm_mullo_epi32(y1, CSC_MATRIX_x_0);

                __m128i b1g1r1 = _mm_add_epi32(sumLow01, y1);
                b1g1r1 = _mm_add_epi32(b1g1r1, alphaRound);
                b1g1r1 = _mm_srai_epi32(b1g1r1, SHIFT);

                // 32 bits each (from lsb) { Y2, U1, Y3, V1 }
                __m128i b = _mm_unpackhi_epi16(yuvLow, _mm_setzero_si128());
                b = _mm_sub_epi32(b, OFFSET);

                __m128i u2u3 = _mm_shuffle_epi32(b, _MM_SHUFFLE(1, 1, 1, 1));
                __m128i v2v3 = _mm_shuffle_epi32(b, _MM_SHUFFLE(3, 3, 3, 3));

                u2u3 = _mm_mullo_epi32(u2u3, CSC_MATRIX_x_1);
                v2v3 = _mm_mullo_epi32(v2v3, CSC_MATRIX_x_2);

                __m128i sumLow23 = _mm_add_epi32(u2u3, v2v3);

                // ---

                __m128i y2 = _mm_shuffle_epi32(b, _MM_SHUFFLE(0, 0, 0, 0));

                // b - g - r - 0
                y2 = _mm_mullo_epi32(y2, CSC_MATRIX_x_0);

                __m128i b2g2r2 = _mm_add_epi32(sumLow23, y2);
                b2g2r2 = _mm_add_epi32(b2g2r2, alphaRound);
                b2g2r2 = _mm_srai_epi32(b2g2r2, SHIFT);

                __m128i y3 = _mm_shuffle_epi32(b, _MM_SHUFFLE(2, 2, 2, 2));

                // b - g - r - 0
                y3 = _mm_mullo_epi32(y3, CSC_MATRIX_x_0);

                __m128i b3g3r3 = _mm_add_epi32(sumLow23, y3);
                b3g3r3 = _mm_add_epi32(b3g3r3, alphaRound);
                b3g3r3 = _mm_srai_epi32(b3g3r3, SHIFT);


                __m128i b0g0r0b1g1r1 = _mm_packus_epi32(r0g0b0, b1g1r1);
                __m128i b2g2r2b3g3r3 = _mm_packus_epi32(b2g2r2, b3g3r3);

                __m128i b0g0r0b1g1r1b2g2r2b3g3r3 = _mm_packus_epi16(b0g0r0b1g1r1, b2g2r2b3g3r3);

#if USE_STREAMING_STORE
                _mm_stream_si128(pRowStartOut, b0g0r0b1g1r1b2g2r2b3g3r3);
#else
                _mm_store_si128(pRowStartOut, b0g0r0b1g1r1b2g2r2b3g3r3);
#endif

                // high

                // 16 bits each (from lsb) {  Y4, U2, Y5, V2,  Y6, U3, Y7, V3 }
                __m128i yuvHigh = _mm_unpackhi_epi8(yuv, _mm_setzero_si128());

                // 32 bits each (from lsb) { Y4, U2, Y5, V2 }
                __m128i c = _mm_unpacklo_epi16(yuvHigh, _mm_setzero_si128());
                c = _mm_sub_epi32(c, OFFSET);

                __m128i u4u5 = _mm_shuffle_epi32(c, _MM_SHUFFLE(1, 1, 1, 1));
                __m128i v4v5 = _mm_shuffle_epi32(c, _MM_SHUFFLE(3, 3, 3, 3));

                u4u5 = _mm_mullo_epi32(u4u5, CSC_MATRIX_x_1);
                v4v5 = _mm_mullo_epi32(v4v5, CSC_MATRIX_x_2);

                __m128i sumHigh45 = _mm_add_epi32(u4u5, v4v5);

                // b - g - r - 0
                __m128i y4 = _mm_shuffle_epi32(c, _MM_SHUFFLE(0, 0, 0, 0));
                y4 = _mm_mullo_epi32(y4, CSC_MATRIX_x_0);

                __m128i b4g4r4 = _mm_add_epi32(sumHigh45, y4);
                b4g4r4 = _mm_add_epi32(b4g4r4, alphaRound);
                b4g4r4 = _mm_srai_epi32(b4g4r4, SHIFT);

                // ---

                // b - g - r - 0
                __m128i y5 = _mm_shuffle_epi32(c, _MM_SHUFFLE(2, 2, 2, 2));
                y5 = _mm_mullo_epi32(y5, CSC_MATRIX_x_0);

                __m128i b5g5r5 = _mm_add_epi32(sumHigh45, y5);
                b5g5r5 = _mm_add_epi32(b5g5r5, alphaRound);
                b5g5r5 = _mm_srai_epi32(b5g5r5, SHIFT);

                // 32 bits each (from lsb) { Y6, U3, Y7, V3 }
                __m128i d = _mm_unpackhi_epi16(yuvHigh, _mm_setzero_si128());
                d = _mm_sub_epi32(d, OFFSET);

                // ---

                __m128i u6u7 = _mm_shuffle_epi32(d, _MM_SHUFFLE(1, 1, 1, 1));
                __m128i v6v7 = _mm_shuffle_epi32(d, _MM_SHUFFLE(3, 3, 3, 3));

                u6u7 = _mm_mullo_epi32(u6u7, CSC_MATRIX_x_1);
                v6v7 = _mm_mullo_epi32(v6v7, CSC_MATRIX_x_2);

                __m128i sumHigh67 = _mm_add_epi32(u6u7, v6v7);

                // b - g - r - 0
                __m128i y6 = _mm_shuffle_epi32(d, _MM_SHUFFLE(0, 0, 0, 0));
                y6 = _mm_mullo_epi32(y6, CSC_MATRIX_x_0);

                __m128i b6g6r6 = _mm_add_epi32(sumHigh67, y6);
                b6g6r6 = _mm_add_epi32(b6g6r6, alphaRound);
                b6g6r6 = _mm_srai_epi32(b6g6r6, SHIFT);


                // b - g - r - 0
                __m128i y7 = _mm_shuffle_epi32(d, _MM_SHUFFLE(2, 2, 2, 2));
                y7 = _mm_mullo_epi32(y7, CSC_MATRIX_x_0);

                __m128i b7g7r7 = _mm_add_epi32(sumHigh67, y7);
                b7g7r7 = _mm_add_epi32(b7g7r7, alphaRound);
                b7g7r7 = _mm_srai_epi32(b7g7r7, SHIFT);

                // combine to form 16-bits entities

                __m128i b4g4r4b5g5r5 = _mm_packus_epi32(b4g4r4, b5g5r5);
                __m128i b6g6r6b7g7r7 = _mm_packus_epi32(b6g6r6, b7g7r7);

                // combine to 8 bits entities

                __m128i b4g4r4b5g5r5b6g6r6b7g7r7 = _mm_packus_epi16(b4g4r4b5g5r5, b6g6r6b7g7r7);

#if USE_STREAMING_STORE
                _mm_stream_si128(pRowStartOut + 1, b4g4r4b5g5r5b6g6r6b7g7r7);
#else
                _mm_store_si128(pRowStartOut + 1, b4g4r4b5g5r5b6g6r6b7g7r7);
#endif
            }
        }
    }

    /// <summary>
    /// YUV to RGB
    /// </summary>
    /// <remark>original</remark>
    /// <remark>May optionaly use OpenMP</ramerk>
    /// <param name="_pInData"></param>
    /// <param name="_pOutData"></param>
    /// <param name="imageSize"></param>
    /// <param name="swapOutput"></param>
    void YVU422ToRGB_SSE_i(
        unsigned short* _pInData,
        unsigned char* _pOutData,
        const unsigned int width,
        const unsigned int height,
        const unsigned int inputStride,
        const unsigned int outputStride,
        const bool swapOutput
    )
    {
        unsigned int imageSize = width * height;

        const __m128i* pInData = (__m128i*)_pInData;
        __m128i* pOutData = (__m128i*)_pOutData;

        __m128i CSC_MATRIX_x_0;
        __m128i CSC_MATRIX_x_1;
        __m128i CSC_MATRIX_x_2;

        const __m128i OFFSET = _mm_set_epi32(0, 128, 0, 128);

        // TODO: Handle swapped output
        if (swapOutput)
        {
            // Shuffle output data to match required output format

            CSC_MATRIX_x_0 = _mm_set_epi32(0, CSC_MATRIX_2_0, CSC_MATRIX_1_0, CSC_MATRIX_0_0);
            CSC_MATRIX_x_1 = _mm_set_epi32(0, CSC_MATRIX_2_1, CSC_MATRIX_1_1, CSC_MATRIX_0_1);
            CSC_MATRIX_x_2 = _mm_set_epi32(0, CSC_MATRIX_2_2, CSC_MATRIX_1_2, CSC_MATRIX_0_2);
        }
        else
        {
            // No shuffle output data

            CSC_MATRIX_x_0 = _mm_set_epi32(0, CSC_MATRIX_0_0, CSC_MATRIX_1_0, CSC_MATRIX_2_0);
            CSC_MATRIX_x_1 = _mm_set_epi32(0, CSC_MATRIX_0_1, CSC_MATRIX_1_1, CSC_MATRIX_2_1);
            CSC_MATRIX_x_2 = _mm_set_epi32(0, CSC_MATRIX_0_2, CSC_MATRIX_1_2, CSC_MATRIX_2_2);
        }

        const __m128i alphaRound = _mm_set_epi32(InuCommon::ALPHA_TRANSPARENT << SHIFT, ROUND, ROUND, ROUND);

        // 8 16-bit items, each representing 2 input pixels
        constexpr int SF{ sizeof(__m128i) / sizeof(_pInData[0]) };

#pragma omp parallel for if(omp)
        for (int row = 0; row < int(height); row++)
        {
            const int ii0 = long(row) * inputStride / (SF * 2);
            const __m128i* pRowStartIn = &pInData[ii0];

            const int oi0 = long(row) * outputStride / (SF * 2);
            __m128i* pRowStartOut = &pOutData[oi0];

            for (int column = 0; column < int(width / SF); column++, pRowStartOut += 2)
            {
                // 16 values, 8 bits each
                // { V0, Y0, U0, Y1, V1, Y2, U1, Y3, V2, Y4, U2, Y5, V3, Y6, U3, Y7 }
                __m128i yuv = _mm_loadu_si128(&pRowStartIn[column]);


                // { V0, Y0, U0, Y1, V1, Y2, U1, Y3 } -- 16 bits each
                __m128i yuvLow = _mm_unpacklo_epi8(yuv, _mm_setzero_si128());

                // low

                  // { V0, Y0, U0, Y1 } -- 32 bits each
                __m128i a = _mm_unpacklo_epi16(yuvLow, _mm_setzero_si128());
                a = _mm_sub_epi32(a, OFFSET);

                __m128i u0u1 = _mm_shuffle_epi32(a, _MM_SHUFFLE(2, 2, 2, 2));
                __m128i v0v1 = _mm_shuffle_epi32(a, _MM_SHUFFLE(0, 0, 0, 0));

                u0u1 = _mm_mullo_epi32(u0u1, CSC_MATRIX_x_1);
                v0v1 = _mm_mullo_epi32(v0v1, CSC_MATRIX_x_2);

                __m128i sumLow01 = _mm_add_epi32(u0u1, v0v1);

                // b - g - r - 0
                __m128i y0 = _mm_shuffle_epi32(a, _MM_SHUFFLE(1, 1, 1, 1));
                y0 = _mm_mullo_epi32(y0, CSC_MATRIX_x_0);

                __m128i b0g0r0 = _mm_add_epi32(sumLow01, y0);
                b0g0r0 = _mm_add_epi32(b0g0r0, alphaRound);
                b0g0r0 = _mm_srai_epi32(b0g0r0, SHIFT);

                // b - g - r - 0
                __m128i y1 = _mm_shuffle_epi32(a, _MM_SHUFFLE(3, 3, 3, 3));
                y1 = _mm_mullo_epi32(y1, CSC_MATRIX_x_0);

                __m128i b1g1r1 = _mm_add_epi32(sumLow01, y1);
                b1g1r1 = _mm_add_epi32(b1g1r1, alphaRound);
                b1g1r1 = _mm_srai_epi32(b1g1r1, SHIFT);

                // { V1, Y3, U1, Y2 } -- 32 bith each
                __m128i b = _mm_unpackhi_epi16(yuvLow, _mm_setzero_si128());
                b = _mm_sub_epi32(b, OFFSET);

                __m128i u2u3 = _mm_shuffle_epi32(b, _MM_SHUFFLE(2, 2, 2, 2));
                __m128i v2v3 = _mm_shuffle_epi32(b, _MM_SHUFFLE(0, 0, 0, 0));

                u2u3 = _mm_mullo_epi32(u2u3, CSC_MATRIX_x_1);
                v2v3 = _mm_mullo_epi32(v2v3, CSC_MATRIX_x_2);

                __m128i sumLow23 = _mm_add_epi32(u2u3, v2v3);

                // ---

                __m128i y2 = _mm_shuffle_epi32(b, _MM_SHUFFLE(1, 1, 1, 1));

                // b - g - r - 0
                y2 = _mm_mullo_epi32(y2, CSC_MATRIX_x_0);

                __m128i b2g2r2 = _mm_add_epi32(sumLow23, y2);
                b2g2r2 = _mm_add_epi32(b2g2r2, alphaRound);
                b2g2r2 = _mm_srai_epi32(b2g2r2, SHIFT);

                __m128i y3 = _mm_shuffle_epi32(b, _MM_SHUFFLE(3, 3, 3, 3));

                // b - g - r - 0
                y3 = _mm_mullo_epi32(y3, CSC_MATRIX_x_0);

                __m128i b3g3r3 = _mm_add_epi32(sumLow23, y3);
                b3g3r3 = _mm_add_epi32(b3g3r3, alphaRound);
                b3g3r3 = _mm_srai_epi32(b3g3r3, SHIFT);


                __m128i b0g0r0b1g1r1 = _mm_packus_epi32(b1g1r1, b0g0r0);
                __m128i b2g2r2b3g3r3 = _mm_packus_epi32(b3g3r3, b2g2r2);

                __m128i b0g0r0b1g1r1b2g2r2b3g3r3 = _mm_packus_epi16(b0g0r0b1g1r1, b2g2r2b3g3r3);

#if USE_STREAMING_STORE
                _mm_stream_si128(pRowStartOut, b0g0r0b1g1r1b2g2r2b3g3r3);
#else
                _mm_store_si128(pRowStartOut, b0g0r0b1g1r1b2g2r2b3g3r3);
#endif

                // high

                // { V2, Y4, U2, Y5, V3, Y7, U3, Y7 } -- 16 bits each
                __m128i yuvHigh = _mm_unpackhi_epi8(yuv, _mm_setzero_si128());

                // { V2, Y5, U2, Y4 } -- 32 bits each
                __m128i c = _mm_unpacklo_epi16(yuvHigh, _mm_setzero_si128());
                c = _mm_sub_epi32(c, OFFSET);

                __m128i u4u5 = _mm_shuffle_epi32(c, _MM_SHUFFLE(2, 2, 2, 2));
                __m128i v4v5 = _mm_shuffle_epi32(c, _MM_SHUFFLE(0, 0, 0, 0));

                u4u5 = _mm_mullo_epi32(u4u5, CSC_MATRIX_x_1);
                v4v5 = _mm_mullo_epi32(v4v5, CSC_MATRIX_x_2);

                __m128i sumHigh45 = _mm_add_epi32(u4u5, v4v5);

                // b - g - r - 0
                __m128i y4 = _mm_shuffle_epi32(c, _MM_SHUFFLE(1, 1, 1, 1));
                y4 = _mm_mullo_epi32(y4, CSC_MATRIX_x_0);

                __m128i b4g4r4 = _mm_add_epi32(sumHigh45, y4);
                b4g4r4 = _mm_add_epi32(b4g4r4, alphaRound);
                b4g4r4 = _mm_srai_epi32(b4g4r4, SHIFT);

                // ---

                // b - g - r - 0
                __m128i y5 = _mm_shuffle_epi32(c, _MM_SHUFFLE(3, 3, 3, 3));
                y5 = _mm_mullo_epi32(y5, CSC_MATRIX_x_0);

                __m128i b5g5r5 = _mm_add_epi32(sumHigh45, y5);
                b5g5r5 = _mm_add_epi32(b5g5r5, alphaRound);
                b5g5r5 = _mm_srai_epi32(b5g5r5, SHIFT);

                // { V3, Y7, U3, Y6 } -- 32 bith each
                __m128i d = _mm_unpackhi_epi16(yuvHigh, _mm_setzero_si128());
                d = _mm_sub_epi32(d, OFFSET);

                // ---

                __m128i u6u7 = _mm_shuffle_epi32(d, _MM_SHUFFLE(2, 2, 2, 2));
                __m128i v6v7 = _mm_shuffle_epi32(d, _MM_SHUFFLE(0, 0, 0, 0));

                u6u7 = _mm_mullo_epi32(u6u7, CSC_MATRIX_x_1);
                v6v7 = _mm_mullo_epi32(v6v7, CSC_MATRIX_x_2);

                __m128i sumHigh67 = _mm_add_epi32(u6u7, v6v7);

                // b - g - r - 0
                __m128i y6 = _mm_shuffle_epi32(d, _MM_SHUFFLE(1, 1, 1, 1));
                y6 = _mm_mullo_epi32(y6, CSC_MATRIX_x_0);

                __m128i b6g6r6 = _mm_add_epi32(sumHigh67, y6);
                b6g6r6 = _mm_add_epi32(b6g6r6, alphaRound);
                b6g6r6 = _mm_srai_epi32(b6g6r6, SHIFT);


                // b - g - r - 0
                __m128i y7 = _mm_shuffle_epi32(d, _MM_SHUFFLE(3, 3, 3, 3));
                y7 = _mm_mullo_epi32(y7, CSC_MATRIX_x_0);

                __m128i b7g7r7 = _mm_add_epi32(sumHigh67, y7);
                b7g7r7 = _mm_add_epi32(b7g7r7, alphaRound);
                b7g7r7 = _mm_srai_epi32(b7g7r7, SHIFT);

                // combine to form 16-bits entities

                __m128i b4g4r4b5g5r5 = _mm_packus_epi32(b5g5r5, b4g4r4);
                __m128i b6g6r6b7g7r7 = _mm_packus_epi32(b7g7r7, b6g6r6);

                // combine to 8 bits entities

                __m128i b4g4r4b5g5r5b6g6r6b7g7r7 = _mm_packus_epi16(b4g4r4b5g5r5, b6g6r6b7g7r7);

#if USE_STREAMING_STORE
                _mm_stream_si128(pRowStartOut + 1, b4g4r4b5g5r5b6g6r6b7g7r7);
#else
                _mm_store_si128(pRowStartOut + 1, b4g4r4b5g5r5b6g6r6b7g7r7);
#endif
            }
        }
#endif
    }

    COMMONALGO_API bool YUV422ToRGB_AlgoExt(
        unsigned char* oFirstBuffer,
        unsigned char* oSecondBuffer,

        unsigned char *iYImage,

        unsigned int iHeight,
        unsigned int iWidth,
        unsigned int iInputStride,

        int iOptimizationLevel,

        bool swapOutput,
        bool iUBeforeV,
        bool i16bpp,    // if false then 24 bpp

        bool _omp  // use OpenMP
    )
    {
        omp = _omp;

        int family = iOptimizationLevel / 10;
        InuCommon::COsUtilities::EOptimizationLevel optimizationLevel =
            (InuCommon::COsUtilities::EOptimizationLevel)(iOptimizationLevel % 10);

        // A workaround for invocation for interleaved input image.
        // InuService passes '0' as stride, alweys...
        // Adjust the iInputStride, which will also be used as
        // output stride (only YUV422ToRGB_SSE_i/YVU422ToRGB_SSE_i
        // use input or output strides)

        // TODO: introduce/use "OutputStride"

        if (!iInputStride)
        {
            iInputStride = iWidth * 2 * (oSecondBuffer ? 2 : 1);
        }

        // 99   YUV422ToRGB_Naive_i_Old
        // 98   YUV422ToRGB_Naive_i
        // 97   YUV422ToRGB_Naive_f
        // 96   YUV422ToRGB_SSE_f
        // 95   YUV422ToRGB_SSE_i

        switch (family)
        {
        case 0: // default

            switch (optimizationLevel)
            {
            case InuCommon::COsUtilities::eNone:

#if SSE_FP
                iOptimizationLevel = 97; // YUV422ToRGB_Naive_f
#else
#if OLD_IMPLEMENTATION
                iOptimizationLevel = 99; // YUV422ToRGB_Naive_i_Old
#else
                iOptimizationLevel = 98; // YUV422ToRGB_Naive_i
#endif
#endif

                break;

            case InuCommon::COsUtilities::eSSE:

#if !defined(__arm__) || defined(__ARM_NEON__)
#if SSE_FP
                iOptimizationLevel = 96;
#else
                iOptimizationLevel = 95; // YUV422ToRGB_SSE_i / YVU422ToRGB_SSE_i
#endif
#endif
                break;

            case InuCommon::COsUtilities::eAVX:

                break;

            case InuCommon::COsUtilities::eNumberOfOptimizationLevels:

                return false;
            }

            break;

        case 1: // integer

            switch (optimizationLevel)
            {
            case InuCommon::COsUtilities::eNone:

                iOptimizationLevel = 98; // YUV422ToRGB_Naive_i
                break;

            case InuCommon::COsUtilities::eSSE:
#if !defined(__arm__) || defined(__ARM_NEON__)
                iOptimizationLevel = 95; // YUV422ToRGB_SSE_i / YVU422ToRGB_SSE_i
#endif
                break;

            case InuCommon::COsUtilities::eAVX:

                break;

            case InuCommon::COsUtilities::eNumberOfOptimizationLevels:

                return false;
            }

            break;

        case 2: // float

            switch (optimizationLevel)
            {
            case InuCommon::COsUtilities::eNone:

                iOptimizationLevel = 97; // YUV422ToRGB_Naive_f
                break;

            case InuCommon::COsUtilities::eSSE:
#if !defined(__arm__) || defined(__ARM_NEON__)
                iOptimizationLevel = 96; // YUV422ToRGB_SSE_f
#endif
                break;

            case InuCommon::COsUtilities::eAVX:

                break;

            case InuCommon::COsUtilities::eNumberOfOptimizationLevels:

                return false;
            }

            break;
        }

        //

        iOptimizationLevel = i16bpp ? iOptimizationLevel : 100;

        switch (iOptimizationLevel)
        {
        case 100:

            if (!iUBeforeV)
            {
                return false;;
            }

            YUV422ToRGB_24bpp_Naive_i(
                oFirstBuffer,
                oSecondBuffer,
                iYImage,
                iWidth,
                iHeight,
                iInputStride,
                swapOutput,
                iUBeforeV
            );

            return true;


#if OLD_IMPLEMENTATION
        case 99:

           YUV422ToRGB_Naive_i_Old(
               oFirstBuffer,
               iYImage,
               iWidth,
               iHeight,
               iInputStride,
               swapOutput
           );

           return true;
#endif

        case 98:

            YUV422ToRGB_Naive_i(
                oFirstBuffer,
                oSecondBuffer,
                reinterpret_cast<unsigned short*>(iYImage),
                iWidth,
                iHeight,
                iInputStride,
                swapOutput,
                iUBeforeV
            );

            return true;

        case 97:

            if (!iUBeforeV)
            {
                return false; // unsupported!
            }

            YUV422ToRGB_Naive_f(
                oFirstBuffer,
                reinterpret_cast<unsigned short*>(iYImage),
                iWidth,
                iHeight,
                iInputStride,
                swapOutput,
                iUBeforeV
            );

            return true;

#if !defined(__arm__) || defined(__ARM_NEON__)

        case 96:

            if (!InuCommon::COsUtilities::SSE4IsSupported(iWidth * sizeof(unsigned short), optimizationLevel))
            {
                //std::cout << "SSE Unsupported" << std::endl;
                return false;
            }

            if (!iUBeforeV)
            {
                return false; // unsupported!
            }

            YUV422ToRGB_SSE_f(
                reinterpret_cast<unsigned short*>(iYImage),
                oFirstBuffer,
                iWidth,
                iHeight,
                iInputStride,
                swapOutput
            );

            return true;

        case 95:

            if (!InuCommon::COsUtilities::SSE4IsSupported(iWidth * sizeof(unsigned short), optimizationLevel))
            {
                return false; // unsupported!
            }

            if (oSecondBuffer)
            {
                // input is interleaved - adjust parameter accordingly.

                if (iUBeforeV)
                {
                    // swap input

                    // inputStride is used for outputStride parameter here.
                    // As this is the case for interleaved input - the output
                    // image width will equal to iWidth 
                    //
                    // Overall, the same value results.

                    YUV422ToRGB_SSE_i(
                        reinterpret_cast<unsigned short*>(iYImage),
                        oFirstBuffer,
                        iWidth,
                        iHeight,
                        iInputStride,
                        iInputStride,
                        swapOutput
                    );

                    YUV422ToRGB_SSE_i(
                        reinterpret_cast<unsigned short*>(iYImage) + iWidth,
                        oSecondBuffer,
                        iWidth,
                        iHeight,
                        iInputStride,
                        iInputStride,
                        swapOutput
                    );
                }
                else
                {
                    // don't swap input

                    YVU422ToRGB_SSE_i(
                        reinterpret_cast<unsigned short*>(iYImage),
                        oFirstBuffer,
                        iWidth,
                        iHeight,
                        iInputStride,
                        iInputStride,
                        swapOutput
                    );

                    YVU422ToRGB_SSE_i(
                        reinterpret_cast<unsigned short*>(iYImage) + iWidth,
                        oSecondBuffer,
                        iWidth,
                        iHeight,
                        iInputStride,
                        iInputStride,
                        swapOutput
                    );
                }
            }
            else
            {
                // input is not interleaved

                // outputStride is calcalated as inputSeride*2, as the whole
                // width of the input image will be used, yielding outputImage
                // the is twice the size - hence double the stride.

                if (iUBeforeV)
                {
                    // swap input
                    YUV422ToRGB_SSE_i(
                        reinterpret_cast<unsigned short*>(iYImage),
                        oFirstBuffer,
                        iWidth,
                        iHeight,
                        iInputStride,
                        iInputStride * 2,
                        swapOutput
                    );
                }
                else
                {
                    // don't swap input
                    YVU422ToRGB_SSE_i(
                        reinterpret_cast<unsigned short*>(iYImage),
                        oFirstBuffer,
                        iWidth,
                        iHeight,
                        iInputStride,
                        iInputStride * 2,
                        swapOutput
                    );
                }
            }

            return true;

        case InuCommon::COsUtilities::eAVX:

            if (!InuCommon::COsUtilities::AVXIsSupported(iWidth * sizeof(unsigned short), optimizationLevel))
            {
                return false;  // unsupported!
            }

            // TODO: AVX

            return false;
#endif
        }

        std::cout << "Error: Unknown or Unsupported optimization level " << iOptimizationLevel << std::endl;
        return false;
    }
}
