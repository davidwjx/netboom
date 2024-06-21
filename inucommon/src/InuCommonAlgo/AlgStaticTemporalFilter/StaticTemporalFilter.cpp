#define NEW (1)

#include <iostream>
#include <inttypes.h>
#include <cassert>

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

#ifdef _MSC_VER
#include <opencv2/opencv.hpp>
#endif

#if defined(_OPENMP)
#include <omp.h>
#endif

#include "config.h"
#include "StaticTemporalFilter.h"

#define INVALID_PIXEL_VALUE (0)
#define PIXEL_IS_VALID(_p) (_p != INVALID_PIXEL_VALUE)

void CStaticTemporalFilter::SetOpenMP(bool openMP)
{
    mbOpenMP = openMP;
}

std::vector<WorkerFunc_t> CStaticTemporalFilter::workers =
{
    &CStaticTemporalFilter::filter_worker0_wrapper, // mbAll internal to code
    &CStaticTemporalFilter::filter_worker1,
    &CStaticTemporalFilter::filter_worker2,
    &CStaticTemporalFilter::filter_worker3,
};

std::vector<WorkerFunc_t> CStaticTemporalFilter::workers_all =
{
    &CStaticTemporalFilter::filter_worker0_wrapper, // mbAll internal to code
    &CStaticTemporalFilter::filter_worker1_all,
    &CStaticTemporalFilter::filter_worker2_all,
    &CStaticTemporalFilter::filter_worker3_all,
};

void CStaticTemporalFilter::SetWorkerType(
    unsigned int family
)
{
    mFamily = family;
}

void CStaticTemporalFilter::filter_worker0(
    unsigned int nbegin,
    unsigned int nend,
    uint16_t* iFramePtr,
    uint16_t* iExitingFramePtr,
    uint16_t* oFramePtr,
    std::vector<uint16_t*>& iImageHistoryPtr,
    bool ibOpenMP
)
{
#ifdef _MSC_VER
    cv::Mat imgDst(mnImgHeight, mnImgWidth, CV_16UC1, oFramePtr);
    cv::Mat mFrame(mnImgHeight, mnImgWidth, CV_16UC1, iFramePtr);

    cv::MatConstIterator_<ushort> bgr_it = mFrame.begin<ushort>() + nbegin;
    cv::MatConstIterator_<ushort> bgr_end = mFrame.begin<ushort>() + nend;// frame.end  <ushort>();

    std::vector<std::vector<ushort>>::iterator his_it = mvHistoryLists.begin() + nbegin;
    cv::MatIterator_<ushort> dst_it = imgDst.begin<ushort>() + nbegin;

    for (; bgr_it != bgr_end; bgr_it++, dst_it++, his_it++)
    {
        (*his_it)[mnFrameid] = *bgr_it;
        ushort cur = *bgr_it;

        if (PIXEL_IS_VALID(cur) && !mbAll)
        {
            *dst_it = cur;
            continue;
        }

        double sum{ 0 };
        unsigned int nItem{ 0 };

        for (size_t m = 0; m < (*his_it).size(); m++)
        {
            if (PIXEL_IS_VALID((*his_it)[m]))
            {
                sum += (*his_it)[m];
                nItem++;
            }
        }

        if (nItem)
        {
            sum /= nItem;
            *dst_it = (int)sum;
        }
        else
        {
            *dst_it = cur;
        }
    }
#endif
}

void CStaticTemporalFilter::filter_worker0_wrapper(
    uint16_t* iFramePtr,
    uint16_t* iExitingFramePtr,
    uint16_t* oFrame,
    std::vector<uint16_t*>& iImageHistoryPtr,
    bool ibOpenMP
)
{

#if defined(_OPENMP)
    assert(!mbOpenMP || omp_get_max_threads() > 1);
    //std::cerr << "omp_get_max_threads() = " << omp_get_max_threads() << std::endl;
#endif

    if (mnThreadNum == 1)
    {
        filter_worker0(
            0,
            mnImgSize,
            iFramePtr,
            iExitingFramePtr,
            oFrame,
            mImageHistoryPtr,
            mbOpenMP
            );
    }
    else
    {
        // untested
#ifdef _MSC_VER 
        std::vector<std::thread*> vThreads;
        int nInter = mnImgSize / mnThreadNum;
        int b0 = 0;
        int b1 = nInter;
        for (uint i = 0; i < mnThreadNum; i++)
        {
            std::thread* th1 = new std::thread(
                &CStaticTemporalFilter::filter_worker0,
                this,
                b0,
                b1,
                iFramePtr,
                iExitingFramePtr,
                oFrame,
                mImageHistoryPtr,
                mbOpenMP
            );

            vThreads.push_back(th1);
            b0 = b1;
            b1 = b1 + nInter;

            if (i == mnThreadNum - 2)
            {
                b1 = mnImgSize;
            }
        }

        for (uint i = 0; i < mnThreadNum; i++)
        {
            vThreads[i]->join();
        }

        for (uint i = 0; i < mnThreadNum; i++)
        {
            delete vThreads[i];
        }
#endif
    }
}

void CStaticTemporalFilter::filter_worker1(
    uint16_t* iFramePtr,
    uint16_t* iExitingFramePtr,
    uint16_t* oFramePtr,
    std::vector<uint16_t*>& iImageHistoryPtr,
    bool ibOpenMP
)
{
#pragma omp parallel for if (ibOpenMP)
    for (int row = 0; row < mnImgHeight; row++)
    {
        int index = row * mnImgWidth;

        memcpy(
            oFramePtr + index,
            iFramePtr + index,
            mnImgWidth * sizeof(uint16_t)
        );

        for (unsigned int col = 0; col < mnImgWidth; col++, index++)
        {
            std::vector<uint16_t>& h = mvHistoryLists[index];

            uint16_t cur = iFramePtr[index];
            h[mnFrameid] = cur;

            if (PIXEL_IS_VALID(cur))
            {
                continue;
            }

            unsigned int sum{ 0 };
            unsigned int nItem{ 0 };

            for (unsigned int m = 0; m < mnFilterlength; m++)
            {
                if (PIXEL_IS_VALID(h[m]))
                {
                    sum += h[m];
                    nItem++;
                }
            }

            if (nItem)
            {
                oFramePtr[index] = int(float(sum) / nItem);
            }
        }
    }
}

void CStaticTemporalFilter::filter_worker1_all(
    uint16_t* iFramePtr,
    uint16_t* iExitingFramePtr,
    uint16_t* oFramePtr,
    std::vector<uint16_t*>& iImageHistoryPtr,
    bool ibOpenMP
)
{
#pragma omp parallel for if (ibOpenMP)
    for (int row = 0; row < mnImgHeight; row++)
    {
        int index = row * mnImgWidth;

        memcpy(
            oFramePtr + index,
            iFramePtr + index,
            mnImgWidth * sizeof(uint16_t)
        );

        for (unsigned int col = 0; col < mnImgWidth; col++, index++)
        {
            std::vector<uint16_t>& h = mvHistoryLists[index];

            uint16_t cur = iFramePtr[index];
            h[mnFrameid] = cur;

            unsigned int sum{ 0 };
            unsigned int nItem{ 0 };

            for (unsigned int m = 0; m < mnFilterlength; m++)
            {
                if (PIXEL_IS_VALID(h[m]))
                {
                    sum += h[m];
                    nItem++;
                }
            }

            if (nItem)
            {
                oFramePtr[index] = int(float(sum) / nItem);
            }
        }
    }
}

void CStaticTemporalFilter::filter_worker2(
    uint16_t* iFramePtr,
    uint16_t* iExitingFramePtr,
    uint16_t* oFramePtr,
    std::vector<uint16_t*>& iImageHistoryPtr,
    bool ibOpenMP
)
{
#pragma omp parallel for if (ibOpenMP)
    for (int row = 0; row < mnImgHeight; row++)
    {
        int index = row * mnImgWidth;

        memcpy(
            oFramePtr + index,
            iFramePtr + index,
            mnImgWidth * sizeof(uint16_t)
        );

        for (unsigned int col = 0; col < mnImgWidth; col++, index++)
        {
            uint16_t cur = iFramePtr[index];

            if (PIXEL_IS_VALID(cur))
            {
                continue;
            }

            unsigned int sum{ 0 };
            unsigned int nItem{ 0 };

            for (unsigned int m = 0; m < iImageHistoryPtr.size(); m++)
            {
                uint16_t v = iImageHistoryPtr[m][index];

                if (PIXEL_IS_VALID(v))
                {
                    sum += v;
                    nItem++;
                }
            }

            if (nItem)
            {
                oFramePtr[index] = sum / nItem;
            }
        }
    }
}

void CStaticTemporalFilter::filter_worker2_all(
    uint16_t* iFramePtr,
    uint16_t* iExitingFramePtr,
    uint16_t* oFramePtr,
    std::vector<uint16_t*>& iImageHistoryPtr,
    bool ibOpenMP
)
{
#pragma omp parallel for if (ibOpenMP)
    for (int row = 0; row < mnImgHeight; row++)
    {
        int index = row * mnImgWidth;

        memcpy(
            oFramePtr + index,
            iFramePtr + index,
            mnImgWidth * sizeof(uint16_t)
        );

        for (unsigned int col = 0; col < mnImgWidth; col++, index++)
        {
            unsigned int sum{ 0 };
            unsigned int nItem{ 0 };

            for (unsigned int m = 0; m < iImageHistoryPtr.size(); m++)
            {
                uint16_t v = iImageHistoryPtr[m][index];

                if (PIXEL_IS_VALID(v))
                {
                    sum += v;
                    nItem++;
                }
            }

            if (nItem)
            {
                oFramePtr[index] = sum / nItem;
            }
        }
    }
}

void CStaticTemporalFilter::filter_worker3(
    uint16_t* iFramePtr,
    uint16_t* iExitingFramePtr,
    uint16_t* oFramePtr,
    std::vector<uint16_t*>& iImageHistoryPtr,
    bool ibOpenMP
)
{
#pragma omp parallel for if (ibOpenMP)
    for (int row = 0; row < mnImgHeight; row++)
    {
        int index = row * mnImgWidth;

        memcpy(
            oFramePtr + index,
            iFramePtr + index,
            mnImgWidth * sizeof(uint16_t)
        );

        for (unsigned int col = 0; col < mnImgWidth; col++, index++)
        {
            pixelInfo_t& pi = mPixelInfo[index];

            uint16_t prevCur = iExitingFramePtr[index];
            if (PIXEL_IS_VALID(prevCur))
            {
                pi.sum -= prevCur;
                pi.nItem--;
            }

            uint16_t cur = iFramePtr[index];
            if (PIXEL_IS_VALID(cur))
            {
                pi.sum += cur;
                pi.nItem++;
            }

            if (PIXEL_IS_VALID(cur))
            {
                continue;
            }

            if (pi.nItem)
            {
                oFramePtr[index] = pi.avg();
            }
        }
    }
}

void CStaticTemporalFilter::filter_worker3_all(
    uint16_t* iFramePtr,
    uint16_t* iExitingFramePtr,
    uint16_t* oFramePtr,
    std::vector<uint16_t*>& iImageHistoryPtr,
    bool ibOpenMP
)
{
#pragma omp parallel for if (ibOpenMP)
    for (int row = 0; row < mnImgHeight; row++)
    {
        int index = row * mnImgWidth;

        memcpy(
            oFramePtr + index,
            iFramePtr + index,
            mnImgWidth * sizeof(uint16_t)
        );

        for (unsigned int col = 0; col < mnImgWidth; col++, index++)
        {
            pixelInfo_t& pi = mPixelInfo[index];

            uint16_t prevCur = iExitingFramePtr[index];
            if (PIXEL_IS_VALID(prevCur))
            {
                pi.sum -= prevCur;
                pi.nItem--;
            }

            uint16_t cur = iFramePtr[index];
            if (PIXEL_IS_VALID(cur))
            {
                pi.sum += cur;
                pi.nItem++;
            }

            if (pi.nItem)
            {
                oFramePtr[index] = pi.avg();
            }
        }
    }
}

void CStaticTemporalFilter::init_median_lists(int size)
{
    mnImgSize = size;
    mnFrameid = 0;

    switch (mFamily)
    {
    case 0:
    case 1:
    {
        std::vector<uint16_t> vec(mnFilterlength, 0);
        mvHistoryLists = std::vector<std::vector<uint16_t>>(size, vec);

#if 0
        // Not really needed - as vec is already initialzied to 0.
        // (but keeps the original code "spirit".)
        for (auto& vec : mvHistoryLists)
        {
            for (auto& pix : vec)
            {
                pix = std::rand();
            }
        }
#endif
        break;
    }

    case 3:

        mPixelInfo.resize(size);
        // fallthrough
#if __cplusplus >= 201703L
        [[fallthrough]];
#endif

    case 2:
    {
        std::shared_ptr<uint16_t> p = std::shared_ptr<uint16_t>(new uint16_t[size]);
        memset(p.get(), 0, size * sizeof(uint16_t));

        mImageHistory.resize(mnFilterlength);
        mImageHistoryPtr.resize(mnFilterlength);

        for (unsigned int i = 0; i < mnFilterlength; i++)
        {
            mImageHistory[i] = p;
            mImageHistoryPtr[i] = p.get();
        }

        break;
    }

    default:
        return;
    }

    mbInit = true;
}

void CStaticTemporalFilter::process_frame2(
    uint16_t* iFrame,
    uint16_t* iExitingFrame,
    uint16_t* oFrame,
    std::vector<uint16_t*>& iImageHistoryPtr
)
{
    auto& v = mbAll ? workers_all : workers;

    ((*this).*(v.at(mFamily)))(
        iFrame,
        iExitingFrame,
        oFrame,
        iImageHistoryPtr,
        mbOpenMP
    );
}

void CStaticTemporalFilter::process_frame(
    std::shared_ptr<uint16_t> iFrame,
    uint16_t* oFrame
)
{
    if (!mbInit)
    {
        init_median_lists(mnImgWidth * mnImgHeight);
    }

    uint16_t* framePtr{ iFrame.get() };
    uint16_t* exitingFramePtr{ nullptr };

    // This is a reference to an old frame - will be going out of scope,
    // so hold on to it for it's data for the duration of this method
    std::shared_ptr<uint16_t> existingFrame;

    if (mImageHistory.size())
    {
        existingFrame = mImageHistory[mnFrameid];
        exitingFramePtr = mImageHistoryPtr[mnFrameid];

        mImageHistory[mnFrameid] = iFrame; // hold on to this frame's data
        mImageHistoryPtr[mnFrameid] = framePtr;
    }

#if defined(_OPENMP)
    // Check if asked to do OpenMP, but compiled without...
    assert(!mbOpenMP || omp_get_max_threads() > 1);
#endif

    process_frame2(
        framePtr,
        exitingFramePtr,
        oFrame,
        mImageHistoryPtr
    );

    mnFrameid = (mnFrameid + 1) % mnFilterlength;
}

void CStaticTemporalFilter::SetParams(
    bool ibAll,
    unsigned int inThreadNum
)
{
    mbAll = ibAll;
    mnThreadNum = inThreadNum;
}

void CStaticTemporalFilter::Init(
    unsigned int iImgWidth,
    unsigned int iImgHeight
)
{
    if (!mbInit)
    {
        mnImgWidth = iImgWidth;
        mnImgHeight = iImgHeight;
        mnImgSize = mnImgHeight * mnImgWidth;

        init_median_lists(mnImgSize);
    }
}

CStaticTemporalFilter::CStaticTemporalFilter(
    unsigned int family,
    bool openMP
)
    : mbAll{ MB_ALL }
    , mnFilterlength{ MN_FILTER_LENGTH }
    , mnThreadNum{ MN_THREAD_NUM }
    , mbInit{ false }
    , mnFrameid{ 0 }
    , mnImgSize{ 0 }
{
    SetWorkerType(family);
    SetOpenMP(openMP);
}

#if USE_SSE
// SSE Code here

CStaticTemporalFilterSSE::CStaticTemporalFilterSSE(
    int family,
    bool openMP
)
{
}

CStaticTemporalFilterSSE::~CStaticTemporalFilterSSE() 
{
}
#endif

#if USE_AVX && !defined(AVX_IS_NOT_SUPPORTED) && defined(__AVX2__)
// AVX Code here

CStaticTemporalFilterAVX::CStaticTemporalFilterAVX(
    int family,
    bool openMP
)
{
}

CStaticTemporalFilterAVX::~CStaticTemporalFilterAVX()
{
}
#endif
