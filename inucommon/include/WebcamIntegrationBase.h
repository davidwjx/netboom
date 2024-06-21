#pragma once

#include <string>
#include <stdint.h>

#include "AlgWebcamIntegrationDLL.h"
#include "CalibrationData.h"
#include "OSUtilities.h"

class ALG_WEBCAM_INTEGRATION_API sCVE_stream
{
public:

    sCVE_stream()
        : width{ 0 }
        , height{ 0 }
        , data{ nullptr }
        , pixelSize { -1 } // unknown
    {
        // Empty
    }

    unsigned int        width;
    unsigned int        height;
    unsigned char*      data;
    int                 pixelSize;
};

class ALG_WEBCAM_INTEGRATION_API sCVE_StreamsVec
{
protected:

    sCVE_StreamsVec();

public:

    virtual ~sCVE_StreamsVec();

    static sCVE_StreamsVec* Factory();

    uint64_t timeStamp;

    sCVE_stream webcam;
    sCVE_stream leftRGB;
    sCVE_stream rightRGB;
    sCVE_stream depth;
    sCVE_stream disparity;
};

class ALG_WEBCAM_INTEGRATION_API sCVE_WebcamIntegration_Results
{
public:

    sCVE_WebcamIntegration_Results()
        : frame_index{ 0 }
        , isRGBinDepthCoordinates{ false }
        , isDepthInRGBCoordinates{ false }
    { 
       // Empty
    }

    int frame_index;
    bool isRGBinDepthCoordinates;
    bool isDepthInRGBCoordinates;
    sCVE_stream RGB_data;
    sCVE_stream Converted_ZBuff;
    sCVE_stream XYZ_Buff; // raster of XYZ

    // PLY output
    sCVE_stream PointCloudRGB; // vector of RGB (valids only)
    sCVE_stream PointCloudXYZ; // vector of XYZ (valids only)
};

enum class eWI_WebcamIntegrationMode
{
    eDepth2RGB = 0,     // Depth2WebCam (WebCam Undistorted and Rotated) algo recieves WebCam Undist&Rotated
    eRGBUndistortion,   // WebCam Undistortion and Rotation
    eRGB2Depth,         // WebCam Registration (WebCam2Depth)
    eNumberOfModes
};

class ALG_WEBCAM_INTEGRATION_API sCVE_WebcamIntegrationConfig
{
protected:

    sCVE_WebcamIntegrationConfig();
    virtual ~sCVE_WebcamIntegrationConfig();

public:

    eWI_WebcamIntegrationMode eMode;

    static sCVE_WebcamIntegrationConfig* Factory();
};

class ALG_WEBCAM_INTEGRATION_API sCVE_Config
{
protected:

    sCVE_Config();

public:

    virtual ~sCVE_Config();

    static sCVE_Config* Factory(int _DepthRegistrationCloseHolesWindowSize = 3);

    unsigned int frameWidth;
    unsigned int frameHeight;

    // Depth Registration close hole window size
    // TODO: Add accessor (Get/Set)
    int m_DepthRegistrationCloseHolesWindowSize;

    sCVE_WebcamIntegrationConfig *WebcamIntegrationConfig;

    int mWebcamSensor;

    unsigned int WebWidth;
    unsigned int WebHeight;

#pragma warning(push)
#pragma warning(disable : 4251)
    InuCommon::CCalibrationData mOpticalDataWebcam;

    InuCommon::CCalibrationData mOpticalDataDepth;
    std::pair<int, int> mDepthRightAndLeftSensors;
#pragma warning(pop)
};

class ALG_WEBCAM_INTEGRATION_API CWebcamIntegrationBase
{
public:

    static CWebcamIntegrationBase* Factory(
        InuCommon::COsUtilities::EOptimizationLevel iOptimizationLevel,
        bool openMP = true,
        bool writePLYwebcam = false,
        bool writeColorDepthimg = false
    );

    virtual bool Init(
        sCVE_Config* configData,
        bool isFisheye,
        bool undistort
    ) = 0;

    virtual bool Process(
        const sCVE_StreamsVec& inputData,
        sCVE_WebcamIntegration_Results& results,
        const sCVE_Config* configData,
        bool undistort,
        bool overrideUseVirtualCameraOpticalData = false
    ) = 0;

    virtual bool NeedAlgorithmInitialization() const = 0;
};
