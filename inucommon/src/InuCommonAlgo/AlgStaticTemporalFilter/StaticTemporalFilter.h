#pragma once

#include <stdint.h>

#include "StaticTemporalFilterBaseDLL.h"
#include "config.h"

class CStaticTemporalFilter;
typedef void(CStaticTemporalFilter::* WorkerFunc_t)(
    uint16_t* iFramePtr,
    uint16_t* iExitingFramePtr,
    uint16_t* oFrame,
    std::vector<uint16_t*>& iImageHistoryPtr,
    bool ibOpenMP
);

class CStaticTemporalFilter : public CStaticTemporalFilterBase
{
    friend class CStaticTemporalFilterBase;

protected:

    // Params
    unsigned int mnFilterlength;
    bool mbAll;
    bool mbOpenMP;
    unsigned int mnThreadNum;
    unsigned int mFamily;

    // Working data
    unsigned int mnImgWidth;
    int mnImgHeight;
    unsigned int mnImgSize;
    bool mbInit;
    unsigned int mnFrameid;
    std::vector<std::vector<uint16_t>> mvHistoryLists;

    // Optimized code working data (only used by filter_worker3)
    std::vector<std::shared_ptr<uint16_t>> mImageHistory;
    std::vector<uint16_t*> mImageHistoryPtr;

    // filter_worker3

    struct pixelInfo_t
    {
        uint8_t nItem;
        uint32_t sum;

        inline int avg()
        {
            return sum / nItem;
        }
    };

    std::vector<pixelInfo_t> mPixelInfo;

    static std::vector<WorkerFunc_t> workers;
    static std::vector<WorkerFunc_t> workers_all;

    void init_median_lists(
        int size
    );

    virtual void filter_worker0(
        unsigned int nbegin,
        unsigned int nend,
        uint16_t* iFramePtr,
        uint16_t* iExitingFramePtr,
        uint16_t* oFrame,
        std::vector<uint16_t*> &iImageHistoryPtr,
        bool ibOpenMP
    );

    void filter_worker0_wrapper(
        uint16_t* iFramePtr,
        uint16_t* iExitingFramePtr,
        uint16_t* oFrame,
        std::vector<uint16_t*>& iImageHistoryPtr,
        bool ibOpenMP
    );

    virtual void filter_worker1(
        uint16_t* iFramePtr,
        uint16_t* iExitingFramePtr,
        uint16_t* oFrame,
        std::vector<uint16_t*>& iImageHistoryPtr,
        bool ibOpenMP
    );

    virtual void filter_worker1_all(
        uint16_t* iFramePtr,
        uint16_t* iExitingFramePtr,
        uint16_t* oFrame,
        std::vector<uint16_t*>& iImageHistoryPtr,
        bool ibOpenMP
    );

    virtual void filter_worker2(
        uint16_t* iFramePtr,
        uint16_t* iExitingFramePtr,
        uint16_t* oFrame,
        std::vector<uint16_t*>& iImageHistoryPtr,
        bool ibOpenMP
    );

    virtual void filter_worker2_all(
        uint16_t* iFramePtr,
        uint16_t* iExitingFramePtr,
        uint16_t* oFrame,
        std::vector<uint16_t*>& iImageHistoryPtr,
        bool ibOpenMP
    );

    virtual void filter_worker3(
        uint16_t* iFramePtr,
        uint16_t* iExitingFramePtr,
        uint16_t* oFrame,
        std::vector<uint16_t*>& iImageHistoryPtr,
        bool ibOpenMP
    );

    virtual void filter_worker3_all(
        uint16_t* iFramePtr,
        uint16_t* iExitingFramePtr,
        uint16_t* oFrame,
        std::vector<uint16_t*>& iImageHistoryPtr,
        bool ibOpenMP
    );

    virtual void SetWorkerType(unsigned int family);

    CStaticTemporalFilter(
        unsigned int family = WOREKER_TYPE_DEFAULT,
        bool openMP = OPENMP_DEFALUT
    );

    virtual void SetOpenMP(
        bool ibOpenMP
    );

public:

    virtual void SetParams(
        bool ibAll,
        unsigned int inThreadNum
    );

    virtual void Init(
        unsigned int iImgWidth,
        unsigned int iImgHeight
    );

    virtual void process_frame(
        std::shared_ptr<uint16_t> iFrame,
        uint16_t*oFrame
    );

    virtual void process_frame2(
        uint16_t* iFrame,
        uint16_t* iExitingFrame,
        uint16_t* oFrame,
        std::vector<uint16_t*>& iImageHistoryPtr
    );
};

#if USE_SSE

class CStaticTemporalFilterSSE : public CStaticTemporalFilterBase
{
public:

    CStaticTemporalFilterSSE(int family, bool openMP);
    virtual ~CStaticTemporalFilterSSE();
};

#endif

#if USE_AVX && !defined(AVX_IS_NOT_SUPPORTED) && defined(__AVX2__)

class CStaticTemporalFilterAVX : public CStaticTemporalFilterBase
{
public:
    CStaticTemporalFilterAVX(
        int family,
        bool openMP
    );

    virtual ~CStaticTemporalFilterAVX();
};

#endif
