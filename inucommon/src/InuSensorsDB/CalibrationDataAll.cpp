#include "CalibrationDataAll.h"
#include "TemperatureDataSet.h"
#include "InuDevice.h"
#include "TemperatureMetaData.h"
#include "MissingCalibrationsAlg.h"

#include "OSUtilities.h"

#include <boost/filesystem/path.hpp>
#include <boost/filesystem.hpp>

#include <fstream>

using namespace InuCommon;
using namespace std;

const string CCalibrationDataAll::REVISION_FOLDER_PREFIX("Rev");


CCalibrationDataAll::CCalibrationDataAll() :
    mTemperatureDataSets(),
    mMissingCalibrationsAlg(nullptr)
{
    for (int i = CTiffFile::eBinning; i < CTiffFile::eNumOfResolutions; i++)
    {
        mMinSizePerResolution[(CTiffFile::ESensorResolution)i] = std::make_pair(numeric_limits<int>::max(), numeric_limits<int>::max());
    }
    // initialize resolution enum to string with releavant values. Must be updated if resolution is changed/added.
    CResolutionDataSet::Resolution2String(CTiffFile::eBinning) = "Binning";
    CResolutionDataSet::Resolution2String(CTiffFile::eFull) = "Full";
    CResolutionDataSet::Resolution2String(CTiffFile::eVerticalBinning) = "VerticalBinning";
}

CCalibrationDataAll::~CCalibrationDataAll()
{
    delete mMissingCalibrationsAlg;
}

EInuSensorsDBErrors CCalibrationDataAll::Load(const boost::filesystem::path& iCalibrationDir, const CInuDevice& iParentSensor, const vector<int>& iRequiredTemperatures, int iSaturationMin, int iSaturationMax, bool iTemperatureCompenstation)
{
    EInuSensorsDBErrors rc = eOK;
    // Get sensor model name
    mSensorName = iParentSensor.GetSensorModelName();

    // Load meta data from file name
    if (!LoadFromFile(iCalibrationDir, iParentSensor.SensorID()))
    {
        rc =  eMetaDataFileNotFound;
    }
    else
    {
        // Read all different calibrations (Per revision or per temperature) and find the match sub-folder
        rc = ReadTemperaturesCalibrations(iCalibrationDir, iParentSensor.HwType());
        if (rc != eOK)
        {
            rc = ReadRevisionCalibration(iCalibrationDir, iParentSensor.HwType());
        }
    }

    if ( (rc == eOK) && iTemperatureCompenstation)
    {
        mMissingCalibrationsAlg = new CMissingCalibrationsAlg(shared_from_this(), iRequiredTemperatures, iSaturationMin, iSaturationMax);
        rc = mMissingCalibrationsAlg->Calculate();
    }

    return rc;
}

bool CCalibrationDataAll::LoadFromFile(const boost::filesystem::path& iCalibrationDir, const std::string& iSensorName)
{
    bool result = true;

    boost::filesystem::path fileName(iCalibrationDir);
    fileName /= iSensorName;

    ifstream  fileObj(COsUtilities::ConvertEncoding(fileName.string()));
    if (fileObj.good())
    {
        // Place holder to read meta data     
    }

    return result;
}

void CCalibrationDataAll::UpdateMinSizes(std::shared_ptr<CTemperatureDataSet> iCalibrationToUpdateWith)
{
    // Iterate through all resolutions and save the minimal videos size for each one. 
    for (int i = CTiffFile::eBinning; i < CTiffFile::eNumOfResolutions; i++)
    {
        CTiffFile::ESensorResolution currentResolution = CTiffFile::ESensorResolution(i);
        // Check if resolution exists. if it doesn't - continue.
        if (iCalibrationToUpdateWith->GetResolution(currentResolution) == nullptr)
        {
            continue;
        }
        // Check for minimal width
        if (iCalibrationToUpdateWith->GetResolution(currentResolution)->GetRegistersSet()->GetVideoSize().first
            < mMinSizePerResolution.find(currentResolution)->second.first)
        {
            // Set minimum size
            mMinSizePerResolution[currentResolution].first =
                iCalibrationToUpdateWith->GetResolution(currentResolution)->GetRegistersSet()->GetVideoSize().first;
        }
        // Check for minimal height
        if (iCalibrationToUpdateWith->GetResolution(currentResolution)->GetRegistersSet()->GetVideoSize().second
            < mMinSizePerResolution.find(currentResolution)->second.second)
        {
            // Set minimum size
            mMinSizePerResolution[currentResolution].second =
                iCalibrationToUpdateWith->GetResolution(currentResolution)->GetRegistersSet()->GetVideoSize().second;
        }
    }
}

EInuSensorsDBErrors CCalibrationDataAll::ReadTemperaturesCalibrations(const boost::filesystem::path& iCalibrationDir, InuCommon::CTiffFile::EHWType iHwType)
{
    EInuSensorsDBErrors rc = eOK;
    // Try to read all temperatures
    std::map<int, CTemperatureMetaData> allTemperaturesFolders;
   if ((CTemperatureMetaData::GetAllMetaFiles(iCalibrationDir.string(), allTemperaturesFolders) != CTemperatureMetaData::eOK) ||
        allTemperaturesFolders.empty())
    {
        rc = eMissingTemperatures;
    }
    else
    {
        // Find the best temperature
        // if no calibration was selected (iTemprature = UNDEFIEND_CALIB_TEMPERATURE), then the Hot calibration will be used
        for (const auto& elem : allTemperaturesFolders)
        {
            int actualTemp = 0;
            elem.second.GetActualTemperature(actualTemp);
            mTemperatureDataSets[elem.first] = make_shared<CTemperatureDataSet>(actualTemp);
            string folderName;
            if (elem.second.GetFolderName(folderName) == CTemperatureMetaData::eOK)
            {
                rc = mTemperatureDataSets[elem.first]->Load(folderName, iHwType, false);
                if (rc != eOK)
                {
                    break;
                }
                else
                {
                    UpdateMinSizes(mTemperatureDataSets[elem.first]);

                }
            }
        }

    }

    return rc;
}

EInuSensorsDBErrors CCalibrationDataAll::ReadRevisionCalibration(const boost::filesystem::path& iCalibrationDir, InuCommon::CTiffFile::EHWType iHwType)
{
    EInuSensorsDBErrors rc = eNoRevisions;

    //All names from here are UTF16 (everything in allRevisions)
    struct sort_functor
    {
        bool operator ()(const boost::filesystem::directory_entry & a, const boost::filesystem::directory_entry & b)
        {
            return a > b;  // Last calibration should be loaded first
        }
    };

    vector<boost::filesystem::directory_entry> subDirectories;
    copy(boost::filesystem::directory_iterator(COsUtilities::ConvertEncoding(iCalibrationDir.string())), boost::filesystem::directory_iterator(), back_inserter(subDirectories));

    sort(subDirectories.begin(), subDirectories.end(), sort_functor());

    //  Find the Revision and Temperatures of all sub folders
    map<unsigned int, boost::filesystem::path>  revisionFoldersMap;
    for(const auto& subdir :  subDirectories)
    {
        boost::system::error_code boostErr;
        if (boost::filesystem::is_directory(subdir, boostErr) && (boost::system::errc::success == boostErr.value()))
        {
            std::string subdirStr(subdir.path().leaf().string());

            // Revision calibration (old)
            if (subdirStr.find_first_of(REVISION_FOLDER_PREFIX) == 0)
            {
                string  tmpStr = subdirStr.substr(REVISION_FOLDER_PREFIX.size());
                if (!tmpStr.empty())
                {
                    mTemperatureDataSets[CInuDevice::UNDEFIEND_CALIB_TEMPERATURE] = make_shared<CTemperatureDataSet>(CInuDevice::UNDEFIEND_CALIB_TEMPERATURE);
                    rc = mTemperatureDataSets[CInuDevice::UNDEFIEND_CALIB_TEMPERATURE]->Load(subdir, iHwType, false);
                    if (rc != eOK)
                    {
                        break;
                    }
                }
            }
        }
    }

    return rc;
}

std::shared_ptr<const CTemperatureDataSet> CCalibrationDataAll::GetCalibrationData(int iTemprature, bool iPreciseTemp) const
{
    if (mTemperatureDataSets.empty())
    {
        return nullptr;
    }

    shared_ptr<const CTemperatureDataSet> calib;

    if (iPreciseTemp)
    {
        for (auto& elem : mTemperatureDataSets)
        {
            if (elem.first == iTemprature)
            {
                calib = elem.second;
            }
        }
    }

    else 
    {
        int minTemperature = mTemperatureDataSets.begin()->first;
        unsigned int minDiff = std::abs(int(iTemprature - minTemperature));
        calib = mTemperatureDataSets.begin()->second;

        for (auto& elem : mTemperatureDataSets)
        {
            if (minDiff > (unsigned int)(std::abs(int(iTemprature - elem.first))))
            {
                minTemperature = elem.first;
                minDiff = (unsigned int)std::abs(int(iTemprature - minTemperature));
                calib = elem.second;
            }
        }
    }

    return calib;
}

bool  CCalibrationDataAll::CalibrationDataExist(int iTemprature) const
{
    return (mTemperatureDataSets.find(iTemprature) != mTemperatureDataSets.end());
}

void CCalibrationDataAll::AddCalibrationData(shared_ptr<CTemperatureDataSet> iCalibrationDataset, bool iOverWrite)
{
    if (!CalibrationDataExist(iCalibrationDataset->GetActualTemperature()) || iOverWrite)
    {
        mTemperatureDataSets[iCalibrationDataset->GetActualTemperature()] = iCalibrationDataset;
    }
}

bool CCalibrationDataAll::TemperatureCalibration() const
{
    return ((mTemperatureDataSets.size() > 1) &&
            (mTemperatureDataSets.begin()->second->GetActualTemperature() != CInuDevice::UNDEFIEND_CALIB_TEMPERATURE));}
