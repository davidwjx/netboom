#ifndef __INUMODELWRAPPER_H__
#define __INUMODELWRAPPER_H__

#pragma once

#include "TiffFile.h"
#using <system.drawing.dll>

using namespace System;
using namespace System::Collections::Generic;

using namespace std;

namespace InuCommon
{

    ////////////////////////////////////////////////////////////////////////
    /// \brief    C#wrapper to Sensor model
    ///
    /// Role: Each Sensor model has pre-defined parameters that are loaded from internal config file
    ///
    /// Responsibilities:
    ///      1. Knows all parameters that are associated with Sensor model
    ///      2. Knows how to load these parameters from internal config file
    ////////////////////////////////////////////////////////////////////////

    public enum class ETiffFormat
    {
        eYUV = CTiffFile::ETiffFormat::eYUV,
        eY = CTiffFile::ETiffFormat::eY,
        eRGB565 = CTiffFile::ETiffFormat::eRGB565,
        eBayerGRBG = CTiffFile::ETiffFormat::eBayerGRBG,
        eEmpty = InuCommon::CTiffFile::eEmpty,
        eDepth = InuCommon::CTiffFile::eDepth,
        eNumOfFormats = InuCommon::CTiffFile::eNumOfFormats
    };


    public enum class ESensorResolution
    {
        eBinning             = InuCommon::CTiffFile::eBinning,
		eVerticalBinning	 = InuCommon::CTiffFile::eVerticalBinning,
        eFull                = InuCommon::CTiffFile::eFull,
        eUnknownResolution   = InuCommon::CTiffFile::eUnknownResolution

    };


    public enum class EHTSource
    {
        eVideo  = InuCommon::CTiffFile::eVideo,
        eWebcam = InuCommon::CTiffFile::eWebcam
    };


    public ref class InuModelWrapper
    {
    public:

        InuModelWrapper();

        /// \brief   Initialize the current model object.
        ///
        /// CInuModel attributes are read from internal configuration file
        /// \param[in]     iSensorID    Sensor name that should be loaded
        /// \param[in]     iHWType     HW revision type
        /// \return    false if the object could not be loaded
        bool Init( System::String^ iSensorName, System::String^ iHWType);

        /// \brief   Initialize the current model object.
        ///
        /// \param[in]     sensorModel    loaded InuModel data
        /// \return    false if the object could not be initialize
        bool Init(const CInuModel& sensorModel);

        /// \brief      Current object unique name (identifier)
        property System::String^ ModelName;

        /// \brief      HW Revision ID (identifier)
        property String^ HwType;

        /// \brief      Model description
        property System::String^ Description;

        /// \brief      Exposure Time Register address
        property unsigned int ETRegAddress;

        /// \brief      Exposure Time Register size
        property unsigned int ETRegSize;

        /// \brief      FW unique ID for that model (real value is INU_DEFSG_sensorModelE)
        property unsigned int	FWEnumerator;

        /// \brief      Default FPS that should be used for this model (Depth Camera)
        property float DefaultDepthFPS;

		/// \brief      Default FPS that should be used for this model (RGB Camera)
		property float DefaultRGBFPS;

		/// \brief      Default resolution that should be used for this model (Depth Camera)
		property  ESensorResolution DefaultDepthResolution;

		/// \brief      Default resolution that should be used for this model (RGB Camera)
		property  ESensorResolution DefaultRGBResolution;

		/// \brief      Default Interleaved mode that should be used for this model
        property bool DefaultInterleavedMode;

        /// \brief      True if automatic Sensor Control should be used.
        property bool DefaultAutoControl;

		/// \brief      List of all available Image streams and their names
        property System::String^    ImageStreamsChannels;

        property System::Drawing::Size DepthCrop;

        property Dictionary<System::String^, System::String^>^ ChannelsActiveSensors;
    };
}

#endif
