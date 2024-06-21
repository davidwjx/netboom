
#include "InuModel.h"
#include "Logger.h"
#include "OSUtilities.h"
#include "TiffFile.h"
#include "CsvFile.h"

#include <sstream>
#include <fstream>

#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>

#include <boost/algorithm/string/replace.hpp>
#include <boost/lexical_cast.hpp>

#include <boost/tokenizer.hpp>

using namespace InuCommon;
using namespace std;

const std::string CInuModel::CSV_FILE_NAME="InuModelDB.csv";
const std::string CInuModel::INUMODELDB_REVISION("35");
const std::string CInuModel::TITLES_NAME("Model Name");
const std::string CInuModel::EMPTY_MODEL_NAME("Default");

std::map<std::string, CTiffFile::ESensorResolution> CInuModel::sResolutionMap = {
    {"UnkownResolution",CTiffFile::ESensorResolution::eUnknownResolution },
    {"DefaultResolution",CTiffFile::ESensorResolution::eUnknownResolution },
    {"VGA",CTiffFile::ESensorResolution::eBinning },
    {"Binning",CTiffFile::ESensorResolution::eBinning},
    {"HD",CTiffFile::ESensorResolution::eFull},
    {"Full",CTiffFile::ESensorResolution::eFull},
    {"VerticalBinning",CTiffFile::ESensorResolution::eVerticalBinning},
    {"Alternate",CTiffFile::ESensorResolution::eAlternate} };


std::map<std::string, CTiffFile::ETiffFormat> CInuModel::sDefaultVideoFormatMap = {
    {"YUV_Packed",CTiffFile::ETiffFormat::eYUV},
    {"Y_Only",CTiffFile::ETiffFormat::eY},
    {"Y8",CTiffFile::ETiffFormat::eY8},
    {"RGB565",CTiffFile::ETiffFormat::eRGB565},
    {"YUV422",CTiffFile::ETiffFormat::eYUV422},
    {"Bayer",CTiffFile::ETiffFormat::eBayerGRBG},
    {"NA",CTiffFile::ETiffFormat::eEmpty},
    {"RGB888",CTiffFile::ETiffFormat::eRGB} };

std::map<std::string, CTiffFile::EHWType> CInuModel::sHwTypeMap = {
        { "NU4000C0",CTiffFile::eNU4000C0 },
        { "NU4000C1", CTiffFile::eNU4000C1 }, 
        { "NU4100A0", CTiffFile::eNU4100A0 }
};


enum EModelData
{
    eModelName,
    eHwType,
    eAutoSensorControl,
    ERevision = eAutoSensorControl, // revision is on the second column
    eDefaultResolution,
    eDefaultFPS,
    eMasterSide,
    eSensor0Side,
    eAlternate,
    eImuTypeLocation,
    eConfigIregVersion,
    eAccDefaultFPS,
    eAccDefaultScale,
    eGyroDefaultFPS,
    eGyroDefaultScale,
    eMagnoDefaultFPS,
    eMagnoDefaultScale,
    eDefaultImuTimeAlignment,
    eImageStreamsChannels,
    eFaceRecognitionOutputChannel,
    eRGBRescaleChannel,
    eCnnAppStreamer,
    eCnnAppFunction,
    eCnnAppVisionStreamer,
    eCnnAppVisionFunc,
    eDPE,
    eInterleaveMode,
    eComplexGraphConfig,
    eDisparityType,
    eTemperaureSensors,
    eGammaLutFile,
    eEepromBufferSize,
    ePowerControlMask,
    eI2CNumber,
    eFWUnDistort,
    eAWB,
    ePP,
    eHWDepthRegistrationSupport,
    eEVCompensationSupport,

    ENUMBER_OF_FIELDS, // number of data fields per model
};

std::string  CInuModel::GetInuModelsDBPath()
{
    // Check in config path
    boost::filesystem::path modelsDevPath = boost::filesystem::path(COsUtilities::GetCommonConfigPath());
    modelsDevPath /= CSV_FILE_NAME.c_str();
    if (boost::filesystem::exists(modelsDevPath))
    {
        return modelsDevPath.string();
    }

    return std::string();
}

CInuModel::CInuModel() 
    :   mHwType("NU4000B0")
    ,   mDefaultAutoControl(false)
    ,   mSensorMasterIsRight(true)
    ,   mSensor0IsRight(true)
    ,   mAlternateMode(0)
    ,   mImuTypeLocation(1)
    ,   mFaceRecognitionOutputChannel(4)
    ,   mRGBRescaleChannel(4)
    ,   mCnnAppStreamer("")
    ,   mCnnAppFunction("")
    ,   mCnnAppVisionStreamer("")
    ,   mCnnAppVisionFunc("")
    ,   mEepromBufferSize(0)
    ,   mPowerControlMask("0X0")
    ,   mI2CNumber(0)
    ,   mFWUndistort(false)
    ,   mAWB(false)
    ,   mPostProcessingMode(0)
    ,   mHWDepthRegistrationSupport(false)
    ,   mEVCompensationSupport(false)

{
    mDefaultResolution[DEFAULT_CHANNEL_ID] = CTiffFile::ESensorResolution::eFull;
    mDefaultFPS[DEFAULT_CHANNEL_ID] = 20;
}

EInuSensorsDBErrors CInuModel::Init(const std::string& iModelName, const string& iHwType)
{
    return Init(iModelName, sHwTypeMap[iHwType]);
}

EInuSensorsDBErrors CInuModel::Init(const std::string& iModelName, CTiffFile::EHWType iHwType)
{
    EInuSensorsDBErrors result = eOK;

    //check sensor name is result
    if(iModelName.empty())
    {
        return eModelNameNotFound;
    }

    //checking if sensor is in our current data Base
    std::vector<std::string> moduleData;
    result = GetModelData(iModelName, iHwType, moduleData);
    if (result == eOK)
    {
        result = ParseData(moduleData);
    }

    return result;
}

EInuSensorsDBErrors CInuModel::ParseData(const std::vector<std::string>& iModuleData)
{
    EInuSensorsDBErrors result = eOK;

    // Model was not recognized
    if (iModuleData.empty())
    {
        return eInternalError;
    }

    // InuModelDB.csv is not updated
    if (iModuleData.size() != ENUMBER_OF_FIELDS)
    {
        return eInternalError;
    }


    try
    {
        mModelName = iModuleData[eModelName];
        mHwType = iModuleData[eHwType];
        mDefaultAutoControl = iModuleData[eAutoSensorControl] == "TRUE";

        boost::char_separator<char> sep(";:");

        string defaultResolution = iModuleData[eDefaultResolution];
        boost::tokenizer<boost::char_separator<char>> tokens(defaultResolution,sep);
        auto iter = tokens.begin();
        uint32_t channel = DEFAULT_CHANNEL_ID;
        while (true)
        {
            mDefaultResolution[channel] = sResolutionMap[*iter++];
            if (iter == tokens.end())
            {
                break;
            }
            channel = std::stoul(*iter++);
        }

        string defaultFPS = iModuleData[eDefaultFPS];
        tokens = boost::tokenizer<boost::char_separator<char>>(defaultFPS, sep);
        iter = tokens.begin();
        channel = DEFAULT_CHANNEL_ID;
        while (true)
        {
            mDefaultFPS[channel] = std::stoul(*iter++);
            if (iter == tokens.end())
            {
                break;
            }
            channel = std::stoul(*iter++);
        }

        mSensorMasterIsRight = (iModuleData[eMasterSide] == "Right");

        mSensor0IsRight = (iModuleData[eSensor0Side] == "Right");

        if(!iModuleData[eAlternate].empty())
        {
            mAlternateMode  = std::stoul(iModuleData[eAlternate]);
        }
        else
        {
            mAlternateMode = 0;
        }

        if(!iModuleData[eImuTypeLocation].empty())
        {
            mImuTypeLocation = std::stoul(iModuleData[eImuTypeLocation]);
        }
        else
        {
            mImuTypeLocation = 1;
        }

        if (!iModuleData[eConfigIregVersion].empty())
        {
            mConfigIrgVer = iModuleData[eConfigIregVersion];
        }

        if (!iModuleData[eAccDefaultFPS].empty())
        {
            mDefaultAccFPS = std::stof(iModuleData[eAccDefaultFPS]);
        }

        if (!iModuleData[eAccDefaultScale].empty())
        {
            mDefaultAccScale = std::stof(iModuleData[eAccDefaultScale]);
        }

        if (!iModuleData[eGyroDefaultFPS].empty())
        {
            mDefaultGyroFPS = std::stof(iModuleData[eGyroDefaultFPS]);
        }

        if (!iModuleData[eGyroDefaultScale].empty())
        {
            mDefaultGyroScale = std::stof(iModuleData[eGyroDefaultScale]);
        }

        if (!iModuleData[eMagnoDefaultFPS].empty())
        {
            mDefaultMagnoFPS = std::stof(iModuleData[eMagnoDefaultFPS]);
        }

        if (!iModuleData[eMagnoDefaultScale].empty())
        {
            mDefaultMagnoScale = std::stof(iModuleData[eMagnoDefaultScale]);
        }

        if (!iModuleData[eDefaultImuTimeAlignment].empty())
        {
            mDefaultImuTimeAlignment = std::stof(iModuleData[eDefaultImuTimeAlignment]);
        }

        mImageStreamsChannels = iModuleData[eImageStreamsChannels];
        boost::tokenizer<> tok(mImageStreamsChannels);
        for (boost::tokenizer<>::iterator beg = tok.begin(); beg != tok.end(); ++beg)
        {
            int chan = std::stoul(*beg);
            ++beg;
            string streamName = *beg;
            mChannelToStream.insert(std::make_pair(chan, streamName));
        }

        if (!iModuleData[eFaceRecognitionOutputChannel].empty())
        {
            mFaceRecognitionOutputChannel = std::stoul(iModuleData[eFaceRecognitionOutputChannel]);
        }

        if (!iModuleData[eRGBRescaleChannel].empty())
        {
            mRGBRescaleChannel = std::stoul(iModuleData[eRGBRescaleChannel]);
        }

        if (!iModuleData[eCnnAppStreamer].empty())
        {
            mCnnAppStreamer = iModuleData[eCnnAppStreamer];
        }

        if (!iModuleData[eCnnAppFunction].empty())
        {
            mCnnAppFunction = iModuleData[eCnnAppFunction];
        }

        if (!iModuleData[eCnnAppVisionStreamer].empty())
        {
            mCnnAppVisionStreamer = iModuleData[eCnnAppVisionStreamer];
        }

        if (!iModuleData[eCnnAppVisionFunc].empty())
        {
            mCnnAppVisionFunc = iModuleData[eCnnAppVisionFunc];
        }

        if (!iModuleData[eDPE].empty())
        {
            std::string strFullLine = iModuleData[eDPE];
            boost::char_separator<char> sep(";", "", boost::keep_empty_tokens);
            boost::tokenizer<boost::char_separator<char>> tokensBoost(strFullLine, sep);
            for (boost::tokenizer<boost::char_separator<char>>::iterator Resolutioniter = tokensBoost.begin();
                Resolutioniter != tokensBoost.end();
                ++Resolutioniter)
            {
                std::string ResolutionDPE = *Resolutioniter++;
                std::vector<CDpeParams> VecDpeParams;
                std::string strLine = *Resolutioniter;
                boost::char_separator<char> sep(":", "", boost::keep_empty_tokens);
                boost::tokenizer<boost::char_separator<char>> tokensBoost(strLine, sep);

                for (boost::tokenizer<boost::char_separator<char>>::iterator iter = tokensBoost.begin();
                    iter != tokensBoost.end();
                    ++iter)
                {
                    std::string strDPE = *iter;
                    boost::algorithm::trim(strDPE);// Remove white spaces from end and start of string
                    boost::char_separator<char> sep("#", "", boost::keep_empty_tokens);
                    boost::tokenizer<boost::char_separator<char>> tokensBoost1(strDPE, sep);

                    boost::tokenizer<boost::char_separator<char>>::iterator iter1 = tokensBoost1.begin();

                    CInuModel::CDpeParams params;
                    params.fileName = *iter1++;
                    params.frameNum = std::atoi((*iter1).c_str());
                    VecDpeParams.push_back(params);
                }
                mMapDpeParams.insert(std::make_pair(
                sResolutionMap.find(ResolutionDPE)->second, VecDpeParams));
                VecDpeParams.clear();
            }
        }

        std::string channelsActiveSensors = iModuleData[eInterleaveMode];
        if (!channelsActiveSensors.empty())
        {
            boost::tokenizer<> tokActiveSensors(channelsActiveSensors);
            for (boost::tokenizer<>::iterator beg = tokActiveSensors.begin(); beg != tokActiveSensors.end(); ++beg)
            {
                std::string  chanType = *beg;
                ++beg;
                std::string activeSensors = *beg;
                mChannelsInterleaveMode.insert(std::make_pair(chanType, activeSensors));
            }
        }

        std::string complexGraphConfig = iModuleData[eComplexGraphConfig];
        if (!complexGraphConfig.empty())
        {
            boost::char_separator<char> sepComplexGraph{ "#" };
            boost::tokenizer<boost::char_separator<char>> tokComplexGraph{ complexGraphConfig, sepComplexGraph };
            boost::tokenizer<boost::char_separator<char>>::iterator complexGraphTokIter = tokComplexGraph.begin();

            std::string isComplex;
            std::string complexConfigFile;

            if(complexGraphTokIter != tokComplexGraph.end())
            {
                isComplex = *complexGraphTokIter;
                ++complexGraphTokIter;
                if (complexGraphTokIter != tokComplexGraph.end())
                {
                    complexConfigFile = *complexGraphTokIter;
                }
                mComplexGraphConfig.insert(std::make_pair(isComplex, complexConfigFile));
            }
        }

        if (iModuleData[eDisparityType] == "DISPARITY_732")
        {
            mDisparityType = eDisparity732;
        }
        else if (iModuleData[eDisparityType] == "DISPARITY_934")
        {
            mDisparityType = eDisparity934;
        }
        else//"DISPARITY_844"
        {
            mDisparityType = eDisparity844;
        }

        std::string temperatureSensors = iModuleData[eTemperaureSensors];
        if (!temperatureSensors.empty())
        {
            boost::tokenizer<> tokActiveSensors(temperatureSensors);
            for (boost::tokenizer<>::iterator beg = tokActiveSensors.begin(); beg != tokActiveSensors.end(); ++beg)
            {
                std::string sensorId = *beg;
                ++beg;
                std::string  sensorAddress = *beg;
                mTemperatureSensors.insert(std::make_pair(atoi(sensorId.c_str()), atoi(sensorAddress.c_str())));
            }
        }

        mGammaLutFile = iModuleData[eGammaLutFile];

        mEepromBufferSize = std::stoul(iModuleData[eEepromBufferSize]);

        mPowerControlMask = iModuleData[ePowerControlMask];

        mI2CNumber = std::stoul(iModuleData[eI2CNumber]);

        mFWUndistort = iModuleData[eFWUnDistort] == "TRUE";
        
        mAWB = iModuleData[eAWB] == "TRUE";
       
        mPostProcessingMode = std::stoul(iModuleData[ePP]);

        mHWDepthRegistrationSupport = iModuleData[eHWDepthRegistrationSupport] == "TRUE";

        mEVCompensationSupport = iModuleData[eEVCompensationSupport] == "TRUE";

        result = eOK;
    }

    catch(...)
    {
        return eInternalError;
    }

    return result;
}

EInuSensorsDBErrors  CInuModel::GetModelData(const std::string& iModelName, CTiffFile::EHWType iHwType, std::vector<std::string>& oModuleData)
{
    vector<vector<string>> buffer;

    std::string fileName(GetInuModelsDBPath());

    std::string revision;
    if (CCsvFile::Read(fileName, buffer, revision) != InuCommon::CCsvFile::eOK)
    {
        return eFileOpen;
    }
    else if (revision != INUMODELDB_REVISION)
    {
        InuCommon::CLogger::Write("InuModelDB.csv revision is different from expected revision: " + INUMODELDB_REVISION +"!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!", InuCommon::CLogger::eError, "InuDev-Service"/*INUSERVICE_LOG_PREFIX*/);
        return eBadFileRevision;
    }

    for (auto model : buffer)
    {
        if (boost::iequals(iModelName, model[eModelName]) && (iHwType == sHwTypeMap[model[eHwType]]) )
        {
            oModuleData = model;
            return eOK;
        }
    }

    return eModelNameNotFound;
}

CTiffFile::ESensorResolution InuCommon::CInuModel::GetDefaultResolution(uint32_t iChannelID) const
{

    if (mDefaultResolution.find(iChannelID) != mDefaultResolution.end())
    {
        return mDefaultResolution.at(iChannelID);
    }
    return mDefaultResolution.at(DEFAULT_CHANNEL_ID);
}

int InuCommon::CInuModel::GetDefaultFPS(uint32_t iChannelID) const
{

    if (mDefaultFPS.find(iChannelID) != mDefaultFPS.end())
    {
        return mDefaultFPS.at(iChannelID);
    }
    return mDefaultFPS.at(DEFAULT_CHANNEL_ID);
}