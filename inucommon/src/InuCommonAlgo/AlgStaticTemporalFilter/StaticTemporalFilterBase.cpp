#include "StaticTemporalFilter.h"

#include "Logger.h"

#define APP_NAME "AlgStaticTemporalFilter"

CStaticTemporalFilterBase::CStaticTemporalFilterBase()
{
    // Empty
}

CStaticTemporalFilterBase::~CStaticTemporalFilterBase()
{
    // Empty
}


class CStaticTemporalFilter;
#if USE_SSE
class CStaticTemporalFilterSSE;
#endif
#if USE_AVX && !defined(AVX_IS_NOT_SUPPORTED) && defined(__AVX2__)
class CStaticTemporalFilterAVX;
#endif

CStaticTemporalFilterBase* CStaticTemporalFilterBase::Factory(
    InuCommon::COsUtilities::EOptimizationLevel iOptimizationLevel,
    unsigned int family,
    bool openMP
)
{
    switch (iOptimizationLevel)
    {
    case InuCommon::COsUtilities::EOptimizationLevel::eNone:

        return dynamic_cast<CStaticTemporalFilterBase*>(new CStaticTemporalFilter(family, openMP));

#if USE_SSE && !defined(SSE_IS_NOT_SUPPORTED)
    case InuCommon::COsUtilities::EOptimizationLevel::eSSE:

        return dynamic_cast<CStaticTemporalFilterBase*>(new CStaticTemporalFilterSSE(family, openMP));
#endif
#if USE_AVX && !defined(AVX_IS_NOT_SUPPORTED) && defined(__AVX2__)
    case InuCommon::COsUtilities::EOptimizationLevel::eAVX:

        return dynamic_cast<CStaticTemporalFilterBase*>(new CStaticTemporalFilterAVX(family, openMP));
#endif

    default:
        break;
    }

    LOGGER_ERROR("Invalid optimization level provided", APP_NAME);
    return nullptr;
}
