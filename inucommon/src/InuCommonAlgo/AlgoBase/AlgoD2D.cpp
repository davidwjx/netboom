#define DO_CONFIDENCE_FILTERING                             (1)

#define CALC_NUMBER_OF_INVALID_DEPTH_PIXLES                 (0)

#define D2D_GLOBAL                                          (1)
#define D2D_CONST                                           (0)

#define USE_AARCH64_SPECIFIC_INSTRUCTIONS                   (0)

#if _OPENMP
    #include <omp.h>
#endif

#include "OSUtilities.h"
#include "AlgoBase.h"
#include "AlgoD2D.h"
#include "AlgoAvx.h"

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

#include "FpException.h"

using namespace InuCommon;

#include <algorithm>
#include <iomanip>
#include <cmath>
#undef min
#undef max

// No cofidence map
uint32_t CAlgoD2D::Disparity2Depth(
        unsigned short*                     outBuffer,
        const unsigned short*               inBuffer,
        unsigned int                        inBufferSize,
        const InuCommon::CDepthOpticalData& opticalData,
        unsigned int                        confidenceThreshold,
        unsigned short                      minDepth,
        unsigned short                      maxDepth,
        unsigned short                      nonValidDepthValue,
        CTiffFile::EHWType                  iHWModel,
        COsUtilities::EOptimizationLevel    iRequestedLevel,
        float                               iScale,
        unsigned int                        iDisparityShiftRight,
        unsigned int                        iDisparityMask,
        unsigned int                        iConfidenceShiftRight,
        unsigned int                        iConfidenceMask,
        uint32_t                            disparityFrameOffsetValue,
        bool                                iWithConfidence,
        bool                                _omp  // use OpenMP
    )
{
    // by default, an exception will be raised for division by zero. Disable it.
    // Rouding, by default (x86, at least...) is set to _RC_NEAR (C++: FE_TONEAREST using fegetround()) or "round to nearest (int)"
    volatile FPExceptionDisabler disabled;

    const unsigned short* inputBuffer=inBuffer;
    unsigned short* outputBuffer=outBuffer;

    // In case of B0 the actual disparityDivider should be 16.0 (because there are only 12 effective bits), but we
    // divide only by 8.0 since the optical factor and disparity offset are multiplied by 2 (Defined by Sami)
    float disparityDivider = 8.0f;

    unsigned short outputConfidenceShift = 4;
    unsigned short outputDepthShift = 0;

    // The maximum depth is MAX_SHORT and not USHROT because of SSE which works with signed short and not unsigned short
    maxDepth = (unsigned short)(std::min(float(std::numeric_limits<short>::max() - 1), maxDepth * iScale) + 0.5f);

    if (iWithConfidence)
    {
        outputDepthShift = CAlgoBase::sNumOfDepthConfidenenceBits;
        outputConfidenceShift = CAlgoBase::sNumOfDepthConfidenenceBits;

        maxDepth = std::min((unsigned short)(0x3FFF), maxDepth);
    }

    unsigned short disparityShift = iDisparityShiftRight;
    unsigned short disparityMask = iDisparityMask;
    unsigned short confidenceShift = iConfidenceShiftRight;
    unsigned short confidenceMask = iConfidenceMask;

    unsigned short disparityMaxValue = 0xF00;

    if (InuCommon::COsUtilities::AVXIsSupported(inBufferSize*sizeof(unsigned short), iRequestedLevel))
    {
#if CALC_NUMBER_OF_INVALID_DEPTH_PIXLES
        uint32_t numberOfInvalidDepthPixles =
#endif
            CAlgoAvx::Disparity2Depth(
                outBuffer,
                inBuffer,
                inBufferSize,
                opticalData,
                disparityMask,
                disparityShift,
                disparityMaxValue,
                disparityDivider,
                confidenceMask,
                confidenceShift,
                confidenceThreshold,
                minDepth,
                maxDepth,
                nonValidDepthValue,
                disparityFrameOffsetValue,
                outputConfidenceShift,
                outputDepthShift,
                iScale
            );

#if CALC_NUMBER_OF_INVALID_DEPTH_PIXLES
        return numberOfInvalidDepthPixles;
#else
        return 0;
#endif
    }
    else if (InuCommon::COsUtilities::SSE4IsSupported(inBufferSize*sizeof(unsigned short), iRequestedLevel))
    {
#if CALC_NUMBER_OF_INVALID_DEPTH_PIXLES
        uint32_t  numberOfInvalidDepthPixles = 0;
#endif

#if !defined(__arm__) || defined(__ARM_NEON__)
        const __m128i  CONFIDENCE_MASK=_mm_set1_epi16(confidenceMask);
        const __m128i  DISPARITY_MASK=_mm_set1_epi16(disparityMask);
        const __m128i  UPPER_BOUND = _mm_set1_epi16(maxDepth);

        const __m128i  confidenceThreshold128 = _mm_set1_epi16((unsigned short)confidenceThreshold);

        const __m128i  upperLimit128=_mm_set1_epi16(disparityMaxValue + disparityFrameOffsetValue);
        const __m128i  frameOffset   = _mm_set1_epi16(disparityFrameOffsetValue);

        static const __m128   ZERO_FLOAT = _mm_set1_ps(0);
        const __m128  factor128=_mm_set1_ps(opticalData.mOpticalFactor);
        const __m128  disparityOffset128=_mm_set1_ps(opticalData.mDisparityOffset);
        const __m128  disparityDivider128=_mm_set1_ps(disparityDivider);
        const __m128  scale128 = _mm_set1_ps(iScale);

        __m128i nonValidDepthValue128 = _mm_set1_epi16(nonValidDepthValue);

#if CALC_NUMBER_OF_INVALID_DEPTH_PIXLES
        numberOfInvalidDepthPixles = 0u;
#endif

#if CALC_NUMBER_OF_INVALID_DEPTH_PIXLES
        // Non-valid are counted in 16 bits quanta. As 8 are counted simultaneously, we cannot handle images whose
        // number of invalid values will reach 65536 (16 bits.) In fact, we must be sure the number of invalids
        // will be less than 65536, hence, only 15 bits are useful.
        //
        // Max image size is, then:
        //
        //        8*32768 = 262144 pixels (2^18).
        //
        // Images of larger size will have to be processed in chuncks of that size.

        // TODO: No need for next double for loop. Remove unless CALC_NUMBER_OF_INVALID_DEPTH_PIXLES is defined non-zero
#endif

        constexpr unsigned int STEP = 1u << 16;
        constexpr unsigned int SMALL_STEP = sizeof(__m128i) / sizeof(unsigned short);

#pragma omp parallel for if(_omp)
        for (int i1 = 0 ; i1 < int(inBufferSize); i1 += STEP)
        {
            const __m128i* inputBuffer128 = reinterpret_cast<const __m128i*>(inputBuffer + i1);
            __m128i* outputBuffer128 = reinterpret_cast<__m128i*>(outputBuffer + i1);

#if CALC_NUMBER_OF_INVALID_DEPTH_PIXLES
            __m128i numNoValidAcc = _mm_set1_epi16(0);
#endif

            for (unsigned int i = 0; i < std::min(inBufferSize - i1, STEP); i += SMALL_STEP)
            {
                __m128i d = _mm_loadu_si128(inputBuffer128++);
                __m128i confidence = _mm_and_si128(d, CONFIDENCE_MASK);
                confidence = _mm_srli_epi16(confidence, confidenceShift);

                __m128i disparity = _mm_and_si128(d, DISPARITY_MASK);
                disparity = _mm_srli_epi16(disparity, disparityShift);

                // All values that are greater than max disparity should be zero
                __m128i  upperBound = _mm_cmplt_epi16(disparity, upperLimit128);
                disparity = _mm_and_si128(upperBound, disparity);

                // All disparity values that are 0 should be invalidated (at the end)
                __m128i zeroDisparity = _mm_cmpeq_epi16(disparity, _mm_set1_epi16(0));

                disparity = _mm_add_epi16(disparity, frameOffset);                        //Add frame offset  after threshold check

                // Low floats
                __m128i low = _mm_unpacklo_epi16(disparity, _mm_set1_epi16(0));
                __m128  fInputLow = _mm_cvtepi32_ps(low);
                __m128  maskedFactor = _mm_cmpgt_ps(fInputLow, ZERO_FLOAT);    // true if greater than threshold
                maskedFactor = _mm_and_ps(factor128, maskedFactor);

                fInputLow = _mm_div_ps(fInputLow, disparityDivider128);
                fInputLow = _mm_add_ps(fInputLow, disparityOffset128);
                fInputLow = _mm_div_ps(maskedFactor, fInputLow);
                fInputLow = _mm_mul_ps(fInputLow, scale128);

                // High floats
                __m128i high = _mm_unpackhi_epi16(disparity, _mm_set1_epi16(0));
                __m128  fInputHigh = _mm_cvtepi32_ps(high);
                maskedFactor = _mm_cmpgt_ps(fInputHigh, ZERO_FLOAT);    // true if greater than threshold
                maskedFactor = _mm_and_ps(factor128, maskedFactor);

                fInputHigh = _mm_div_ps(fInputHigh, disparityDivider128);
                fInputHigh = _mm_add_ps(fInputHigh, disparityOffset128);
                fInputHigh = _mm_div_ps(maskedFactor, fInputHigh);
                fInputHigh = _mm_mul_ps(fInputHigh, scale128);

                // Pack back to unsigned short
                __m128i lowInt = _mm_cvtps_epi32(fInputLow);
                __m128i highInt = _mm_cvtps_epi32(fInputHigh);

                __m128i result = _mm_packs_epi32(lowInt, highInt);

                // Invalidate all depth that are greater than MAX DEPTH
                __m128i tmpUpper = _mm_cmplt_epi16(result, UPPER_BOUND);
                __m128i valid = _mm_and_si128(result, tmpUpper);
                __m128i nonValid = _mm_andnot_si128(tmpUpper, nonValidDepthValue128);
                result = _mm_or_si128(valid, nonValid);

                // Invalidate all 0 disparities
                valid = _mm_andnot_si128(zeroDisparity, result);
                nonValid = _mm_and_si128(zeroDisparity, nonValidDepthValue128);
                result = _mm_or_si128(valid, nonValid);

                // Invalidate all values with low confidence
                __m128i  lowerConf = _mm_cmplt_epi16(confidence, confidenceThreshold128);
                valid = _mm_andnot_si128(lowerConf, result);
                nonValid = _mm_and_si128(lowerConf, nonValidDepthValue128);
                result = _mm_or_si128(valid, nonValid);

#if CALC_NUMBER_OF_INVALID_DEPTH_PIXLES
                numNoValidAcc = _mm_sub_epi16(numNoValidAcc, _mm_cmpeq_epi16(result, nonValidDepthValue128));
#endif

                // Add confidence (if required)
                confidence = _mm_srli_epi16(confidence, outputConfidenceShift);
                result = _mm_slli_epi16(result, outputDepthShift);
                result = _mm_or_si128(confidence, result);

                _mm_storeu_si128(outputBuffer128++, result);
            }

#if CALC_NUMBER_OF_INVALID_DEPTH_PIXLES
            __m128i t1 = _mm_add_epi32(
                _mm_unpacklo_epi16(numNoValidAcc, _mm_set1_epi16(0)),
                _mm_unpackhi_epi16(numNoValidAcc, _mm_set1_epi16(0))
            );

            numberOfInvalidDepthPixles += (uint32_t)(
                0
                + _mm_extract_epi32(t1, 0)
                + _mm_extract_epi32(t1, 1)
                + _mm_extract_epi32(t1, 2)
                + _mm_extract_epi32(t1, 3)
                );
#endif
        }
#endif

#if CALC_NUMBER_OF_INVALID_DEPTH_PIXLES
        return numberOfInvalidDepthPixles;
#else
        return 0;
#endif
    }

    // native

#if CALC_NUMBER_OF_INVALID_DEPTH_PIXLES
    uint32_t  numberOfInvalidDepthPixles = 0;
#endif

    float opticalFactor = opticalData.mOpticalFactor;
    float disparityOffset = opticalData.mDisparityOffset;

    for (unsigned int i = 0; i < inBufferSize ; i++)
    {
        unsigned short confidence = (*inputBuffer & confidenceMask) >> confidenceShift;
        unsigned short disparity = (*inputBuffer++ & disparityMask) >> disparityShift;

        int depth = CAlgoD2D::CalcDepth(
                disparity,
                disparityDivider,
                disparityOffset,
                opticalFactor,
                disparityMaxValue + disparityFrameOffsetValue,
                disparityFrameOffsetValue,
                nonValidDepthValue,
                iScale
            );

        depth = depth > int(maxDepth) ? nonValidDepthValue : depth;
        int tmp = confidence < confidenceThreshold ? nonValidDepthValue : depth;

#if CALC_NUMBER_OF_INVALID_DEPTH_PIXLES
        numberOfInvalidDepthPixles += !(tmp ^ nonValidDepthValue);
#endif

        tmp = confidence >> outputConfidenceShift | tmp << outputDepthShift;

        *outputBuffer++ = tmp;
    }

#if CALC_NUMBER_OF_INVALID_DEPTH_PIXLES
    return numberOfInvalidDepthPixles;
#else
    return 0;
#endif
}

#if defined(__ARM_NEON__) || defined(__aarch64__) || defined(_MSC_VER) || defined(__SSE2__)
#if D2D_CONST
    #define ZERO_FLOAT _mm_set1_ps(0)
#else
    #if D2D_GLOBAL
        static const __m128   ZERO_FLOAT = _mm_set1_ps(0);
    #endif
#endif
#endif

// with confidence and depth (distance) maps
// InitDistanceMap
uint32_t CAlgoD2D::Disparity2Depth(
        unsigned short*                     outBuffer,
        const unsigned short*               inBuffer,
        const std::shared_ptr<uint16_t>     iConfidenceMap,
        const std::shared_ptr<uint16_t>     iMaxDistanceMap,
        const std::shared_ptr<uint16_t>     iMinDistanceMap,
        unsigned int                        inBufferSize,
        const CDepthOpticalData&            opticalData,
        unsigned short                      nonValidDepthValue,
        CTiffFile::EHWType                  iHWModel,
        COsUtilities::EOptimizationLevel    iRequestedLevel,
        float                               iScale,
        unsigned int                        iDisparityShiftRight,
        unsigned int                        iDisparityMask,
        unsigned int                        iConfidenceShiftRight,
        unsigned int                        iConfidenceMask,
        uint32_t                            disparityFrameOffsetValue,
        bool                                iWithConfidence,
        bool                                _omp
    )
{
    // by default, an exception will be raised for division by zero. Disable it.
    // Rouding, by default (x86, at least...) is set to _RC_NEAR (C++: FE_TONEAREST using fegetround()) or "round to nearest (int)"
    volatile FPExceptionDisabler disabled;

    const unsigned short* inputBuffer = inBuffer;
    unsigned short* outputBuffer = outBuffer;

    // In case of B0 the actual disparityDivider should be 16.0 (because there are only 12 effective bits), but we
    // divide only by 8.0 since the optical factor and disparity offset are multiplied by 2 (Defined by Sami)
    float disparityDivider = 8.0f;


    unsigned short outputConfidenceShift = 4;
    unsigned short outputDepthShift = 0;

    if (iWithConfidence)
    {
        outputDepthShift = CAlgoBase::sNumOfDepthConfidenenceBits;
        outputConfidenceShift = CAlgoBase::sNumOfDepthConfidenenceBits;
    }

    unsigned short disparityShift = iDisparityShiftRight;
    unsigned short disparityMask = iDisparityMask;
    unsigned short confidenceShift = iConfidenceShiftRight;
    unsigned short confidenceMask = iConfidenceMask;

    unsigned short disparityMaxValue = 0xF00u;

    const uint16_t* pConfidenceThresh = iConfidenceMap.get();
    const uint16_t* pMaxDistance = iMaxDistanceMap.get();
    const uint16_t* pMinDistance = iMinDistanceMap.get();

    if (InuCommon::COsUtilities::AVXIsSupported(inBufferSize * sizeof(unsigned short), iRequestedLevel))
    {
        uint16_t confidenceThreshold = pConfidenceThresh[inBufferSize / 2];  //deprecated

        #pragma region maxDepth

        uint16_t maxDepth = pMaxDistance[inBufferSize / 2]; // deprecated
        // The maximum depth is MAX_SHORT and not USHROT because of SSE which works with signed short and not unsigned short
        maxDepth = (unsigned short)(std::min(float(std::numeric_limits<short>::max() - 1), maxDepth * iScale) + 0.5f);

        if (iWithConfidence)
        {
            maxDepth = std::min((unsigned short)0x3FFFu, maxDepth);
        }

        #pragma endregion

        #pragma region minDepth

        uint16_t minDepth = pMinDistance[inBufferSize / 2]; // deprecated
        // The minimum depth is MAX_SHORT and not USHROT because of SSE which works with signed short and not unsigned short
        minDepth = (unsigned short)(std::min(float(std::numeric_limits<short>::max() - 1), minDepth * iScale) + 0.5f);

        if (iWithConfidence)
        {
            minDepth = std::min((unsigned short)0x3FFFu, minDepth);
        }

        #pragma endregion

#if CALC_NUMBER_OF_INVALID_DEPTH_PIXLES
        uint32_t numberOfInvalidDepthPixles =
#endif
            CAlgoAvx::Disparity2Depth(
                outBuffer,
                inBuffer,
                inBufferSize,
                opticalData,
                disparityMask,
                disparityShift,
                disparityMaxValue,
                disparityDivider,
                confidenceMask,
                confidenceShift,
                confidenceThreshold,
                minDepth,
                maxDepth,
                nonValidDepthValue,
                disparityFrameOffsetValue, 
                outputConfidenceShift,
                outputDepthShift,
                iScale
            );

#if CALC_NUMBER_OF_INVALID_DEPTH_PIXLES
        return numberOfInvalidDepthPixles;
#else
        return 0;
#endif
    }
    else if (InuCommon::COsUtilities::SSE4IsSupported(inBufferSize * sizeof(unsigned short), iRequestedLevel))
    {
#if defined(__ARM_NEON__) || defined(__aarch64__) || defined(_MSC_VER) || defined(__SSE2__)

#if CALC_NUMBER_OF_INVALID_DEPTH_PIXLES
        uint32_t  numberOfInvalidDepthPixles = 0;
#endif

        constexpr unsigned int STEP = 1u << 16;
        constexpr unsigned int SMALL_STEP = sizeof(__m128i) / sizeof(unsigned short);

        const __m128i  CONFIDENCE_MASK = _mm_set1_epi16(confidenceMask);
        const __m128i  DISPARITY_MASK = _mm_set1_epi16(disparityMask);

        const __m128i  upperLimit128 = _mm_set1_epi16(disparityMaxValue + disparityFrameOffsetValue);
        const __m128i  frameOffset = _mm_set1_epi16(disparityFrameOffsetValue);

#if !D2D_GLOBAL && !D2D_CONST
        static const __m128   ZERO_FLOAT = _mm_set1_ps(0);
#endif
        const __m128  factor128 = _mm_set1_ps(opticalData.mOpticalFactor * iScale);
        const __m128  disparityOffset128 = _mm_set1_ps(opticalData.mDisparityOffset);
        const __m128  reciprocalDisparityDivider128 = _mm_set1_ps(1.0f / disparityDivider);

        __m128i notValidThreshold = _mm_set1_epi16(nonValidDepthValue);

#if CALC_NUMBER_OF_INVALID_DEPTH_PIXLES
        numberOfInvalidDepthPixles = 0u;
#endif

#if CALC_NUMBER_OF_INVALID_DEPTH_PIXLES
        // Non-valid are counted in 16 bits quanta. As 8 are counted simultaneously, we cannot handle images whose
        // number of invalid values will reach 65536 (16 bits.) In fact, we must be sure the number of invalids
        // will be less than 65536, hence, only 15 bits are useful.
        //
        // Max image size is, then:
        //
        //        8*32768 = 262144 pixels (2^18).
        //
        // Images of larger size will have to be processed in chuncks of that size.

        // TODO: No need for next double for loop. Remove unless CALC_NUMBER_OF_INVALID_DEPTH_PIXLES is defined non-zero
#endif

#pragma omp parallel for if(_omp)
        for (int i1 = 0; i1 < int(inBufferSize); i1 += STEP)
        {
            const __m128i* inputBuffer128 = reinterpret_cast<const __m128i*>(inputBuffer) + i1;
            __m128i* outputBuffer128 = reinterpret_cast<__m128i*>(outputBuffer) + i1;

#if DO_CONFIDENCE_FILTERING
            const __m128i* pThresh128 = reinterpret_cast<const __m128i*>(pConfidenceThresh) + i1;
#endif
            const __m128i* pMaxDistance128 = reinterpret_cast<const __m128i*>(pMaxDistance) + i1;
            const __m128i* pMinDistance128 = reinterpret_cast<const __m128i*>(pMinDistance) + i1;

#if CALC_NUMBER_OF_INVALID_DEPTH_PIXLES
            __m128i numNoValidAcc = _mm_set1_epi16(0);
#endif

            for (unsigned int i = 0; i < std::min(inBufferSize - i1, STEP); i += SMALL_STEP)
            {
                __m128i invalidateMask128 = _mm_set1_epi16(0);

                __m128i d = _mm_loadu_si128(inputBuffer128++);

                __m128i confidence = _mm_and_si128(d, CONFIDENCE_MASK);
                __m128i disparity = _mm_and_si128(d, DISPARITY_MASK);

#if DO_CONFIDENCE_FILTERING
                // Invalidate all values with low confidence
                __m128i confidenceThreshold128 = _mm_loadu_si128(pThresh128++);
                confidence = _mm_srli_epi16(confidence, confidenceShift);
                invalidateMask128 = _mm_or_si128(invalidateMask128, _mm_cmplt_epi16(confidence, confidenceThreshold128));
#endif

                disparity = _mm_srli_epi16(disparity, disparityShift);

                // All values that are greater than max disparity should be zero
                __m128i  upperBound = _mm_cmplt_epi16(disparity, upperLimit128);
                disparity = _mm_and_si128(upperBound, disparity);

                disparity = _mm_add_epi16(disparity, frameOffset);                        //Add frame offset  after threshold check

                // Low floats
#if USE_AARCH64_SPECIFIC_INSTRUCTIONS && defined(__aarch64__)
                __m128i low = (__m128i)vshll_n_u16(vget_low_u16(disparity), 0);
                //__m128i low = vshll_n_u16(disparity,0);
#else
                __m128i low = _mm_unpacklo_epi16(disparity, _mm_set1_epi16(0));
#endif

                __m128  fInputLow = _mm_cvtepi32_ps(low);

                fInputLow = _mm_mul_ps(fInputLow, reciprocalDisparityDivider128);
                fInputLow = _mm_add_ps(fInputLow, disparityOffset128);
                fInputLow = _mm_div_ps(factor128, fInputLow);

                // High floats
#if USE_AARCH64_SPECIFIC_INSTRUCTIONS && defined(__aarch64__)
                __m128i high = (__m128i)vshll_high_n_u16(disparity, 0);
#else
                __m128i high = _mm_unpackhi_epi16(disparity, _mm_set1_epi16(0));
#endif
                __m128  fInputHigh = _mm_cvtepi32_ps(high);

                fInputHigh = _mm_mul_ps(fInputHigh, reciprocalDisparityDivider128);
                fInputHigh = _mm_add_ps(fInputHigh, disparityOffset128);
                fInputHigh = _mm_div_ps(factor128, fInputHigh);

                // Pack back to unsigned short

                __m128i lowInt = _mm_cvtps_epi32(fInputLow);
                __m128i highInt = _mm_cvtps_epi32(fInputHigh);

                __m128i result = _mm_packs_epi32(lowInt, highInt);

                // Invalidate all depth that are greater than MAX DEPTH
                __m128i UPPER_BOUND = _mm_loadu_si128(pMaxDistance128++);
                invalidateMask128 = _mm_or_si128(invalidateMask128, _mm_cmplt_epi16(UPPER_BOUND, result));

                // Invalidate all depth that are less than MIN_DEPTH
                __m128i LOWER_BOUND = _mm_loadu_si128(pMinDistance128++);
                invalidateMask128 = _mm_or_si128(invalidateMask128, _mm_cmpgt_epi16(LOWER_BOUND, result));

                // All disparity values that are 0 should be invalidated (at the end)
                invalidateMask128 = _mm_or_si128(invalidateMask128, _mm_cmpeq_epi16(disparity, _mm_setzero_si128()));
 
#if USE_AARCH64_SPECIFIC_INSTRUCTIONS && defined(__aarch64__)
                result = (__m128i)vbslq_u16(outputClearMask128, nonValidDepthValue128, result);
#else
                result = _mm_or_si128(
                    _mm_and_si128(invalidateMask128, notValidThreshold),
                    _mm_andnot_si128(invalidateMask128, result)
                );
#endif

#if CALC_NUMBER_OF_INVALID_DEPTH_PIXLES
                numNoValidAcc = _mm_sub_epi16(numNoValidAcc, _mm_cmpeq_epi16(result, nonValidDepthValue128));
#endif

                // Add confidence (if required)
                confidence = _mm_srli_epi16(confidence, outputConfidenceShift);
                result = _mm_slli_epi16(result, outputDepthShift);
                result = _mm_or_si128(confidence, result);

                _mm_storeu_si128(outputBuffer128++, result);
            }

#if CALC_NUMBER_OF_INVALID_DEPTH_PIXLES
            __m128i t1 = _mm_add_epi32(
                _mm_unpacklo_epi16(numNoValidAcc, _mm_set1_epi16(0)),
                _mm_unpackhi_epi16(numNoValidAcc, _mm_set1_epi16(0))
            );

            numberOfInvalidDepthPixles += (uint32_t)(
                0
                + _mm_extract_epi32(t1, 0)
                + _mm_extract_epi32(t1, 1)
                + _mm_extract_epi32(t1, 2)
                + _mm_extract_epi32(t1, 3)
                );
#endif
        }
#endif
#if CALC_NUMBER_OF_INVALID_DEPTH_PIXLES
        return numberOfInvalidDepthPixles;
#else
        return 0;
#endif
    }

    // naive

#if CALC_NUMBER_OF_INVALID_DEPTH_PIXLES
    uint32_t  numberOfInvalidDepthPixles{ 0 };
#endif

    float opticalFactor = opticalData.mOpticalFactor;
    float disparityOffset = opticalData.mDisparityOffset;

    for (unsigned int i = 0; i < inBufferSize; i++)
    {
        unsigned short confidence = (*inputBuffer & confidenceMask) >> confidenceShift;
        unsigned short disparity = (*inputBuffer++ & disparityMask) >> disparityShift;

        int depth = CAlgoD2D::CalcDepth(
            disparity,
            disparityDivider,
            disparityOffset,
            opticalFactor,
            disparityMaxValue + disparityFrameOffsetValue,
            disparityFrameOffsetValue,
            nonValidDepthValue,
            iScale
        );

        depth = depth > int(*pMaxDistance++) ? nonValidDepthValue : depth;

#if DO_CONFIDENCE_FILTERING
        int tmp = confidence < *pConfidenceThresh++ ? nonValidDepthValue : depth;
#else
        int tmp = confidence < confidenceThreshold ? nonValidDepthValue : depth;
#endif

#if CALC_NUMBER_OF_INVALID_DEPTH_PIXLES
        numberOfInvalidDepthPixles += !(tmp ^ nonValidDepthValue);
#endif

        tmp = confidence >> outputConfidenceShift | tmp << outputDepthShift;

        *outputBuffer++ = tmp;
    }

#if CALC_NUMBER_OF_INVALID_DEPTH_PIXLES
    return numberOfInvalidDepthPixles;
#else
    return 0;
#endif
}

#include <cfenv>
#if defined(_MSC_VER)
    #pragma fenv_access (on)
#else
    #pragma STDC FENV_ACCESS on
#endif

// with confidence and disparity maps
// InitConfidenceAndDisparityMap
uint32_t CAlgoD2D::Disparity2Depth(
    unsigned short*                     outBuffer,
    const unsigned short*               inBuffer,
    const std::shared_ptr<uint16_t>     iConfidenceAndMinDisparityMap,
    const std::shared_ptr<uint16_t>     iConfidenceAndMaxDisparityMap,
    unsigned int                        inBufferSize,
    const CDepthOpticalData&            opticalData,
    unsigned short                      nonValidDepthValue,
    CTiffFile::EHWType                  iHWModel,
    COsUtilities::EOptimizationLevel    iRequestedLevel,
    float                               iScale,
    unsigned int                        iDisparityShiftRight,
    unsigned int                        iDisparityMask,
    unsigned int                        iConfidenceShiftRight,
    unsigned int                        iConfidenceMask,
    uint32_t                            disparityFrameOffsetValue,
    bool                                iWithConfidence,
    bool                                _omp
)
{
    // by default, an exception will be raised for division by zero. Disable it.
    // Rouding, by default (x86, at least...) is set to _RC_NEAR (C++: FE_TONEAREST using fegetround()) or "round to nearest (int)"
    volatile FPExceptionDisabler disabled;

    const unsigned short* inputBuffer = inBuffer;
    unsigned short* outputBuffer = outBuffer;

    // In case of B0 the actual disparityDivider should be 16.0 (because there are only 12 effective bits), but we
    // divide only by 8.0 since the optical factor and disparity offset are multiplied by 2 (Defined by Sami)
    float disparityDivider = 8.0f;

    unsigned short outputConfidenceShift;
    unsigned short outputDepthShift;

    if (iWithConfidence)
    {
        outputDepthShift = CAlgoBase::sNumOfDepthConfidenenceBits;
        outputConfidenceShift = CAlgoBase::sNumOfDepthConfidenenceBits;
    }
    else
    {
        outputConfidenceShift = 4;
        outputDepthShift = 0;
    }

    unsigned short disparityShift = iDisparityShiftRight;
    unsigned short disparityMask = iDisparityMask;
    unsigned short confidenceShift = iConfidenceShiftRight;
    unsigned short confidenceMask = iConfidenceMask;

    unsigned short disparityMaxValue = 0xF00;

    const uint16_t* pConfidenceAndMinDisparityThreshMap = iConfidenceAndMinDisparityMap.get();
    const uint16_t* pConfidenceAndMaxDisparityThreshMap = iConfidenceAndMaxDisparityMap.get();

    if (InuCommon::COsUtilities::AVXIsSupported(inBufferSize * sizeof(unsigned short), iRequestedLevel))
    {
        #pragma region maxDepth

        // pick threshold at the centre of the image...
        const uint16_t minDisp = pConfidenceAndMinDisparityThreshMap[inBufferSize / 2];
        const uint16_t confidenceThreshold = (minDisp & confidenceMask) >> confidenceShift;  //depricate

        int maxDepth = CAlgoD2D::CalcDepth(
            (minDisp & disparityMask) >> disparityShift,
            disparityDivider,
            opticalData.mDisparityOffset,
            opticalData.mOpticalFactor,
            disparityMaxValue + disparityFrameOffsetValue,
            disparityFrameOffsetValue,
            nonValidDepthValue,
            iScale
        );

        // The maximum depth is MAX_SHORT and not USHROT because of SSE which works with signed short and not unsigned short
        maxDepth = (unsigned short)(std::min(float(std::numeric_limits<short>::max() - 1), maxDepth * iScale) + 0.5f);

        if (iWithConfidence)
        {
            maxDepth = std::min(0x3FFF, maxDepth);
        }

        #pragma endregion

        #pragma region minDepth

        // pick threshold at the centre of the image...
        const uint16_t maxDisp = pConfidenceAndMaxDisparityThreshMap[inBufferSize / 2];

        int minDepth = CAlgoD2D::CalcDepth(
            (maxDisp & disparityMask) >> disparityShift,
            disparityDivider,
            opticalData.mDisparityOffset,
            opticalData.mOpticalFactor,
            disparityMaxValue + disparityFrameOffsetValue,
            disparityFrameOffsetValue,
            nonValidDepthValue,
            iScale
        );

        // The maximum depth is MAX_SHORT and not USHROT because of SSE which works with signed short and not unsigned short
        minDepth = (unsigned short)(std::min(float(std::numeric_limits<short>::max() - 1), minDepth * iScale) + 0.5f);

        if (iWithConfidence)
        {
            minDepth = std::min(0x3FFF, minDepth);
        }

        #pragma endregion

#if CALC_NUMBER_OF_INVALID_DEPTH_PIXLES
        uint32_t numberOfInvalidDepthPixles =
#endif
            CAlgoAvx::Disparity2Depth(
                outBuffer,
                inBuffer,
                inBufferSize,
                opticalData,
                disparityMask,
                disparityShift,
                disparityMaxValue,
                disparityDivider,
                confidenceMask,
                confidenceShift,
                confidenceThreshold,
                minDepth,
                maxDepth,
                nonValidDepthValue,
                disparityFrameOffsetValue,
                outputConfidenceShift,
                outputDepthShift,
                iScale
            );

#if CALC_NUMBER_OF_INVALID_DEPTH_PIXLES
        return numberOfInvalidDepthPixles;
#else
        return 0;
#endif
    }
    else if (InuCommon::COsUtilities::SSE4IsSupported(inBufferSize * sizeof(unsigned short), iRequestedLevel))
    {
#if defined(__ARM_NEON__) || defined(__aarch64__) || defined(_MSC_VER) || defined(__SSE2__)

#if CALC_NUMBER_OF_INVALID_DEPTH_PIXLES
        uint32_t  numberOfInvalidDepthPixles = 0;
#endif

        constexpr unsigned int STEP = 1u << 16;
        constexpr unsigned int SMALL_STEP = sizeof(__m128i) / sizeof(unsigned short);

        const __m128i  CONFIDENCE_MASK = _mm_set1_epi16(confidenceMask);
        const __m128i  DISPARITY_MASK = _mm_set1_epi16(disparityMask);

        const __m128i  upperLimit128 = _mm_set1_epi16(disparityMaxValue + disparityFrameOffsetValue - 1);  // ???
        const __m128i  disparityFrameOffsetValue128 = _mm_set1_epi16(disparityFrameOffsetValue);

#if !D2D_GLOBAL && !D2D_CONST
        static const __m128   ZERO_FLOAT = _mm_set1_ps(0);
#endif
        const __m128  factor128 = _mm_set1_ps(opticalData.mOpticalFactor * iScale);
        const __m128  disparityOffset128 = _mm_set1_ps(opticalData.mDisparityOffset);
        const __m128  reciprocalDisparityDivider128 = _mm_set1_ps(1.0f / disparityDivider);

        const __m128i nonValidDepthValue128 = _mm_set1_epi16(nonValidDepthValue);

#if CALC_NUMBER_OF_INVALID_DEPTH_PIXLES
        numberOfInvalidDepthPixles = 0u;
#endif

#if CALC_NUMBER_OF_INVALID_DEPTH_PIXLES
        // Non-valid are counted in 16 bits quanta. As 8 are counted simultaneously, we cannot handle images whose
        // number of invalid values will reach 65536 (16 bits.) In fact, we must be sure the number of invalids
        // will be less than 65536, hence, only 15 bits are useful.
        //
        // Max image size is, then:
        //
        //        8*32768 = 262144 pixels (2^18).
        //
        // Images of larger size will have to be processed in chuncks of that size.

        // TODO: No need for next double for loop. Remove unless CALC_NUMBER_OF_INVALID_DEPTH_PIXLES is defined non-zero
#endif

#pragma omp parallel for if(_omp)
        for (int i1 = 0; i1 < int(inBufferSize); i1 += STEP)
        {
            const __m128i* inputBuffer128 = reinterpret_cast<const __m128i*>(inputBuffer + i1);
            __m128i* outputBuffer128 = reinterpret_cast<__m128i*>(outputBuffer + i1);
            const __m128i* pMinDispThresh128 = reinterpret_cast<const __m128i*>(pConfidenceAndMinDisparityThreshMap + i1);
            const __m128i* pMaxDispThresh128 = reinterpret_cast<const __m128i*>(pConfidenceAndMaxDisparityThreshMap + i1);

#if CALC_NUMBER_OF_INVALID_DEPTH_PIXLES
            __m128i numNoValidAcc = _mm_set1_epi16(0);
#endif

            for (unsigned int i = 0; i < std::min(inBufferSize - i1, STEP); i += SMALL_STEP)
            {
                __m128i d = _mm_loadu_si128(inputBuffer128++);

                __m128i confidence = _mm_and_si128(d, CONFIDENCE_MASK);
                __m128i disparity = _mm_and_si128(d, DISPARITY_MASK);

                __m128i maxDispAndConfThreshold = _mm_loadu_si128(pMaxDispThresh128++);
                __m128i maxDisparityThreshold128 = _mm_and_si128(maxDispAndConfThreshold, DISPARITY_MASK);

                __m128i minDispAndConfThreshold = _mm_loadu_si128(pMinDispThresh128++);
                __m128i minDisparityThreshold128 = _mm_and_si128(minDispAndConfThreshold, DISPARITY_MASK);

                __m128i confidenceThreshold128 = _mm_and_si128(minDispAndConfThreshold, CONFIDENCE_MASK);

                // these bits which are '1' will *NOT* appear in the output

#if defined(__ARM_NEON__) || defined(__aarch64__)

                // No such instruction for X86

                __m128i outputClearMask128 = _mm_cmplt_epu16(disparity, minDisparityThreshold128); // or equal?

                outputClearMask128 = _mm_or_si128(
                    outputClearMask128,
                    _mm_cmpgt_epu16(disparity, maxDisparityThreshold128)
                );

                outputClearMask128 = _mm_or_si128(
                    outputClearMask128,
                    _mm_cmplt_epu16(confidence, confidenceThreshold128)
                );

#else
                // We cannot directly compare disparity with its threshold, as these are unsigned 16 bit
                // values, whereas SSE (X86) only has _mm_cmplt_epi16. We therefore get the unsigned max
                // and compare it to the threshold

                __m128i outputClearMask128 = _mm_cmpeq_epi16(
                    _mm_max_epu16(disparity, minDisparityThreshold128),
                    minDisparityThreshold128
                );

                // Compare disparity with max disparity value (for min distance.) See above...

                outputClearMask128 = _mm_or_si128(
                    outputClearMask128,
                    _mm_cmpeq_epi16(
                        _mm_min_epu16(disparity, maxDisparityThreshold128),
                        maxDisparityThreshold128
                    )
                );

                // Assumes bit 15 (sign) will not have confidence info.

                outputClearMask128 = _mm_or_si128(
                    outputClearMask128,
                    _mm_cmplt_epi16(confidence, confidenceThreshold128)
                );
#endif

                disparity = _mm_srli_epi16(disparity, disparityShift);

                // All values that are greater than or equal to max disparity should be zero
                outputClearMask128 = _mm_or_si128(
                    outputClearMask128,
                    _mm_cmplt_epi16(upperLimit128, disparity));

#if 0
                // This is commented out as it does not yield performance gain on
                // test input. It may produce substantial performance gain, however,
                // this is highly input dependent.

                if (_mm_test_all_ones(outputClearMask128))
                {
                    _mm_storeu_si128(outputBuffer128++, nonValidDepthValue128);
                    continue;
                }
#endif

                // Add frame offset  after threshold check
                disparity = _mm_add_epi16(disparity, disparityFrameOffsetValue128);

                // Low floats
#if USE_AARCH64_SPECIFIC_INSTRUCTIONS && defined(__aarch64__)
                __m128i low = (__m128i)vshll_n_u16(vget_low_u16(disparity), 0);
                //__m128i low = vshll_n_u16(disparity,0);
#else
                __m128i low = _mm_unpacklo_epi16(disparity, _mm_set1_epi16(0));
#endif

                __m128  fInputLow = _mm_cvtepi32_ps(low);

                fInputLow = _mm_mul_ps(fInputLow, reciprocalDisparityDivider128);
                fInputLow = _mm_add_ps(fInputLow, disparityOffset128);
                fInputLow = _mm_div_ps(factor128, fInputLow);

                __m128i lowInt = _mm_cvtps_epi32(fInputLow);

                // High floats
#if USE_AARCH64_SPECIFIC_INSTRUCTIONS && defined(__aarch64__)
                __m128i high = (__m128i)vshll_high_n_u16(disparity, 0);
#else
                __m128i high = _mm_unpackhi_epi16(disparity, _mm_set1_epi16(0));
#endif
                __m128  fInputHigh = _mm_cvtepi32_ps(high);

                fInputHigh = _mm_mul_ps(fInputHigh, reciprocalDisparityDivider128);
                fInputHigh = _mm_add_ps(fInputHigh, disparityOffset128);
                fInputHigh = _mm_div_ps(factor128, fInputHigh);

                __m128i highInt = _mm_cvtps_epi32(fInputHigh);

                // Pack back to unsigned short

                __m128i result = _mm_packs_epi32(lowInt, highInt);

#if USE_AARCH64_SPECIFIC_INSTRUCTIONS && defined(__aarch64__)
                result = (__m128i)vbslq_u16(outputClearMask128, nonValidDepthValue128, result);
#else
                result = _mm_or_si128(_mm_and_si128(outputClearMask128, nonValidDepthValue128), _mm_andnot_si128(outputClearMask128, result));
#endif

#if CALC_NUMBER_OF_INVALID_DEPTH_PIXLES
                numNoValidAcc = _mm_sub_epi16(numNoValidAcc, _mm_cmpeq_epi16(result, nonValidDepthValue128));
#endif

                // Add confidence (if required)
                confidence = _mm_srli_epi16(confidence, outputConfidenceShift);
                result = _mm_slli_epi16(result, outputDepthShift);
                result = _mm_or_si128(confidence, result);

                _mm_storeu_si128(outputBuffer128++, result);
            }

#if CALC_NUMBER_OF_INVALID_DEPTH_PIXLES
            __m128i t1 = _mm_add_epi32(
                _mm_unpacklo_epi16(numNoValidAcc, _mm_set1_epi16(0)),
                _mm_unpackhi_epi16(numNoValidAcc, _mm_set1_epi16(0))
            );

            numberOfInvalidDepthPixles += (uint32_t)(
                0
                + _mm_extract_epi32(t1, 0)
                + _mm_extract_epi32(t1, 1)
                + _mm_extract_epi32(t1, 2)
                + _mm_extract_epi32(t1, 3)
                );
#endif
        }
#endif

#if CALC_NUMBER_OF_INVALID_DEPTH_PIXLES
        return numberOfInvalidDepthPixles;
#else
        return 0;
#endif
    }

    // naive

    uint32_t  numberOfInvalidDepthPixles = 0;

    float opticalFactor = opticalData.mOpticalFactor;
    float disparityOffset = opticalData.mDisparityOffset;

    for (unsigned int i = 0; i < inBufferSize; i++)
    {
        unsigned short data = *inBuffer++;

        unsigned short confidence = data & confidenceMask;
        unsigned short disparity = data & disparityMask;

        unsigned short condifenceAndMinDisparityThresh = *pConfidenceAndMinDisparityThreshMap++;
        unsigned short condifenceAndMaxDisparityThresh = *pConfidenceAndMaxDisparityThreshMap++;

        unsigned short  confidenceThreshold = condifenceAndMinDisparityThresh & confidenceMask;
        unsigned short minDisparityThreshold = condifenceAndMinDisparityThresh & disparityMask;
        unsigned short maxDisparityThreshold = condifenceAndMaxDisparityThresh & disparityMask;

        int depth = CAlgoD2D::CalcDepth(
            disparity >> disparityShift,
            disparityDivider,
            disparityOffset,
            opticalFactor,
            disparityMaxValue + disparityFrameOffsetValue,
            disparityFrameOffsetValue,
            nonValidDepthValue,
            iScale
        );

        int tmp{ nonValidDepthValue };

        if (true
            && confidence >= confidenceThreshold
            && disparity >= minDisparityThreshold
            && disparity <= maxDisparityThreshold
        )
        {
            tmp = depth << outputDepthShift | confidence >> outputConfidenceShift;
        }

#if CALC_NUMBER_OF_INVALID_DEPTH_PIXLES
        numberOfInvalidDepthPixles += !(tmp ^ nonValidDepthValue);
#endif

        *outputBuffer++ = tmp;
    }

    return numberOfInvalidDepthPixles;
}

// familty == 2
CAlgoD2D::EErrorType CAlgoD2D::InitDistanceMap(
    std::shared_ptr<uint16_t>   iMaxDistanceMap,
    std::shared_ptr<uint16_t>   iMinDistanceMap,
    uint32_t                    iHeight,
    uint32_t                    iWidth,
    uint32_t                    iMaxDistance,
    uint32_t                    iMinDistance,
    float                       iScale,
    bool                        iWithconfidence,
    float                       iFocalLengthX,
    float                       iFocalLengthY
)
{
    uint16_t *pMax = iMaxDistanceMap.get();
    uint16_t* pMin = iMinDistanceMap.get();

    // The maximum depth is MAX_SHORT and not USHROT because of SSE which works with signed short and not unsigned short
    float maxDistanceF = std::min(float(std::numeric_limits<short>::max() - 1), iMaxDistance * iScale);
    float minDistanceF = std::max(0.0f, iMinDistance * iScale);

    if (iWithconfidence)
    {
        maxDistanceF = std::min(float(0x3FFFu), maxDistanceF);
    }

    if (minDistanceF >= maxDistanceF)
    {
        return EErrorType::eMaxMinDistanceMismatch;
    }

    const float centerX = float(iWidth) / 2.0f;
    const float centerY = float(iHeight) / 2.0f;

    for (unsigned y = 0; y < iHeight; y++)
    {
        for (unsigned int x = 0; x < iWidth; x++)
        {
            float rad = std::sqrt(1.0f + (((x - centerX) * (x - centerX)) / (iFocalLengthX*iFocalLengthX)) + (((y - centerY) * (y - centerY)) / (iFocalLengthY*iFocalLengthY)));
            float depthMaxF = maxDistanceF / rad;
            float depthMinF = minDistanceF / rad;

            //uint16_t depth = (uint16_t)(depthF + 0.5f);
            uint16_t depthMax = static_cast<uint16_t>(depthMaxF);
            uint16_t depthMin = static_cast<uint16_t>(depthMinF);

            *pMax++ = depthMax;
            *pMin++ = depthMin;
        }
    }

    return eOk;
}

// familty == 2
bool CAlgoD2D::InitConfidenceMap(
    std::shared_ptr<uint16_t>    iConfidenceMap,
    uint32_t                     iHeight,
    uint32_t                     iWidth,
    uint32_t                     iConfidneceStartRegion2,
    uint32_t                     iConfidneceEndRegion2,
    uint16_t                     iConfidenceThresholdRegion1,
    uint16_t                     iConfidenceThresholdRegion2
)
{
    uint16_t *p = iConfidenceMap.get();

    for (unsigned int y = 0; y < iHeight; y++)
    {
        for (unsigned int x = 0; x < iWidth; x++)
        {
            if (x < iConfidneceStartRegion2 || x > iConfidneceEndRegion2)
            {
                *p++ = iConfidenceThresholdRegion1;
            }
            else
            {
                *p++ = iConfidenceThresholdRegion2;
            }
        }
    }

    return true;
}

// family == 3
bool CAlgoD2D::InitConfidenceAndDisparityMap(
    std::shared_ptr<uint16_t>   iConfidenceAndMinDisparityMap,
    std::shared_ptr<uint16_t>   iConfidenceAndMaxDisparityMap,
    unsigned int                iHeight,
    unsigned int                iWidth,
    unsigned int                iConfidneceStartRegion2,
    unsigned int                iConfidneceEndRegion2,
    uint16_t                    iConfidenceThresholdRegion1,
    uint16_t                    iConfidenceThresholdRegion2,

    uint32_t                    iMaxDistance,
    uint32_t                    iMinDistance,
    float                       iScale,
    bool                        iWithconfidence,
    float                       iFocalLengthX,
    float                       iFocalLengthY,

    uint16_t                    confidenceMask,
    unsigned short              confidenceShift,
    uint16_t                    disparityMask,
    unsigned short              disparityShift,

    float                       opticalFactor,
    float                       disparityOffset,
    float                       disparityDivider,
    uint32_t                    disparityFrameOffsetValue
)
{
    uint16_t *pMinDisparity = iConfidenceAndMinDisparityMap.get();
    uint16_t* pMaxDisparity = iConfidenceAndMaxDisparityMap.get();

    // The maximum depth is MAX_SHORT and not USHROT because of SSE which works with signed short and not unsigned short
    // TODO: Check! SSE now does work with ushort!!!
    float maxDistanceF = std::min(float(std::numeric_limits<short>::max() - 1), iMaxDistance * iScale);

    if (iWithconfidence)
    {
        maxDistanceF = std::min(float(0x3fffu), maxDistanceF);
    }

    float minDistanceF = iMinDistance * iScale;


    const float centerX = float(iWidth) / 2.0f;
    const float centerY = float(iHeight) / 2.0f;

    for (unsigned int y = 0; y < iHeight; y++)
    {
        for (unsigned int x = 0; x < iWidth; x++)
        {
            uint16_t confidenceThreshold;

            // confidence

            if (x < iConfidneceStartRegion2 || x > iConfidneceEndRegion2)
            {
                confidenceThreshold = iConfidenceThresholdRegion1;
            }
            else
            {
                confidenceThreshold = iConfidenceThresholdRegion2;
            }

            // calculate radial disparity

            float rad = std::sqrt(1.0f
                + (((x - centerX) * (x - centerX)) / (iFocalLengthX * iFocalLengthX))
                + (((y - centerY) * (y - centerY)) / (iFocalLengthY * iFocalLengthY))
            );

            #pragma region Max Depth / Min Disparity

            uint32_t dispMin = 0x3fffu;

            if (maxDistanceF)
            {
                float  minDispF = (((iScale * opticalFactor) / maxDistanceF) * rad - disparityOffset) * disparityDivider - disparityFrameOffsetValue;
                dispMin = static_cast<uint32_t>(minDispF + 0.5f);
            }

            // bonus: Calculate depth
            //float maxDepthF = maxDistanceF / rad;

            // As the depth value decreases, the disparity - increases. However, there is an upper max to
            // the disparity value that may be returned by the chip, so we cap it's value here.

            uint16_t dispValueMin = 0u
                | std::min(confidenceThreshold, uint16_t(confidenceMask >> confidenceShift)) << confidenceShift
                | (std::min(dispMin, uint32_t(disparityMask >> disparityShift)) << disparityShift) & disparityMask
                ;

            // save

            *pMinDisparity++ = dispValueMin;

            #pragma endregion

            #pragma region Min Depth / Max Disparity

            uint32_t dispMax = 0x3fffu;

            if (iMinDistance)
            {
                float  maxDispF = (((iScale * opticalFactor) / minDistanceF) * rad - disparityOffset) * disparityDivider - disparityFrameOffsetValue;
                dispMax = static_cast<uint32_t>(maxDispF + 0.5f);
            }

            // bonus: Calculate depth
            //float minDepthF = maxDistanceF / rad;

            // As the depth value decreases, the disparity - increases. However, there is an upper max to
            // the disparity value that may be returned by the chip, so we cap it's value here.

            uint16_t dispValueMax = 0u
                | std::min(confidenceThreshold, uint16_t(confidenceMask >> confidenceShift)) << confidenceShift
                | (std::min(dispMax, uint32_t(disparityMask >> disparityShift)) << disparityShift) & disparityMask
                ;

            // save

            *pMaxDisparity++ = dispValueMax;

            #pragma endregion
        }
    }

    return true;
}

uint32_t CAlgoD2D::Depth2DistanceDepth(
    uint16_t* outBuffer,
    const uint16_t* inBuffer,
    const std::shared_ptr<uint16_t> iMaxDistanceMap,
    const std::shared_ptr<uint16_t> iMinDistanceMap,
    uint32_t                        iBufferSize,
    uint16_t                        iNonValidDepthValue,
    float                           iScale
)
{
    if (!iMaxDistanceMap || !iMinDistanceMap)
    {
        return -1;
    }

    uint32_t numberOfInvalidDepthPixles{ 0 };
    const uint16_t* pMaxDistance = iMaxDistanceMap.get();
    const uint16_t* pMinDistance = iMinDistanceMap.get();
    uint16_t* pOutput = outBuffer;

    const uint16_t* pInputEnd = inBuffer + iBufferSize;
    for (const uint16_t* pInput = inBuffer; pInput < pInputEnd; pInput++, pOutput++, pMaxDistance++, pMinDistance++)
    {
        const uint16_t input = (uint16_t)((*pInput) * iScale + 0.5f);
        int depth = InuCommon::COsUtilities::bitselect(input > int(*pMaxDistance), iNonValidDepthValue, input);
        depth = InuCommon::COsUtilities::bitselect(input < int(*pMinDistance), iNonValidDepthValue, depth);
        *pOutput = depth;
        numberOfInvalidDepthPixles += !(depth ^ iNonValidDepthValue);
    }

    return numberOfInvalidDepthPixles;
}
