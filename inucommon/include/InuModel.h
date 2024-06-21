#ifndef __INUMODEL__
#define __INUMODEL__

#include "InuSensorDBDefs.h"

#include "TiffFile.h"

#include <vector>
#include <map>
#include <string>

#ifdef _MSC_VER
#pragma warning(disable:4251)
#endif

namespace InuCommon
{
    // must be defined here and not in the class, otherwise the Asserter in InuStreams will fail
    const uint32_t DEFAULT_CHANNEL_ID = std::numeric_limits<uint32_t>::max();

    ////////////////////////////////////////////////////////////////////////
    /// \brief    Represents the data associated with Sensor model
    ///
    /// Role: Each Sensor model has pre-defined parameters that are loaded from internal config file
    ///
    /// Responsibilities:
    ///      1. Knows all parameters that are associated with Sensor model
    ///      2. Knows how to load these parameters from internal config file
    ////////////////////////////////////////////////////////////////////////

    class INUSENSORSDB_API CInuModel
    {
    public:

        struct CDpeParams
        {
            std::string fileName;
            uint32_t frameNum;

            CDpeParams(
                std::string iFileName = "",
                uint32_t iFrameNum = 1)
                : fileName(iFileName), frameNum(iFrameNum)
            {}
        };

        enum EDisparityType
        {
            eDisparity732,
            eDisparity844,
            eDisparity934
        };

    public:
        static const std::string CSV_FILE_NAME;

        static const std::string TITLES_NAME;

        static const std::string INUMODELDB_REVISION;

        /// This model name is used when sensor isn't flashed.
        static const std::string EMPTY_MODEL_NAME;

        static std::string  GetInuModelsDBPath();

        CInuModel();

        virtual ~CInuModel(){};

        /// \brief   Initialize the current model object.
        ///
        /// CInuModel attributes are read from internal configuration file
        /// \param[in]     iSensorID    Sensor name that should be loaded
        /// \param[in]     iHwType      HW Revision
        /// \return    false if the object could not be loaded
        EInuSensorsDBErrors Init(const std::string& iModelName, CTiffFile::EHWType iHwType);

        /// \brief   Initialize the current model object.
        ///
        /// CInuModel attributes are read from internal configuration file
        /// \param[in]     iSensorID    Sensor name that should be loaded
        /// \param[in]     iHwType      HW Revision
        /// \return    false if the object could not be loaded
        EInuSensorsDBErrors Init(const std::string& iModelName, const std::string& iHwType);

        /// \brief   Initialize the current model object.
        ///
        /// CInuModel attributes are read from internal configuration file
        /// \param[in]     iModuleData    Sensor data to parse
        /// \return    false if the object could not be parsed
        EInuSensorsDBErrors ParseData(const std::vector<std::string>& iModuleData);

        // -------------------------
        // Access to private members
        // -------------------------

        const std::string& GetModelName() const;

        const std::string&             GetHwTypeAsString() const { return mHwType; }
        CTiffFile::EHWType             GetHwType() const { return sHwTypeMap[mHwType];};

        CTiffFile::ESensorResolution   GetDefaultResolution(uint32_t iChannelID) const;

        int                            GetDefaultFPS(uint32_t iChannelID) const;

        bool                           GetDefaultAutoControl() const {return mDefaultAutoControl;}

        bool							GetSensorMasterIsRight() const { return mSensorMasterIsRight; }

        bool							GetSensor0IsRight() const { return mSensor0IsRight; }

        unsigned int                   GetAlternateProjectorMode() const { return mAlternateMode; }

        const std::string &           GetConfigIregVersion() const { return mConfigIrgVer; }

        unsigned int                  GetImuType() const { return mImuTypeLocation; }

        const std::pair<int,int>&     GetWebCamFullSize() const { return mWebCamFullSize; }

        const std::pair<int, int>&    GetWebCamVerticalBinningSize() const { return mWebCamVerticalBinningSize; }

        const std::pair<int,int>&     GetWebCamBinningSize() const { return mWebCamBinningSize; }

        float							GetDefaultAccFPS() const { return mDefaultAccFPS; }

        float							GetDefaultAccScale() const { return mDefaultAccScale; }

        float							GetDefaultGyroFPS() const { return mDefaultGyroFPS; }

        float							GetDefaultGyroScale() const { return mDefaultGyroScale; }

        float							GetDefaultMagnoFPS() const { return mDefaultMagnoFPS; }

        float							GetDefaultMagnoScale() const { return mDefaultMagnoScale; }

        float                             GetDefaultImuTimeAlignment() const { return mDefaultImuTimeAlignment; }

        const std::string &				GetImageStreamsChannels() const { return mImageStreamsChannels; }

        const std::map<uint32_t, std::string> & GetChannelToStream() const { return mChannelToStream; }

        const int                       GetFaceRecognitionOutputChannel() const { return mFaceRecognitionOutputChannel;}

        const int		                GetRGBRescaleChannel() const { return mRGBRescaleChannel; }

        const std::string		        GetCnnAppStreamer() const { return mCnnAppStreamer; }

        const std::string		        GetCnnAppFunction() const { return mCnnAppFunction; }

        const std::string               GetCnnAppVisionStreamer() const { return mCnnAppVisionStreamer; }

        const std::string               GetCnnAppVisionFuncion() const { return mCnnAppVisionFunc; }


        const std::map<CTiffFile::ESensorResolution, std::vector<CDpeParams>> &	GetDpeParams() const { return mMapDpeParams;	 }
       
        std::map<std::string, std::string>	GetChannelsInterleaveMode() const { return mChannelsInterleaveMode; }

        std::map<std::string, std::string>	GetComplexGraphConfig() const;

        EDisparityType         GetDisparityType() const;

        std::map<uint32_t, uint32_t>	GetTemperatureSensorsAddresses() const { return mTemperatureSensors; }

        std::string	GetGammaLutTableFile() const;

        uint32_t GetEepromBufferSize() const;

        std::string GetPowerControlMask() const;

        uint32_t GetI2CNumber() const;

        bool GetFWUndistort() const { return mFWUndistort; }

        bool GetAWB() const { return mAWB; }

        uint32_t GetPostProcessingMode() const { return mPostProcessingMode; }

        bool GetHWDepthRegistrationSupport() const { return mHWDepthRegistrationSupport; }

        bool GetEVCompensationSupport() const { return mEVCompensationSupport;  }
private:

        /// \brief      Current object unique name (identifier)
        std::string mModelName;

        /// \brief      HW Revision
        std::string mHwType;

        /// \brief      Default resolution that should be used for Depth sensors of this model
        CTiffFile::ESensorResolution mDefaultDepthResolution;

        /// \brief      Default resolution per channel
        std::map<uint32_t, CTiffFile::ESensorResolution> mDefaultResolution;

        /// \brief      Default FPS per channel
        std::map<uint32_t, int> mDefaultFPS;

         /// \brief      True if automatic Sensor Control should be used.
        bool mDefaultAutoControl;
        
        /// \brief      True if the depth is aligned with right camera
        bool mSensorMasterIsRight;

        /// \brief      True if sensor0 is connected to right camera
        bool mSensor0IsRight;

        /// \brief      AlternateMode None-0 Exposure Based-1 Static Toggling-2
        unsigned int mAlternateMode;

        // For internal use
        static std::map<std::string, CTiffFile::ESensorResolution> sResolutionMap;

        static std::map<std::string, CTiffFile::ETiffFormat> sDefaultVideoFormatMap;

        static std::map<std::string, CTiffFile::EHWType> sHwTypeMap;

        std::map<std::string, CInuModel> mInuModels;

        /// \brief      Default RGB output channel for face recognition
        int mFaceRecognitionOutputChannel;

        /// \brief      Default RGB chan for Cnn App stream
        int mRGBRescaleChannel;

        /// \brief  determines type of IMU: BMX055, BMI160 or others
        unsigned int mImuTypeLocation;

        /// \brief  Config.ireg Version
        std::string mConfigIrgVer;

        std::pair<int,int> mWebCamFullSize;

        std::pair<int, int> mWebCamVerticalBinningSize;

        std::pair<int,int> mWebCamBinningSize;

        /// \brief      Default FPS that should be used for IMU: Accelerometer
        float mDefaultAccFPS;

        /// \brief      Default scale that should be used for IMU: Accelerometer
        float mDefaultAccScale;

        /// \brief      Default FPS that should be used for IMU: Gyro
        float mDefaultGyroFPS;

        /// \brief      Default scale that should be used for IMU: Gyro
        float mDefaultGyroScale;

        /// \brief      Default FPS that should be used for IMU: Magnometer
        float mDefaultMagnoFPS;

        /// \brief      Default scale that should be used for IMU: Magnometer
        float mDefaultMagnoScale;

        /// \brief      Default scale that should be used for IMU: Magnometer
        float mDefaultImuTimeAlignment;

        /// \brief      List of all available Image streams and their names
        std::string		mImageStreamsChannels;

        std::map<uint32_t, std::string> mChannelToStream;

        /// \brief      Default Streamer out name for Cnn App stream
        std::string     mCnnAppStreamer;

        /// \brief      Default function name for Cnn App stream
        std::string     mCnnAppFunction;

        /// \brief      Default stremaer name for Cnn App Vision Proc
        std::string     mCnnAppVisionStreamer;

        /// \brief      Default function name for Cnn App Vision Proc
        std::string     mCnnAppVisionFunc;

        std::map<CTiffFile::ESensorResolution, std::vector<CDpeParams>> mMapDpeParams;

        /// \brief      Default interleave configurations
        std::map<std::string, std::string> 		mChannelsInterleaveMode;

        /// \brief      Default complex graph configurations
        std::map<std::string, std::string> 		mComplexGraphConfig;

        /// \brief      Disparity Type
        EDisparityType     mDisparityType = eDisparity844;

        /// \brief      Temperature sensors addresses
        std::map<uint32_t, uint32_t> 		mTemperatureSensors;

        /// \brief      Gamma Lut Table File
        std::string mGammaLutFile;
        
        uint32_t mEepromBufferSize;

        std::string mPowerControlMask;

        uint32_t mI2CNumber;

        /// \brief      TRUE if RGB Undistort can be done by FW
        bool mFWUndistort;

        /// \brief      Auto White Balance
        bool mAWB;

        /// \brief      Bitwise copy for PP mode
        uint32_t mPostProcessingMode;

        /// \brief      True if model supports registration
        bool mHWDepthRegistrationSupport;

        /// \brief      True if model supports registration
        bool mEVCompensationSupport;

        /// \brief    Load model from one row in file.
        /// \param[in]  iMdoelName    Model name to load.
        /// \param[out] oModuleData   All model data
        /// \return bool    true if success
        EInuSensorsDBErrors  GetModelData(const std::string& iModelName, CTiffFile::EHWType iHwType, std::vector<std::string>& oModuleData);
    };

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////  inline
    inline CInuModel::EDisparityType  CInuModel::GetDisparityType() const
    {
        return mDisparityType;
    }

    inline std::string	CInuModel::GetGammaLutTableFile() const 
    {
        return mGammaLutFile; 
    }

    inline const std::string& CInuModel::GetModelName() const
    { 
        return mModelName; 
    }

    inline std::map<std::string, std::string> CInuModel::GetComplexGraphConfig() const
    {
        return mComplexGraphConfig;
    }

    inline uint32_t CInuModel::GetEepromBufferSize() const
    {
        return mEepromBufferSize;
    }

    inline std::string CInuModel::GetPowerControlMask() const
    {
        return mPowerControlMask;
    }

    inline uint32_t CInuModel::GetI2CNumber() const
    {
        return mI2CNumber;
    }
}
#endif
