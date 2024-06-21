#if defined(__arm__) || defined(__aarch64__)
#define SKIP_SHORTCUTS                                      (1)
#else
#define SKIP_SHORTCUTS                                      (0)
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

#ifndef NO_BASIC_ALGO
#include <opencv2/opencv.hpp>
#endif

using namespace InuCommon;
using namespace std;

#include <algorithm>
#include <iomanip>
#undef min
#undef max

unsigned short InuCommon::CAlgoBase::sNumOfDepthConfidenenceBits(2);

// Static LUT which are used to translate depth value to RGB
const unsigned char CAlgoBase::BLut[LUT_SIZE] =
{

      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
      4,   8,  12,  16,  20,  24,  28,  32,  36,  40,  44,  48,  52,  56,  60,  64,
     68,  72,  76,  80,  84,  88,  92,  96, 100, 104, 108, 112, 116, 120, 124, 128,
    132, 136, 140, 144, 148, 152, 156, 160, 164, 168, 172, 176 ,180, 184, 188, 192,
    196, 200, 204, 208, 212, 216, 220, 224, 228, 232, 236, 240, 244, 248, 252, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    252, 248, 244, 240, 236, 232, 228, 224, 220, 216, 212, 208, 204, 200, 196, 192,
    188, 184, 180, 176, 172, 168, 164, 160, 156, 152, 148, 144, 140, 136, 132, 128,
};
const unsigned char CAlgoBase::GLut[LUT_SIZE] =
{
      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
      4,   8,  12,  16,  20,  24,  28,  32,  36,  40,  44,  48,  52,  56,  60,  64,
     68,  72,  76,  80,  84,  88,  92,  96, 100, 104, 108, 112, 116, 120, 124, 128,
    132, 136, 140, 144, 148, 152, 156, 160, 164, 168, 172, 176, 180, 184, 188, 192,
    196, 200, 204, 208, 212, 216, 220, 224, 228, 232, 236, 240, 244, 248, 252, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    252, 248, 244, 240, 236, 232, 228, 224, 220, 216, 212, 208, 204, 200, 196, 192,
    188, 184, 180, 176, 172, 168, 164, 160, 156, 152, 148, 144, 140, 136, 132, 128,
    124, 120, 116, 112, 108, 104, 100,  96,  92,  88,  84,  80,  76,  72,  68,  64,
     60,  56,  52,  48,  44,  40,  36,  32,  28,  24,  20,  16,  12,   8,   4,   0,
      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
};

const unsigned char CAlgoBase::RLut[LUT_SIZE] =
{
      0, 136, 140, 144, 148, 152, 156, 160, 164, 168, 172, 176, 180, 184, 188, 192,
    196, 200, 204, 208, 212, 216, 220, 224, 228, 232, 236, 240, 244, 248, 252, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    252, 248, 244, 240, 236, 232, 228, 224, 220, 216, 212, 208, 204, 200, 196, 192,
    188, 184, 180, 176, 172, 168, 164, 160, 156, 152, 148, 144, 140, 136, 132, 128,
    124, 120, 116, 112, 108, 104, 100,  96,  92,  88,  84,  80,  76,  72,  68,  64,
     60,  56,  52,  48,  44,  40,  36,  32,  28,  24,  20,  16,  12,   8,   4,   0,
      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
};

/// \brief Histogram the disparity map
/// \param iDisparity
/// \param iWidth
/// \param iHeight
/// \param iDMin
/// \param iDMax
/// \return true if found a range, false otherwize
///
bool CAlgoBase::DisparityImrovementFastHistogram(
        unsigned short *histogram,
        unsigned short* iDisparity,
        unsigned int    iWidth,
        unsigned int    iHeight,
        unsigned int &  iDMin,
        unsigned int &  iDMax
    )
{
#if _MSC_VER > 1700

    // Size of disparity info as received
    constexpr int DISPARIY_SIZE_BITS = 16;

    // Max possible value for valid disparity (num of disparity machines)
    constexpr int MAX_DISPARIY_VALUE = 143;

    constexpr int WINDOW_SIZE = 20;

    // Log of number of bins (6 for 64 bins)
    constexpr int BIN_INDEX_SIZE = 6;

    // weight of window wrt total number of disparity values in depthimage
    constexpr float RATIO = 0.2f;

    // DISPARIY_SIZE_BITS - BIN_INDEX_SIZE
    constexpr unsigned int DISPARITY_SHIFT = 10;

    // ----------- calculated values --------------------
    // values mentiones are based on above values

    // size, in bits, of bin (4)
    constexpr int BIT_SIZE_BINS = (DISPARIY_SIZE_BITS - 8 - BIN_INDEX_SIZE);

    // size of bin (num disparities in bin) (4)
    constexpr int BIN_SIZE = (1u << BIT_SIZE_BINS);

    // Number of bins (64)
    constexpr int NUM_BINS = (1u << BIN_INDEX_SIZE);

    // Size of Window (5)
    constexpr int SLIDING_WINDOW_SIZE_BINS = (WINDOW_SIZE + BIN_SIZE - 1) >> BIT_SIZE_BINS;

    // Max dispariy bin (35)
    constexpr unsigned int MAX_DISPARITY_BIN = MAX_DISPARIY_VALUE >> BIT_SIZE_BINS;
#else

    // Size of disparity info as received
#define DISPARIY_SIZE_BITS (16)

    // Max possible value for valid disparity (num of disparity machines)
#define MAX_DISPARIY_VALUE (143)

#define WINDOW_SIZE (20)

    // Log of number of bins (6 for 64 bins)
#define BIN_INDEX_SIZE (6)

    // weight of window wrt total number of disparity values in depthimage
    #define RATIO (0.2f)

    // DISPARIY_SIZE_BITS - BIN_INDEX_SIZE
    #define DISPARITY_SHIFT (10)

    // ----------- calculated values --------------------
    // values mentiones are based on above values

    // size, in bits, of bin (4)
#define BIT_SIZE_BINS ((DISPARIY_SIZE_BITS - 8 - BIN_INDEX_SIZE))

    // size of bin (num disparities in bin) (16)
#define BIN_SIZE (1u << BIT_SIZE_BINS)

    // Number of bins (64)
#define NUM_BINS (1u << BIN_INDEX_SIZE)

    // Size of Window (5)
    #define SLIDING_WINDOW_SIZE_BINS ((WINDOW_SIZE + BIN_SIZE - 1) >> BIT_SIZE_BINS)

    // Max dispariy bin (35)
#define MAX_DISPARITY_BIN (MAX_DISPARIY_VALUE >> BIT_SIZE_BINS)

#endif

    // -------- histogram algorithm --------------

    unsigned short *dispP = iDisparity;

    std::fill_n(histogram, NUM_BINS, 0);

    for (unsigned int i = 0; i < iHeight; i++)
    {
        for (unsigned int j = 0; j < iWidth; j++)
        {
            unsigned short dispVal = *dispP++;

            int bin = dispVal >> DISPARITY_SHIFT;

            histogram[bin] += 1;
        }
    }

    unsigned int m = 0;
    for (unsigned int i_bin = 1; i_bin <= MAX_DISPARITY_BIN; i_bin++)
    {
        m += histogram[i_bin];
    }

    unsigned int n = 0;
    for (unsigned int i_bin = 0; i_bin < SLIDING_WINDOW_SIZE_BINS; i_bin++)
    {
        int bin = MAX_DISPARITY_BIN - SLIDING_WINDOW_SIZE_BINS + 2 + i_bin;
        n += histogram[bin];
    }

    for (unsigned int i_bin = MAX_DISPARITY_BIN - SLIDING_WINDOW_SIZE_BINS + 1; i_bin > 0; i_bin--)
    {
        int i1_bin = i_bin + SLIDING_WINDOW_SIZE_BINS;

        n += histogram[i_bin] - histogram[i1_bin];

        if (m * RATIO < n)
        {
            iDMax = ((i1_bin + 1) << DISPARITY_SHIFT) - 1u;
            iDMin = (i_bin << DISPARITY_SHIFT);

            //printf("local: %d %d\n", iDMin>>8, iDMax >> 8);

            return true;
        }
    }

    if (m * RATIO < (unsigned int)(histogram[MAX_DISPARITY_BIN] + histogram[MAX_DISPARITY_BIN - 1u]))
    {
        iDMax = ((MAX_DISPARITY_BIN + 1) << DISPARITY_SHIFT) - 1u;
        iDMin = (MAX_DISPARITY_BIN - 1u) << DISPARITY_SHIFT;

        //printf("  max: %d %d\n", iDMin >> 8, iDMax >> 8);

        return true;
    }

    return false;
}

void CAlgoBase::DisparityImrovementFastModeOneLine(
        unsigned short*         iDisparity,
        unsigned short *        disparityLineBuffer,
        const unsigned short*   iYimage,
        unsigned int            iWidth,
        unsigned short          iDMin,
        unsigned short          iDMax,
        unsigned int            iThreshold
    )
{
    // Fill disparity holes

#define DISPARITY_IN_RANGE(_v) ((_v) >= iDMin && (_v) <= iDMax)

    unsigned short *dispP = iDisparity;
    const unsigned short *imageP = iYimage;

    unsigned short updatedCurrentDispVal;
    unsigned short currentInputDispVal;
    unsigned short adjacentLeftInputDisp;
    unsigned short lastUpdatedDispVal;

    adjacentLeftInputDisp = lastUpdatedDispVal = (unsigned short)((iDMin + iDMax) / 2); // initial "last" value

    for (unsigned int i = 0; i < iWidth; i++)
    {
        currentInputDispVal = *dispP;

        bool d0_inRange = DISPARITY_IN_RANGE(currentInputDispVal);

        if (d0_inRange)
        {
            updatedCurrentDispVal = currentInputDispVal;
        }
        else
        {
            // out of range

            unsigned short d_up = *disparityLineBuffer; // prev disp value, one line up
            unsigned short d_left = adjacentLeftInputDisp; // adjacent in line

            bool d_up_inRange = DISPARITY_IN_RANGE(d_up);
            bool d_left_inRange = DISPARITY_IN_RANGE(d_left);

#define LINE_UP     (1<<1)
#define PIXEL_LEFT  (1<<0)

            int mode = (d_up_inRange ? LINE_UP : 0) + (d_left_inRange ? PIXEL_LEFT : 0);

            switch (mode)
            {
            case 0: // both out of range

                d_up = *(disparityLineBuffer + iWidth);
                d_left = lastUpdatedDispVal;

                /* fallthrough */

            case (PIXEL_LEFT + LINE_UP): // both are in range

                updatedCurrentDispVal = (d_up + d_left) / 2;
                break;

            case PIXEL_LEFT: // only adjacent in line is in range

                updatedCurrentDispVal = d_left;
                break;

            case LINE_UP: // only pixel one line above is in range

                updatedCurrentDispVal = d_up;
                break;
            }
        }

        *(disparityLineBuffer + iWidth) = updatedCurrentDispVal;

        // mask / threshold Y value

        if (*imageP++ < iThreshold)
        {
            updatedCurrentDispVal = 0xff00u;
        }
        else
        {
            adjacentLeftInputDisp = currentInputDispVal;
            lastUpdatedDispVal = updatedCurrentDispVal;
        }

        *dispP++ = updatedCurrentDispVal; // update
        *disparityLineBuffer++ = currentInputDispVal; // save original
    }
}

void CAlgoBase::DisparityImrovementFastMode(
        unsigned short* iDisparity,
        const unsigned short*               iYimage,
        unsigned int                        iWidth,
        unsigned int                        iHeight,
        unsigned int                        iAlg,
        unsigned int                        iThreshold,
        unsigned int                        iMinDisparity,
        unsigned int                        iMaxDisparity,
        unsigned int                        iDecimation,
        std::shared_ptr<bool>               iWorkingBuffer,
        std::shared_ptr<unsigned short>     ioFillHolesBuffer,
        COsUtilities::EOptimizationLevel    iRequestedLevel
    )
{
#if !defined(__arm__) || defined(__ARM_NEON__)
//  if (InuCommon::COsUtilities::AVXIsSupported(iImageSize, iRequestedLevel))
//     {
//         CAlgoAvx::DisparityImrovementFastMode(iDisparity, iYimage, iImageSize);
//     }

    if (false) //InuCommon::COsUtilities::SSE4IsSupported(iWidth*iHeight, iRequestedLevel))
    {
        __m128i* pDisparity = (__m128i*)iDisparity;
        __m128i* pY = (__m128i*)iYimage;
        static const __m128i  THRESHOLD_128=_mm_set1_epi16((unsigned short)iThreshold);
        static const __m128i  DEPTH_RESET=_mm_set1_epi16((unsigned short)0xFF00);

        for (int i=iWidth*iHeight ; i> 0 ; i-=8)
        {
            __m128i  mask=_mm_cmplt_epi16(*pY++, THRESHOLD_128);    // true if greater than threshold
            __m128i  mask255 = _mm_and_si128(mask, DEPTH_RESET);
            *pDisparity = _mm_andnot_si128(mask, *pDisparity);
            *pDisparity = _mm_or_si128(mask255, *pDisparity);
            pDisparity++;
        }
    }

    else // Naive
    {
        unsigned short* pDisparity = iDisparity;
        const unsigned short* pY = iYimage;

        unsigned int yWidth = (iDecimation-1) * (iWidth * iDecimation);

        if (iAlg & eMaskY)
        {
            // In this case a mask of +- one pixel of each direction is examined. It should remove isolated valid pixels.

            // Prepare mask buffer (per channel ID)

            // First pass mark all pixel that should be removed by mask
            // Ignore First row
            bool* mask = iWorkingBuffer.get();
            mask += iWidth;

            pY += iWidth * iDecimation;
            const unsigned short* pYPrev = pY - iWidth;
            const unsigned short* pYNext = pY + iWidth;


            for (int i=iHeight-2 ; i > 0 ; i--)
            {
                // Ignore First Elem in row
                pY += iDecimation;
                pYPrev += iDecimation;
                pYNext += iDecimation;
                mask++;

                for (int j=iWidth-2 ; j > 0 ; j--)
                {
                    *mask = (*pY < iThreshold || (*pYPrev < iThreshold || *pYNext < iThreshold || *(pY - 1) < iThreshold || *(pY + 1) < iThreshold));

                    pY += iDecimation;
                    pYPrev += iDecimation;
                    pYNext += iDecimation;
                    mask++;
                }

                // Ignore Last Elem in row and jump to next row
                pY += (iDecimation + yWidth);
                pYPrev += (iDecimation + yWidth);
                pYNext += (iDecimation + yWidth);
                mask++;
            }

            // Second pass, delete all disparity pixels the the mask is true
            mask = iWorkingBuffer.get();
            const bool* pYPrevMask = iWorkingBuffer.get();
            const bool* pYNextMask = iWorkingBuffer.get() + 2 * iWidth;

            // Ignore First row
            mask += iWidth;
            pDisparity += iWidth;

            for (int i=iHeight-2 ; i > 0 ; i--)
            {
                // Ignore First Elem in row
                pDisparity++;
                pYPrevMask++;
                pYNextMask++;
                mask++;

                for (int j=iWidth-2 ; j > 0 ; j--)
                {
                    if (*mask && *pYPrevMask && *pYNextMask &&  *(mask-1) && *(mask+1) )
                    {
                        *pDisparity = 0xFF00;
                    }
                    pDisparity++;
                    pYPrevMask++;
                    pYNextMask++;
                    mask++;
                }

                // Ignore last Elem in row
                pDisparity++;
                pYPrevMask++;
                pYNextMask++;
                mask++;
            }
        }

        pDisparity = iDisparity;
        pY = iYimage;
        if (iAlg & eFillHoles)
        {
            iMinDisparity <<= 8;
            iMaxDisparity <<= 8;

            //unsigned short *histogram = ioFillHolesBuffer.get();

            ///* bool res = */ DisparityImrovementFastHistogram(
            //      histogram,
            //      pDisparity,
            //      iWidth,
            //      iHeight,
            //      iMinDisparity,
            //      iMaxDisparity
            //  );


            //iMinDisparity = 33 << 8; // 1m
            //iMaxDisparity = 140 << 8; // 24cm

            // two lines: first for previous disparity value, other for updated values
            unsigned short *disparityLineBuffer = ioFillHolesBuffer.get() + 64;

            std::fill_n(disparityLineBuffer, 2*iWidth, (iMinDisparity + iMaxDisparity) / 2);

            for (unsigned int i = 0; i < iHeight; i++)
            {
                DisparityImrovementFastModeOneLine(
                    pDisparity,
                    disparityLineBuffer,
                    pY,
                    iWidth,
                    iMinDisparity,
                    iMaxDisparity,
                    iThreshold
                    );

                pDisparity += iWidth;
                pY += iWidth;
            }
        }
        else
        {
            // Reset any Disparity pixel that has low Y than Threshold
            for (int i=iHeight ; i > 0 ; i--)
            {
                for (int j=iWidth ; j > 0 ; j--)
                {
                    if (*pY < iThreshold)
                    {
                        *pDisparity = 0xFF00;
                    }
                    pY += iDecimation;
                    pDisparity++;
                }
                pY += yWidth;
            }
        }
    }
#endif
}

bool CAlgoBase::WritePLYHeader(const string& fileName, int nVertex, bool depthOnly, bool binary)
{
    ofstream outPly;
    outPly.open (fileName);
    if (!outPly.good())
    {
        return false;
    }

    outPly << "ply\n";

    if (binary)
    {
        outPly << "format binary_little_endian 1.0\n";
    }
    else
    {
        outPly << "format ascii 1.0\n";
    }

    outPly << "comment CSE generated\n";
    //outPly << "element vertex  " << nVertex + 5 << "\n";
    outPly << "element vertex  " << nVertex << "\n";
    outPly << "property float x\n";
    outPly << "property float y\n";
    outPly << "property float z\n";
    if (!depthOnly)
    {
        outPly << "property uchar red\n";
        outPly << "property uchar green\n";
        outPly << "property uchar blue\n";
        outPly << "property uchar alpha\n";
    }
    outPly << "end_header\n";

    outPly.close();
    if (!outPly.good())
    {
        return false;
    }

    return true;
}

#ifndef NO_BASIC_ALGO

uint32_t CAlgoBase::Register3D(
    const cv::Mat& matXYZ,
    const cv::Mat& matRGBRef,
    std::shared_ptr<uint8_t> oBuff,
    uint32_t buffSize,
    bool iConfidence,
    const std::pair<unsigned int,
    unsigned int>& iDecimationFactor
)
{
    if (!oBuff || !buffSize || !iDecimationFactor.first || !iDecimationFactor.second)
    {
        return 0;
    }

    uint32_t size=0;
    const uint32_t pixelSize = iConfidence ?  4 * sizeof(float) : 3 * sizeof(float);
    const uint32_t rgbChannels = matRGBRef.channels();
    const uint32_t w = matXYZ.cols / 4;
    const uint32_t h = matXYZ.rows;
    uint8_t* bufferPtr = oBuff.get();

	// Need to run line by line
	for (unsigned int j = 0; j < h; j += iDecimationFactor.second)
	{
		const float *pXYZ = matXYZ.ptr<float>(j);
		const uchar *pRGB = matRGBRef.ptr<uchar>(j);

		for (uint32_t i = 0; i < w; i += iDecimationFactor.first)
		{
			if (*pXYZ++ > 0) // valid point
			{
				COsUtilities::memcpy(bufferPtr, pixelSize, pXYZ, pixelSize);
				bufferPtr += pixelSize;
				*bufferPtr++ = *(pRGB + 2);
				*bufferPtr++ = *(pRGB + 1);
				*bufferPtr++ = *pRGB;
				*bufferPtr++ = ALPHA_TRANSPARENT;
				size ++;
			}
			pXYZ += 3;
			pRGB += rgbChannels;
		}
	}

	return size;
}

bool CAlgoBase::WriteToPly(const cv::Mat& matXYZ, const cv::Mat& matRGBRef, const string& outName, bool binary_only)
{
    // count number of valid points
    int nVertex = 0;
    int w = matXYZ.cols /4;
    int h = matXYZ.rows;

    const float *pInd = matXYZ.ptr<float>(0);
    for (int j= h*w; j >0 ; j--, pInd+=4)
    {
        if (*pInd > 0)
        {
            nVertex++;
        }
    }

    // write ply header
    ofstream outPly, outCsv;

    if (!binary_only)
    {
        outCsv.open(outName+".csv");
        if (!outCsv.good())
        {
            return false;
        }

        if (outCsv.is_open())
        {
            outCsv << "row" << "," << "col" << "," << "X" << "," << "Y" << "," << "Z" << "," << "R" << "," << "G" << "," << "B" << "\n";
        }
    }

    bool bBinary = true;
    if (!WritePLYHeader(outName, nVertex, false, bBinary))
    {
        return false;
    }

    static const int MAX_BUFFER_SIZE = 1280*920*8*sizeof(float);  // Number of pixels * number of items per pixel * max size of item
    static unique_ptr<char[]> buffer(new char[MAX_BUFFER_SIZE]);

    // Initialize output buffer
    char* bufferPtr=buffer.get();
    memset(bufferPtr, 0, MAX_BUFFER_SIZE);

    // second scan - write points to ply
    int size=0;
    int pixelSize = 3*sizeof(float);
    int rgbChannels = matRGBRef.channels();

    if (matRGBRef.isContinuous() && matXYZ.isContinuous())
    {
        const float *pXYZ = matXYZ.ptr<float>(0);
        const uchar *pRGB = matRGBRef.ptr<uchar>(0);

        for (int j=h*w; j>0; j--)
        {
            if (*pXYZ++ > 0) // valid point
            {
                COsUtilities::memcpy(bufferPtr, pixelSize, pXYZ, pixelSize);
                bufferPtr += pixelSize;
                *bufferPtr++ = *(pRGB+2);
                *bufferPtr++ = *(pRGB+1);
                *bufferPtr++ = *pRGB;
                *bufferPtr++ = ALPHA_TRANSPARENT;
                size += pixelSize + 4;
            }
            pXYZ += 3;
            pRGB += rgbChannels;
        }
    }
    else
    {
        // Need to run line by line
        for (int j= 0;j<h; j++)
        {
            const float *pXYZ = matXYZ.ptr<float>(j);
            const uchar *pRGB =  matRGBRef.ptr<uchar>(j);

            for (int i=0;i<w;i++)
            {
                if (*pXYZ++ > 0) // valid point
                {
                    COsUtilities::memcpy(bufferPtr, pixelSize, pXYZ, pixelSize);
                    bufferPtr += pixelSize;
                    *bufferPtr++ = *(pRGB+2);
                    *bufferPtr++ = *(pRGB+1);
                    *bufferPtr++ = *pRGB;
                    *bufferPtr++ = ALPHA_TRANSPARENT;
                    size += pixelSize + 4;
                }
                pXYZ += 3;
                pRGB += rgbChannels;
            }
        }
    }

    // reopen file for writing data
    if (bBinary)
        outPly.open (outName, ios::binary | ios::app);
    else
        outPly.open (outName, ios::app);

    if (!outPly.good())
    {
        return false;
    }

    outPly.write(buffer.get(), size);
    outPly.close();
    if (!outPly.good())
    {
        return false;
    }

    // save also CSV file for debugging
    if (!binary_only && outCsv.is_open())
    {
        // format "%0.3f"
        outCsv << fixed;
        outCsv.precision(3);

        for (int j= 0;j<h; j++)
        {
            const float *pXYZ = matXYZ.ptr<float>(j);
            const uchar *pRGB =  matRGBRef.ptr<uchar>(j);

            for (int i=0;i<w;i++)
            {
                if (*pXYZ > 0) // valid point
                {
                    outCsv << j << "," << i << "," << pXYZ[0] << "," << pXYZ[1] << "," << pXYZ[2] << ","
                        << int(*(pRGB + 2)) << "," << int(*(pRGB + 1)) << "," << int(*(pRGB)) << "\n";
                }
                pXYZ += 3;
                pRGB += rgbChannels;
            }
        }
    }

    return true;
}

bool CAlgoBase::WriteToPly(const float* pXYZ, int iNumberOfVerteces, const std::string& outName, bool iConfidence)
// Create PLY out of point cloud that was created by Depth2XYZ
{
    // write ply header
    ofstream outPly;
    if (!WritePLYHeader(outName, iNumberOfVerteces, false, true))
    {
        return false;
    }

    static const int MAX_BUFFER_SIZE = 1280*920*8*sizeof(float);  // Number of pixels * number of items per pixel * max size of item
    static unique_ptr<char[]> buffer(new char[MAX_BUFFER_SIZE]);

    // Initialize output buffer
    char* bufferPtr=buffer.get();
    memset(bufferPtr, 0, MAX_BUFFER_SIZE);

    int stepSize = iConfidence ? 4 : 3;

    // Find minimum maximum buffer depth value in order to normalize values
    float maxDepth = std::numeric_limits<float>::min();
    float minDepth = std::numeric_limits<float>::max();
    const float* pZ = pXYZ;
    for (int j = iNumberOfVerteces; j > 0; j--, pZ+= stepSize)
    {
        if (pZ[2] < minDepth)
        {
            minDepth = pZ[2];
        }
        if (pZ[2] > maxDepth)
        {
            maxDepth = pZ[2];
        }
    }

    float nomrmalizeFactor = (maxDepth - minDepth);

    if (std::abs(nomrmalizeFactor) < std::numeric_limits<float>::epsilon())
    {
        return false;
    }

    // second scan - write points to ply
    int size=0;

    for (int j= iNumberOfVerteces; j>0; j--)
    {
        COsUtilities::memcpy(bufferPtr, 3 * sizeof(float), pXYZ, 3 * sizeof(float));
        bufferPtr += 3 * sizeof(float);

        int noramlizedZ = int(255.0f * (pXYZ[2] - minDepth) / nomrmalizeFactor + 0.5f);
        // The order matches to Meshlab display
        *bufferPtr++ = RLut[noramlizedZ];
        *bufferPtr++ = GLut[noramlizedZ];
        *bufferPtr++ = BLut[noramlizedZ];
        *bufferPtr++ = ALPHA_TRANSPARENT;

        size += 3 * sizeof(float) + 4;

        pXYZ += stepSize;
    }

    // reopen file for writing data
    outPly.open (outName, ios::binary | ios::app);
    if (!outPly.good())
    {
        return false;
    }

    outPly.write(buffer.get(), size);


    outPly.close();
    if (!outPly.good())
    {
        return false;
    }

    return true;
}

#endif

#ifndef NO_BASIC_ALGO

bool CAlgoBase::ParseiregData(const string& iregfile, unsigned short &dispoffset)
{
    using std::string;
    using std::ifstream;

    string curr_line;
    ifstream infile;
    infile.open (iregfile);
    unsigned short mindisp = 0;
    if (!infile.good())
    {
        return false;
    }
    size_t eq_index;
    string sub_line;
    string iregval_val;
    while (!infile.eof())
    {
        getline(infile,curr_line);
        sub_line = curr_line.substr(0,14);
        if(strcmp(sub_line.c_str(),"REG 0x08010070") == 0 )
        {
            eq_index = curr_line.find_last_of("x");
            iregval_val = curr_line.substr(eq_index+1, curr_line.length() - eq_index - 1);
            stringstream ss;
            ss << hex << uppercase << iregval_val;
            ss >> dispoffset;
            dispoffset = (dispoffset & 0xFF00)>>8;
        }
        if(strcmp(sub_line.c_str(),"REG 0x08010080") == 0 )
        {
            eq_index = curr_line.find_last_of("x");
            iregval_val = curr_line.substr(eq_index+1, curr_line.length() - eq_index - 1);
            stringstream ss;
            ss << hex << uppercase << iregval_val;
            ss >> mindisp;
            mindisp = (mindisp & 0xFF);
        }
    }
    if (mindisp>=dispoffset)
        dispoffset = mindisp-dispoffset;

    return true;
}
#endif

