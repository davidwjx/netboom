/*
* File - CFeaturesTrackingData.h
*
* This file is part of the Inuitive SDK
*
* Copyright (C) 2021 All rights reserved to Inuitive
*
*/

#ifndef __SLAM_DATA_H__
#define __SLAM_DATA_H__

#ifndef _RND_1110_

#include <array>

////////////////////////////////////////////////////////////////////////
 /// \brief    Represents SLAM Data structs
 ///
 /// Responsibilities:
 /// 
 ////////////////////////////////////////////////////////////////////////

namespace InuDev
{
    namespace Slam
    {
        enum EOutputFormat
        {
            ePose,
            eExtended
        };

        struct CSlamData
        {
            EOutputFormat Format = ePose;

            ///Inu SLAM eSlamOriginal Data
            /// 
            /// \brief    Camera location in (x, y, z) relative to an initial position in first frame
            //float Translation3D[3] = {0};
            std::array<float, 3> Translation3D;

            /// \brief    Camera rotation in a quaternion format relative to an initial position in first frame (in Radians)
            //float Quaternion[4] = { 0 };
            std::array<float, 4> Quaternion;

            /// \brief    Camera rotation[3x3] matrix and translation[3x1] matrix in one 4x4 matrix. Is created from Translation3D and Quaternion.
            //float TransformMatrix[TRANSFORM_MATRIX_SIZE] = { 0 };
            std::array<float, 16> TransformMatrix;

            ///Inu SLAM eExtended Data
            ///
            std::shared_ptr<std::vector<std::array<float, 3>>> RefMapPoints;
            std::shared_ptr<std::vector<std::array<float, 3>>> AllMapPoints;
            std::shared_ptr<std::vector<std::array<float, 2>>> KeyPoints;
            std::shared_ptr<std::vector<std::pair<std::array<int, 2>, std::array<int, 2>>>> CubePoints;
            std::shared_ptr<std::vector<std::array<float, 16>>> KeyframesPoseInverses;
            std::shared_ptr<std::vector<std::pair<std::array<float, 3>, std::vector<int>>>> CameraCenters;

            CSlamData()
                : RefMapPoints(std::make_shared<std::vector<std::array<float, 3>>>())
                , AllMapPoints(std::make_shared<std::vector<std::array<float, 3>>>())
                , KeyPoints(std::make_shared<std::vector<std::array<float, 2>>>())
                , CubePoints(std::make_shared< std::vector<std::pair<std::array<int, 2>, std::array<int, 2>>>>())
                , KeyframesPoseInverses(std::make_shared<std::vector<std::array<float, 16>>>())
                , CameraCenters(std::make_shared<std::vector<std::pair<std::array<float, 3>, std::vector<int>>>>())
            {
            }

            CSlamData(const CSlamData& iData)
                : RefMapPoints(std::make_shared<std::vector<std::array<float, 3>>>())
                , AllMapPoints(std::make_shared<std::vector<std::array<float, 3>>>())
                , KeyPoints(std::make_shared<std::vector<std::array<float, 2>>>())
                , CubePoints(std::make_shared< std::vector<std::pair<std::array<int, 2>, std::array<int, 2>>>>())
                , KeyframesPoseInverses(std::make_shared<std::vector<std::array<float, 16>>>())
                , CameraCenters(std::make_shared<std::vector<std::pair<std::array<float, 3>, std::vector<int>>>>())
            {
                if (iData.RefMapPoints)
                {
                    *RefMapPoints.get() = *(iData.RefMapPoints.get());
                }
                if (iData.AllMapPoints)
                {
                    *AllMapPoints.get() = *(iData.AllMapPoints.get());
                }
                if (iData.KeyPoints)
                {
                    *KeyPoints.get() = *(iData.KeyPoints.get());
                }
                if (iData.CubePoints)
                {
                    *CubePoints.get() = *(iData.CubePoints.get());
                }
                if (iData.KeyframesPoseInverses)
                {
                    *KeyframesPoseInverses.get() = *(iData.KeyframesPoseInverses.get());
                }
                if (iData.CameraCenters)
                {
                    *CameraCenters.get() = *(iData.CameraCenters.get());
                }
            }

            void Clear()
            {
                if (RefMapPoints)
                {
                    RefMapPoints->clear();
                }
                if (AllMapPoints)
                {
                    AllMapPoints->clear();
                }
                if (KeyPoints)
                {
                    KeyPoints->clear();
                }
                if (CubePoints)
                {
                    CubePoints->clear();
                }
                if (KeyframesPoseInverses)
                {
                    KeyframesPoseInverses->clear();
                }
                if (CameraCenters)
                {
                    CameraCenters->clear();
                }
            }
        };
    }
}

#endif

#endif//__SLAM_DATA_H__
