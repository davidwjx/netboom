#pragma once

#include <vector>
#include <memory>

#include "OSUtilities.h"
#include "CommonAlgo.h"

////////////////////////////////////////////////////////////////////////
/// \brief   Static Temporal Filter public interface
///
////////////////////////////////////////////////////////////////////////

// This class is exported from the dll
class COMMONALGO_API CStaticTemporalFilterBase
{
protected:

    /// <summary>
    /// Constructor
    /// </summary>
    CStaticTemporalFilterBase();

public:

    /// <summary>
    /// StaticTemporalFilter factory
    /// </summary>
    /// <param name="iOptimizationLevel">Naive(0), SSE(1) or AVX(2). Only Naitve is supported.</param>
    /// <param name="family">implenetation family. See code for details.</param>
    /// <param name="iOpenMP">true of OpenMP is to be used</param>
    /// <returns></returns>
    static CStaticTemporalFilterBase* Factory(
        InuCommon::COsUtilities::EOptimizationLevel iOptimizationLevel,
        unsigned int family = 3,
        bool iOpenMP = true
    );

    /// <summary>
    /// Destructor
    /// </summary>
    virtual ~CStaticTemporalFilterBase();

    /// <summary>
    /// Set StaticTemporalFilter working parameters
    /// </summary>
    /// <param name="ibAll">it true, avarages all history frames for all pixels,
    /// else only avarages history frames for invalid pixels</param>
    /// <param name="inThreadNum">Number of thread to run for naive algorithm/family "0"</param>
    virtual void SetParams(
        bool ibAll,
        unsigned int inThreadNum = 1
    ) = 0;

    /// <summary>
    /// Initiazlies image size info.
    /// </summary>
    /// Must be invoked immediatly after Factory(), and before any other method
    /// <param name="iImgWidth"></param>
    /// <param name="iImgHeigh"></param>
    virtual void Init(
        unsigned int iImgWidth,
        unsigned int iImgHeigh
    ) = 0;

    /// <summary>
    /// Process shared-pointer frame
    /// </summary>
    /// To be invoked by tester only
    /// <param name="iFrame"></param>
    /// <param name="oFrame"></param>
    virtual void process_frame(
        std::shared_ptr<uint16_t> iFrame,
        uint16_t* oFrame
    ) = 0;

    /// <summary>
    /// Process frame
    /// </summary>
    /// <param name="iFrame">Pointer to input frame</param>
    /// <param name="iExitingFrame">Pointer to frame leaving history (only used for family==3)</param>
    /// <param name="oFrame">Output frame pointer</param>
    /// <param name="iImageHistoryPtr">Reference to history buffer vector</param>
    virtual void process_frame2(
        uint16_t* iFrame,
        uint16_t* iExitingFrame,
        uint16_t* oFrame,
        std::vector<uint16_t*>& iImageHistoryPtr
    ) = 0;
};
