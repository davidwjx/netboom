/*
* File - CFeaturesTrackingFrame.h
*
* This file is part of the Inuitive SDK
*
* Copyright (C) 2021 All rights reserved to Inuitive
*
*/

#ifndef __FEATURESTRACKING_CIIF_H__
#define __FEATURESTRACKING_CIIF_H__

#include "CommonUtilitiesDefs.h"
#include "FeaturesTrackingDataCommon.h"
#include <ostream>
#include <limits>

////////////////////////////////////////////////////////////////////////
 /// \brief    Represents CFeatures Tracking Frame Interface
 ///
 /// Responsibilities:
 /// 
 ////////////////////////////////////////////////////////////////////////

namespace InuCommon
{
    class COMMONUTILITIES_API CFeaturesTrackingCiif
    {
    public:
        enum ECaptureMode
        {
            eDecimal = 1,
            eHexadecimal =2,
            eBinary = 3
        }; 

        enum ERecordImage
        {
            eNone = 0,
            eLeft = 1,
            eRight = 2,
            eBoth = eLeft | eRight,
        };

        /// \brief    write Features Tracking Frame in CIIF format to File
        ///
        /// \param[in] iOutputType  EOutputType can be eParsed or eProcessed (eRaw doesn't support)
        /// \param[in] iCaptureMode  ECaptureMode 
        /// \param[in] iKeyPointNumberRight  Number of KeyPoints in Right Image
        /// \param[in] iKeyPointNumberLeft  Number of KeyPoints in Left Image
        /// \param[in] iImageWidth   Image Width
        /// \param[in] iImageHeight   Image Height
        /// \param[in] iFrameIndexForRecording  Frame Index For Recording
        /// \param[in] iData Features Tracking Data. Can be ParcedData of ProcessedData depends of iOutputType
        /// \param[in] iFileName  Recording File Name
        /// \param[in] iImage  Record left, right or both
        /// \return bool true if operation successfully completed.
        static bool write(
            FeaturesTracking::EOutputType iOutputType,
            ECaptureMode iCaptureMode,
            unsigned int iKeyPointNumber,
            unsigned int iKeyPointNumberRight,
            unsigned int iKeyPointNumberLeft,
            unsigned short iImageWidth,
            unsigned short iImageHeight,
            unsigned long long iFrameIndexForRecording,
            FeaturesTracking::EFeatureType iFeatureType,
            void* iData,
            std::string& iFileName,
            ERecordImage iImage
        );


        static bool ReconstructFrameFromRecordedData(
            std::string& oFileName,
            FeaturesTracking::EOutputType& oOutputType,
            ECaptureMode& oCaptureMode,
            unsigned int& oKeyPointNumber,
            unsigned int& oKeyPointNumberRight,
            unsigned int& oKeyPointNumberLeft,
            unsigned short& oImageWidth,
            unsigned short& oImageHeight,
            unsigned long long& oFrameIndexForRecording,
            FeaturesTracking::EFeatureType& oFeatureType,
            void* iData
        );

    protected:
    


        // This corresponds to the sub-pixel index of the laplacian image
 // that the extremum was found.
        
        /// \brief    write Features Tracking KeyPoint in CIIF format to File
        ///
        /// \param[in] iOut stream to data recording
        /// \param[in] iMode  ECaptureMode 
        /// \param[in] iData  reference to ParcedData structure
        /// \param[in] iCnt  KeyPoint's  counter
        /// \param[in] iSS   This corresponds to the sub-pixel index of the laplacian image. That the extremum was found.
        /// \param[in] iImageHeight   Image Height
        /// \param[in] iFrameIndexForRecording  Frame Index For Recording
        /// \param[in] iUID KeyPoint's uniq ID
        /// \param[in] iConfidence  KeyPoint's Confidence
        /// \return bool true if operation successfully completed.
        static bool write(
            std::ostream& iOut,
            ECaptureMode iMode,
            FeaturesTracking::ParsedData& iData,
            unsigned int iCnt,
            unsigned int iSS,
            unsigned int iUID = std::numeric_limits<uint32_t>::max(),
            unsigned int iConfidence = std::numeric_limits<uint32_t>::max());



        
    };
}
#endif//__FEATURESTRACKING_CIIF_H__