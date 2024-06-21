/*
* File - CFeaturesTrackingData.h
*
* This file is part of the Inuitive SDK
*
* Copyright (C) 2021 All rights reserved to Inuitive
*
*/

#ifndef __FEATURESTRACKING_DATA_COMMON_H__
#define __FEATURESTRACKING_DATA_COMMON_H__

#include <ostream>

////////////////////////////////////////////////////////////////////////
 /// \brief    Represents CFeatures Tracking Data structs
 ///
 /// Responsibilities:
 /// 
 ////////////////////////////////////////////////////////////////////////

namespace InuCommon
{
    namespace FeaturesTracking
    {
        /// \brief CIIF Version
        constexpr unsigned int FORMAT_VERSION = 9;

        constexpr unsigned int DESCR_SIZE = 16;
 
        /// \brief 512 bits � sizeof FREAK / LIFT binary descriptor
        constexpr unsigned int FT_DESCRIPTOR_SIZE = DESCR_SIZE * sizeof(unsigned int );

        /// \brief FeaturesTrackin OutputType
        enum EOutputType
        {
            eRaw,
            eParsed,
            eProcessed
        };

        enum EFeatureType
        {
            eUknownType,
            eDogFreak,
            eORB
        };

        struct ParsedData
        {
            /// \brief 512 bits � FREAK / LIFT binary descriptor
            unsigned int    Descriptor[DESCR_SIZE] = { 0 }; // descriptor - 512 bit

            /// \brief Subpixel keypoint position (mX, mY) [pixels * 2^14] (divide it by 2^14 to get the X,Y position in subpixel)
            /// In Hybrid mode, if the inputs are 2 images concatenated one next to the other, the sub pixel position is relative
            /// to the starting point of each image (parameter mNotImg1 below tells you if the key point is from the left image or
            /// from the right image
            float    X = 0.0F;
            float    Y = 0.0F;

            /// \brief Key point angle - Anti-clockwise, [in radians]
            float    Angle = 0.0F;

            /// \brief Radius around the point
            /// Radius around the point (in pixels). To be used for the descriptor calculation - you can extract the key point scale
            /// using this formula: scale = 2/log(2)*log((mPatternSize � 1)/16) (or with a lookup table)
            unsigned int    PatternSize = 0;

            /// \brief IsRightImage
            /// If the inputs are 2 images concatenated one next to the other
            /// Key points can be from the left image or from the right image
            unsigned int    IsRightImage = false;
        };

        struct ProcessedData : public ParsedData
        {
            /// \brief  Unique Id for the key point.
            /// If the key point has a match to another key point in the next frame its Id stays the same
            unsigned int    UniqId = 0;

            /// \brief   Number of sequential frames in which this key point was detected
            unsigned int    Confidence = 0;
        };
    }
}
#endif//__FEATURESTRACKING_DATA_H__