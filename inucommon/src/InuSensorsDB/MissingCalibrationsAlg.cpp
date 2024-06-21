#include "MissingCalibrationsAlg.h"
#include "TemperatureDataSet.h"
#include "InuDevice.h"
//#include "RunDSR.h"
#include "OSUtilities.h"
#include "Logger.h"
#include "CalibrationDataImp.h"
#include "CalibrationTablesUtilities.h"

#include <boost/filesystem.hpp>
#include <boost/filesystem/path.hpp>
#include <fstream>
#include <sstream>



using namespace InuCommon;
using namespace std;
//using namespace Algo;

const std::string CMissingCalibrationsAlg::DSR_FOLDER_NAME = "DSR";
const std::string CMissingCalibrationsAlg::DSR_FILE_NAME = "dsr.yml";
const std::string CMissingCalibrationsAlg::FOLDER_INITIAL = "T_";
const std::string CMissingCalibrationsAlg::TEMPORARY_CALCULATED_FOLDER = "tmp";
const std::string CMissingCalibrationsAlg::DSR_MATLAB_VERSION = "4.03.03.008";
const std::string CMissingCalibrationsAlg::OVERWRITTEN_FOLDER = "overwritten.txt";

CMissingCalibrationsAlg::CMissingCalibrationsAlg(std::shared_ptr<CCalibrationDataAll> iCalibrationData, const std::vector<int>& iRequiredTemperatures, int iSaturationMin, int iSaturationMax) :
    mCalibationData(iCalibrationData),
    mRequiredTemperatures(iRequiredTemperatures),
    mColdCalibration(CInuDevice::UNDEFIEND_CALIB_TEMPERATURE),
    mWarmCalibration(CInuDevice::UNDEFIEND_CALIB_TEMPERATURE),
    mMinSaturation(iSaturationMin),
    mMaxSaturation(iSaturationMax),
    mCalculationThread(),
    mStopCalcuationThread(false)
{
}

CMissingCalibrationsAlg::~CMissingCalibrationsAlg()
{
    mStopCalcuationThread = true;
    COsUtilities::WaitForThreadTermination(mCalculationThread);
}

EInuSensorsDBErrors CMissingCalibrationsAlg::SaveToDiskAndAddCalibration(const std::shared_ptr<CTemperatureDataSet>& iCalib) const
{
    EInuSensorsDBErrors ret = eOK;
    // Save it to disk After odp reg changes 
    SaveConfigIregAndOpticalData(iCalib);
    if (ret != eOK)
    {
    CLogger::Write(std::string("Failed to save config ireg and/or optical data"), CLogger::eError, "Calculation Thread");
    return ret;
    }

    iCalib->SaveDSROutput();
    if (ret != eOK)
    {
        CLogger::Write(std::string("Failed to save DSR output metadata"), CLogger::eError, "Calculation Thread");
        return ret;
    }

    {
        shared_ptr<CCalibrationDataAll> calibationDataShared = mCalibationData.lock();
        if (calibationDataShared == nullptr)
        {
            return ret;
        }
        calibationDataShared->AddCalibrationData(iCalib);
    }
    return ret;
}

EInuSensorsDBErrors CMissingCalibrationsAlg::Calculate()
{
    boost::filesystem::path yamlFile(COsUtilities::GetCommonConfigPath());
    yamlFile /= DSR_FOLDER_NAME;
    shared_ptr<CCalibrationDataAll> calibationDataShared = mCalibationData.lock();

    if (calibationDataShared == nullptr)
    {
        return eInternalError;
    }

    yamlFile /= calibationDataShared->GetSensorName().c_str();
    yamlFile /= DSR_FILE_NAME;

    boost::system::error_code boostErr;
    if (!boost::filesystem::exists(yamlFile, boostErr) || (boostErr.value() != boost::system::errc::success))
    {
        // Yml file for this module doesn't exist. return eOK but notify user.
        CLogger::Write(std::string("YML file missing for this model."), CLogger::eError, "Calculation thread");
        return eOK;
    }

    const std::map<int, std::shared_ptr<CTemperatureDataSet>>& calibrationDataSets = calibationDataShared->GetTemperatureDataSets();
    EInuSensorsDBErrors retCode;

    if (calibrationDataSets.size() == 2 || calibrationDataSets.size() == 3)
    {
        // Recalculate the warm and cold calibration, there might be 3 folders if InuService exited during the previous execution of this code

        // Since the calibrationDataSets is sorted by temperature then we can assume that cold comes first
        if (mColdCalibration == CInuDevice::UNDEFIEND_CALIB_TEMPERATURE)
        {
            auto iter = calibrationDataSets.begin();
            while ((iter != calibrationDataSets.end()) && (mColdCalibration == CInuDevice::UNDEFIEND_CALIB_TEMPERATURE))
            {
                retCode = RunDSROverOriginalData(iter->second, calibationDataShared, calibrationDataSets, true);
                if (retCode != eOK)
                {
                    return retCode;
                }
                iter++;
            }
        }

        if (mWarmCalibration == CInuDevice::UNDEFIEND_CALIB_TEMPERATURE)
        {
            auto iter = calibrationDataSets.rbegin();
            while ((iter != calibrationDataSets.rend()) && (mWarmCalibration == CInuDevice::UNDEFIEND_CALIB_TEMPERATURE))
            {
                retCode = RunDSROverOriginalData(iter->second, calibationDataShared, calibrationDataSets, false);
                if (retCode != eOK)
                {
                    return retCode;
                }
                iter++;
            }
        }
    }

    if (calibrationDataSets.size() > 1)
    {
        // Check if there is missing calibrations that should be calculated
        bool missingCalibrations = false;
        for (auto elem : mRequiredTemperatures)
        {
            if (!calibationDataShared->CalibrationDataExist(elem))
            {
                missingCalibrations = true;
                break;
            }
        }

        if (missingCalibrations)
        {
            if ((mColdCalibration == CInuDevice::UNDEFIEND_CALIB_TEMPERATURE) ||
                (mWarmCalibration == CInuDevice::UNDEFIEND_CALIB_TEMPERATURE) ||
                (mWarmCalibration == mColdCalibration))
            {
                mColdCalibration = mWarmCalibration = CInuDevice::UNDEFIEND_CALIB_TEMPERATURE;
                // Find the cold and warm, based on the sorted by temperature map
                for (const auto& elem : calibrationDataSets)
                {
                    if (mColdCalibration == CInuDevice::UNDEFIEND_CALIB_TEMPERATURE)
                    {
                        if (!elem.second->IsCalculated())
                        {
                            mColdCalibration = elem.first;
                        }
                    }
                    else if (mWarmCalibration == CInuDevice::UNDEFIEND_CALIB_TEMPERATURE)
                    {
                        if (!elem.second->IsCalculated())
                        {
                            mWarmCalibration = elem.first;
                            break;
                        }
                    }
                }
                if ((mColdCalibration == CInuDevice::UNDEFIEND_CALIB_TEMPERATURE) ||
                    (mWarmCalibration == CInuDevice::UNDEFIEND_CALIB_TEMPERATURE) ||
                    (mWarmCalibration == mColdCalibration))
                {
                    return eCantGetWarmOrCOld;
                }
            }

            mCalculationThread = std::thread(&CMissingCalibrationsAlg::CalculationThreadFunc, this);
        }
    }

    return eOK;
}

string CMissingCalibrationsAlg::IntVersionToString(uint32_t iVersion) const
{
    string ret;
    int tempVersion = iVersion;
    int token0 = tempVersion / 10000000;
    tempVersion -= (token0 * 10000000);
    int token1 = tempVersion / 100000;
    tempVersion -= (token1 * 100000);
    int token2 = tempVersion / 1000;
    tempVersion -= (token2 * 1000);
    int token3 = tempVersion;
    ret = to_string(token0) + "." + to_string(token1) + "." + to_string(token2) + "." + to_string(token3);

    return ret;
}

EInuSensorsDBErrors CMissingCalibrationsAlg::RunDSROverOriginalData(std::shared_ptr<CTemperatureDataSet>& ioCalibration) const
{
    EInuSensorsDBErrors ret = eOK;
    // Check if Calibration version runs DSR over Matlab or Python
    // If Matlab - rerun DSR.
    // Assuming "Full" version is identical for all resolutions

    // Run DSR
    boost::filesystem::path yamlFile(COsUtilities::GetCommonConfigPath());
    yamlFile /= DSR_FOLDER_NAME;
    shared_ptr<CCalibrationDataAll> calibationDataShared = mCalibationData.lock();
    if (calibationDataShared == nullptr)
    {
        return eInternalError;
    }

    yamlFile /= calibationDataShared->GetSensorName().c_str();
    yamlFile /= DSR_FILE_NAME;

    // Create new data set copied from given calibration
    shared_ptr<CTemperatureDataSet> newCalibration = nullptr;

    newCalibration = shared_ptr<CTemperatureDataSet>(new CTemperatureDataSet(*calibationDataShared->GetCalibrationData(ioCalibration->GetActualTemperature(), true)));
    ret = CalculateDataset(newCalibration, yamlFile);
    if (ret != eOK)
    {
        CLogger::Write(std::string("Failed to run DSR over original data"), CLogger::eError, "Recalculate original data");
        return eDSRFailed;
    }

    // Convert InuCalibration number from Int to string
    //string InuCalibVersionStr = IntVersionToString(InuCalibrationVersion);

    // Reload calibration data after DSR
    ret = newCalibration->Load(newCalibration->GetCalibrationDir(), InuCommon::CTiffFile::EHWType::eB0, true);
    if (ret != eOK)
    {
        CLogger::Write(std::string("could not read model params under YML file: "), CLogger::eError, "Recalculate original data");
        return ret;
    }
    // Update InuCalibration Version

    // Add it to calibration data
    calibationDataShared->AddCalibrationData(newCalibration, true);

    ret = newCalibration->UpdateSize(calibationDataShared->GetSensorName(), calibationDataShared->GetMinSizeForAllResolutions(), true);
    if (ret != eOK && ret != eUpdateHotOrCold)
    {
        CLogger::Write(std::string("Failed to update size after DSR"), CLogger::eError, "Calculation Thread");
        return ret;
    }

    ioCalibration.swap(newCalibration);

    return ret;
}

EInuSensorsDBErrors CMissingCalibrationsAlg::RunDSROverOriginalData(std::shared_ptr<CTemperatureDataSet> ioOriginalData, 
    shared_ptr<CCalibrationDataAll> iCalibationDataAll,
    const std::map<int, std::shared_ptr<CTemperatureDataSet>>& iCalibrationDataSets,
    bool iColdTemperature)
{
    boost::system::error_code boostErr;
    if (ioOriginalData->IsCalculated() || (ioOriginalData->GetCalibrationDir().leaf() == TEMPORARY_CALCULATED_FOLDER) )
    {
        // Need to execute the algorithm on the original data only if it has not been calculated yet.
        return eOK;
    }

    if (iColdTemperature)
    {
        mColdCalibration = ioOriginalData->GetActualTemperature();
    }
    else
    {
        mWarmCalibration = ioOriginalData->GetActualTemperature();
    }

    // Original folder name (should be deleted at then end of the process)
    boost::filesystem::path originalFolder = ioOriginalData->GetCalibrationDir();

    if (boost::filesystem::exists(originalFolder / OVERWRITTEN_FOLDER, boostErr) && (boostErr == boost::system::errc::success))
    {
        // New format, after InuSW_4.06.0012
        return eOK;
    }

    // Need to recalculate (run DSR)

    // Create temporary folder to generate the interpolation 
    EInuSensorsDBErrors ret = CreateTemporaryFolderForDSR(ioOriginalData);
    if  ( (ret != eOK) && (ret != eFolderAlreadyExists) )
    {
        CLogger::Write(std::string("Failed to rename folder with the correct temperature"), CLogger::eError, "Calculation thread");
        return ret;
    }

    // Even if the folder exists there is a need to confirm that the size is fine

    ret = RunDSROverOriginalData(ioOriginalData);
    if (ret != eOK && ret != eUpdateHotOrCold)
    {
        CLogger::Write(std::string("Failed to run DSR over original warm calibration data"), CLogger::eError, "Calculation thread");
        return ret;
    }

    EInuSensorsDBErrors tmpRet = SaveToDiskAndAddCalibration(ioOriginalData);
	if (tmpRet != eOK)
	{
	    CLogger::Write(std::string("Failed to save data after DSR on Cold calibration"), CLogger::eError, "Calculation thread");
	    return tmpRet;
	}
	
	if (ret == eUpdateHotOrCold)
	{
        iCalibationDataAll->UpdateMinSizes(ioOriginalData);
        const std::map<int, std::shared_ptr<CTemperatureDataSet>>& allCalibrations = iCalibationDataAll->GetTemperatureDataSets();
	    shared_ptr<CTemperatureDataSet> otherCalibration = iColdTemperature ? allCalibrations.rbegin()->second : allCalibrations.begin()->second;

	    ret = otherCalibration->UpdateSize(iCalibationDataAll->GetSensorName(), iCalibationDataAll->GetMinSizeForAllResolutions(),true);
	
        ret = SaveToDiskAndAddCalibration(otherCalibration);
        if (ret != eOK)
        {
            CLogger::Write(std::string("Failed to save data after DSR on Cold calibration"), CLogger::eError, "Calculation thread");
            return ret;
        }
	}
    
    return RenameTempFolderByTemperature(ioOriginalData, originalFolder);
}

EInuSensorsDBErrors CMissingCalibrationsAlg::CreateTemporaryFolderForDSR(std::shared_ptr<CTemperatureDataSet> iCalibration) const
{
    boost::filesystem::path newPath = iCalibration->GetCalibrationDir();
    newPath.remove_leaf();
    newPath /= TEMPORARY_CALCULATED_FOLDER;

    boost::system::error_code boostErr;
    if (boost::filesystem::exists(newPath, boostErr) || (boostErr == boost::system::errc::success))
    {
        // Remove previous uncompleted calculation
        boost::filesystem::remove_all(newPath, boostErr);
        if (boostErr.value() != boost::system::errc::success)
        {
            return eFailedToRemoveFolder;
        }
    }

    // copy original folder to the new path
    if (!CCalibrationTablesUtilities::CopyCalibrationDir(iCalibration->GetCalibrationDir(), newPath))
    {
        return eFailedToRemoveFolder;
    }

    iCalibration->SetCalibrationDir(newPath);
    
    return eOK;
}

EInuSensorsDBErrors CMissingCalibrationsAlg::RenameTempFolderByTemperature(std::shared_ptr<CTemperatureDataSet> iCalibration, const boost::filesystem::path& iOriginalPath) const
{
    boost::filesystem::path newPath = iCalibration->GetCalibrationDir();
    newPath.remove_leaf();
    newPath /= FOLDER_INITIAL + std::to_string(iCalibration->GetActualTemperature());

    boost::system::error_code boostErr;
    if (boost::filesystem::exists(newPath, boostErr) && (boostErr == boost::system::errc::success))
    {
        // Remove previous uncompleted calculation
        boost::filesystem::remove_all(newPath, boostErr);
        if (boostErr.value() != boost::system::errc::success)
        {
            return eFailedToRemoveFolder;
        }
    }

    // Move created new folder to the temperature folder
    boost::filesystem::rename(iCalibration->GetCalibrationDir(), newPath, boostErr);
    if (boostErr.value() != boost::system::errc::success)
    {
        return eCantGetWarmOrCOld;
    }

    // This section is a critical code, if InuService exits before this function is ended then it might be in inconsistent state

    // Add the 
    std::ofstream outfile((newPath / OVERWRITTEN_FOLDER).string());
    outfile << "Recalculation of original folder" << std::endl;
    outfile.close();

    if (iOriginalPath != newPath)
    {
	    // Remove the original folder that was read from flash
	    boost::filesystem::remove_all(iOriginalPath, boostErr);
	    if (boostErr.value() != boost::system::errc::success)
	    {
	        return eFailedToRemoveFolder;
	    }
    }

    iCalibration->SetCalibrationDir(newPath);

    return eOK;
}

void CMissingCalibrationsAlg::CalculationThreadFunc()
{
    CLogger::SetSeverity("Calculation Thread", CLogger::eInfo);
    EInuSensorsDBErrors ret = eOK;

    for (auto elem : mRequiredTemperatures)
    {
        if (mStopCalcuationThread)
        {
            break;
        }

        boost::filesystem::path yamlFile(COsUtilities::GetCommonConfigPath());
        yamlFile /= DSR_FOLDER_NAME;
        {
            shared_ptr<CCalibrationDataAll> calibationDataShared = mCalibationData.lock();
            if (calibationDataShared == nullptr)
            {
                break;
            }

            if (calibationDataShared->GetCalibrationData(elem, true) != nullptr)
            {
                continue;
            }

            yamlFile /= calibationDataShared->GetSensorName().c_str();
            yamlFile /= DSR_FILE_NAME;
        }

        CLogger::Write("" + to_string(eOK), CLogger::eWarning, "CalculationThreadFunc");

        // Create new data set copied from Cold calibration
        shared_ptr<CTemperatureDataSet> newCalibration = nullptr;

        {
            shared_ptr<CCalibrationDataAll> calibationDataShared = mCalibationData.lock();
            if (calibationDataShared == nullptr)
            {
                break;
            }
            newCalibration = shared_ptr<CTemperatureDataSet>(new CTemperatureDataSet(*calibationDataShared->GetCalibrationData(mColdCalibration, true)));

            newCalibration->SetActualTemperature(elem);

            // Estimate the calibration input parameters. make sure it doesn't exceed saturation values.
            // If exceeds, calculate according to warm or cold.
            if (elem < mMinSaturation)
            {
                if (calibationDataShared->GetCalibrationData(mMinSaturation, true) != nullptr)
                {
                    // Min temperature calibration exists, no need to create it again
                    continue;
                }
                newCalibration->SetActualTemperature(mMinSaturation);
            }
            else if (elem > mMaxSaturation)
            {
                if (calibationDataShared->GetCalibrationData(mMaxSaturation, true) != nullptr)
                {
                    // Min temperature calibration exists, no need to create it again
                    continue;
                }
                newCalibration->SetActualTemperature(mMaxSaturation);
            }
        }

        //set new calibration dir
        boost::filesystem::path newPath = newCalibration->GetCalibrationDir();
        newPath.remove_leaf();
        newPath /= FOLDER_INITIAL + std::to_string(newCalibration->GetActualTemperature());
        newCalibration->SetCalibrationDir(newPath);

        if (mStopCalcuationThread)
        {
            break;
        }
        
        ret = Interpulation(newCalibration);
        if (ret != eOK)
        {
            CLogger::Write(std::string("could not finish interpolation"), CLogger::eError, "Calculation Thread");
            continue;
        }

        // Save it to disk for next execution
        ret = newCalibration->SavePreliminaryData();
        if (ret != eOK)
        {
            CLogger::Write(std::string("Could not save preliminary calibration data"), CLogger::eError, "Calculation Thread");
            newCalibration->RemovePreliminaryData();
            continue;
        }
        // Calculate calibration data set
        ret = CalculateDataset(newCalibration, yamlFile);
        if (ret != eOK)
        {
            CLogger::Write(std::string("Failed to run DSR over interpolated data"), CLogger::eError, "Calculation Thread");
            newCalibration->RemovePreliminaryData();
            continue;
        }
        if (mStopCalcuationThread)
        {
            break;
        }

        // Reload calibration data after DSR
        if (newCalibration->Load(newCalibration->GetCalibrationDir(), InuCommon::CTiffFile::EHWType::eB0, true) != eOK)
        {
            CLogger::Write(std::string("could not read model params under YML file: "), CLogger::eError, "Calculation Thread");
        }

        // Add it to calibration data
        {
            shared_ptr<CCalibrationDataAll> calibationDataShared = mCalibationData.lock();
            if (calibationDataShared == nullptr)
            {
                break;
            }

            ret = newCalibration->UpdateSize(calibationDataShared->GetSensorName(), calibationDataShared->GetMinSizeForAllResolutions(), false);
            if (ret != eOK)
            {
                newCalibration->RemovePreliminaryData();
                CLogger::Write(std::string("Failed to update size after DSR"), CLogger::eError, "Calculation Thread");
                continue;
            }
        } 

        ret = SaveToDiskAndAddCalibration(newCalibration);
        if (ret != eOK)
        {
            CLogger::Write(std::string("Failed to save data after DSR on Cold calibration"), CLogger::eError, "Calculation thread");
            continue;
        }
    }
}

EInuSensorsDBErrors CMissingCalibrationsAlg::SaveConfigIregAndOpticalData(shared_ptr<CTemperatureDataSet> iNewCalibration) const
{
    EInuSensorsDBErrors ret = eOK;
    boost::filesystem::path tempPath = iNewCalibration->GetCalibrationDir();
    tempPath = tempPath.branch_path() / (FOLDER_INITIAL + to_string(iNewCalibration->GetActualTemperature())).c_str();
    for (auto elem : iNewCalibration->GetResolutionDataSets())
    {
        ret = elem.second->SaveConfigIregAndOpticalData(iNewCalibration->GetCalibrationDir());
    }
    return ret;
}

void CMissingCalibrationsAlg::InterpulationCalculation(int32_t iCalibCold, int32_t iCalibWarm,
    float iParamCalibCold, float iParamCalibWarm, float& ioParamsCalibNew, int32_t ioNewTemperature) const
{
    //calc slope
    float A = (iParamCalibWarm - iParamCalibCold) / (iCalibWarm - iCalibCold);
    float B = iParamCalibWarm - (A * iCalibWarm);
    //calc new param
    ioParamsCalibNew = (A * ioNewTemperature) + B;

}


EInuSensorsDBErrors CMissingCalibrationsAlg::Interpulation(std::shared_ptr<CTemperatureDataSet> ioTemperatureDataSet) const
{
    shared_ptr<CCalibrationDataAll> calibationDataShared = mCalibationData.lock();    
    if (calibationDataShared == nullptr)
    {
        return eInternalError;
    }

    std::shared_ptr<const CCalibrationData> coldDeviceData = calibationDataShared->GetCalibrationData(mColdCalibration, true)->GetResolutionDataSets().find("Full")->second->GetCalibrationData();
    //COpticalDataImp coldOpticalData(*dynamic_pointer_cast<const CCalibrationDataImp>(coldDeviceData));
    std::shared_ptr<const CCalibrationData> warmDeviceData = calibationDataShared->GetCalibrationData(mWarmCalibration, true)->GetResolutionDataSets().find("Full")->second->GetCalibrationData();
    //COpticalDataImp warmOpticalData(*dynamic_pointer_cast<const CCalibrationDataImp>(warmDeviceData));
       
    std::shared_ptr<const CCalibrationData> tempDeviceData = ioTemperatureDataSet->GetResolutionDataSets().find("Full")->second->GetCalibrationData();
    //COpticalDataImp  temp(*dynamic_pointer_cast<const CCalibrationDataImp>(tempDeviceData));

    /*
    InterpulationCalculation(mColdCalibration, mWarmCalibration, coldOpticalData.FocalLengthRealRight.X(),
        warmOpticalData.FocalLengthRealRight.X(), temp.FocalLengthRealRight.X(), ioTemperatureDataSet->GetActualTemperature());
    InterpulationCalculation(mColdCalibration, mWarmCalibration, coldOpticalData.FocalLengthRealRight.Y(),
        warmOpticalData.FocalLengthRealRight.Y(), temp.FocalLengthRealRight.Y(), ioTemperatureDataSet->GetActualTemperature());
    InterpulationCalculation(mColdCalibration, mWarmCalibration, coldOpticalData.OpticalCenterRealRight.X(),
        warmOpticalData.OpticalCenterRealRight.X(), temp.OpticalCenterRealRight.X(), ioTemperatureDataSet->GetActualTemperature());
    InterpulationCalculation(mColdCalibration, mWarmCalibration, coldOpticalData.OpticalCenterRealRight.Y(),
        warmOpticalData.OpticalCenterRealRight.Y(), temp.OpticalCenterRealRight.Y(), ioTemperatureDataSet->GetActualTemperature());
    InterpulationCalculation(mColdCalibration, mWarmCalibration, coldOpticalData.LensDistortionsRealRight[0],
        warmOpticalData.LensDistortionsRealRight[0], temp.LensDistortionsRealRight[0], ioTemperatureDataSet->GetActualTemperature());
    InterpulationCalculation(mColdCalibration, mWarmCalibration, coldOpticalData.LensDistortionsRealRight[1],
        warmOpticalData.LensDistortionsRealRight[1], temp.LensDistortionsRealRight[1], ioTemperatureDataSet->GetActualTemperature());
    InterpulationCalculation(mColdCalibration, mWarmCalibration, coldOpticalData.LensDistortionsRealRight[2],
        warmOpticalData.LensDistortionsRealRight[2], temp.LensDistortionsRealRight[2], ioTemperatureDataSet->GetActualTemperature());
    InterpulationCalculation(mColdCalibration, mWarmCalibration, coldOpticalData.LensDistortionsRealRight[3],
        warmOpticalData.LensDistortionsRealRight[3], temp.LensDistortionsRealRight[3], ioTemperatureDataSet->GetActualTemperature());
    InterpulationCalculation(mColdCalibration, mWarmCalibration, coldOpticalData.LensDistortionsRealRight[4],
        warmOpticalData.LensDistortionsRealRight[4], temp.LensDistortionsRealRight[4], ioTemperatureDataSet->GetActualTemperature());
    InterpulationCalculation(mColdCalibration, mWarmCalibration, coldOpticalData.FocalLengthRealLeft.X(),
        warmOpticalData.FocalLengthRealLeft.X(), temp.FocalLengthRealLeft.X(), ioTemperatureDataSet->GetActualTemperature());
    InterpulationCalculation(mColdCalibration, mWarmCalibration, coldOpticalData.FocalLengthRealLeft.Y(),
        warmOpticalData.FocalLengthRealLeft.Y(), temp.FocalLengthRealLeft.Y(), ioTemperatureDataSet->GetActualTemperature());
    InterpulationCalculation(mColdCalibration, mWarmCalibration, coldOpticalData.OpticalCenterRealLeft.X(),
        warmOpticalData.OpticalCenterRealLeft.X(), temp.OpticalCenterRealLeft.X(), ioTemperatureDataSet->GetActualTemperature());
    InterpulationCalculation(mColdCalibration, mWarmCalibration, coldOpticalData.OpticalCenterRealLeft.Y(),
        warmOpticalData.OpticalCenterRealLeft.Y(), temp.OpticalCenterRealLeft.Y(), ioTemperatureDataSet->GetActualTemperature());
    InterpulationCalculation(mColdCalibration, mWarmCalibration, coldOpticalData.TranslationRealRight.X(),
        warmOpticalData.TranslationRealRight.X(), temp.TranslationRealRight.X(), ioTemperatureDataSet->GetActualTemperature());
    InterpulationCalculation(mColdCalibration, mWarmCalibration, coldOpticalData.TranslationRealRight.Y(),
        warmOpticalData.TranslationRealRight.Y(), temp.TranslationRealRight.Y(), ioTemperatureDataSet->GetActualTemperature());
    InterpulationCalculation(mColdCalibration, mWarmCalibration, coldOpticalData.TranslationRealRight.Z(),
        warmOpticalData.TranslationRealRight.Z(), temp.TranslationRealRight.Z(), ioTemperatureDataSet->GetActualTemperature());
    InterpulationCalculation(mColdCalibration, mWarmCalibration, coldOpticalData.TranslationRealLeft.X(),
        warmOpticalData.TranslationRealLeft.X(), temp.TranslationRealLeft.X(), ioTemperatureDataSet->GetActualTemperature());
    InterpulationCalculation(mColdCalibration, mWarmCalibration, coldOpticalData.TranslationRealLeft.Y(),
        warmOpticalData.TranslationRealLeft.Y(), temp.TranslationRealLeft.Y(), ioTemperatureDataSet->GetActualTemperature());
    InterpulationCalculation(mColdCalibration, mWarmCalibration, coldOpticalData.TranslationRealLeft.Z(),
        warmOpticalData.TranslationRealLeft.Z(), temp.TranslationRealLeft.Z(), ioTemperatureDataSet->GetActualTemperature());
    InterpulationCalculation(mColdCalibration, mWarmCalibration, coldOpticalData.RotationRealRight.X(),
        warmOpticalData.RotationRealRight.X(), temp.RotationRealRight.X(), ioTemperatureDataSet->GetActualTemperature());
    InterpulationCalculation(mColdCalibration, mWarmCalibration, coldOpticalData.RotationRealRight.Y(),
        warmOpticalData.RotationRealRight.Y(), temp.RotationRealRight.Y(), ioTemperatureDataSet->GetActualTemperature());
    InterpulationCalculation(mColdCalibration, mWarmCalibration, coldOpticalData.RotationRealRight.Z(),
        warmOpticalData.RotationRealRight.Z(), temp.RotationRealRight.Z(), ioTemperatureDataSet->GetActualTemperature());
    InterpulationCalculation(mColdCalibration, mWarmCalibration, coldOpticalData.RotationRealLeft.X(),
        warmOpticalData.RotationRealLeft.X(), temp.RotationRealLeft.X(), ioTemperatureDataSet->GetActualTemperature());
    InterpulationCalculation(mColdCalibration, mWarmCalibration, coldOpticalData.RotationRealLeft.Y(),
        warmOpticalData.RotationRealLeft.Y(), temp.RotationRealLeft.Y(), ioTemperatureDataSet->GetActualTemperature());
    InterpulationCalculation(mColdCalibration, mWarmCalibration, coldOpticalData.RotationRealLeft.Z(), 
        warmOpticalData.RotationRealLeft.Z(), temp.RotationRealLeft.Z(), ioTemperatureDataSet->GetActualTemperature());
    */
   // ioTemperatureDataSet->GetResolutionDataSets().find("Full")->second->SetCalibrationData(temp);

    return eOK;
}


EInuSensorsDBErrors CMissingCalibrationsAlg::CalculateDataset(std::shared_ptr<CTemperatureDataSet> ioTemperatureDataSet, const boost::filesystem::path& iYamlFile) const
{
    boost::system::error_code boostErr;
    if (!boost::filesystem::exists(iYamlFile, boostErr) || (boostErr.value() != boost::system::errc::success))
    {
        return eYmlNotFound;
    }
    ioTemperatureDataSet->SetIsCalculated(true);
 
    std::shared_ptr<const CCalibrationData> deviceData = ioTemperatureDataSet->GetResolutionDataSets().find("Full")->second->GetCalibrationData();

    return eDSRFailed;
/*
    if (RunDSR(ioTemperatureDataSet->GetCalibrationDir().string(), eNu3000B0, *deviceData, iYamlFile.string()) != eOk)
    {
        return eDSRFailed;
    }
    
    return eOK;
*/
}
