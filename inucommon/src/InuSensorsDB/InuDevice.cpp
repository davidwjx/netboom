
#include "OSUtilities.h"
#include "InuDevice.h"

#include "Storage.h"
#include "CalibrationTablesUtilities.h"
#include "TemperatureMetaData.h"
#include "TemperatureDataSet.h"
#include "ResolutionDataSet.h"

#include <boost/filesystem/operations.hpp>
#include <fstream>

#ifdef max
#undef max
#endif

using namespace InuCommon;
using namespace std;

const int CInuDevice::UNDEFIEND_CALIB_TEMPERATURE = std::numeric_limits<int>::max();
const unsigned int CInuDevice::UNDEFIEND_REVISION = std::numeric_limits<unsigned int>::max();

CInuDevice::CInuDevice(const std::string& iSensorID, const std::string& iSensorModel, InuCommon::CTiffFile::EHWType iHwType) :
    mSensorID(iSensorID),
    mSensorModelName(iSensorModel),
    mSensorModel(),
    mPartNumber(),
    mSensorRevision(),
    mBootId(std::numeric_limits<unsigned int>::max()),
    mBootfixTimestamp(std::numeric_limits<uint64_t>::max()),
    mBootfixVersion(),
    mHwType(iHwType),
    mDualSensorsMaster(true),
    mLastBurnTime(std::numeric_limits<uint64_t>::max()),
    mCalibrationVersion(),
    mCalibrationDataAll(nullptr),
    mUSBSpeed(0),
	mAudioFreq(0),
	mCpuFreq(0),
	mDdrFreq(0),
	mDspFreq(0),
	mSysFreq(0),
	mUsb2Freq(0),
	mUsb3Freq(0),
    mRevisionID(0)
{
}

EInuSensorsDBErrors CInuDevice::Init()
{
    return mSensorModel.Init(mSensorModelName, mHwType);
}

EInuSensorsDBErrors CInuDevice::LoadCalibration(const std::vector<int>& iRequiredTemperatures, int iSaturationMin, int iSaturationMax, const string& iDirectoryName, bool iTemperatureCompenstation)
{
    mCalibrationDataAll = shared_ptr<CCalibrationDataAll>(new CCalibrationDataAll());

    // Define the calibration folder
    boost::filesystem::path calibrationDir;
    if (!iDirectoryName.empty())
    {
        calibrationDir = iDirectoryName;
    }
    else
    {
        calibrationDir = LocalPath(mSensorID);
    }

    if (calibrationDir.empty())
    {
        return eEmptyCalibrationDir;
    }

    EInuSensorsDBErrors result = mCalibrationDataAll->Load(calibrationDir, *this, iRequiredTemperatures, iSaturationMin, iSaturationMax, iTemperatureCompenstation);

    // Update the calibration version from any SystemParameters.ini file
    if ( (mCalibrationDataAll != nullptr) &&
         (!mCalibrationDataAll->GetTemperatureDataSets().empty()) &&
         (!mCalibrationDataAll->GetTemperatureDataSets().begin()->second->GetResolutionDataSets().empty()) )
    {
        std::shared_ptr<const CCalibrationData> calibrationData = mCalibrationDataAll->GetTemperatureDataSets().begin()->second->GetResolutionDataSets().begin()->second->GetCalibrationData();
        if (calibrationData != nullptr)
        {
            mCalibrationVersion = calibrationData->Version;
        }
    }

    return result;
}


shared_ptr<const CTemperatureDataSet> CInuDevice::GetTemperatureDataSet(int iTemprature, unsigned int iRevision)
{
    if (mCalibrationDataAll == nullptr)
    {
        return nullptr;
    }
    return mCalibrationDataAll->GetCalibrationData(iTemprature, false);
}

bool CInuDevice::LocalPath() const
{
    string localPath = LocalPath(mSensorID);
    if (localPath.empty())
    {
        return false;
    }

    uint64_t swRevisionLocal(0);

    // Check if it is up to date
    vector<boost::filesystem::directory_entry> subDir;
    copy(boost::filesystem::directory_iterator(COsUtilities::ConvertEncoding(localPath)), boost::filesystem::directory_iterator(), back_inserter(subDir));
    //All names from here are UTF16 (If used from iterators)
    for (const boost::filesystem::directory_entry& iter : subDir)
    {
        if (boost::filesystem::is_regular_file(iter))
        {
            ifstream infoFile(iter.path().string());
            if (infoFile.good())
            {
                string tmp1, tmp2;
                infoFile >> tmp1 >> tmp2 >> swRevisionLocal;
                if (infoFile.bad() || (tmp1 != InuCommon::CCalibrationTablesUtilities::SW_REVISION_STR))
                {
                    swRevisionLocal = 0;
                }
            }
        }
    }

    // if mLastBurnTime == -1 then it means that calibration data is not burned on flash so we take the local data
    return mLastBurnTime == std::numeric_limits<uint64_t>::max() || swRevisionLocal == mLastBurnTime;

}

string CInuDevice::LocalPath(const std::string& iSensorID) 
{
    string result;
    boost::filesystem::path dir(COsUtilities::GetCommonConfigPath());
    dir /= "InuSensors";
    if (COsUtilities::FileExists(dir.string()))
    {
        try
        {
            vector<boost::filesystem::directory_entry> subDir;
            copy(boost::filesystem::directory_iterator(COsUtilities::ConvertEncoding(dir.string())), boost::filesystem::directory_iterator(), back_inserter(subDir));
            //All names from here are UTF16 (If used from iterators)
            for (const boost::filesystem::directory_entry iter : subDir)
            {
                if (boost::filesystem::is_directory(iter))
                {
                    boost::filesystem::path  tmpFileName(iter);
                    string dirName = iter.path().leaf().string();
                    if (iSensorID == dirName.substr(0, iSensorID.size()))
                    {
    #ifdef UNICODE
                        result = COsUtilities::ConvertEncodingBack(tmpFileName.wstring());
    #else
                        result =tmpFileName.string();
    #endif
                        break;
                    }
                    else
                    {
                        tmpFileName /= iSensorID;
                        if (boost::filesystem::exists(tmpFileName))
                        {
                            // File named as Sensor ID exist
                            dir /= dirName;
    #ifdef UNICODE
                            result = COsUtilities::ConvertEncodingBack(dir.wstring());
    #else
                            result = dir.string();
    #endif
                            break;                        
                        }
                    }
                }
            }
        }
        catch(...)
        {
        }
    }
    return result;
}

bool CInuDevice::LutExist(unsigned int iSensorNumber, CTiffFile::ESensorResolution iResolution ) const
{
    if (mCalibrationDataAll == nullptr ||
        mCalibrationDataAll->GetTemperatureDataSets().size() == 0 ||
        mCalibrationDataAll->GetTemperatureDataSets().begin()->second == nullptr)
    {
        return false;
    }

    bool exist = true;
    for (const auto& resolution : mCalibrationDataAll->GetTemperatureDataSets().begin()->second->GetResolutionDataSets())
    {
        if ( (iResolution == CTiffFile::eUnknownResolution) || 
             (resolution.second->GetResolutionName() == CResolutionDataSet::Resolution2String(iResolution)) )
        {
            try
            {
                resolution.second->GetLUT(CLutsDataSet::eDistortionAndRectification, iSensorNumber);
            }
            catch (...) {
                exist = false;
                break;
            }
        }
    }
    if (exist)
    {
        // Check if the current model can exexcute the Undistort
        exist = mSensorModel.GetFWUndistort();
    }
    return exist;
}

bool CInuDevice::DepthRegistrationCalibrationExits(uint32_t isensorId, CTiffFile::ESensorResolution iResolution) const
{
    string SensorPrefix = "sensor_";
    string FWRegFilePrefix = "_InuReg_";
    FWRegFilePrefix = SensorPrefix + std::to_string(isensorId) + FWRegFilePrefix;
    std::map<std::string, std::shared_ptr<CResolutionDataSet>> RSet = mCalibrationDataAll->GetTemperatureDataSets().begin()->second->GetResolutionDataSets();

    std::string resStr = CResolutionDataSet::Resolution2String(iResolution);
    if (RSet.find(resStr) != RSet.end())
    {
        boost::filesystem::path regpath = RSet.find(resStr)->second->GetCalibrationDirectory();
        if (InuCommon::COsUtilities::FileExists(regpath.string()) && boost::filesystem::is_directory(InuCommon::COsUtilities::ConvertEncoding(regpath.string())))
        {
            boost::filesystem::directory_iterator end_iter;
            for (boost::filesystem::directory_iterator dir_itr(regpath.string()); dir_itr != end_iter; ++dir_itr)
            {
                if (boost::filesystem::is_regular_file(dir_itr->status()))
                {
                    std::string fileName(dir_itr->path().filename().string());

                    if (fileName.find(FWRegFilePrefix) != string::npos)
                    {
                        return true;
                    }
                }
            }
        }
    }
    return false;
}

