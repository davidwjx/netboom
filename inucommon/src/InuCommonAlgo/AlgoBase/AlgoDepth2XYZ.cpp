#define USE_SHORTCUTS                                       (1)

#define LOG_PARAMETERS                                      (0)

#if LOG_PARAMETERS
#ifdef __ANDROID__
#include <android/log.h>
#else
#endif
#endif

#include "AlgoBase.h"

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

#include <algorithm>
#include <fstream>
#include <iomanip>

#ifndef NO_BASIC_ALGO
#include <opencv2/opencv.hpp>
#endif

#undef min
#undef max

int InuCommon::CAlgoBase::Depth2XYZ_SSE(
    const unsigned short *  inDepthMap,
    unsigned int            width,
    unsigned int            height,
    float                   f_x,
    float                   f_y,
    int                     opt_cen_x,
    int                     opt_cen_y,
    float                   decimation,
    unsigned int            minZ,
    unsigned int            maxZ,
    float*                  outXYZMat,
    float                   iScale,
    int                     iPointCloudZAxis,
    int                     iPointCloudYAxis,
    bool                    iWithConfidence,
    InuCommon::COsUtilities::EOptimizationLevel    iRequestedLevel
)
{
    const int x0 = opt_cen_x;
    const int y0 = opt_cen_y;
    int countVertex = 0;
    /* float fB = f*B; */
    float *p = outXYZMat;

#if defined(__ARM_NEON__) || defined(__aarch64__) || defined(_MSC_VER) || defined(__SSE2__)

    const unsigned short * pPixel = inDepthMap;

    float reciprocalFocalX = 1.0f / f_x;
    float reciprocalFocalY = 1.0f / f_y;

    const unsigned short CONFIDENCE_MASK = (1u << sNumOfDepthConfidenenceBits) - 1;

    constexpr unsigned int smallStep = sizeof(__m128i) / sizeof(unsigned short);

    __m128i *inDepthMap128 = (__m128i *)inDepthMap;
#if defined(__aarch64__)
    float32x4_t *p128 = (float32x4_t *)p;
#else
    __m128 *p128 = (__m128 *)p;
#endif

    const __m128i maxZ128 = _mm_set1_epi16(maxZ);
    const __m128i minZ128 = _mm_set1_epi16(minZ);
    const __m128 reciprocalFocalX128 = _mm_set1_ps(reciprocalFocalX);
    const __m128 reciprocalFocalY128 = _mm_set1_ps(reciprocalFocalY);

    const __m128 x0_128 = _mm_set1_ps(float(x0));
    const __m128 y0_128 = _mm_set1_ps(float(y0));

    static const __m128i ZERO = _mm_set1_epi16(0);

    __m128 pointCloudZAxis128 = _mm_set1_ps(iPointCloudZAxis * iScale);

    if (iWithConfidence)
    {
        const __m128i confidenceMask128 = _mm_set1_epi16(CONFIDENCE_MASK);

        for (unsigned int y = 0; y < height; y++)
        {
            __m128 yyy = _mm_set1_ps(float(int(y) - y0) * iPointCloudYAxis / iPointCloudZAxis);
            __m128 xStep128 = _mm_set_ps(3.0f, 2.0f, 1.0f, 0.0f);

            for (unsigned int x = 0; x < width; x += smallStep)
            {
                __m128i depthAndConfidence128 = _mm_loadu_si128(inDepthMap128++);

                __m128i depth128 = _mm_srli_epi16(depthAndConfidence128, sNumOfDepthConfidenenceBits);

                __m128i t1 = _mm_cmplt_epi16(minZ128, depth128);
                __m128i t2 = _mm_cmpgt_epi16(maxZ128, depth128);
                __m128i res128 = _mm_and_si128(t1, t2);

#if USE_SHORTCUTS
                // optimization: if 'res128' is all 0, nothing will be written. Skip...
                if (_mm_test_all_zeros(res128, res128))
                {
                    xStep128 = _mm_add_ps(xStep128, _mm_set1_ps(4 * 2));
                    continue;
                }
#endif

#if defined(__aarch64__)
                register float32x4_t x128 asm("v0");
                register float32x4_t y128 asm("v1");
                register float32x4_t fInput asm("v2");
                register float32x4_t p3 asm("v3");
#else
                __m128 x128;
                __m128 y128;
                __m128 fInput;
                __m128 p3;
#endif
                __m128 zcoeffX128;
                __m128 zcoeffY128;

                __m128i confidence128 = _mm_and_si128(depthAndConfidence128, confidenceMask128);

#if USE_SHORTCUTS
                // optimization: if 'res128' is all 1, all calculations will be used, and we can skip testing.
                if (_mm_test_all_ones(res128))
                {
                    // low

#if defined(__aarch64__)
                    asm volatile (
                            "sshll %[OUT].4s,%[VAL].4h,#0\n\t"
                            "scvtf %[OUT].4s,%[OUT].4s\n\t"
                            : // output
                            [OUT]"+w" (fInput)
                            : // inputs
                            [VAL]"w" (depth128)
                            : // clobber list
                        );
#else
                    fInput = _mm_cvtepi32_ps(_mm_unpacklo_epi16(depth128, ZERO));
#endif
                    fInput = _mm_mul_ps(fInput, pointCloudZAxis128);

                    zcoeffX128 = _mm_mul_ps(fInput, reciprocalFocalX128);
                    zcoeffY128 = _mm_mul_ps(fInput, reciprocalFocalY128);

                    x128 = _mm_mul_ps(_mm_sub_ps(xStep128, x0_128), zcoeffX128);
                    y128 = _mm_mul_ps(yyy, zcoeffY128);

#if defined(__aarch64__)
                    asm volatile (
                            "sshll %[OUT].4s,%[VAL].4h,#0\n\t"
                            "scvtf %[OUT].4s,%[OUT].4s\n\t"
                            : // output
                            [OUT]"+w" (p3)
                            : // inputs
                            [VAL]"w" (confidence128)
                            : // clobber list
                        );
#else
                    p3 = _mm_cvtepi32_ps(_mm_unpacklo_epi16(confidence128, ZERO));
#endif

#if defined(__aarch64__)
                    asm volatile (
                            "st4 {%[SRC_X].s, %[SRC_Y].s, %[SRC_F].s, %[SRC_P].s}[0], [%[DST]], #16\n\t"
                            "st4 {%[SRC_X].s, %[SRC_Y].s, %[SRC_F].s, %[SRC_P].s}[1], [%[DST]], #16\n\t"
                            "st4 {%[SRC_X].s, %[SRC_Y].s, %[SRC_F].s, %[SRC_P].s}[2], [%[DST]], #16\n\t"
                            "st4 {%[SRC_X].s, %[SRC_Y].s, %[SRC_F].s, %[SRC_P].s}[3], [%[DST]], #16\n\t"
                            // output operands
                            : [DST]"+r" (p128)
                            // input operands
                            : [SRC_X]"w" (x128), [SRC_Y]"w" (y128), [SRC_F]"w" (fInput), [SRC_P]"w" (p3)
                            // list of clobbered registers
                            : "memory"
                        );
#else
                    _MM_TRANSPOSE4_PS(x128, y128, fInput, p3);

                    _mm_store_ps((float *)p128++, x128);
                    _mm_store_ps((float *)p128++, y128);
                    _mm_store_ps((float *)p128++, fInput);
                    _mm_store_ps((float *)p128++, p3);
#endif
                    xStep128 = _mm_add_ps(xStep128, _mm_set1_ps(4));

                    // heigh

#if defined(__aarch64__)
                    asm volatile (
                            "sshll2 %[OUT].4s,%[VAL].8h,#0\n\t"
                            "scvtf %[OUT].4s,%[OUT].4s\n\t"
                            : // output
                            [OUT]"+w" (fInput)
                            : // inputs
                            [VAL]"w" (depth128)
                            : // clobber list
                        );
#else
                    fInput = _mm_cvtepi32_ps(_mm_unpackhi_epi16(depth128, ZERO));
#endif
                    fInput = _mm_mul_ps(fInput, pointCloudZAxis128);

                    zcoeffX128 = _mm_mul_ps(fInput, reciprocalFocalX128);
                    zcoeffY128 = _mm_mul_ps(fInput, reciprocalFocalY128);

                    x128 = _mm_mul_ps(_mm_sub_ps(xStep128, x0_128), zcoeffX128);
                    y128 = _mm_mul_ps(yyy, zcoeffY128);

#if defined(__aarch64__)
                    asm volatile (
                            "sshll2 %[OUT].4s,%[VAL].8h,#0\n\t"
                            "scvtf %[OUT].4s,%[OUT].4s\n\t"
                            : // output
                            [OUT]"+w" (p3)
                            : // inputs
                            [VAL]"w" (confidence128)
                            : // clobber list
                        );
#else
                    p3 = _mm_cvtepi32_ps(_mm_unpackhi_epi16(confidence128, ZERO));
#endif

#if defined(__aarch64__)
                    asm volatile (
                            "st4 {%[SRC_X].s, %[SRC_Y].s, %[SRC_F].s, %[SRC_P].s}[0], [%[DST]], #16\n\t"
                            "st4 {%[SRC_X].s, %[SRC_Y].s, %[SRC_F].s, %[SRC_P].s}[1], [%[DST]], #16\n\t"
                            "st4 {%[SRC_X].s, %[SRC_Y].s, %[SRC_F].s, %[SRC_P].s}[2], [%[DST]], #16\n\t"
                            "st4 {%[SRC_X].s, %[SRC_Y].s, %[SRC_F].s, %[SRC_P].s}[3], [%[DST]], #16\n\t"
                            // output operands
                            : [DST]"+r" (p128)
                            // input operands
                            : [SRC_X]"w" (x128), [SRC_Y]"w" (y128), [SRC_F]"w" (fInput), [SRC_P]"w" (p3)
                            // list of clobbered registers
                            : "memory"
                        );
#else
                    _MM_TRANSPOSE4_PS(x128, y128, fInput, p3);

                    _mm_store_ps((float *)p128++, x128);
                    _mm_store_ps((float *)p128++, y128);
                    _mm_store_ps((float *)p128++, fInput);
                    _mm_store_ps((float *)p128++, p3);
#endif

                    xStep128 = _mm_add_ps(xStep128, _mm_set1_ps(4));

                    continue;
                }
#endif
                // low

#if defined(__aarch64__)
                asm volatile (
                        "sshll %[OUT].4s,%[VAL].4h,#0\n\t"
                        "scvtf %[OUT].4s,%[OUT].4s\n\t"
                        : // output
                        [OUT]"+w" (fInput)
                        : // inputs
                        [VAL]"w" (depth128)
                        : // clobber list
                    );
#else
                fInput = _mm_cvtepi32_ps(_mm_unpacklo_epi16(depth128, ZERO));
#endif
                fInput = _mm_mul_ps(fInput, pointCloudZAxis128);

                zcoeffX128 = _mm_mul_ps(fInput, reciprocalFocalX128);
                zcoeffY128 = _mm_mul_ps(fInput, reciprocalFocalY128);

                x128 = _mm_mul_ps(_mm_sub_ps(xStep128, x0_128), zcoeffX128);
                y128 = _mm_mul_ps(yyy, zcoeffY128);

#if defined(__aarch64__)
                asm volatile (
                        "sshll %[OUT].4s,%[VAL].4h,#0\n\t"
                        "scvtf %[OUT].4s,%[OUT].4s\n\t"
                        : // output
                        [OUT]"+w" (p3)
                        : // inputs
                        [VAL]"w" (confidence128)
                        : // clobber list
                    );
#else
                p3 = _mm_cvtepi32_ps(_mm_unpacklo_epi16(confidence128, ZERO));
#endif

#if !defined(__aarch64__)
                _MM_TRANSPOSE4_PS(x128, y128, fInput, p3);
#endif

                uint64_t res = _mm_extract_epi64(res128, 0);

                if ((res >> 0) & 0xfffflu)
                {
#if defined(__aarch64__)
                    asm volatile (
                            "st4 {%[SRC_X].s, %[SRC_Y].s, %[SRC_F].s, %[SRC_P].s}[0], [%[DST]], #16\n\t"
                            // output operands
                            : [DST]"+r" (p128)
                            // input operands
                            : [SRC_X]"w" (x128), [SRC_Y]"w" (y128), [SRC_F]"w" (fInput), [SRC_P]"w" (p3)
                            // list of clobbered registers
                            : "memory"
                        );
#else
                    _mm_store_ps((float *)p128++, x128);
#endif
                }

                if ((res >> 16) & 0xfffflu)
                {
#if defined(__aarch64__)
                    asm volatile (
                            "st4 {%[SRC_X].s, %[SRC_Y].s, %[SRC_F].s, %[SRC_P].s}[1], [%[DST]], #16\n\t"
                            // output operands
                            : [DST]"+r" (p128)
                            // input operands
                            : [SRC_X]"w" (x128), [SRC_Y]"w" (y128), [SRC_F]"w" (fInput), [SRC_P]"w" (p3)
                            // list of clobbered registers
                            : "memory"
                        );
#else
                    _mm_store_ps((float *)p128++, y128);
#endif
                }

                if ((res >> 32) & 0xfffflu)
                {
#if defined(__aarch64__)
                    asm volatile (
                            "st4 {%[SRC_X].s, %[SRC_Y].s, %[SRC_F].s, %[SRC_P].s}[2], [%[DST]], #16\n\t"
                            // output operands
                            : [DST]"+r" (p128)
                            // input operands
                            : [SRC_X]"w" (x128), [SRC_Y]"w" (y128), [SRC_F]"w" (fInput), [SRC_P]"w" (p3)
                            // list of clobbered registers
                            : "memory"
                        );
#else
                    _mm_store_ps((float *)p128++, fInput);
#endif
                }

                if ((res >> 48) & 0xfffflu)
                {
#if defined(__aarch64__)
                    asm volatile (
                            "st4 {%[SRC_X].s, %[SRC_Y].s, %[SRC_F].s, %[SRC_P].s}[3], [%[DST]], #16\n\t"
                            // output operands
                            : [DST]"+r" (p128)
                            // input operands
                            : [SRC_X]"w" (x128), [SRC_Y]"w" (y128), [SRC_F]"w" (fInput), [SRC_P]"w" (p3)
                            // list of clobbered registers
                            : "memory"
                        );
#else
                    _mm_store_ps((float *)p128++, p3);
#endif
                }

                xStep128 = _mm_add_ps(xStep128, _mm_set1_ps(4));

                // heigh

#if defined(__aarch64__)
                asm volatile (
                        "sshll2 %[OUT].4s,%[VAL].8h,#0\n\t"
                        "scvtf %[OUT].4s,%[OUT].4s\n\t"
                        : // output
                        [OUT]"+w" (fInput)
                        : // inputs
                        [VAL]"w" (depth128)
                        : // clobber list
                    );
#else
                fInput = _mm_cvtepi32_ps(_mm_unpackhi_epi16(depth128, ZERO));
#endif
                fInput = _mm_mul_ps(fInput, pointCloudZAxis128);

                zcoeffX128 = _mm_mul_ps(fInput, reciprocalFocalX128);
                zcoeffY128 = _mm_mul_ps(fInput, reciprocalFocalY128);

                x128 = _mm_mul_ps(_mm_sub_ps( xStep128, x0_128), zcoeffX128);
                y128 = _mm_mul_ps(yyy, zcoeffY128);

#if defined(__aarch64__)
                asm volatile (
                        "sshll2 %[OUT].4s,%[VAL].8h,#0\n\t"
                        "scvtf %[OUT].4s,%[OUT].4s\n\t"
                        : // output
                        [OUT]"+w" (p3)
                        : // inputs
                        [VAL]"w" (confidence128)
                        : // clobber list
                    );
#else
                p3 = _mm_cvtepi32_ps(_mm_unpackhi_epi16(confidence128, ZERO));
#endif

#if !defined(__aarch64__)
                _MM_TRANSPOSE4_PS(x128, y128, fInput, p3);
#endif

                res = _mm_extract_epi64(res128, 1);

                if ((res >> 0) & 0xfffflu)
                {
#if defined(__aarch64__)
                    asm volatile (
                            "st4 {%[SRC_X].s, %[SRC_Y].s, %[SRC_F].s, %[SRC_P].s}[0], [%[DST]], #16\n\t"
                            // output operands
                            : [DST]"+r" (p128)
                            // input operands
                            : [SRC_X]"w" (x128), [SRC_Y]"w" (y128), [SRC_F]"w" (fInput), [SRC_P]"w" (p3)
                            // list of clobbered registers
                            : "memory"
                        );
#else
                    _mm_store_ps((float *)p128++, x128);
#endif
                }

                if ((res >> 16) & 0xfffflu)
                {
#if defined(__aarch64__)
                    asm volatile (
                            "st4 {%[SRC_X].s, %[SRC_Y].s, %[SRC_F].s, %[SRC_P].s}[1], [%[DST]], #16\n\t"
                            // output operands
                            : [DST]"+r" (p128)
                            // input operands
                            : [SRC_X]"w" (x128), [SRC_Y]"w" (y128), [SRC_F]"w" (fInput), [SRC_P]"w" (p3)
                            // list of clobbered registers
                            : "memory"
                        );
#else
                    _mm_store_ps((float *)p128++, y128);
#endif
                }

                if ((res >> 32) & 0xfffflu)
                {
#if defined(__aarch64__)
                    asm volatile (
                            "st4 {%[SRC_X].s, %[SRC_Y].s, %[SRC_F].s, %[SRC_P].s}[2], [%[DST]], #16\n\t"
                            // output operands
                            : [DST]"+r" (p128)
                            // input operands
                            : [SRC_X]"w" (x128), [SRC_Y]"w" (y128), [SRC_F]"w" (fInput), [SRC_P]"w" (p3)
                            // list of clobbered registers
                            : "memory"
                        );
#else
                    _mm_store_ps((float *)p128++, fInput);
#endif
                }

                if ((res >> 48) & 0xfffflu)
                {
#if defined(__aarch64__)
                    asm volatile (
                            "st4 {%[SRC_X].s, %[SRC_Y].s, %[SRC_F].s, %[SRC_P].s}[3], [%[DST]], #16\n\t"
                            // output operands
                            : [DST]"+r" (p128)
                            // input operands
                            : [SRC_X]"w" (x128), [SRC_Y]"w" (y128), [SRC_F]"w" (fInput), [SRC_P]"w" (p3)
                            // list of clobbered registers
                            : "memory"
                        );
#else
                    _mm_store_ps((float *)p128++, p3);
#endif
                }

                xStep128 = _mm_add_ps(xStep128, _mm_set1_ps(4));
            }
        }

        countVertex = (int)(((float *)p128 - outXYZMat) / 4); // 3 items per pixel
    }
    else
    {
        // no confidence vote

        __m128 p3 = _mm_set1_ps(0.0f);

        for (unsigned int y = 0; y < height; y++)
        {
            __m128 yyy = _mm_set1_ps(float(int(y) - y0) * iPointCloudYAxis / iPointCloudZAxis);
            __m128 xStep128 = _mm_set_ps(3.0f, 2.0f, 1.0f, 0.0f);

            for (unsigned int x = 0; x < width; x += smallStep)
            {
                __m128i depth128 = _mm_loadu_si128(inDepthMap128++);

                __m128i t1 = _mm_cmplt_epi16(minZ128, depth128);
                __m128i t2 = _mm_cmpgt_epi16(maxZ128, depth128);
                __m128i res128 = _mm_and_si128(t1, t2);

#if USE_SHORTCUTS
                // optimization: if 'res128' is all 0, nothing will be written. Skip...

                if (_mm_test_all_zeros(res128, res128))
                {
                    xStep128 = _mm_add_ps(xStep128, _mm_set1_ps(4 * 2));
                    continue;
                }
#endif

#if defined(__aarch64__)
                register float32x4_t x128 asm("v0");
                register float32x4_t y128 asm("v1");
                register float32x4_t fInput asm("v2");
#else
                __m128 x128;
                __m128 y128;
                __m128 fInput;
#endif
                __m128 zcoeffX128;
                __m128 zcoeffY128;

#if USE_SHORTCUTS
                // optimization: if 'res128' is all 1, all calculations will be used, and we can skip testing.
                if (_mm_test_all_ones(res128))
                {
                    // low

#if defined(__aarch64__)
                    asm volatile (
                            "sshll %[OUT].4s,%[VAL].4h,#0\n\t"
                            "scvtf %[OUT].4s,%[OUT].4s\n\t"
                            : // output
                            [OUT]"+w" (fInput)
                            : // inputs
                            [VAL]"w" (depth128)
                            : // clobber list
                        );
#else
                    fInput = _mm_cvtepi32_ps(_mm_unpacklo_epi16(depth128, ZERO));
#endif
                    fInput = _mm_mul_ps(fInput, pointCloudZAxis128);

                    zcoeffX128 = _mm_mul_ps(fInput, reciprocalFocalX128);
                    zcoeffY128 = _mm_mul_ps(fInput, reciprocalFocalY128);

                    x128 = _mm_mul_ps(_mm_sub_ps( xStep128, x0_128), zcoeffX128);
                    y128 = _mm_mul_ps(yyy, zcoeffY128);

#if defined(__aarch64__)
                    asm volatile (
                            "st3 {%[SRC_X].s, %[SRC_Y].s, %[SRC_F].s}[0], [%[DST]], #12\n\t"
                            "st3 {%[SRC_X].s, %[SRC_Y].s, %[SRC_F].s}[1], [%[DST]], #12\n\t"
                            "st3 {%[SRC_X].s, %[SRC_Y].s, %[SRC_F].s}[2], [%[DST]], #12\n\t"
                            "st3 {%[SRC_X].s, %[SRC_Y].s, %[SRC_F].s}[3], [%[DST]], #12\n\t"
                            // output operands
                            : [DST]"+r" (p128)
                            // input operands
                            : [SRC_X]"w" (x128), [SRC_Y]"w" (y128), [SRC_F]"w" (fInput), [SRC_P]"w" (p3)
                            // list of clobbered registers
                            : "memory"
                        );
#else
                    _MM_TRANSPOSE4_PS(x128, y128, fInput, p3);

                    _mm_storeu_ps(p + 0, x128);
                    _mm_storeu_ps(p + 3, y128);
                    _mm_storeu_ps(p + 6, fInput);
                    _mm_storeu_ps(p + 9, p3);
                    p += 4 * 3;
#endif
                    xStep128 = _mm_add_ps(xStep128, _mm_set1_ps(4));

                    // heigh

#if defined(__aarch64__)
                    asm volatile (
                            "sshll2 %[OUT].4s,%[VAL].8h,#0\n\t"
                            "scvtf %[OUT].4s,%[OUT].4s\n\t"
                            : // output
                            [OUT]"+w" (fInput)
                            : // inputs
                            [VAL]"w" (depth128)
                            : // clobber list
                        );
#else
                    fInput = _mm_cvtepi32_ps(_mm_unpackhi_epi16(depth128, ZERO));
#endif
                    fInput = _mm_mul_ps(fInput, pointCloudZAxis128);

                    zcoeffX128 = _mm_mul_ps(fInput, reciprocalFocalX128);
                    zcoeffY128 = _mm_mul_ps(fInput, reciprocalFocalY128);

                    x128 = _mm_mul_ps(_mm_sub_ps(xStep128, x0_128), zcoeffX128);
                    y128 = _mm_mul_ps(yyy, zcoeffY128);

#if defined(__aarch64__)
                    asm volatile (
                            "st3 {%[SRC_X].s, %[SRC_Y].s, %[SRC_F].s}[0], [%[DST]], #12\n\t"
                            "st3 {%[SRC_X].s, %[SRC_Y].s, %[SRC_F].s}[1], [%[DST]], #12\n\t"
                            "st3 {%[SRC_X].s, %[SRC_Y].s, %[SRC_F].s}[2], [%[DST]], #12\n\t"
                            "st3 {%[SRC_X].s, %[SRC_Y].s, %[SRC_F].s}[3], [%[DST]], #12\n\t"
                            // output operands
                            : [DST]"+r" (p128)
                            // input operands
                            : [SRC_X]"w" (x128), [SRC_Y]"w" (y128), [SRC_F]"w" (fInput), [SRC_P]"w" (p3)
                            // list of clobbered registers
                            : "memory"
                        );
#else
                    _MM_TRANSPOSE4_PS(x128, y128, fInput, p3);

                    _mm_storeu_ps(p + 0, x128);
                    _mm_storeu_ps(p + 3, y128);
                    _mm_storeu_ps(p + 6, fInput);
                    _mm_storeu_ps(p + 9, p3);
                    p += 4 * 3;
#endif
                    xStep128 = _mm_add_ps(xStep128, _mm_set1_ps(4));

                    continue;
                }
#endif

                // Low

#if defined(__aarch64__)
                asm volatile (
                        "sshll %[OUT].4s,%[VAL].4h,#0\n\t"
                        "scvtf %[OUT].4s,%[OUT].4s\n\t"
                        : // output
                        [OUT]"+w" (fInput)
                        : // inputs
                        [VAL]"w" (depth128)
                        : // clobber list
                    );
#else
                fInput = _mm_cvtepi32_ps(_mm_unpacklo_epi16(depth128, ZERO));
#endif
                fInput = _mm_mul_ps(fInput, pointCloudZAxis128);

                zcoeffX128 = _mm_mul_ps(fInput, reciprocalFocalX128);
                zcoeffY128 = _mm_mul_ps(fInput, reciprocalFocalY128);

                x128 = _mm_mul_ps(_mm_sub_ps(xStep128, x0_128), zcoeffX128);
                y128 = _mm_mul_ps(yyy, zcoeffY128);

#if !defined(__aarch64__)
                _MM_TRANSPOSE4_PS(x128, y128, fInput, p3);
#endif

                uint64_t res = _mm_extract_epi64(res128, 0);

                if ((res >> 0) & 0xfffflu)
                {
#if defined(__aarch64__)
                    asm volatile (
                            "st3 {%[SRC_X].s, %[SRC_Y].s, %[SRC_F].s}[0], [%[DST]], #12\n\t"
                            // output operands
                            : [DST]"+r" (p128)
                            // input operands
                            : [SRC_X]"w" (x128), [SRC_Y]"w" (y128), [SRC_F]"w" (fInput)
                            // list of clobbered registers
                            : "memory"
                        );
#else
                    _mm_storeu_ps(p, x128);
                    p += 3;
#endif
                }

                if ((res >> 16) & 0xfffflu)
                {
#if defined(__aarch64__)
                    asm volatile (
                            "st3 {%[SRC_X].s, %[SRC_Y].s, %[SRC_F].s}[1], [%[DST]], #12\n\t"
                            // output operands
                            : [DST]"+r" (p128)
                            // input operands
                            : [SRC_X]"w" (x128), [SRC_Y]"w" (y128), [SRC_F]"w" (fInput)
                            // list of clobbered registers
                            : "memory"
                        );
#else
                    _mm_storeu_ps(p, y128);
                    p += 3;
#endif
                }

                if ((res >> 32) & 0xfffflu)
                {
#if defined(__aarch64__)
                    asm volatile (
                            "st3 {%[SRC_X].s, %[SRC_Y].s, %[SRC_F].s}[2], [%[DST]], #12\n\t"
                            // output operands
                            : [DST]"+r" (p128)
                            // input operands
                            : [SRC_X]"w" (x128), [SRC_Y]"w" (y128), [SRC_F]"w" (fInput)
                            // list of clobbered registers
                            : "memory"
                        );
#else
                    _mm_storeu_ps(p, fInput);
                    p += 3;
#endif
                }

                if ((res >> 48) & 0xfffflu)
                {
#if defined(__aarch64__)
                    asm volatile (
                            "st3 {%[SRC_X].s, %[SRC_Y].s, %[SRC_F].s}[3], [%[DST]], #12\n\t"
                            // output operands
                            : [DST]"+r" (p128)
                            // input operands
                            : [SRC_X]"w" (x128), [SRC_Y]"w" (y128), [SRC_F]"w" (fInput)
                            // list of clobbered registers
                            : "memory"
                        );
#else
                    _mm_storeu_ps(p, p3);
                    p += 3;
#endif
                }

                xStep128 = _mm_add_ps(xStep128, _mm_set1_ps(4));

                // High

#if defined(__aarch64__)
                asm volatile (
                        "sshll2 %[OUT].4s,%[VAL].8h,#0\n\t"
                        "scvtf %[OUT].4s,%[OUT].4s\n\t"
                        : // output
                        [OUT]"+w" (fInput)
                        : // inputs
                        [VAL]"w" (depth128)
                        : // clobber list
                    );
#else
                fInput = _mm_cvtepi32_ps(_mm_unpackhi_epi16(depth128, ZERO));
#endif
                fInput = _mm_mul_ps(fInput, pointCloudZAxis128);

                zcoeffX128 = _mm_mul_ps(fInput, reciprocalFocalX128);
                zcoeffY128 = _mm_mul_ps(fInput, reciprocalFocalY128);

                x128 = _mm_mul_ps(_mm_sub_ps(xStep128, x0_128), zcoeffX128);
                y128 = _mm_mul_ps(yyy, zcoeffY128);

#if !defined(__aarch64__)
                _MM_TRANSPOSE4_PS(x128, y128, fInput, p3);
#endif

                res = _mm_extract_epi64(res128, 1);

                if ((res >> 0) & 0xfffflu)
                {
#if defined(__aarch64__)
                    asm volatile (
                            "st3 {%[SRC_X].s, %[SRC_Y].s, %[SRC_F].s}[0], [%[DST]], #12\n\t"
                            // output operands
                            : [DST]"+r" (p128)
                            // input operands
                            : [SRC_X]"w" (x128), [SRC_Y]"w" (y128), [SRC_F]"w" (fInput)
                            // list of clobbered registers
                            : "memory"
                        );
#else
                    _mm_storeu_ps(p, x128);
                    p += 3;
#endif
                }

                if ((res >> 16) & 0xfffflu)
                {
#if defined(__aarch64__)
                    asm volatile (
                            "st3 {%[SRC_X].s, %[SRC_Y].s, %[SRC_F].s}[1], [%[DST]], #12\n\t"
                            // output operands
                            : [DST]"+r" (p128)
                            // input operands
                            : [SRC_X]"w" (x128), [SRC_Y]"w" (y128), [SRC_F]"w" (fInput)
                            // list of clobbered registers
                            : "memory"
                        );
#else
                    _mm_storeu_ps(p, y128);   p += 3;
#endif

                }

                if ((res >> 32) & 0xfffflu)
                {
#if defined(__aarch64__)
                    asm volatile (
                            "st3 {%[SRC_X].s, %[SRC_Y].s, %[SRC_F].s}[2], [%[DST]], #12\n\t"
                            // output operands
                            : [DST]"+r" (p128)
                            // input operands
                            : [SRC_X]"w" (x128), [SRC_Y]"w" (y128), [SRC_F]"w" (fInput)
                            // list of clobbered registers
                            : "memory"
                        );
#else
                    _mm_storeu_ps(p, fInput); p += 3;
#endif
                }

                if ((res >> 48) & 0xfffflu)
                {
#if defined(__aarch64__)
                    asm volatile (
                            "st3 {%[SRC_X].s, %[SRC_Y].s, %[SRC_F].s}[3], [%[DST]], #12\n\t"
                            // output operands
                            : [DST]"+r" (p128)
                            // input operands
                            : [SRC_X]"w" (x128), [SRC_Y]"w" (y128), [SRC_F]"w" (fInput)
                            // list of clobbered registers
                            : "memory"
                        );
#else
                    _mm_storeu_ps(p, p3);     p += 3;
#endif
                }

                xStep128 = _mm_add_ps(xStep128, _mm_set1_ps(4));
            }
        }

#if defined(__aarch64__)
        countVertex = ((float *)p128 - outXYZMat) / 3; // 3 items per pixel
#else
        countVertex = int((p - outXYZMat) / 3); // 3 items per pixel
#endif

    }
#endif

    return countVertex;
}

//

int InuCommon::CAlgoBase::Depth2XYZ(
    const unsigned short *  inDepthMap,
    unsigned int            width,
    unsigned int            height,
    float                   f_x,
    float                   f_y,
    int                     opt_cen_x,
    int                     opt_cen_y,
    unsigned int            minZ,
    unsigned int            maxZ,
    float*                  outXYZMat,
    float                   iScale,
    int                     iPointCloudZAxis,
    int                     iPointCloudYAxis,
    std::pair<unsigned int, unsigned int> iDecimationFactor,
    bool                    iWithConfidence,
    InuCommon::COsUtilities::EOptimizationLevel    iRequestedLevel,
    const float * inRGBMap)
{
#if LOG_PARAMETERS

    std::ostringstream oss;
    oss
        << "\nCAlgoBase::Depth2XYZ (with decimation factor) parameters"
        << "\n"
        << "\n         inDepthMap = " << inDepthMap
        << "\n              width = " << width
        << "\n             height = " << height
        << "\n                f_x = " << f_x
        << "\n                f_y = " << f_y
        << "\n          opt_cen_x = " << opt_cen_x
        << "\n          opt_cen_y = " << opt_cen_y
        << "\n               minZ = " << minZ
        << "\n               maxZ = " << maxZ
        << "\n          outXYZMat = " << outXYZMat
        << "\n             iScale = " << iScale
        << "\n   iPointCloudZAxis = " << iPointCloudZAxis
        << "\n   iPointCloudYAxis = " << iPointCloudYAxis
        << "\niMaxPointCloudRange = " << iMaxPointCloudRange
        << "\niMinPointCloudRange = " << iMinPointCloudRange
        << "\n  iDecimationFactor = " << iDecimationFactor
        << "\n    iWithConfidence = " << iWithConfidence
        << "\n    iRequestedLevel = " << iRequestedLevel
        << std::endl;

#if __ANDROID__

    std::string oss_str(oss.str());
    __android_log_print(ANDROID_LOG_INFO, "InuDev", "%s", oss_str.c_str());
#else
    std::cout << oss.str();
#endif

#endif

    const int x0 = opt_cen_x;
    const int y0 = opt_cen_y;
    int countVertex = 0;
    /* float fB = f*B; */
    float *p = outXYZMat;
    char *rgbMapStart;

    if (!inRGBMap)
    {
        rgbMapStart = (char*)inRGBMap;
    }

    float reciprocalFocalX = 1.0f / f_x;
    float reciprocalFocalY = 1.0f / f_y;

    const unsigned short CONFIDENCE_MASK = (1u << sNumOfDepthConfidenenceBits) - 1;

    if (iDecimationFactor.first == 1 && iDecimationFactor.second == 1 && !inRGBMap)
    {
        if (InuCommon::COsUtilities::AVXIsSupported(width*height * sizeof(float), iRequestedLevel))
        {
            return 0;
        }

        if (InuCommon::COsUtilities::SSE4IsSupported(width*height * sizeof(float), iRequestedLevel))
        {
            return Depth2XYZ_SSE(
                inDepthMap,
                width,
                height,
                f_x,
                f_y,
                opt_cen_x,
                opt_cen_y,
                1.0f,
                minZ,
                maxZ,
                outXYZMat,
                iScale,
                iPointCloudZAxis,
                iPointCloudYAxis,
                iWithConfidence,
                iRequestedLevel
            );
        }
    }

    // naive

    if (iWithConfidence)
    {
        for (unsigned int rownum = 0; rownum < height; rownum += iDecimationFactor.second)
        {
            const unsigned short * pPixel = inDepthMap + rownum * width;

            for (unsigned int colnum = 0; colnum < width; colnum += iDecimationFactor.first)
            {
                uint16_t pixel = *pPixel;
                pPixel += iDecimationFactor.first;

                float confidence = float(pixel & CONFIDENCE_MASK);

                pixel >>= sNumOfDepthConfidenenceBits;

                if (true
                    && pixel > 0
                    && pixel <= maxZ
                    && pixel >= minZ
                    )
                {
                    float z = float(pixel);

                    float zcoeffX = z * reciprocalFocalX;
                    float zcoeffY = z * reciprocalFocalY;

                    float x = (int(colnum) - x0) * zcoeffX;
                    float y = (int(rownum) - y0) * zcoeffY;

                    0[p] = x * iScale;
                    1[p] = y * iScale * iPointCloudYAxis; //Axis direction
                    2[p] = z * iScale * iPointCloudZAxis;
                    3[p] = confidence;

                    p += 4;

                    countVertex++;
                }
            }
        }
    }
    else
    {
        for (unsigned int rownum = 0; rownum < height; rownum += iDecimationFactor.second)
        {
            const unsigned short * pPixel = inDepthMap + rownum * width;
            const float * stereoPixel = inRGBMap + rownum * width;

            for (unsigned int colnum = 0; colnum < width; colnum += iDecimationFactor.first)
            {
                uint16_t pixel = *pPixel;
                pPixel += iDecimationFactor.first;
                stereoPixel += iDecimationFactor.first;

                if (true
                    && pixel > 0
                    && pixel <= maxZ
                    && pixel >= minZ
                    )
                {
                    float z = float(pixel);

                    float zcoeffX = z*reciprocalFocalX;
                    float zcoeffY = z*reciprocalFocalY;

                    float x = (int(colnum) - x0) * zcoeffX;
                    float y = (int(rownum) - y0) * zcoeffY;

                    0[p] = x * iScale;
                    1[p] = y * iScale * iPointCloudYAxis; //Axis direction
                    2[p] = z * iScale * iPointCloudZAxis;
                    if (inRGBMap != nullptr)
                    {
                        3[p] = stereoPixel[0];
                        p += 4;
                    }
                    else
                    {
                        p += 3;
                    }

                    countVertex++;
                }
            }
        }
    }

    return countVertex;
}

#ifndef NO_BASIC_ALGO

void InuCommon::CAlgoBase::Depth2XYZValid(
    const cv::Mat& inDepthMap,
    float f_x,
    float f_y,
    int opt_cen_x,
    int opt_cen_y,
    float xy_binning,
    int minZ,
    int maxZ,
    cv::Mat &outXYZMat
)
{
    int x0 = opt_cen_x;
    int y0 = opt_cen_y;
    /* float fB = f*B; */

    float reciprocalFocalX = (float)(1. / f_x);
    float reciprocalFocalY = (float)(1. / f_y);

    for (int rownum = 0; rownum<inDepthMap.rows; rownum++)
    {
        const unsigned short *pPixel = inDepthMap.ptr<unsigned short>(rownum);
        float   *pInd = outXYZMat.ptr<float>(rownum);
        float   *pX = pInd + 1;
        float   *pY = pX + 1;
        float   *pZ = pY + 1;

        for (int colnum = 0; colnum<inDepthMap.cols; colnum++)
        {
            if (*pPixel > 0) // valid disparity
            {
                float z = float(*pPixel);

                float zcoeffX = z*reciprocalFocalX;
                float zcoeffY = z*reciprocalFocalY;

                if (z <= maxZ && z >= minZ)
                {

                    float x = (colnum - x0)*zcoeffX;
                    float y = (rownum - y0)*zcoeffY;

                    *pInd = 1;
                    *pX = xy_binning*x;
                    *pY = -xy_binning*y;
                    *pZ = -z;
                }
                else
                {
                    *pInd = 0;
                }
            }
            else
            {
                *pInd = 0;
            }

            pPixel++;
            pInd += 4;
            pX += 4;
            pY += 4;
            pZ += 4;
        }
    }
}

#endif

#ifndef NO_BASIC_ALGO
void InuCommon::CAlgoBase::Disparity2XYZ(
    const cv::Mat& inDispMap,
    float f,
    float B,
    float d_offset,
    int opt_cen_x,
    int opt_cen_y,
    float xy_binning,
    int minZ,
    int maxZ,
    cv::Mat &outXYZMat,
    cv::Mat &Zmat
)
{
    int x0 = opt_cen_x;
    int y0 = opt_cen_y;
    int countVertex = 0;
    /* float fB = f*B; */
    float d2;

    for (int rownum = 0; rownum<inDispMap.rows; rownum++)
    {
        const unsigned short *pPixel = inDispMap.ptr<unsigned short>(rownum);
        float   *pInd = outXYZMat.ptr<float>(rownum);
        unsigned short  *pzmat = Zmat.ptr<unsigned short>(rownum);
        float   *pX = pInd + 1;
        float   *pY = pX + 1;
        float   *pZ = pY + 1;

        for (int colnum = 0; colnum<inDispMap.cols; colnum++)
        {
            if (*pPixel > 0) // valid disparity
            {
                d2 = float(*pPixel) / 2;
                float zcoeff = B / (d2 + d_offset);
                float z = f*zcoeff;

                if (z <= maxZ && z >= minZ)
                {
                    float x = (colnum - x0)*zcoeff;
                    float y = (rownum - y0)*zcoeff;
                    *pInd = 1;
                    *pX = xy_binning*x;
                    *pY = -xy_binning*y;
                    *pZ = -z;
                    *pzmat = (unsigned short)z;
                }
                else
                {
                    *pInd = 0;
                    *pzmat = 0;
                }
            }
            else
            {
                *pInd = 0;
                *pzmat = 0;
            }

            pPixel++;
            pzmat++;
            pInd += 4;
            pX += 4;
            pY += 4;
            pZ += 4;
        }
    }
}

void InuCommon::CAlgoBase::Disparity2XYZ_B0(
    const cv::Mat& inDispMap,
    float f,
    float B,
    float d_offset,
    int opt_cen_x,
    int opt_cen_y,
    float xy_binning,
    int minZ,
    int maxZ,
    cv::Mat &outXYZMat,
    cv::Mat &Zmat
)
{
    int x0 = opt_cen_x;
    int y0 = opt_cen_y;
    int countVertex = 0;
    /* float fB = f*B; */
    float d2;

    for (int rownum = 0; rownum<inDispMap.rows; rownum++)
    {
        const unsigned short *pPixel = inDispMap.ptr<unsigned short>(rownum);
        float   *pInd = outXYZMat.ptr<float>(rownum);
        unsigned short  *pzmat = Zmat.ptr<unsigned short>(rownum);
        float   *pX = pInd + 1;
        float   *pY = pX + 1;
        float   *pZ = pY + 1;

        for (int colnum = 0; colnum<inDispMap.cols; colnum++)
        {
            unsigned short vPixel = *pPixel;
            vPixel = vPixel >> 4;
            unsigned short dPixel = vPixel >> 4;
            if ((dPixel > 4) && (dPixel <240)) // valid disparity
            {


                d2 = float(vPixel) / 16;
                float zcoeff = B / (d2 + d_offset);
                float z = f*zcoeff;

                if (z <= maxZ && z >= minZ)
                {
                    float x = (colnum - x0)*zcoeff;
                    float y = (rownum - y0)*zcoeff;
                    *pInd = 1;
                    *pX = xy_binning*x;
                    *pY = -xy_binning*y;
                    *pZ = -z;
                    *pzmat = (unsigned short)z;
                }
                else
                {
                    *pInd = 0;
                    *pzmat = 0;
                }
            }
            else
            {
                *pInd = 0;
                *pzmat = 0;
            }
            pPixel++;
            pzmat++;
            pInd += 4;
            pX += 4;
            pY += 4;
            pZ += 4;

        }
    }
}
#endif
