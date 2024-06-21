#if defined(_OPENMP)
    #include <omp.h>
#endif

#include <sstream>
#include <iostream>
#include <stdint.h>
#include <fstream>

#include "WebcamIntegrationBase.h"

#include "AlgoDepthOpticalData.h"
#include "AlgoBase.h"

#include "AuxFunctions.h"
#include "WebCamInterClass.h"
#include "basic_functions.h"

#include <cmath>

sCVE_WebcamIntegrationConfig::sCVE_WebcamIntegrationConfig()
    : eMode{ eWI_WebcamIntegrationMode::eNumberOfModes }
{
    // Empty
}

sCVE_WebcamIntegrationConfig::~sCVE_WebcamIntegrationConfig()
{
     // Empty
}

class _sCVE_WebcamIntegrationConfigImp : public sCVE_WebcamIntegrationConfig
{
    friend class sCVE_WebcamIntegrationConfig;

    _sCVE_WebcamIntegrationConfigImp()
        : sCVE_WebcamIntegrationConfig()
        , Lutx{ nullptr }
        , Luty{ nullptr }
    {
        // Empty
    }

public:

    virtual ~_sCVE_WebcamIntegrationConfigImp()
    {
        delete[] Lutx;
        Lutx = nullptr;
        delete[] Luty;
        Luty = nullptr;
    }

    float* Lutx, * Luty;//Structures for rectification LUTs
};

sCVE_WebcamIntegrationConfig* sCVE_WebcamIntegrationConfig::Factory()
{
    return dynamic_cast<sCVE_WebcamIntegrationConfig*>(new _sCVE_WebcamIntegrationConfigImp());
}

sCVE_Config::sCVE_Config()
    : frameWidth{ 0u }
    , frameHeight{ 0u }
    , WebWidth{ 0u }
    , WebHeight{ 0u }
    , WebcamIntegrationConfig{ _sCVE_WebcamIntegrationConfigImp::Factory() } // polymorphic + defaults
    , mOpticalDataDepth{ InuCommon::CCalibrationData() }
    , mOpticalDataWebcam{ InuCommon::CCalibrationData() }
    , m_DepthRegistrationCloseHolesWindowSize{ 0 } // set in factory
    , mWebcamSensor{ -1 }
{
    // Empty (but private...)
}

sCVE_Config::~sCVE_Config()
{
    delete dynamic_cast<_sCVE_WebcamIntegrationConfigImp*>(WebcamIntegrationConfig);
    WebcamIntegrationConfig = nullptr;
}

class sCVE_ConfigImp : public sCVE_Config
{
    friend class CWebcamIntegrationImp;
    friend class CWebcamIntegrationSSEImp;

    bool buildPLY;

public:

    sCVE_ConfigImp()
        : sCVE_Config()
        , buildPLY { false }
    {
        // Empty

    }
};

sCVE_Config* sCVE_Config::Factory(int _DepthRegistrationCloseHolesWindowSize)
{
    // Will also initialize cfg->WebcamIntegrationConfig

    sCVE_ConfigImp* ptr = new sCVE_ConfigImp();

    ptr->m_DepthRegistrationCloseHolesWindowSize = _DepthRegistrationCloseHolesWindowSize;

    return dynamic_cast<sCVE_Config*>(ptr);
}

sCVE_StreamsVec::~sCVE_StreamsVec()
{

}

sCVE_StreamsVec::sCVE_StreamsVec()
    :timeStamp{ 0 }
{
     // Empty
}

class sCVE_StreamsVecImp : public sCVE_StreamsVec
{
public:

    sCVE_StreamsVecImp()
        : sCVE_StreamsVec{}
    {
        // Empty
    }

    sCVE_StreamsVecImp(const sCVE_StreamsVecImp& other)
    {
        operator=(other);
    }

    ~sCVE_StreamsVecImp()
    {

    }

    sCVE_StreamsVecImp& operator=(const sCVE_StreamsVecImp& other)
    {
        timeStamp = other.timeStamp;

        webcam = other.webcam;
        leftRGB = other.leftRGB;
        rightRGB = other.rightRGB;
        depth = other.depth;
        disparity = other.disparity;

        return *this;
    }
};

sCVE_StreamsVec* sCVE_StreamsVec::Factory()
{
    return dynamic_cast<sCVE_StreamsVec*>(new sCVE_StreamsVecImp());
}

class CWebcamIntegrationImp : public CWebcamIntegrationBase
{
    friend class CWebcamIntegrationBase;

protected:

    cv::Mat Z2Web; // Must be a mamber - used in BuildPLY
    cv::Mat Z2Web_copy;

    float minZ;
    float maxZ;

    // True if algorithm need to be initiated
    bool mNeedAlgorithmInitialization;

    /******** DEBUG FLAGS ****************************/
    bool rgb_flipped;
    /******* END OF DEBUG FLAGS **********************/

    cv::Mat colr_onto_depth; /* the result. Global for this .cpp */
    cv::Mat color2dispar; /* the result, alternative algorithm. Global for this .cpp */
    cv::Mat usedcolor;
    cv::Mat useddepth;

    virtual void WriteDebugFiles(
        const sCVE_StreamsVec& inputData,
        cv::Mat colr_onto_depth
    )
    {
        // Empty in base class
    }

    virtual void BuildPLY(
        const int webcamrows,
        const int webcamcols,
        const InuCommon::CCalibrationData& rOpticalDataWebcam,
        sCVE_WebcamIntegration_Results& results,
        const sCVE_Config* _configData,
        bool undistort
    )
    {
        // empty
    }

    virtual bool WebcamIntegrationProcessWeb2Depth(
        const sCVE_StreamsVec& inputData,
        sCVE_WebcamIntegration_Results& results,
        const sCVE_ConfigImp* configData,
        bool undistort,
        bool overrideUseVirtualCameraOpticalData
    );

    virtual bool WebcamIntegrationProcessDepth2Web(
        const sCVE_StreamsVec& inputData,
        sCVE_WebcamIntegration_Results& results,
        const sCVE_ConfigImp* configData,
        bool undistort
    );

    virtual bool WebcamIntegrationProcessWebUndistortion(
        const sCVE_StreamsVec& inputData,
        sCVE_WebcamIntegration_Results& results,
        const sCVE_ConfigImp* configData
    );

    // static methods

    static uchar BilinearInterp(uchar Irc, uchar Ir1c, uchar Irc1, uchar Ir1c1, float dr, float dc);

    void buildXYZmat(
        const cv::Mat& depth,
        const InuCommon::CCalibrationData::COneSensorData::CCameraData::CIntrinsicData& rOpticalDataWebcam,
        cv::Mat& XYZmat
    );

    static void SetWebCamIntegrResults(
        const sCVE_StreamsVec& inputData,
        sCVE_WebcamIntegration_Results& Webcam_Results
    );

protected:

    CWebcamIntegrationImp();

    virtual ~CWebcamIntegrationImp();

public:

    /******** DEBUG FLAGS ****************************/
    bool writePLYwebcam;
    bool writeColorDepthimg;
    /******* END OF DEBUG FLAGS **********************/

    bool openMP;

    bool Init(
        sCVE_Config* configData,
        bool isFisheye,
        bool undistort
    );

    bool Process(
        const sCVE_StreamsVec& inputData,
        sCVE_WebcamIntegration_Results& results,
        const sCVE_Config* configData,
        bool undistort,
        bool overrideUseVirtualCameraOpticalData = false
    );

    bool NeedAlgorithmInitialization() const;
};

CWebcamIntegrationImp::CWebcamIntegrationImp()
    : mNeedAlgorithmInitialization{ true }
    , rgb_flipped{ false }
    , writePLYwebcam{ false }
    , writeColorDepthimg{ false }
    , minZ{ 0 }
    , maxZ{ 1e6 }
    , openMP{ false }
{
    // Empty
}

CWebcamIntegrationImp::~CWebcamIntegrationImp()
{
    // Empty
}

bool CWebcamIntegrationImp::NeedAlgorithmInitialization() const
{
    return mNeedAlgorithmInitialization;
}

bool CWebcamIntegrationImp::Init(
    sCVE_Config* _configData,
    bool isFisheye,
    bool undistort
)
{
    sCVE_ConfigImp* configData = dynamic_cast<sCVE_ConfigImp*>(_configData);

    // TODO: Read parameters?
    //fs::path configFilePath = AddGeneralConfigPath("InitParams.txt");

    cv::Size imsize(configData->WebWidth, configData->WebHeight);
    
    if (configData->mOpticalDataWebcam.Sensors.find(configData->mWebcamSensor) == configData->mOpticalDataWebcam.Sensors.end())
    {
        return false;
    }

    const auto& odw{ configData->mOpticalDataWebcam.Sensors.at(configData->mWebcamSensor) };

    const auto& webcamOptical = (false
            || undistort
            || !odw.VirtualCamera.Intrinsic.Valid
        )
        ? odw.RealCamera.Intrinsic
        : odw.VirtualCamera.Intrinsic
        ;

    if (!webcamOptical.Valid)
    {
        return false;
    }

    float focalX = float(webcamOptical.FocalLength[0]);
    float focalY = float(webcamOptical.FocalLength[1]);
    float Oc[2] = { float(webcamOptical.OpticalCenter[0]), float(webcamOptical.OpticalCenter[1]) };

    /*********** DISTORTION - definition & initialisation ***************/
    float Kd[5];

    assert(webcamOptical.LensDistortion.size() <= 5);

    for (int i = 0; i < sizeof(Kd) / sizeof(decltype(Kd[0])); i++)
    {
        Kd[i] = float(webcamOptical.LensDistortion[i]);
    }

    /*********** ROTATION - definition & initialisation ***************/
    cv::Mat rotat_mtrx = cv::Mat::eye(3, 3, CV_32F); /* rotation vector must be translated by Rodrigues2 into rotation matrix */
    //Mat rotat_vectr = Mat::zeros(3, 1, CV_32F);
    //rotat_vectr.at<float>(0,0) = configData->mOpticalData.mWebcamRotate[0];
    //rotat_vectr.at<float>(1,0) = configData->mOpticalData.mWebcamRotate[1];
    //rotat_vectr.at<float>(2,0) = configData->mOpticalData.mWebcamRotate[2];
    //FT::Rodrigues_(rotat_vectr, rotat_mtrx);

    WebCamInterClass pWI;

    _sCVE_WebcamIntegrationConfigImp *ecic =
        dynamic_cast<_sCVE_WebcamIntegrationConfigImp *>(configData->WebcamIntegrationConfig);

    if (false
        || ecic->Lutx
        || ecic->Luty
        )
    {
        return false;
    }

    ecic->Lutx = new float[configData->WebWidth * configData->WebHeight];
    ecic->Luty = new float[configData->WebWidth * configData->WebHeight];

    pWI.init(
        imsize,
        focalX,
        focalY,
        Oc,
        Kd,
        rotat_mtrx,
        ecic->Lutx,
        ecic->Luty,
        isFisheye
    );

    //pWI->initCEVA(imsize, focal, Oc, Kd, rotat_mtrx);
    //FT::writeMat2(pWI->map_x, "D:/Users/genadiyv/Desktop/xi.mat", "xi", false);
    //FT::writeMat2(pWI->map_y, "D:/Users/genadiyv/Desktop/yi.mat", "yi", false);

    /////// read and parse debug flags from external txt file. If debug output is not needed, just don't create
    // the file. This way, no special treatment will be needed regarding version management systems

    std::string debugflags_line;
    std::ifstream debug_flags_file;

    debug_flags_file.open("c:\\temp\\debug_flags.txt");

    if (debug_flags_file.good())
    {
        size_t eidx;
        std::string flagval;

        while (getline(debug_flags_file, debugflags_line))
        {
            char debug_flg_nam[48];
            eidx = debugflags_line.find_last_of('=');
            size_t fst_nonspace = debugflags_line.find_first_not_of(' ', 0);
#if _MSC_VER && __STDC_WANT_SECURE_LIB__
            strcpy_s(debug_flg_nam, sizeof(debug_flg_nam), (debugflags_line.substr(fst_nonspace, eidx)).c_str());
#else
            strcpy(debug_flg_nam, (debugflags_line.substr(fst_nonspace, eidx)).c_str());
#endif
            flagval = debugflags_line.substr(eidx + 1, debugflags_line.length() - eidx - 1);
            int int_val = atoi(flagval.c_str());

            if (!strcmp(debug_flg_nam, "rgb_flipped"))
            {
                rgb_flipped = int_val != 0;
            }
            else if (!strcmp(debug_flg_nam, "writePLYwebcam"))
            {
                writePLYwebcam = int_val != 0;
            }
            else if (!strcmp(debug_flg_nam, "writeColorDepthimg"))
            {
                writeColorDepthimg = int_val != 0;
            }
            else
            {
                /* nothing */;
            }
        }

        debug_flags_file.close();

    } //////////// end of read/parse edebug flags

    //pHS->updateFrameSize(inputData.rightY.height,inputData.rightY.width);

    mNeedAlgorithmInitialization = false;

    return true;
}

void CWebcamIntegrationImp::SetWebCamIntegrResults(
    const sCVE_StreamsVec& inputData,
    sCVE_WebcamIntegration_Results& Webcam_Results
)
{
    Webcam_Results.frame_index = int(inputData.timeStamp);
    Webcam_Results.isDepthInRGBCoordinates = false;
    Webcam_Results.isRGBinDepthCoordinates = true;
}

uchar CWebcamIntegrationImp::BilinearInterp(
    uchar Irc,
    uchar Ir1c,
    uchar Irc1,
    uchar Ir1c1,
    float dr,
    float dc
)
{
    float Iinterp = 0.0f
        + float(Irc) * (1.f - dr) * (1.f - dc)
        + float(Ir1c) * dr * (1.f - dc)
        + float(Irc1) * (1.f - dr) * dc
        + float(Ir1c1) * dr * dc
        ;

    int I = cvRound(Iinterp);

    return uchar(I);
}

// Web Cam Rectification Web2Depth New
bool CWebcamIntegrationImp::WebcamIntegrationProcessWeb2Depth(
    const sCVE_StreamsVec& inputData,
    sCVE_WebcamIntegration_Results& results,
    const sCVE_ConfigImp* configData,
    bool undistort,
    bool overrideUseVirtualCameraOpticalData
)
{
    const int webcamrows = inputData.webcam.height;
    const int webcamcols = inputData.webcam.width;
    const int dpthrows = inputData.depth.height;
    const int dpthcols = inputData.depth.width;

    cv::Mat webimg = cv::Mat(webcamrows, webcamcols, CV_8UC4, inputData.webcam.data);

    if (undistort)
    {
        cv::Mat web_rectified;

        _sCVE_WebcamIntegrationConfigImp* wcic =
            dynamic_cast<_sCVE_WebcamIntegrationConfigImp*>(configData->WebcamIntegrationConfig);

        cv::Mat mapx(webcamrows, webcamcols, CV_32FC1, wcic->Lutx);
        cv::Mat mapy(webcamrows, webcamcols, CV_32FC1, wcic->Luty);

        WebCamInterClass pWI;
        pWI.rectify_undistort(webimg, mapx, mapy, web_rectified);

        webimg = web_rectified;
    }

    // Handle all thing optical data...

    const auto& rOpticalDataVideo = configData->mOpticalDataDepth;
    const auto& rOpticalDataWebCam = configData->mOpticalDataWebcam;

    const auto& intrinsicVirtualRight =
        rOpticalDataVideo.Sensors.at(configData->mDepthRightAndLeftSensors.first).VirtualCamera.Intrinsic;

    float reciprocalFocalX = float(1.0 / intrinsicVirtualRight.FocalLength[0]);
    float reciprocalFocalY = float(1.0 / intrinsicVirtualRight.FocalLength[1]);

    float opticalCenterToFocalLengthX = float(intrinsicVirtualRight.OpticalCenter[0] / intrinsicVirtualRight.FocalLength[0]);
    float opticalCenterToFocalLengthY = float(intrinsicVirtualRight.OpticalCenter[1] / intrinsicVirtualRight.FocalLength[1]);

    auto& odv{ rOpticalDataVideo.Sensors.at(configData->mWebcamSensor) };

    const auto& extrinsicRealWebcam = (false
            || overrideUseVirtualCameraOpticalData
            || undistort
            || !odv.VirtualCamera.Extrinsic.Valid
        )
        ? odv.RealCamera.Extrinsic
        : odv.VirtualCamera.Extrinsic
        ;

    if (!extrinsicRealWebcam.Valid)
    {
        return false;
    }

    float extrinsicRealWebcamTranslationX = float(extrinsicRealWebcam.Translation[0]);
    float extrinsicRealWebcamTranslationY = float(extrinsicRealWebcam.Translation[1]);
    float extrinsicRealWebcamTranslationZ = float(extrinsicRealWebcam.Translation[2]);

    auto& odw{ rOpticalDataWebCam.Sensors.at(configData->mWebcamSensor) };

    const auto& intrinsicRealWebCacm = (false
            || overrideUseVirtualCameraOpticalData
            || undistort
            || !odw.VirtualCamera.Intrinsic.Valid
        )
        ? odw.RealCamera.Intrinsic
        : odw.VirtualCamera.Intrinsic
        ;

    if (!intrinsicRealWebCacm.Valid)
    {
        return false;
    }

    float intrinsicRealWebCacmFocalLengthX = float(intrinsicRealWebCacm.FocalLength[0]);
    float intrinsicRealWebCacmFocalLengthY = float(intrinsicRealWebCacm.FocalLength[1]);

    float intrinsicRealWebCacmOpticalCenterX = float(intrinsicRealWebCacm.OpticalCenter[0]);
    float intrinsicRealWebCacmOpticalCenterY = float(intrinsicRealWebCacm.OpticalCenter[1]);

    if (intrinsicVirtualRight.FocalLength[0] <= 0)
    {
        return false;
    }

    // End optical data mess

    // to prevent the effect of false coloring of points occluded from
    cv::Mat webcam_pix_used(webcamrows, webcamcols, CV_32F);
    memset(webcam_pix_used.data, 0, webcam_pix_used.total() * webcam_pix_used.elemSize());

    cv::Mat depth(dpthrows, dpthcols, CV_16UC1, inputData.depth.data);

    // Du/Dv are always 0
    float mux = 0; // rOpticalDataWebcam.TranslateUV.X();
    float muy = 0; //  rOpticalDataWebcam.TranslateUV.Y();

    // maxZ = rOpticalData.mFocalR*rOpticalData.mDistance/(float)inputData.DPE_MaxDisparity; /** 5 meters **/

    // Output. 
    // NOTE: No need to zero out - all data is written to in the code below.
    // NOTE/TODO: Does not need a cv::Mat - only used in WriteDebugFiles() (optional)
    colr_onto_depth = cv::Mat(dpthrows, dpthcols, CV_8UC4, results.RGB_data.data);

    /*********** ROTATION - definition & initialisation ***************/
    cv::Mat rotat_mtrx = cv::Mat::zeros(3, 3, CV_32F); /* rotation vector must be translated by Rodrigues2 into rotation matrix */
    cv::Mat rotat_vectr = cv::Mat::zeros(3, 1, CV_32F);

    rotat_vectr.at<float>(0, 0) = float(extrinsicRealWebcam.Rotation[0]);
    rotat_vectr.at<float>(1, 0) = float(extrinsicRealWebcam.Rotation[1]);
    rotat_vectr.at<float>(2, 0) = float(extrinsicRealWebcam.Rotation[2]);

    FT::Rodrigues_(rotat_vectr, rotat_mtrx);

    cv::Mat XYZmat;

    if (results.XYZ_Buff.data)
    {
        XYZmat = cv::Mat(dpthrows, 4 * dpthcols, CV_32F, results.XYZ_Buff.data);
        memset(XYZmat.data, 0, XYZmat.total() * XYZmat.elemSize());
    }

    bool _omp = this->openMP;

#pragma omp parallel for if (_omp)
    for (int iydepth = 0; iydepth < dpthrows; iydepth++)
    {
        // ===================== IF WEBCAM to the RIGHT from the DEPTH_CAM =====================

        unsigned short* p_depth = depth.ptr<unsigned short>(iydepth); // disparity input
        uint32_t* p_cod = colr_onto_depth.ptr<uint32_t>(iydepth); // output
        float* pInd = results.XYZ_Buff.data ? XYZmat.ptr<float>(iydepth) : nullptr; // PointCloud

        // ===================== IF WEBCAM to the LEFT from the DEPTH_CAM =====================

        for (int ixdepth = 0; ixdepth < dpthcols; ixdepth++, p_depth++, p_cod++, pInd += 4)
        {
            // ixdepth is synchronised with disparity/depth map, so point(iydepth,ixdepth,z)
            // corresponds to depth|disparity(iydepth,ixdepth) 

            float z_curr = float(p_depth[0]);

            if (z_curr > maxZ || z_curr <= minZ)
            {
                if (results.XYZ_Buff.data)
                {
                    pInd[0] = 0;
                }

                *p_cod = 0x00000000u;

                continue; /* zero -> nonvalid 3Dpoint entry */
            }

            float x_curr = z_curr * (ixdepth * reciprocalFocalX - opticalCenterToFocalLengthX);
            float y_curr = z_curr * (iydepth * reciprocalFocalY - opticalCenterToFocalLengthY);

            if (results.XYZ_Buff.data)
            {
                pInd[0] = 1;
                pInd[1] = x_curr;
                pInd[2] = -y_curr;
                pInd[3] = -z_curr;
            }

            // reproject XYZ onto the imaginary (undistorted/rectified) viewport of RGB camera
            // defined by RGB/DepthCam extrinsic params and RGB undistorted center+resolution...
            // output = (u_undist, v_undist)
            // It's like the XYZ point is first translated from the depthCam coord-s to the 
            // RGB-based coordinates...

            // rotate + translate

            float* rdata = reinterpret_cast<float*>(rotat_mtrx.data);

            float x_RGB = rdata[0] * x_curr + rdata[1] * y_curr + rdata[2] * z_curr + extrinsicRealWebcamTranslationX;
            float y_RGB = rdata[3] * x_curr + rdata[4] * y_curr + rdata[5] * z_curr + extrinsicRealWebcamTranslationY;
            float z_RGB = rdata[6] * x_curr + rdata[7] * y_curr + rdata[8] * z_curr + extrinsicRealWebcamTranslationZ;

            // end rotation translation

            // re-distort (u_undist, v_undist), i.e., find (u,v) on the original Color pixelmap...
            // which is DISTORTED, UNRECTIFIED

            float xnorm = x_RGB / z_RGB;
            float ynorm = y_RGB / z_RGB;

            float u_ = intrinsicRealWebCacmOpticalCenterX + intrinsicRealWebCacmFocalLengthX * xnorm + mux;
            float v_ = intrinsicRealWebCacmOpticalCenterY + intrinsicRealWebCacmFocalLengthY * ynorm + muy;

            int col_ = int(u_);
            int row_ = int(v_);

            // check if the re-distorted point is within the webcam pixelmap

            if (false
                || row_ < 0
                || row_ >= webcamrows - 1
                || col_ < 0
                || col_ >= webcamcols - 1
                )
            {
                // for the pixels that are beyond the color map - invalid (black)

                *p_cod = 0xff000000u;
                continue;
            }

            // assign this color to the current DepthMap/colr_onto_depth "pixel"(idep, jdep)

            // check if the webcam pixel was not used previously... TBD!!! but what
            // to do with the case when the webcam pixel found has to be used more
            // than once because it corresponds to more than one depthmap
            // pixels because of undistortion and resizing ?!!! 

            // either zero (color pixel was untouched until now)...

            float prev_depth = webcam_pix_used.at<float>(row_, col_);

            // ...or the depth to which this color is assigned

            if (!prev_depth || z_curr < prev_depth || z_curr - prev_depth < 50)
            {
                /* this color pix is being used for the 1st time, or it is closer to the sensors */

                float dr = v_ - float(row_);
                float dc = u_ - float(col_);

                float omdr = 1.0f - dr;
                float omdc = 1.0f - dc;

                float drdc = dr * dc;
                float omdrdc = omdr * dc;
                float dromdc = dr * omdc;
                float omdromdc = omdr * omdc;

                uint32_t* pVIr0 = webimg.ptr<uint32_t>(row_, col_);
                uint32_t* pVIr1 = webimg.ptr<uint32_t>(row_ + 1, col_);

                uint32_t _VIr0c0 = pVIr0[0];
                uint32_t _VIr1c0 = pVIr0[1];
                uint32_t _VIr0c1 = pVIr1[0];
                uint32_t _VIr1c1 = pVIr1[1];

                uint32_t icolor = 0xff000000u; // initial output pixel value

                /* iteration 0 */

                float Ir0c0 = float(_VIr0c0 & 0xffu);
                float Ir1c0 = float(_VIr1c0 & 0xffu);
                float Ir0c1 = float(_VIr0c1 & 0xffu);
                float Ir1c1 = float(_VIr1c1 & 0xffu);

                icolor |= uchar(Ir0c0 * omdromdc + Ir1c0 * dromdc + Ir0c1 * omdrdc + Ir1c1 * drdc);

                _VIr0c0 >>= 8;
                _VIr1c0 >>= 8;
                _VIr0c1 >>= 8;
                _VIr1c1 >>= 8;

                /* iteration 1 */

                Ir0c0 = float(_VIr0c0 & 0xffu);
                Ir1c0 = float(_VIr1c0 & 0xffu);
                Ir0c1 = float(_VIr0c1 & 0xffu);
                Ir1c1 = float(_VIr1c1 & 0xffu);

                icolor |= uchar(Ir0c0 * omdromdc + Ir1c0 * dromdc + Ir0c1 * omdrdc + Ir1c1 * drdc) << 8;

                _VIr0c0 >>= 8;
                _VIr1c0 >>= 8;
                _VIr0c1 >>= 8;
                _VIr1c1 >>= 8;

                /* iteration 2 */

                Ir0c0 = float(_VIr0c0 & 0xffu);
                Ir1c0 = float(_VIr1c0 & 0xffu);
                Ir0c1 = float(_VIr0c1 & 0xffu);
                Ir1c1 = float(_VIr1c1 & 0xffu);

                icolor |= uchar(Ir0c0 * omdromdc + Ir1c0 * dromdc + Ir0c1 * omdrdc + Ir1c1 * drdc) << 16;

                *p_cod = icolor;

                webcam_pix_used.at<float>(row_, col_) = p_depth[0];
            }
            else
            {
                // This color pix has been used already - for a point that deserves to be colored
                // because it was closer to the baseline. We write down this new point as well. We
                // just make it grey.

                *p_cod = 0xff565656u;
            }
        }
    }

    SetWebCamIntegrResults(inputData, results);

    WriteDebugFiles(inputData, colr_onto_depth); // only in derived class...

    return true;
}

void CWebcamIntegrationImp::buildXYZmat(
    const cv::Mat& depth,
    const InuCommon::CCalibrationData::COneSensorData::CCameraData::CIntrinsicData& rOpticalDataWebcam,
    cv::Mat& XYZmat
)
{
    int dpthrows = depth.rows;
    int dpthcols = depth.cols;

    float reciprocalFocalx = float(1.f / rOpticalDataWebcam.FocalLength[0]);
    float reciprocalFocaly = float(1.f / rOpticalDataWebcam.FocalLength[1]);

    for (int iydepth = 0; iydepth < dpthrows; iydepth++)
    {
        const unsigned short* p_depth = depth.ptr<const unsigned short>(iydepth);

        float* pInd = XYZmat.ptr<float>(iydepth);

        for (int ixdepth = 0; ixdepth < dpthcols; ixdepth++, p_depth++, pInd += 4)
        {
            /* ixdepth is synchronised with disparity/depth map, so point(iydepth,ixdepth,z) corresponds to depth|dispar(iydepth,ixdepth) */
            float yes_valid = float(p_depth[0]);

            if (yes_valid > maxZ || yes_valid <= minZ)
            {
                pInd[0] = 0;
                continue; /* zero -> nonvalid 3Dpoint entry */
            }

            float z_curr = p_depth[0];

            float x_curr = ((float)ixdepth - float(rOpticalDataWebcam.OpticalCenter[0])) * z_curr * reciprocalFocalx;
            float y_curr = ((float)iydepth - float(rOpticalDataWebcam.OpticalCenter[1])) * z_curr * reciprocalFocaly;

            pInd[0] = 1;
            pInd[1] = x_curr;
            pInd[2] = -y_curr;
            pInd[3] = -z_curr;
        }
    }
}

bool CWebcamIntegrationImp::WebcamIntegrationProcessWebUndistortion(
    const sCVE_StreamsVec& inputData,
    sCVE_WebcamIntegration_Results& results,
    const sCVE_ConfigImp* configData
)
{
    const int webcamrows = inputData.webcam.height;
    const int webcamcols = inputData.webcam.width;

    /********Intepolate Undistort Web*********/

    int dt{ -1 };

    switch (inputData.webcam.pixelSize)
    {
    case 4:
    case -1:

        dt = CV_8UC4;
        break;

    case 3:

        dt = CV_8UC3;
        break;

    default:

        return false; // Unknown
    }

    cv::Mat webimg = cv::Mat(webcamrows, webcamcols, dt, inputData.webcam.data);
    cv::Mat web_undistorted(webimg.rows, webimg.cols, dt, results.RGB_data.data);

    _sCVE_WebcamIntegrationConfigImp* wcic =
        dynamic_cast<_sCVE_WebcamIntegrationConfigImp*>(configData->WebcamIntegrationConfig);

    cv::Mat mapx(webimg.rows, webimg.cols, CV_32FC1, wcic->Lutx);
    cv::Mat mapy(webimg.rows, webimg.cols, CV_32FC1, wcic->Luty);

    WebCamInterClass pWI;
    pWI.rectify_undistort(webimg, mapx, mapy, web_undistorted);

    return true;
}

bool CWebcamIntegrationImp::WebcamIntegrationProcessDepth2Web(
    const sCVE_StreamsVec& inputData,
    sCVE_WebcamIntegration_Results& results,
    const sCVE_ConfigImp* configData,
    bool undistort
)
{
    if (!results.Converted_ZBuff.data || !inputData.depth.data)
    {
        // Must provide input depth, and a place to locate the output created.

        return false;
    }

    const InuCommon::CCalibrationData& rOpticalDataVideo = configData->mOpticalDataDepth;
    const InuCommon::CCalibrationData& rOpticalDataWebcam = configData->mOpticalDataWebcam;

    const auto& intrinsicVirtualRight =
        rOpticalDataVideo.Sensors.at(configData->mDepthRightAndLeftSensors.first).VirtualCamera.Intrinsic;

    if (intrinsicVirtualRight.FocalLength[0] <= 0)
    {
        return false;
    }

    // Du/Dv are always 0
    float mux = 0; // rOpticalDataWebcam.TranslateUV.X();
    float muy = 0; //  rOpticalDataWebcam.TranslateUV.Y();

    /*********** ROTATION - definition & initialisation ***************/

    cv::Mat rotat_mtrx = cv::Mat::zeros(3, 3, CV_32F); /* rotation vector must be translated by Rodrigues2 into rotation matrix */
    cv::Mat rotat_vectr = cv::Mat::zeros(3, 1, CV_32F);

    const auto& odwe{ rOpticalDataVideo.Sensors.at(configData->mWebcamSensor) };

    const auto& extrinsicRealWebcam = (false
            || undistort
            || !odwe.VirtualCamera.Extrinsic.Valid
        )
        ? odwe.RealCamera.Extrinsic
        : odwe.VirtualCamera.Extrinsic
        ;

    if (!extrinsicRealWebcam.Valid)
    {
        return false;
    }

    rotat_vectr.at<float>(0, 0) = float(extrinsicRealWebcam.Rotation[0]);
    rotat_vectr.at<float>(1, 0) = float(extrinsicRealWebcam.Rotation[1]);
    rotat_vectr.at<float>(2, 0) = float(extrinsicRealWebcam.Rotation[2]);
    FT::Rodrigues_(rotat_vectr, rotat_mtrx);

    /*********** definition & initialisation *************/

    float reciprocalFocalX = float(1. / intrinsicVirtualRight.FocalLength[0]);
    float reciprocalFocalY = float(1. / intrinsicVirtualRight.FocalLength[1]);

    const auto& odwi{ rOpticalDataWebcam.Sensors.at(configData->mWebcamSensor) };

    const auto& intrinsicRealWebcam = (false
            || undistort
            || !odwi.VirtualCamera.Intrinsic.Valid
        )
        ? odwi.RealCamera.Intrinsic
        : odwi.VirtualCamera.Intrinsic
        ;

    if (!intrinsicRealWebcam.Valid)
    {
        return false;
    }

    const int webcamrows = inputData.webcam.height;
    const int webcamcols = inputData.webcam.width;
    const int dpthrows = inputData.depth.height;
    const int dpthcols = inputData.depth.width;

    Z2Web = cv::Mat(webcamrows, webcamcols, CV_16U, results.Converted_ZBuff.data);

    // MUST zero - algo assumes pre-initiazlied to value of '0'
    memset(Z2Web.data, 0, Z2Web.total() * Z2Web.elemSize());
    cv::Mat depth(dpthrows, dpthcols, CV_16UC1, inputData.depth.data);

    cv::Mat XYZmat;

    if (results.XYZ_Buff.data)
    {
        XYZmat = cv::Mat(dpthrows, 4 * dpthcols, CV_32F, results.XYZ_Buff.data);

        // MUST zero - size is determinated by the numer of non-zero elements
        memset(XYZmat.data, 0, XYZmat.total() * XYZmat.elemSize());
    }

    const bool CLOSE_HOLES = configData->m_DepthRegistrationCloseHolesWindowSize != 0;

    cv::Mat pixelMask;

    if (CLOSE_HOLES)
    {
        pixelMask = cv::Mat::zeros(webcamrows, webcamcols, CV_8U);
    }

    //maxZ = rOpticalData.mFocalR*rOpticalData.mDistance/(float)inputData.DPE_MaxDisparity; /** 5 meters **/

    bool _omp = this->openMP;
    unsigned short z2_prev = 0;
    unsigned short z_curr_ = 0;

#pragma omp parallel for if (_omp)
    for (int iydepth = 0; iydepth < dpthrows; iydepth++)
    {
        // ===================== IF WEBCAM to the RIGHT from the DEPTH_CAM =====================

        unsigned short* p_depth = depth.ptr<unsigned short>(iydepth);
        float* pInd = results.XYZ_Buff.data ? XYZmat.ptr<float>(iydepth) : nullptr;

        // ===================== IF WEBCAM to the LEFT from the DEPTH_CAM =====================
        for (int ixdepth = 0; ixdepth < dpthcols; ixdepth++, p_depth++, pInd += 4)
        {
            /* ixdepth is synchronised with disparity/depth map, so point(iydepth,ixdepth,z) corresponds to depth|dispar(iydepth,ixdepth) */
            float yes_valid = float(p_depth[0]);

            if (yes_valid > maxZ || yes_valid <= minZ)
            {
                continue; /* zero -> nonvalid 3Dpoint entry */
            }

            float z_curr = p_depth[0];

            float x_curr = float((ixdepth - intrinsicVirtualRight.OpticalCenter[0]) * z_curr * reciprocalFocalX);
            float y_curr = float((iydepth - intrinsicVirtualRight.OpticalCenter[1]) * z_curr * reciprocalFocalY);

            if (results.XYZ_Buff.data)
            {
                pInd[0] = 1;
                pInd[1] = x_curr;
                pInd[2] = -y_curr;
                pInd[3] = -z_curr;
            }

            // reproject XYZ onto the imaginary (undistorted/rectified) viewport of RGB camera defined by RGB/DepthCam extrinsic params and
            //    RGB undistorted center+resolution... output = (u_undist, v_undist)
            //    It's like the XYZ point is first translated from the depthCam coord-s to the RGB-based coordinates...
            /*** rotate + translate************/

            float* rdata = reinterpret_cast<float*>(rotat_mtrx.data);
            float x_RGB = rdata[0] * x_curr + rdata[1] * y_curr + rdata[2] * z_curr + float(extrinsicRealWebcam.Translation[0]);
            float y_RGB = rdata[3] * x_curr + rdata[4] * y_curr + rdata[5] * z_curr + float(extrinsicRealWebcam.Translation[1]);
            float z_RGB = rdata[6] * x_curr + rdata[7] * y_curr + rdata[8] * z_curr + float(extrinsicRealWebcam.Translation[2]);
            double abs_z_RGB = fabs(z_RGB);

            /***************** end rotation translation ******************************************/

            if (abs_z_RGB < 1.e-10)
            {
                // TODO: Likely unneeded. Will cause this pixel to be skipped later
                // in the code - can "continue" here. Also, does not seem to enter here ever...
                z_RGB = float(abs_z_RGB * 1.e-3);
            }

            // re-distort (u_undist, v_undist), i.e., find (u,v) on the original Color pixelmap... which is DISTORTED, UNRECTIFIED
            double xnorm = x_RGB / z_RGB;
            double ynorm = y_RGB / z_RGB;

            double u_ = intrinsicRealWebcam.OpticalCenter[0] + intrinsicRealWebcam.FocalLength[0] * xnorm + mux;
            double v_ = intrinsicRealWebcam.OpticalCenter[1] + intrinsicRealWebcam.FocalLength[1] * ynorm + muy;

            int col_ = int(u_);
            int row_ = int(v_);

            /* check if the re-distorted point is within the webcam pixelmap */

            if (row_ < 0 || row_ >= webcamrows || col_ < 0 || col_ >= webcamcols)
            {
                /* for the pixels that are beyond the color map - one level of gray */

                continue;
            }

            unsigned short z_curr_ = (unsigned short)z_RGB;
            int i_row = 0;
            int i_col = 0;

            for (int i_row = -1; i_row <= 1; i_row++)
            {
                if ((row_ + i_row) < 0 || (row_ + i_row) >= webcamrows)
                {
                    continue;
                }

                for (int i_col = -1; i_col <= 1; i_col++)
                {
                    if ((col_ + i_col) < 0 || (col_ + i_col) >= webcamcols)
                    {
                        continue;
                    }

                    unsigned short z2 = Z2Web.at<unsigned short>(row_ + i_row, col_ + i_col);

                    if (z2 == 0 || z2 < z_curr_)
                    {
                        Z2Web.at<unsigned short>(row_ + i_row, col_ + i_col) = z_curr_;
                    }
                }
            }

            // End Itai's change
        
        }
    }
//    if (CLOSE_HOLES)
//    {
//        // !!! TODO: later derive the values of those vars from webcam / depth opening angle and resolution
//        int W = configData->m_DepthRegistrationCloseHolesWindowSize;//nearst neighbor window width
//        int H = configData->m_DepthRegistrationCloseHolesWindowSize;//nearst neighbor window height
//        int W_h = W/2;
//        int H_h = H/2;
//
//#pragma omp parallel for if (_omp)
//        for (int row = 0; row < webcamrows; row++)
//        {
//            for (int col = 0; col < webcamcols; col++)
//            {
//                // Itai's changes:
//                unsigned short minDist_test = USHRT_MAX;
//                unsigned short pixelMask_test = USHRT_MAX;
//                minDist_test = Z2Web.at<unsigned short>(row, col);
//                pixelMask_test = pixelMask.at<unsigned char>(row, col);
//                if (pixelMask.at<unsigned char>(row, col) != 0)
//                {
//                    continue;
//                }
//
//                unsigned short minDist = USHRT_MAX;
//                // Itai's changes:
//                unsigned short min_ed = USHRT_MAX;
//                unsigned long sum_ed = 0;
//                int count = 0;
//                unsigned short avg = 0;
//                int n = 0;
//                int m = 0;
//
//
//                for (int m = -H_h; m <= H_h; m++)
//                {
//                    if ((row + m) < 0 || (row + m) >= webcamrows)
//                    {
//                        continue;
//                    }
//
//                    for (int n = -W_h; n <= W_h; n++)
//                    {
//                        if ((col + n) < 0 || (col + n) >= webcamcols)
//                        {
//                            continue;
//                        }
//                        //
//                        //minDist = Z2Web.at<unsigned short>(row + m, col + n);
//                        //count = count + 1;
//                        //sum_ed = sum_ed + mindist;
//                        minDist = Z2Web.at<unsigned short>(row + m, col + n);
//                        if (minDist < min_ed) 
//                        {
//                            min_ed = minDist;
//                        }
//
//                        if (true
//                            && pixelMask.at<unsigned char>(row + m, col + n) != 0
//                            && Z2Web.at<unsigned short>(row + m, col + n) < USHRT_MAX
//                            )
//                        {
//                            Z2Web.at<unsigned short>(row, col) = min_ed;
//                        }
//                    }
//                }
//            }
//        }
//    }

    /********* end mapping RGB onto depth; now save the results ********************/

    BuildPLY(webcamrows, webcamcols, rOpticalDataWebcam, results, configData, undistort);

    SetWebCamIntegrResults(inputData, results);

    WriteDebugFiles(inputData, cv::Mat()); // Only in derived class...

    return true;
}

bool CWebcamIntegrationImp::Process(
    const sCVE_StreamsVec& inputData,
    sCVE_WebcamIntegration_Results& results,
    const sCVE_Config* _configData,
    bool undistort,
    bool overrideUseVirtualCameraOpticalData
)
{
    bool result = false;

    const sCVE_ConfigImp* configData = dynamic_cast<const sCVE_ConfigImp*>(_configData);

    switch (configData->WebcamIntegrationConfig->eMode)
    {
    case eWI_WebcamIntegrationMode::eRGB2Depth:

        result = WebcamIntegrationProcessWeb2Depth(inputData, results, configData, undistort, overrideUseVirtualCameraOpticalData);
        break;

    case eWI_WebcamIntegrationMode::eDepth2RGB:

        result = WebcamIntegrationProcessDepth2Web(inputData, results, configData, undistort);
        break;

    case eWI_WebcamIntegrationMode::eRGBUndistortion:

        result = WebcamIntegrationProcessWebUndistortion(inputData, results, configData);
        break;
    }

    return result;
}

class _CWebcamIntegrationImpPly : public CWebcamIntegrationImp
{
    cv::Mat XYZWebmat;

    void BuildPLY(
        const int webcamrows,
        const int webcamcols,
        const InuCommon::CCalibrationData& rOpticalDataWebcam,
        sCVE_WebcamIntegration_Results& results,
        const sCVE_Config* configData,
        bool undistort
    );

    void WriteDebugFiles(
        const sCVE_StreamsVec& inputData,
        cv::Mat colr_onto_depth
    );

    static void WritePLY(
        const cv::Mat& matXYZ,
        const cv::Mat& matRGBRef,
        const std::string& outName,
        bool binary_only
    );
};

void _CWebcamIntegrationImpPly::BuildPLY(
    const int _webcamrows,
    const int _webcamcols,
    const InuCommon::CCalibrationData& rOpticalDataWebcam,
    sCVE_WebcamIntegration_Results& results,
    const sCVE_Config* _configData,
    bool undistort
)
{
    const InuCommon::CCalibrationData::COneSensorData::CCameraData::CIntrinsicData& c = undistort
        ? rOpticalDataWebcam.Sensors.at(_configData->mWebcamSensor).RealCamera.Intrinsic
        : rOpticalDataWebcam.Sensors.at(_configData->mWebcamSensor).VirtualCamera.Intrinsic
        ;

    XYZWebmat = cv::Mat(_webcamrows, 4 * _webcamcols, CV_32F, results.XYZ_Buff.data);

    buildXYZmat(Z2Web, c, XYZWebmat);
}

void _CWebcamIntegrationImpPly::WriteDebugFiles(
    const sCVE_StreamsVec& inputData,
    cv::Mat colr_onto_depth
)
{
    char filnam1[96];
    char datenow[9], timenow[9], tempchar[2];

#if _MSC_VER && __STDC_WANT_SECURE_LIB__
    _strdate_s(datenow, sizeof(datenow));
    _strtime_s(timenow, sizeof(timenow));

    datenow[2] = datenow[5] = timenow[2] = timenow[5] = '_'; /* for filename - get rid of forbidden symbols like ":" and "/" */
    tempchar[0] = datenow[0]; tempchar[1] = datenow[1]; /* MM-DD-YY to YY-MM-DD */
    datenow[0] = datenow[6]; datenow[1] = datenow[7];
    datenow[6] = datenow[3]; datenow[7] = datenow[4];
    datenow[3] = tempchar[0]; datenow[4] = tempchar[1];

    sprintf_s(filnam1, sizeof(filnam1), "C:\\temp\\%08I64d_%s_%s_", inputData.timeStamp, datenow, timenow);
#elif defined __linux__
    time_t t;
    time(&t);
    struct tm* tmp = localtime(&t);
    char date_time_now[256];
    strftime(date_time_now, sizeof(date_time_now), "%F_%H_%M", tmp);

    sprintf(filnam1, "/tmp/%08ld_%s_", inputData.timeStamp, date_time_now);
#else
#endif

    if (writePLYwebcam && !colr_onto_depth.empty())
    {
        /************************ write PLY (webcam) ***********************/

        std::string fname = std::string(filnam1) + ".ply";
        WritePLY(XYZWebmat, colr_onto_depth, fname, true);
    }

    if (writeColorDepthimg && !colr_onto_depth.empty())
    {
        /***************** write color+depth image **************/

        char filnamcolordepth[64];
#if _MSC_VER && __STDC_WANT_SECURE_LIB__
        sprintf_s(filnamcolordepth, sizeof(filnamcolordepth), "%s%s", filnam1, "colordepth.png");
#else
        sprintf(filnamcolordepth, "%s%s", filnam1, "colordepth.png");
#endif

        //cv::imwrite(filnamcolordepth, colr_onto_depth);

        fs::path fn(filnam1);
        fn += "colordepth";

        InuCommon::CTiffFile::EError rc = InuCommon::CTiffFile::Write(
            fn.string(),
            colr_onto_depth.data,
            colr_onto_depth.cols,
            colr_onto_depth.rows,
            InuCommon::CTiffFile::ETiffFormat::eBGRA // TODO: Find how to set the correct format (can be either BGRA or RGBA...)
        );
    }
}

#if 1
void _CWebcamIntegrationImpPly::WritePLY(
    const cv::Mat& matXYZ,
    const cv::Mat& matRGBRef,
    const std::string& outName,
    bool binary_only
)
{
    InuCommon::CAlgoBase::WriteToPly(matXYZ, matRGBRef, outName, binary_only);
}
#else

// Dplicates CAlgoBase code...

void _CWebcamIntegrationImpPly::WritePLY(
    const cv::Mat& matXYZ,
    const cv::Mat& matRGBRef,
    const std::string& outName,
    bool binary_only
)
{
    std::vector<cv::Mat> rgbLayers;
    int channel = matRGBRef.channels();

    if (channel == 3)
    {
        split(matRGBRef, rgbLayers);
    }
    else
    {
        rgbLayers.push_back(matRGBRef);
        rgbLayers.push_back(matRGBRef);
        rgbLayers.push_back(matRGBRef);
    }

    // count number of valid points
    int nVertex = 0;
    int w = matXYZ.cols / 4;
    int h = matXYZ.rows;

    for (int j = 0; j < h; j++)
    {
        const float* pInd = matXYZ.ptr<float>(j);

        for (int i = 0; i < w; i++, pInd += 4)
        {
            if (*pInd > 0)
            {
                nVertex++;
            }
        }
    }

    // write ply header. Open file as ASCII, overwrite existing.

    std::ofstream outPly, outCsv;
    outPly.open(outName, std::ios::out);

    if (!binary_only)
    {
        outCsv.open(outName + ".csv", std::ios::out);

        if (outCsv.is_open())
        {
            outCsv << "row,col,X,Y,Z,R,G,B\n";
        }
    }

    outPly << "ply\n";

    if (binary_only)
    {
        outPly << "format binary_little_endian 1.0\n";
    }
    else
    {
        outPly << "format ascii 1.0\n";
    }

    outPly
        << "comment CSE generated\n"
        // << "element vertex  "<<nVertex + 5 << "\n"
        // << "element vertex  " << nVertex << "\n"
        << "property float x\n"
        << "property float y\n"
        << "property float z\n"
        << "property uchar red\n"
        << "property uchar green\n"
        << "property uchar blue\n"
        << "property uchar alpha\n"
        << "end_header\n"
        ;

    outPly.close();

    // reopn file for writing data

    if (binary_only)
    {
        outPly.open(outName, std::ios::out | std::ios::binary | std::ios::app);
    }
    else
    {
        outPly.open(outName, std::ios::out | std::ios::app);
    }

    const uchar alpha{ 255 };

    // second scan - write points to ply

    for (int j = 0; j < h; j++)
    {
        const float* pInd = matXYZ.ptr<float>(j);
        const float* pX = pInd + 1;
        const float* pY = pX + 1;
        const float* pZ = pY + 1;

        uchar* pR = rgbLayers[2].ptr<uchar>(j);
        uchar* pG = rgbLayers[1].ptr<uchar>(j);
        uchar* pB = rgbLayers[0].ptr<uchar>(j);

        for (int i = 0; i < w; i++)
        {
            if (*pInd > 0) // valid point
            {
                if (binary_only)
                {
                    outPly.write((const char*)pX, sizeof(pX));
                    outPly.write((const char*)pY, sizeof(pY));
                    outPly.write((const char*)pZ, sizeof(pZ));
                    outPly.write((const char*)pR, sizeof(pR));
                    outPly.write((const char*)pG, sizeof(pG));
                    outPly.write((const char*)pB, sizeof(pB));
                    outPly.write((const char*)&alpha, sizeof(alpha));

                    if (outCsv.is_open())
                    {
                        char line[256];
                        sprintf_s(line, sizeof(line), "%d,%d,%0.3f,%0.3f,%0.3f,%d,%d,%d\n", j, i, *pX, *pY, *pZ, int(*pR), int(*pG), int(*pR));
                        outCsv << line;
                    }
                }
                else
                {
                    outPly << *pX << "\t" << *pY << "\t" << *pZ << "\t" << int(*pR) << "\t" << int(*pG) << "\t" << int(*pB) << "\t" << 255 << "\n";
                }

            }

            pInd += 4;
            pX += 4;
            pY += 4;
            pZ += 4;
            pB++;
            pG++;
            pR++;
        }
    }

    outPly.close();
}
#endif

class CWebcamIntegrationSSEImp : public CWebcamIntegrationImp
{
    friend class CWebcamIntegrationBase;

protected:

    CWebcamIntegrationSSEImp();
    virtual ~CWebcamIntegrationSSEImp();

public:

    virtual bool WebcamIntegrationProcessWeb2Depth(
        const sCVE_StreamsVec& inputData,
        sCVE_WebcamIntegration_Results& results,
        const sCVE_ConfigImp* configData,
        bool undistort
    );

    virtual bool WebcamIntegrationProcessDepth2Web(
        const sCVE_StreamsVec& inputData,
        sCVE_WebcamIntegration_Results& results,
        const sCVE_ConfigImp* configData,
        bool undistort
    );

    virtual bool WebcamIntegrationProcessWebUndistortion(
        const sCVE_StreamsVec& inputData,
        sCVE_WebcamIntegration_Results& results,
        const sCVE_ConfigImp* configData
    );
};

bool CWebcamIntegrationSSEImp::WebcamIntegrationProcessWeb2Depth(
    const sCVE_StreamsVec& inputData,
    sCVE_WebcamIntegration_Results& results,
    const sCVE_ConfigImp* configData,
    bool undistort
)
{
    const int webcamrows = inputData.webcam.height;
    const int webcamcols = inputData.webcam.width;
    const int dpthrows = inputData.depth.height;
    const int dpthcols = inputData.depth.width;

    cv::Mat webimg = cv::Mat(webcamrows, webcamcols, CV_8UC4, inputData.webcam.data);

    if (undistort)
    {
        cv::Mat web_rectified;

        _sCVE_WebcamIntegrationConfigImp* wcic =
            dynamic_cast<_sCVE_WebcamIntegrationConfigImp*>(configData->WebcamIntegrationConfig);

        cv::Mat mapx(webcamrows, webcamcols, CV_32FC1, wcic->Lutx);
        cv::Mat mapy(webcamrows, webcamcols, CV_32FC1, wcic->Luty);

        WebCamInterClass pWI;
        pWI.rectify_undistort(webimg, mapx, mapy, web_rectified);

        webimg = web_rectified;
    }

    // Handle all thing optical data...

    const auto& rOpticalDataVideo = configData->mOpticalDataDepth;
    const auto& rOpticalDataWebCam = configData->mOpticalDataWebcam;

    const auto& intrinsicVirtualRight =
        rOpticalDataVideo.Sensors.at(configData->mDepthRightAndLeftSensors.first).VirtualCamera.Intrinsic;

    float reciprocalFocalX = float(1.0 / intrinsicVirtualRight.FocalLength[0]);
    float reciprocalFocalY = float(1.0 / intrinsicVirtualRight.FocalLength[1]);

    float opticalCenterToFocalLengthX = float(intrinsicVirtualRight.OpticalCenter[0] / intrinsicVirtualRight.FocalLength[0]);
    float opticalCenterToFocalLengthY = float(intrinsicVirtualRight.OpticalCenter[1] / intrinsicVirtualRight.FocalLength[1]);

    const auto& extrinsicRealWebcam = undistort
        ? rOpticalDataVideo.Sensors.at(configData->mWebcamSensor).RealCamera.Extrinsic
        : rOpticalDataVideo.Sensors.at(configData->mWebcamSensor).VirtualCamera.Extrinsic
        ;

    float extrinsicRealWebcamTranslationX = float(extrinsicRealWebcam.Translation[0]);
    float extrinsicRealWebcamTranslationY = float(extrinsicRealWebcam.Translation[1]);
    float extrinsicRealWebcamTranslationZ = float(extrinsicRealWebcam.Translation[2]);
    const auto& intrinsicRealWebCacm = undistort
        ? rOpticalDataWebCam.Sensors.at(configData->mWebcamSensor).RealCamera.Intrinsic
        : rOpticalDataWebCam.Sensors.at(configData->mWebcamSensor).VirtualCamera.Intrinsic
        ;


    float intrinsicRealWebCacmFocalLengthX = float(intrinsicRealWebCacm.FocalLength[0]);
    float intrinsicRealWebCacmFocalLengthY = float(intrinsicRealWebCacm.FocalLength[1]);

    float intrinsicRealWebCacmOpticalCenterX = float(intrinsicRealWebCacm.OpticalCenter[0]);
    float intrinsicRealWebCacmOpticalCenterY = float(intrinsicRealWebCacm.OpticalCenter[1]);

    if (intrinsicVirtualRight.FocalLength[0] <= 0)
    {
        return false;
    }

    // End optical data mess

    // to prevent the effect of false coloring of points occluded from
    cv::Mat webcam_pix_used(webcamrows, webcamcols, CV_32F);
    memset(webcam_pix_used.data, 0, webcam_pix_used.total() * webcam_pix_used.elemSize());

    cv::Mat depth(dpthrows, dpthcols, CV_16UC1, inputData.depth.data);

    // Du/Dv are always 0
    float mux = 0; // rOpticalDataWebcam.TranslateUV.X();
    float muy = 0; //  rOpticalDataWebcam.TranslateUV.Y();

    // maxZ = rOpticalData.mFocalR*rOpticalData.mDistance/(float)inputData.DPE_MaxDisparity; /** 5 meters **/

    colr_onto_depth = cv::Mat(dpthrows, dpthcols, CV_8UC4, results.RGB_data.data);

    /*********** ROTATION - definition & initialisation ***************/
    cv::Mat rotat_mtrx = cv::Mat::zeros(3, 3, CV_32F); /* rotation vector must be translated by Rodrigues2 into rotation matrix */
    cv::Mat rotat_vectr = cv::Mat::zeros(3, 1, CV_32F);

    rotat_vectr.at<float>(0, 0) = float(extrinsicRealWebcam.Rotation[0]);
    rotat_vectr.at<float>(1, 0) = float(extrinsicRealWebcam.Rotation[1]);
    rotat_vectr.at<float>(2, 0) = float(extrinsicRealWebcam.Rotation[2]);

    FT::Rodrigues_(rotat_vectr, rotat_mtrx);

    cv::Mat XYZmat;
    if (results.XYZ_Buff.data)
    {
        XYZmat = cv::Mat(dpthrows, 4 * dpthcols, CV_32F, results.XYZ_Buff.data);
        memset(XYZmat.data, 0, XYZmat.total() * XYZmat.elemSize());
    }
    bool _omp = this->openMP;

#pragma omp parallel for if (_omp)
    for (int iydepth = 0; iydepth < dpthrows; iydepth++)
    {
        // ===================== IF WEBCAM to the RIGHT from the DEPTH_CAM =====================

        unsigned short* p_depth = depth.ptr<unsigned short>(iydepth); // disparity input
        uint32_t* p_cod = colr_onto_depth.ptr<uint32_t>(iydepth); // output
        float* pInd = results.XYZ_Buff.data ? XYZmat.ptr<float>(iydepth) : nullptr; // PointCloud

        // ===================== IF WEBCAM to the LEFT from the DEPTH_CAM =====================

        for (int ixdepth = 0; ixdepth < dpthcols; ixdepth++, p_depth++, p_cod++, pInd += 4)
        {
             // ixdepth is synchronised with disparity/depth map, so point(iydepth,ixdepth,z)
             // corresponds to depth|disparity(iydepth,ixdepth) 

            float z_curr = float(p_depth[0]);

            if (z_curr > maxZ || z_curr <= minZ)
            {
                if (results.XYZ_Buff.data)
                {
                    pInd[0] = 0;
                }
                *p_cod = 0x00000000u;
                continue; /* zero -> nonvalid 3Dpoint entry */
            }

            float x_curr = z_curr * (ixdepth * reciprocalFocalX - opticalCenterToFocalLengthX);
            float y_curr = z_curr * (iydepth * reciprocalFocalY - opticalCenterToFocalLengthY);
            if (results.XYZ_Buff.data)
            {
                pInd[0] = 1;
                pInd[1] = x_curr;
                pInd[2] = -y_curr;
                pInd[3] = -z_curr;
            }

            // reproject XYZ onto the imaginary (undistorted/rectified) viewport of RGB camera
            // defined by RGB/DepthCam extrinsic params and RGB undistorted center+resolution...
            // output = (u_undist, v_undist)
            // It's like the XYZ point is first translated from the depthCam coord-s to the 
            // RGB-based coordinates...

            // rotate + translate

            float* rdata = reinterpret_cast<float*>(rotat_mtrx.data);

            float x_RGB = rdata[0] * x_curr + rdata[1] * y_curr + rdata[2] * z_curr + extrinsicRealWebcamTranslationX;
            float y_RGB = rdata[3] * x_curr + rdata[4] * y_curr + rdata[5] * z_curr + extrinsicRealWebcamTranslationY;
            float z_RGB = rdata[6] * x_curr + rdata[7] * y_curr + rdata[8] * z_curr + extrinsicRealWebcamTranslationZ;

            // end rotation translation

            // re-distort (u_undist, v_undist), i.e., find (u,v) on the original Color pixelmap...
            // which is DISTORTED, UNRECTIFIED

            float xnorm = x_RGB / z_RGB;
            float ynorm = y_RGB / z_RGB;

            float u_ = intrinsicRealWebCacmOpticalCenterX + intrinsicRealWebCacmFocalLengthX * xnorm + mux;
            float v_ = intrinsicRealWebCacmOpticalCenterY + intrinsicRealWebCacmFocalLengthY * ynorm + muy;

            int col_ = int(u_);
            int row_ = int(v_);

            // check if the re-distorted point is within the webcam pixelmap

            if (false
                || row_ < 0
                || row_ >= webcamrows - 1
                || col_ < 0
                || col_ >= webcamcols - 1
                )
            {
                // for the pixels that are beyond the color map - invalid (black)

                *p_cod = 0xff000000u;
                continue;
            }

            // assign this color to the current DepthMap/colr_onto_depth "pixel"(idep, jdep)

            // check if the webcam pixel was not used previously... TBD!!! but what
            // to do with the case when the webcam pixel found has to be used more
            // than once because it corresponds to more than one depthmap
            // pixels because of undistortion and resizing ?!!! 

            // either zero (color pixel was untouched until now)...

            float prev_depth = webcam_pix_used.at<float>(row_, col_);

            // ...or the depth to which this color is assigned

            if (!prev_depth || z_curr < prev_depth || z_curr - prev_depth < 50)
            {
                /* this color pix is being used for the 1st time, or it is closer to the sensors */

                float dr = v_ - float(row_);
                float dc = u_ - float(col_);

                float omdr = 1.0f - dr;
                float omdc = 1.0f - dc;

                uint32_t* pVIr0 = webimg.ptr<uint32_t>(row_, col_);
                uint32_t* pVIr1 = webimg.ptr<uint32_t>(row_ + 1, col_);

#if defined(_MSC_VER)
#if 1
                float drdc = dr * dc;
                float omdrdc = omdr * dc;
                float dromdc = dr * omdc;
                float omdromdc = omdr * omdc;

                __m256 dromdc_omdromdc_256 = _mm256_set_m128(_mm_set1_ps(dromdc), _mm_set1_ps(omdromdc));
                __m256 drdc_omdrdc_256 = _mm256_set_m128(_mm_set1_ps(drdc), _mm_set1_ps(omdrdc));
#else
                __m256 dromdc_omdromdc_256 = _mm256_set_m128(
                    _mm_mul_ps(_mm_set1_ps(dr), _mm_set1_ps(omdc)),
                    _mm_mul_ps(_mm_set1_ps(omdr), _mm_set1_ps(omdc))
                );

                __m256 drdc_omdrdc_256 = _mm256_set_m128(
                    _mm_mul_ps(_mm_set1_ps(dr), _mm_set1_ps(dc)),
                    _mm_mul_ps(_mm_set1_ps(omdr), _mm_set1_ps(dc))
                );
#endif

                __m128i _VIr0c1r0c0_128_ = _mm_loadl_epi64(reinterpret_cast<__m128i const*>(pVIr0));
                __m128i _VIr1c1r1c0_128_ = _mm_loadl_epi64(reinterpret_cast<__m128i const*>(pVIr1));

#if 1
                __m256i _VIr1c0Ir0c0_256 = _mm256_cvtepu8_epi32(_VIr0c1r0c0_128_);
                __m256i _VIr1c1Ir0c1_256 = _mm256_cvtepu8_epi32(_VIr1c1r1c0_128_);

                __m256 _VIr1c0Ir0c0_256ps = _mm256_cvtepi32_ps(_VIr1c0Ir0c0_256);
                __m256 _VIr1c1Ir0c1_256ps = _mm256_cvtepi32_ps(_VIr1c1Ir0c1_256);

                __m256 ic256_01 = _mm256_mul_ps(_VIr1c0Ir0c0_256ps, dromdc_omdromdc_256);
                __m256 ic256_23 = _mm256_mul_ps(_VIr1c1Ir0c1_256ps, drdc_omdrdc_256);

                __m256 icc256t = _mm256_add_ps(ic256_01, ic256_23);
                __m256 icc256 = _mm256_add_ps(_mm256_permute2f128_ps(icc256t, icc256t, 1), icc256t);

                __m256i icc256epi32 = _mm256_cvtps_epi32(icc256);

                __m256i icc256epi16 = _mm256_packus_epi32(icc256epi32, icc256epi32);
                __m256i icc256epi8 = _mm256_packus_epi16(icc256epi16, icc256epi16);

                *p_cod = 0xff000000u | _mm256_cvtsi256_si32(icc256epi8);
#else
                // TODO: SSE2
#endif

#endif
                webcam_pix_used.at<float>(row_, col_) = p_depth[0];
            }
            else
            {
                // This color pix has been used already - for a point that deserves to be colored
                // because it was closer to the baseline. We write down this new point as well. We
                // just make it grey.

                *p_cod = 0xff565656u;
            }
        }
    }

    SetWebCamIntegrResults(inputData, results);

    WriteDebugFiles(inputData, colr_onto_depth); // only in derived class...

    return true;
}

bool CWebcamIntegrationSSEImp::WebcamIntegrationProcessDepth2Web(
    const sCVE_StreamsVec& inputData,
    sCVE_WebcamIntegration_Results& results,
    const sCVE_ConfigImp* configData,
    bool undistort
)
{
    return CWebcamIntegrationImp::WebcamIntegrationProcessDepth2Web(
        inputData,
        results,
        configData,
        undistort
    );
}

bool CWebcamIntegrationSSEImp::WebcamIntegrationProcessWebUndistortion(
    const sCVE_StreamsVec& inputData,
    sCVE_WebcamIntegration_Results& results,
    const sCVE_ConfigImp* configData
)
{
    return CWebcamIntegrationImp::WebcamIntegrationProcessWebUndistortion(
        inputData,
        results,
        configData
    );
}

CWebcamIntegrationSSEImp::CWebcamIntegrationSSEImp()
{

}

CWebcamIntegrationSSEImp::~CWebcamIntegrationSSEImp()
{

}

CWebcamIntegrationBase* CWebcamIntegrationBase::Factory(
    InuCommon::COsUtilities::EOptimizationLevel iOptimizationLevel,
    bool openMP,
    bool writePLYwebcam,
    bool writeColorDepthimg
)
{
    CWebcamIntegrationImp* p{ nullptr };

    switch (iOptimizationLevel)
    {
    case InuCommon::COsUtilities::EOptimizationLevel::eNone:

        if (writePLYwebcam || writeColorDepthimg)
        {
            p = new _CWebcamIntegrationImpPly();

            p->writePLYwebcam = writePLYwebcam;
            p->writeColorDepthimg = writeColorDepthimg;
        }
        else
        {
            p = new CWebcamIntegrationImp();
        }

        break;

    case InuCommon::COsUtilities::EOptimizationLevel::eSSE:

        p = new CWebcamIntegrationSSEImp();

        break;

    case InuCommon::COsUtilities::EOptimizationLevel::eAVX:
    default:

        break;

    }

    if (!p)
    {
        return p;
    }

    p->openMP = openMP;
    p->writePLYwebcam = writePLYwebcam;
    p->writeColorDepthimg = writeColorDepthimg;

    return p;
}
