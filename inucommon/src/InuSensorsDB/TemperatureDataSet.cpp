#include "TemperatureDataSet.h"
#include "ResolutionDataSet.h"
#include "InuDevice.h"
#include "TemperatureMetaData.h"

#include "OSUtilities.h"

#include <boost/filesystem.hpp>
#include <boost/filesystem/path.hpp>

#include <fstream>

using namespace InuCommon;
using namespace std;

const std::string CTemperatureDataSet::CALCUATED_FOLDER="calculated.txt";

CTemperatureDataSet::CTemperatureDataSet(const CTemperatureDataSet& iInput) :
    mResolutionsDataSets(),
    mCalibrationDir(iInput.mCalibrationDir),
    mActualTemperature(iInput.mActualTemperature),
    mCalculated(true)
{
    for (const auto& elem : iInput.mResolutionsDataSets)
    {
        shared_ptr<CResolutionDataSet>  resolutionDataset(new CResolutionDataSet(*elem.second));
        mResolutionsDataSets[elem.first] = resolutionDataset;
    }
}

EInuSensorsDBErrors CTemperatureDataSet::Load(const boost::filesystem::path& iCalibrationSetDir, InuCommon::CTiffFile::EHWType iHwType, bool iLoadOdpReg)
{
    EInuSensorsDBErrors rc = eOK;
    mCalibrationDir = iCalibrationSetDir;

    vector<boost::filesystem::directory_entry> subDirectories;
    copy(boost::filesystem::directory_iterator(COsUtilities::ConvertEncoding(mCalibrationDir.string())), boost::filesystem::directory_iterator(), back_inserter(subDirectories));

    //  Find the Revision and Temperatures of all sub folders
    map<unsigned int, boost::filesystem::path>  revisionFoldersMap;
    for(const auto& subdir : subDirectories)
    {
        boost::system::error_code boostErr;
        if ( boost::filesystem::is_directory(subdir / ChipTypeRelatedCalibrarionFolder(iHwType).c_str() , boostErr) &&
             (boost::system::errc::success == boostErr.value()) )
        {
            string resolutionName = subdir.path().leaf().string();
            // Keep original IMU data in newly generated OpticalParams - DSR doesn't copy these values
            if (iLoadOdpReg)
            {
                CCalibrationData originalOptical = *mResolutionsDataSets[resolutionName]->GetCalibrationData();
                mResolutionsDataSets[resolutionName] = make_shared<CResolutionDataSet>(mActualTemperature, resolutionName);
                rc = mResolutionsDataSets[subdir.path().leaf().string()]->Load(subdir / ChipTypeRelatedCalibrarionFolder(iHwType), iHwType, iLoadOdpReg, &originalOptical);
            }
            else 
            {
                mResolutionsDataSets[resolutionName] = make_shared<CResolutionDataSet>(mActualTemperature, resolutionName);
                rc = mResolutionsDataSets[subdir.path().leaf().string()]->Load(subdir / ChipTypeRelatedCalibrarionFolder(iHwType), iHwType, iLoadOdpReg, nullptr);
            }

            if (rc != eOK)
            {
                mResolutionsDataSets.erase(subdir.path().leaf().string());
            }
        }
     }

    boost::system::error_code boostErr;
    boost::filesystem::path tempPath = mCalibrationDir;
    tempPath /= CALCUATED_FOLDER;

    if (!boost::filesystem::exists(tempPath) || (boost::system::errc::success != boostErr))
    {
        mCalculated = false;
    }
    // Save here if you want to make sure copying is done correctly.
    // CTemperatureDataSet::Save();
    return mResolutionsDataSets.empty() ? eNoValidResolution : eOK;
}

EInuSensorsDBErrors CTemperatureDataSet::RemovePreliminaryData()
{
    mCalibrationDir = mCalibrationDir.branch_path() / ("T_" + to_string(mActualTemperature)).c_str();
    boost::system::error_code boostErr;
    boost::filesystem::path tempPath = mCalibrationDir;
    boost::filesystem::remove_all(mCalibrationDir, boostErr);
    if (boostErr.value() != boost::system::errc::success)
    {
        return eFailedToRemoveFolder;
    }

    return eOK;
}


EInuSensorsDBErrors CTemperatureDataSet::SavePreliminaryData()
{
    std::map<std::string, std::shared_ptr<CResolutionDataSet>>::iterator it;;
    mCalibrationDir = mCalibrationDir.branch_path() / ("T_" + to_string(mActualTemperature)).c_str();
    boost::system::error_code boostErr;

    boost::filesystem::path tempPath = mCalibrationDir;
    tempPath /= "temperature_meta_data.ini";
    if (COsUtilities::FileExists(tempPath.string()) != true)
    {
        boost::filesystem::remove_all(mCalibrationDir, boostErr);
        if (boostErr.value() != boost::system::errc::success)
        {
            return eFailedToCreateFolder;
        }
    }

    if (!boost::filesystem::create_directory(mCalibrationDir, boostErr) || (boostErr.value() != boost::system::errc::success))
    {
        return eFailedToCreateFolder;
    }

    EInuSensorsDBErrors ret(eOK);

    for (it = mResolutionsDataSets.begin() ; it != mResolutionsDataSets.end(); it++)
    {
        ret = it->second.get()->Save(mCalibrationDir);
    }
    if (ret != eOK)
    {
        return eOpticalDataFileMissingValue;
    }

    if (IsCalculated())
    {
        boost::filesystem::path tempFolder = mCalibrationDir;
        tempFolder /= CALCUATED_FOLDER;
        std::ofstream outfile(tempFolder.string());
        outfile << "This calibration folder has been Successfully calculated" << std::endl;
        outfile.close();
    }

    return ret;
}

EInuSensorsDBErrors CTemperatureDataSet::SaveDSROutput() const
{
    CTemperatureMetaData metaData(std::to_string(mActualTemperature), mActualTemperature, mActualTemperature);
    return (metaData.Write((mCalibrationDir / "temperature_meta_data.ini").string()) == CTemperatureMetaData::eOK)? eOK : eFailedToCreateMetaData;
}

EInuSensorsDBErrors CTemperatureDataSet::UpdateConfigIregRegisters(const std::string& iModelName, const std::pair<unsigned int, unsigned int> &inewSizes, CTiffFile::ESensorResolution iResolution)
{
    std::shared_ptr<CRegistersSet> registers = std::const_pointer_cast<CRegistersSet>(this->GetResolution(iResolution)->GetRegistersSet());

    if (iModelName == "M3.2TC" || iModelName == "M3.2SPG" || iModelName == "M3.2SPGFL" || iModelName == "M3.2SR" || iModelName == "M3.2SRFL" || iModelName == "M3.2SF")
    {
        for (auto& registerInfo : registers->GetRegistersData())
        {
            if (registerInfo.mAddress == 0x08010004)
            {
                registerInfo.mData = inewSizes.first - 1;
            }
            else if (registerInfo.mAddress == 0x08010044)
            {
                registerInfo.mData = inewSizes.second - 1;
            }
            else if (registerInfo.mAddress == 0x08010008)
            {
                registerInfo.mData = (inewSizes.first) * (inewSizes.second) - 1;
            }
            else if (registerInfo.mAddress == 0x0800150C)
            {
                registerInfo.mData = inewSizes.second - 1;
                registerInfo.mData <<= 12;
                registerInfo.mData |= inewSizes.first - 1;
            }
            else if (registerInfo.mAddress == 0x0800151C)
            {
                registerInfo.mData = inewSizes.second - 1;
                registerInfo.mData <<= 12;
                registerInfo.mData |= inewSizes.first - 1;
            }
            else if (registerInfo.mAddress == 0x08000600)
            {
                registerInfo.mData = inewSizes.first;
                registerInfo.mData <<= 16;
                registerInfo.mData |= inewSizes.first;
            }
        }
        //and set video size
        registers->SetVideoSize(inewSizes);
        registers->SetDepthSize(inewSizes);
    }
    else
    {
        // no register information for this model. If needed - please update code above to support the new model.
        return eNoRegisterDataForReSize;
    }
    return eOK;
}

EInuSensorsDBErrors CTemperatureDataSet::UpdateSize(const std::string& iModelName , 
    const std::map<CTiffFile::ESensorResolution , std::pair<unsigned int, unsigned int>>& inewSizes, CTiffFile::ESensorResolution iResolution, bool iIsOriginalData)
{
    /*
    1. Check size for Hot and Cold calibrations and: save the video pair(and later, for every calibration created)  - occurs in CalibrationData
    2. If equal, continue with producing calibrations 
    3. If one is larger, resize it to size of the smaller calibration(from 1 - as input)
    4. for every calibration generated - if smaller than the two original calibrations - return error. if bigger - resize.
    5. resize: 
        a. Copy DPE and metadata registers(0x08010004, 0x08010044, 0x08010008, 0x0800151C, 0x0800150C) - for M3.2TC modules only! should be per model
        b. calculate Reg  0x08010008 = (0x08010004 + 1) * (0x08010044 + 1) -1
    */

    EInuSensorsDBErrors ret(eOK);

    if (this->GetResolution(iResolution) == nullptr)
    {
        return ret;
    }

    std::shared_ptr<CRegistersSet> registers = std::const_pointer_cast<CRegistersSet>(this->GetResolution(iResolution)->GetRegistersSet());
//        const std::pair<unsigned int, unsigned int> tempSize = registers->GetVideoSize();
    unsigned int width = UINT16_MAX;
    unsigned int height = UINT16_MAX;
    for (auto& regInfo : registers->GetRegistersData())
    {
        // Extract width and height from register info - which might be different than metadata.
        // Assuming 800150c = 800151c
        if (regInfo.mAddress == 0x0800150C)
        {
            width = regInfo.mData;
            width >>= 12;
            height = regInfo.mData;
            height &= 0xfff;
            width++;
            height++;
            break;
        }
    }
        
    // Smaller size - return error if didn't run on origianl data(hot or cold)
    if ((width < inewSizes.at(iResolution).second || height < inewSizes.at(iResolution).first))
    {
        if (iIsOriginalData)
        {
            if (width != registers->GetVideoSize().second || height != registers->GetVideoSize().first)
            {
                // Updating hot or cold calibrations
                ret = UpdateConfigIregRegisters(iModelName, std::make_pair(height, width), iResolution);
                if (ret == eNoRegisterDataForReSize)
                {
                    return ret;
                }
            }
            // Let other calibration know there is a change to calibration
            return eUpdateHotOrCold;
        }
        else
        {
            // interpolated folder is smaller than minimum - ignore this folder
            return eInterpolationSize;
        } 
    }
    // size is bigger - resize.
    else if (width > inewSizes.at(iResolution).second || height > inewSizes.at(iResolution).first)
    {
        ret = UpdateConfigIregRegisters(iModelName, inewSizes.at(iResolution), iResolution);
        if (ret == eNoRegisterDataForReSize)
        {
            return ret;
        }
    }

    // Video size equals - continue.

    return ret;
}
EInuSensorsDBErrors CTemperatureDataSet::UpdateSize(const std::string& iModelName,
    const std::map<CTiffFile::ESensorResolution, std::pair<unsigned int, unsigned int>>& inewSizes, bool iIsOriginalData)
{
    EInuSensorsDBErrors particularResRet(eOK);
    EInuSensorsDBErrors ret(eOK);

    for (int i = CTiffFile::eBinning; i < CTiffFile::eNumOfResolutions; i++)
    {
        CTiffFile::ESensorResolution currentResolution = CTiffFile::ESensorResolution(i);
        particularResRet = UpdateSize(iModelName, inewSizes, currentResolution, iIsOriginalData);
        if (particularResRet != eOK && particularResRet != eUpdateHotOrCold)
        {
            return particularResRet;
        }
        if (particularResRet == eUpdateHotOrCold)
        {
            ret = particularResRet;
        }
    }

    return ret;
}


