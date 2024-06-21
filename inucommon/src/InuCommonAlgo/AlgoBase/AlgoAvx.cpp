#include "AlgoAvx.h"

#include <algorithm>
#undef min
#undef max

using namespace InuCommon;

#if  !defined(AVX_IS_NOT_SUPPORTED) && !defined(__arm__) && !defined(__aarch64__)
#include <smmintrin.h>
#include <immintrin.h>
#endif

#define YUV2RGB_MAT_0_0   ((short int)1192)
#define YUV2RGB_MAT_0_1   ((short int)0)
#define YUV2RGB_MAT_0_2   ((short int)1634)
#define YUV2RGB_MAT_0_3   ((int)-228260)
#define YUV2RGB_MAT_1_0   ((short int)1192)
#define YUV2RGB_MAT_1_1   ((short int)-401)
#define YUV2RGB_MAT_1_2   ((short int)-833)
#define YUV2RGB_MAT_1_3   ((int)138830)
#define YUV2RGB_MAT_2_0   ((short int)1192)
#define YUV2RGB_MAT_2_1   ((short int)2065)
#define YUV2RGB_MAT_2_2   ((short int)0 )
#define YUV2RGB_MAT_2_3   ((int)-283480)

uint32_t CAlgoAvx::Disparity2Depth(
    unsigned short* outBuffer,
    const unsigned short* inBuffer,
    unsigned int inBufferSize,
    const CDepthOpticalData& opticalData,
    unsigned short disparityMask,
    unsigned short disparityShift,
    unsigned short disparityMaxValue,
    float disparityDivider,
    unsigned short confidenceMask,
    unsigned short confidenceShift,
    unsigned int confidenceThreshold,
    unsigned short mindepth, // unused
    unsigned short maxdepth,
    unsigned short nonValidDepthValue,
    uint32_t disparityFrameOffsetValue,
    unsigned short outputConfidenceShift,
    unsigned short outputDepthShift,
    float iScale
)
{
    uint32_t  numberOfInvalidDepthPixles = 0;

#if !defined(AVX_IS_NOT_SUPPORTED) && defined(__AVX2__)
    const unsigned short* inputBuffer=inBuffer;
    unsigned short* outputBuffer=outBuffer;


    const __m256i  CONFIDENCE_MASK   = _mm256_set1_epi16(confidenceMask);
    const __m256i  DISPARITY_MASK    = _mm256_set1_epi16(disparityMask);
    const __m256i  UPPER_BOUND       = _mm256_set1_epi16(maxdepth);

    const __m256i  confidenceThreshold256   = _mm256_set1_epi16((unsigned short)confidenceThreshold);            //AVX doesn't have cmplt, so to get the the equivalent with cmpgt, need to add 1 to take into account the equal situation
    const __m256i  upperLimit256            = _mm256_set1_epi16(disparityMaxValue + disparityFrameOffsetValue);
    const __m256i  frameOffset              = _mm256_set1_epi16(disparityFrameOffsetValue);

    static const __m256i  ZERO              = _mm256_set1_epi32(0);
    static const __m256  ZERO_DIVIDER       = _mm256_set1_ps(0.0000001f);
    static const __m256  ZERO_FLOAT         = _mm256_set1_ps(0);
    const        __m256  factor256          = _mm256_set1_ps(opticalData.mOpticalFactor);
    const        __m256  disparityOffset256 = _mm256_set1_ps(opticalData.mDisparityOffset);
    const        __m256  disparityDivider256 = _mm256_set1_ps(disparityDivider);
    const        __m256  scale256 = _mm256_set1_ps(iScale);

    __m256i notValidThreshold = _mm256_set1_epi16(nonValidDepthValue);

    numberOfInvalidDepthPixles = 0u;

    // Non-valids are counted in 16 bits quanta. As 8 are counted simultanously, we cannot handle images whose
    // number of invalid values will reach 65536 (16 bits.) In fact, we must be sure the number of invalids
    // will be less than 65536, hence, only 15 bits are useful.
    //
    // Max image size is, then:
    //
    //        16*32768 = 524288 pixels (2^19).
    //
    // Images of larger size will have to be processed in chunckes of that size.

    constexpr unsigned int step = 1u << 19;

    for (unsigned int i1 = 0; i1 < inBufferSize; i1 += step)
    {
        __m256i numNoValidAcc = _mm256_set1_epi16(0);

        for (unsigned int i = 0; i < std::min(inBufferSize - i1, step); i += 16)
        {
            __m256i confidence = _mm256_load_si256((__m256i*)inputBuffer);

            confidence = _mm256_and_si256(*(__m256i*)inputBuffer, CONFIDENCE_MASK);
            confidence = _mm256_srli_epi16(confidence, confidenceShift);

            __m256i disparity = _mm256_load_si256((__m256i*)inputBuffer);
            disparity = _mm256_and_si256(disparity, DISPARITY_MASK);
            disparity = _mm256_srli_epi16(disparity, disparityShift);

            // All values that are greater than max disparity should be zero
            __m256i  upperBound = _mm256_cmpgt_epi16(upperLimit256, disparity);
            disparity = _mm256_and_si256(upperBound, disparity);

            // All disparity values that are 0 should be invalidated (at the end)
            __m256i zeroDisparity = _mm256_cmpeq_epi16(disparity, ZERO);

            disparity = _mm256_add_epi16(disparity, frameOffset);                          //Add frame offset  after threshold check

            // Now we have 16 disparity value with high confidence.

            // Perform float calculation:

            // Low floats
            __m256i low = _mm256_unpacklo_epi16(disparity, ZERO);
            __m256  fInputLow = _mm256_cvtepi32_ps(low);
            __m256  maskedFactor = _mm256_cmp_ps(fInputLow, ZERO_FLOAT, _CMP_GT_OQ);    // true if greater than threshold
            maskedFactor = _mm256_and_ps(factor256, maskedFactor);

            fInputLow = _mm256_div_ps(fInputLow, disparityDivider256);
            fInputLow = _mm256_max_ps(fInputLow, ZERO_DIVIDER);
            fInputLow = _mm256_add_ps(fInputLow, disparityOffset256);
            fInputLow = _mm256_div_ps(maskedFactor, fInputLow);
            fInputLow = _mm256_mul_ps(fInputLow, scale256);
            fInputLow = _mm256_round_ps(fInputLow, _MM_FROUND_TO_NEAREST_INT);

            // High floats
            __m256i high = _mm256_unpackhi_epi16(disparity, ZERO);
            __m256  fInputHigh = _mm256_cvtepi32_ps(high);
            maskedFactor = _mm256_cmp_ps(fInputHigh, ZERO_FLOAT, _CMP_GT_OQ);    // true if greater than threshold
            maskedFactor = _mm256_and_ps(factor256, maskedFactor);

            fInputHigh = _mm256_div_ps(fInputHigh, disparityDivider256);
            fInputHigh = _mm256_max_ps(fInputHigh, ZERO_DIVIDER);
            fInputHigh = _mm256_add_ps(fInputHigh, disparityOffset256);
            fInputHigh = _mm256_div_ps(maskedFactor, fInputHigh);
            fInputHigh = _mm256_mul_ps(fInputHigh, scale256);
            fInputHigh = _mm256_round_ps(fInputHigh, _MM_FROUND_TO_NEAREST_INT);

            // Pack back to unsigned short
            __m256i lowInt = _mm256_cvtps_epi32(fInputLow);
            __m256i highInt = _mm256_cvtps_epi32(fInputHigh);

            __m256i result = _mm256_packs_epi32(lowInt, highInt);

            // Invalidate all depth that are greater than MAX DEPTH
            __m256i tmpUpper = _mm256_cmpgt_epi16(UPPER_BOUND, result);
            __m256i valid = _mm256_and_si256(result, tmpUpper);
            __m256i nonValid = _mm256_andnot_si256(tmpUpper, notValidThreshold);
            result = _mm256_or_si256(valid, nonValid);

            // Invalidate all 0 disparities
            valid = _mm256_andnot_si256(zeroDisparity, result);
            nonValid = _mm256_and_si256(zeroDisparity, notValidThreshold);
            result = _mm256_or_si256(valid, nonValid);

            // Invalidate all values with low confidence
            __m256i  lowerConf = _mm256_cmpgt_epi16(confidenceThreshold256, confidence);
            valid = _mm256_andnot_si256(lowerConf, result);
            nonValid = _mm256_and_si256(lowerConf, notValidThreshold);
            result = _mm256_or_si256(valid, nonValid);

            numNoValidAcc = _mm256_sub_epi16(numNoValidAcc, _mm256_cmpeq_epi16(result, notValidThreshold));

            // Add confidence (if required)
            confidence = _mm256_srli_epi16(confidence, outputConfidenceShift);
            result = _mm256_slli_epi16(result, outputDepthShift);
            result = _mm256_or_si256(confidence, result);

            _mm256_store_si256((__m256i*)outputBuffer, result);

            inputBuffer += 16;
            outputBuffer += 16;
        }

        __m256i t0 = _mm256_add_epi32(
            _mm256_unpacklo_epi16(numNoValidAcc, _mm256_set1_epi16(0)),
            _mm256_unpackhi_epi16(numNoValidAcc, _mm256_set1_epi16(0))
        );

        __m128i t1 = _mm256_extracti128_si256(t0, 0);
        __m128i t2 = _mm256_extracti128_si256(t0, 1);

        numberOfInvalidDepthPixles += (uint32_t)(
            0
            + _mm_extract_epi32(t1, 0)
            + _mm_extract_epi32(t1, 1)
            + _mm_extract_epi32(t1, 2)
            + _mm_extract_epi32(t1, 3)
            + _mm_extract_epi32(t2, 0)
            + _mm_extract_epi32(t2, 1)
            + _mm_extract_epi32(t2, 2)
            + _mm_extract_epi32(t2, 3)
            );
    }
#endif

    return numberOfInvalidDepthPixles;
}

void CAlgoAvx::DisparityImrovementFastMode(unsigned short* iDisparity,
                                const unsigned short* iYimage,
                                unsigned int iImageSize)
{
#if !defined(AVX_IS_NOT_SUPPORTED) && defined(__AVX2__)
    __m256i* pDisparity = (__m256i*)iDisparity;
    __m256i* pY = (__m256i*)iYimage;
    static const __m256i  THRESHOLD_128=_mm256_set1_epi16((unsigned short)6);
    static const __m256i  DEPTH_RESET=_mm256_set1_epi16((unsigned short)0xFF00);

    for (int i=iImageSize ; i> 0 ; i-=16)
    {
        __m256i  mask=_mm256_cmpgt_epi16(*pY++, THRESHOLD_128);    // true if greater than threshold
        __m256i  mask255 = _mm256_andnot_si256(mask, DEPTH_RESET);
        *pDisparity = _mm256_and_si256(mask, *pDisparity);
        *pDisparity = _mm256_or_si256(mask255, *pDisparity);
        pDisparity++;
    }
#endif
}


