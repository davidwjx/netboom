#ifndef _WEB_CAM_INTEGRATION_
#define _WEB_CAM_INTEGRATION_

#include "AlgWebcamIntegrationDLL.h"

#include <opencv2/opencv.hpp>

#ifdef _MSC_VER
#pragma warning(disable : 4251)
#endif

enum PixelFormatType {
	DEFAULT = -1,
	Y8 = 0,
	Y10,
	Y12,
	Y16,
	YUV_420_16_COMPACT,
	YUV_422_8,
	YUV_1288,
	YUV8,
	RGB8,
	RGB16,
	BYR8,
	BYR10,
	BYR10SW,
	HISTOGRAMS,
	NUM_OF_PIXEL_FORMATS
};

enum eWI_WebcamIntegrationMode {
	eDepth2RGB = 0, //Depth2WebCam (WebCam Undistorted and Rotated) algo recieves WebCam Undist&Rotated
	eRGBUndistortion, // WebCam Undistortion and Rotation
	eRGB2Depth  // WebCam Registration (WebCam2Depth)
};

template <PixelFormatType FORMAT_TYPE>
struct ALG_WEBCAM_INTEGRATION_API sCVE_stream
{
	sCVE_stream() :width(0), height(0), data(0) {}
	static const		PixelFormatType type = FORMAT_TYPE;
	unsigned int		width;
	unsigned int		height;
	bool				isInterleaved;
	unsigned char* data;
	std::string		name;
//	eCVE_LightingType	lightingType;
};

struct ALG_WEBCAM_INTEGRATION_API sCVE_WebcamIntegration_Results
{
	sCVE_WebcamIntegration_Results() { frame_index = 0; }
	int frame_index;
	bool isUndistortedRGB;
	bool isRGBinDepthCoordinates;
	bool isDepthInRGBCoordinates;
	float Zunit_size;
	sCVE_stream<RGB8> RGB_data;
	sCVE_stream<Y16> Converted_ZBuff;
	sCVE_stream<Y16> XYZ_Buff;//raster of XYZ
	//PLY output
	sCVE_stream<RGB8> PointCloudRGB;//vector of RGB (valids only)
	sCVE_stream<Y16> PointCloudXYZ;//vector of XYZ (valids only)
};

struct ALG_WEBCAM_INTEGRATION_API sCVE_WebcamIntegrationConfig
{
	sCVE_WebcamIntegrationConfig() { memset(this, 0, sizeof(*this)); }
	~sCVE_WebcamIntegrationConfig() { delete[] Lutx; delete[] Luty; }

	float* Lutx, * Luty;//Structures for rectification LUTs

	eWI_WebcamIntegrationMode eMode;
	int RasterOutputFrameRate;//indicate 0 for no Raster output, 1 for each frame, N for each N'th frame;
	int PointCloudOutputFrameRate;//indicate 0 for no PLY output, 1 for each frame, N for each N'th frame;
	//float fc_left; // left camera focal length
	//float fc_right; // right camera focal length
	//float fc_webcam; // web camera focal length
	//float ccleft_x; // left camera X coord
	//float ccleft_y; // left camera Y coord
	//float baseline; // distance btw camera centres
	//float ccright_x; // right cmra X coord
	//float ccright_y; // right cmra Y coord
	//float cc_disp_offset;
	//float ccweb_x;
	//float ccweb_y;
	//float web2depth_transl_1; //mm
	//float web2depth_transl_2;
	//float web2depth_transl_3;
	//float depth2web_rotat_1;
	//float depth2web_rotat_2;
	//float depth2web_rotat_3;
	int WebCamResolutionHor, WebCamResolutionVer;
	char WhichCamDepth; /** must be 'L' if the depth camera is the left one, and 'R' otherwise **/

};

struct ALG_WEBCAM_INTEGRATION_API sCVE_Config
{
	sCVE_Config() { set_default(); }

	bool is_B0, is_passiveM3;
	bool Master_AEGC_On; // Auto Exposure and Gain Control
	bool headTrackingActive, gazeActive, gazeCalibActive, fingerTrackingActive, depthImprovementActive, faceRecogActive, webcamIntegActive;
	//int  cameraDist;
	//float focal;
	int improvedLevel;
	unsigned int frameWidth, frameHeight;
	unsigned int depthWidth, depthHeight;
	unsigned int WebWidth, WebHeight;

	float fps;
	int stat_width;
	InuCommon::CCalibrationData mOpticalDataDepth;
	std::pair<int, int>  mDepthRightAndLeftSensors;

	InuCommon::CCalibrationData mOpticalDataWebcam;
	int mWebcamSensor;

	float cc_disp_offset; //d_offset = (float)((cc_right_x-cc_left_x));

	//eCVE_StreamName name;
	unsigned int first_frame_index;
	unsigned int num_skip_frames;
	sCVE_WebcamIntegrationConfig WebcamIntegrationConfig;
	// parameters for the Head Tracking algorithm

	//CONFIG INPUT WEBCAM OR RIGHT
	bool buildPLY; //true for build

				   // REGISTER VALUES
	int m_MinDisp;
	int m_NumStatBlocksX;
	int m_NumStatBlocksY;
	int m_YUVstatRanges[18];
	int m_DPEstatRanges[4];
	int m_DPEstatParams[2];

	//Depth Registration close hole window size
	int m_DepthRegistrationCloseHolesWindowSize;

	void set_default(void)
	{
		is_B0 = false;
		is_passiveM3 = false;
		Master_AEGC_On = false;
		headTrackingActive = false;
		gazeActive = false;
		fingerTrackingActive = false;
		depthImprovementActive = false;
		faceRecogActive = false;

		mOpticalDataDepth = InuCommon::CCalibrationData();
		mOpticalDataWebcam = InuCommon::CCalibrationData();

		//cameraDist=0;
		//focal=0;
		//focalCenterX=0;
		//focalCenterY=0;
		improvedLevel = 0;
		frameWidth = 0;
		frameHeight = 0;
		depthWidth = 0;
		depthHeight = 0;
		WebWidth = 0;
		WebHeight = 0;
		stat_width = 20;
		fps = 0;
		buildPLY = false;
		m_DepthRegistrationCloseHolesWindowSize = 3;

		WebcamIntegrationConfig.PointCloudOutputFrameRate = 0;
		WebcamIntegrationConfig.RasterOutputFrameRate = 1;
	}
};

struct ALG_WEBCAM_INTEGRATION_API sCVE_StreamsVec
{
	sCVE_StreamsVec() : timeStamp(0) {
	}

	sCVE_StreamsVec(const sCVE_StreamsVec& other) {
		operator=(other);
	}

	sCVE_StreamsVec& operator=(const sCVE_StreamsVec& other) {

		timeStamp = other.timeStamp;
		webcam = other.webcam;
		leftY = other.leftY;
		rightY = other.rightY;
		rightYUV = other.rightYUV;
		leftYUV = other.leftYUV;
		packedYUV = other.packedYUV;
		leftRGB = other.leftRGB;
		rightRGB = other.rightRGB;
		depth = other.depth;
		disparity = other.disparity;
		for (int i = 0; i < 8; i++) {
			disp_stat[i] = other.disp_stat[i];
		}
		RightSensor_Exp = other.RightSensor_Exp;
		RightSensor_AGain = other.RightSensor_AGain;
		RightSensor_DGain = other.RightSensor_DGain;

		return *this;
	}
	~sCVE_StreamsVec() {
	}

	//int	timeStamp;
	uint64_t timeStamp;
	sCVE_stream<RGB8> webcam;
	sCVE_stream<Y16> leftY;
	sCVE_stream<Y16> rightY;
	sCVE_stream<Y8> leftY8;
	sCVE_stream<Y8> rightY8;
	sCVE_stream<Y16> leftYUV;
	sCVE_stream<Y16> rightYUV;
	sCVE_stream<Y16> packedYUV;
	sCVE_stream<RGB8> leftRGB;
	sCVE_stream<RGB8> rightRGB;
	sCVE_stream<Y16> depth;
	sCVE_stream<Y16> disparity;
	sCVE_stream<Y8> disp_stat[8];
	sCVE_stream<Y8> y_stat;

	//Sensor & HW Config From Frame Headers
	uint32_t RightSensor_Exp;
	uint32_t RightSensor_AGain;
	uint32_t RightSensor_DGain;
	uint32_t DPE_MinDisparity;
	uint32_t DPE_MaxDisparity;
};

class ALG_WEBCAM_INTEGRATION_API CWebcamIntegration
{
public:
    bool Init(sCVE_Config *configData, bool isFisheye, bool undistort);
    bool Process(const sCVE_StreamsVec &inputData, sCVE_WebcamIntegration_Results& results, const sCVE_Config *configData, bool undistort);

	bool NeedAlgorithmInitialization() const { return mNeedAlgorithmInitialization; }

private:

	// True if algorithm need to be initiated
	bool mNeedAlgorithmInitialization = true;

    /******** DEBUG FLAGS ****************************/
    bool rgb_flipped = false;
    bool writePLYwebcam = false;
    bool writeColorDepthimg = false;
    bool writefiles = false;
    bool printout1 = false;
    /******* END OF DEBUG FLAGS **********************/

    cv::Mat colr_onto_depth; /* the result. Global for this .cpp */
    cv::Mat color2dispar; /* the result, alternative algorithm. Global for this .cpp */
    cv::Mat usedcolor;
    cv::Mat useddepth;

    bool  WebcamIntegrationProcessWeb2Depth(const sCVE_StreamsVec &inputData, sCVE_WebcamIntegration_Results& results, const sCVE_Config *configData, bool undistort);
    bool  WebcamIntegrationProcessDepth2Web(const sCVE_StreamsVec &inputData, sCVE_WebcamIntegration_Results& results, const sCVE_Config *configData, bool undistort);
    bool  WebcamIntegrationProcessWebUndistortion(const sCVE_StreamsVec &inputData, sCVE_WebcamIntegration_Results& results, const sCVE_Config *configData);
};

#ifdef _MSC_VER
#pragma warning(default : 4251)
#endif

#endif
