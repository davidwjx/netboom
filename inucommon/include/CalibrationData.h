/*
* File - CalibrationData.h
*
* This file is part of the Inuitive SDK
*
* Copyright (C) 2014 All rights reserved to Inuitive
*
*/

#ifndef __CALIBRATIONDATA_COMMON__
#define __CALIBRATIONDATA_COMMON__

#include <map>
#include <limits>
#include <vector>
#include <string>

#ifdef _MSC_VER
#pragma warning(disable : 4251)
#endif

namespace InuCommon
{
    /// \brief   All parameters that describes all cameras and peripherals of this device (Inuitive Sensor) and
    ///          the geometrics relations between them. This information is calculated by a calibration process
    struct CCalibrationData
    {
        /// \brief     Calibration data of one sensor
        struct COneSensorData
        {
            /// \brief     The optical data of one camera that is derived from the physical sensor
            struct CCameraData
            {
                /// \brief   The Intrinsic parameters of the camera
                struct CIntrinsicData
                {
                    /// \brief    Different camera model types
                    enum ECameraModel
                    {
                        eFTanThetha = 0,    ///<    f*tan(theta)
                        eFThetha = 1,       ///<    f*theta
                        eSpline = 7,        ///<    Spline
                        eFThethaExt = 10,
						eEquiDistance = 31
                    };

                    /// \brief     True if the intrinsic parameters are valid
                    bool Valid = false;

                    /// \brief    The focal length  (X,Y)
                    double FocalLength[2];

                    /// \brief    Principal / optical center (X,Y)
                    double OpticalCenter[2];

                    /// \brief    Lens distortion coefficients
                    std::vector<double> LensDistortion;

                    /// \brief    Sensor plane axes misalignment
                    double Alpha;

                    /// \brief    Ccamera model : 0->f*tan(theta); 1->f*theta, 7->Spline
                    ECameraModel Model;

                    std::vector<double> Rc;
                };

                /// \brief    Transformation parameters between the device reference camera and this camera
                struct CExtrinsicData
                {
                    /// \brief     True if the extrinsic parameters are valid
                    bool Valid = false;

                    /// \brief    Translation vector in mm between the device reference camera and this camera
                    double Translation[3];

                    /// \brief    Rotation vector in radians between the device reference camera and this camera
                    double Rotation[3];
                };

                /// \brief     This camera is valid
                bool Valid = false;
                
                /// \brief     The intrinsic parameters of this sensor
                CIntrinsicData Intrinsic;
                
                /// \brief     The extrinsic parameters of this sensor relates to the device reference camera
                CExtrinsicData Extrinsic;

                /// \brief     The size (width, height) of the sensor in pixels
                uint32_t Resolution[2];
                
                /// \brief     The size of each pixel in mm (width, height. Invalid value is MAX double.  
                double PixelSize[2];

                CCameraData() { PixelSize[0] = PixelSize[1] = -1.0f; }
            };

            /// \brief     General description of this sensor
            std::string Description;

            /// \brief     The real (without any manipulation) Sensor's calibration parameters
            CCameraData RealCamera;

            /// \brief     The optical data of the image that is received after DSR manipulations (cropping, rotating, rectification)
            CCameraData VirtualCamera;
        };

        /// \brief     IMU calibration data structure. The IMU is calibrated in reference to the main camera
        struct CImuData
        {
            /// \brief     Intrinsic parameters of each IMU component 
            struct CIntrinsicData
            {
                /// \brief     True if the Intrinsic calibration data is valid.
                bool Valid = false;

                double Misalignment[3][3];
                double Scale[3][3];
                double Bias[3];
                double Noise[3];
            };

            struct CTemperatureData
            {
                /// \brief     True if the IMU temperature calibration parameters are valid
                bool Valid = false;

                /// \brief     Temperature correction vector
                double Slope[3];

                /// \brief     Base temperature for correction vector 
                double Reference;

                CTemperatureData() { Slope[0] = Slope[1] = Slope[2] = 0; Reference = 0; }
            };

            /// \brief     Accelerometer's intrinsic parameters
            CIntrinsicData Accelerometer;

            /// \brief     Accelerometer's temperature parameters
            CTemperatureData AccelerometerTemperature;

            /// \brief     Gyroscope's intrinsic parameters
            CIntrinsicData Gyroscope;

            /// \brief     Gyroscope's temperature parameters
            CTemperatureData GyroscopeTemperature;

            /// \brief     True if the IMU extrinsic data is valid
            bool ValidExterinsic = false;

            /// \brief     Transformation matrix between the device reference camera and IMU
            double Extrinsic[4][4];
        };

        /// \brief     Calibration revision (for backward compatibility)
        std::string Version;

        /// \brief     Calibration data of all sensors, ordered by the unique sensor ID
        std::map<int, COneSensorData>   Sensors;

        /// \brief     The  Euclidean Distance between 2 sensors
        std::map< std::pair<int, int> , double> Baselines;

        /// \brief     IMU calibration parameters
        CImuData  Imu;

        virtual ~CCalibrationData() {}

        /// \brief     Baseline for pair of sensors, 0.0 if baseline is not found
        double Baseline(int iSensor1, int iSensor2) const
        {
            double result = (Baselines.find(std::make_pair(iSensor1, iSensor2)) != Baselines.end()) ?
                Baselines.at(std::make_pair(iSensor1, iSensor2)) :
                ( (Baselines.find(std::make_pair(iSensor2, iSensor1)) != Baselines.end()) ? Baselines.at(std::make_pair(iSensor2, iSensor1)) : 0 );
            return result;
        }
    };
}

#ifdef _MSC_VER
#pragma warning(default : 4251)
#endif

#endif


