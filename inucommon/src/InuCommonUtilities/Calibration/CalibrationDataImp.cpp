#include "CalibrationDataImp.h"
#include "CalibrationData2Yaml.h"
#include "OSUtilities.h"

#include "IniFile.h"

#include <boost/filesystem/path.hpp>
#include <boost/filesystem.hpp>
#include <boost/property_tree/ptree_fwd.hpp>
#include <boost/property_tree/ini_parser.hpp>

using namespace InuCommon;
using namespace std;

const std::string CCalibrationDataImp::OPTICAL_DATA_FILE_NAME("SystemParameters.yml");

CCalibrationDataImp::CCalibrationDataImp() :
    mOldFormatNU4000B0(false)
{
}

EInuSensorsDBErrors CCalibrationDataImp::Load(const boost::filesystem::path& iCalibrationSetDir, InuCommon::CTiffFile::EHWType iHwType, const CCalibrationData* iOrigCalibrationData, bool iFromPython)
{
    EInuSensorsDBErrors rc = eInternalError;

    boost::filesystem::path fileName(iCalibrationSetDir);
    fileName /= OPTICAL_DATA_FILE_NAME.c_str();
    
    boost::system::error_code boostErr;
    if (boost::filesystem::exists(fileName, boostErr) && (boostErr.value() == boost::system::errc::success) )
    {
        rc = LoadFromYaml(fileName, iFromPython);
    }

    // Any change to IMU params requires copy changes here.
    if ((rc == eOK) && (iOrigCalibrationData != nullptr) )
    {
        // Copy all sensors that are not stereo

        // Find all stereo sensor IDs
        std::vector<int> stereoSensors;
        for (const auto& baseline : iOrigCalibrationData->Baselines)
        {
            if ((std::find(stereoSensors.begin(), stereoSensors.end(), baseline.first.first) == stereoSensors.end()))
            {
                stereoSensors.push_back(baseline.first.first);
            }
            if ((std::find(stereoSensors.begin(), stereoSensors.end(), baseline.first.second) == stereoSensors.end()))
            {
                stereoSensors.push_back(baseline.first.second);
            }
        }

        for (const auto& sensor : iOrigCalibrationData->Sensors)
        {
            if ((std::find(stereoSensors.begin(), stereoSensors.end(), sensor.first) == stereoSensors.end()))
            {
                // Not a stereo sensor, need to copy it
                Sensors[sensor.first] = iOrigCalibrationData->Sensors.at(sensor.first);
            }
        }
        
		// Copy the IMU data
        if (iOrigCalibrationData->Imu.ValidExterinsic || iOrigCalibrationData->Imu.Accelerometer.Valid || iOrigCalibrationData->Imu.AccelerometerTemperature.Valid)
        {
            Imu = iOrigCalibrationData->Imu;
        }
    }


    return rc;
}

EInuSensorsDBErrors CCalibrationDataImp::LoadFromYaml(const boost::filesystem::path& iFileName, bool iFromPython)
{
    // Reset data
    std::ifstream  stream(iFileName.string());
    if (stream.bad())
    {
        return eFileOpen;
    }

    return (CCalibrationData2Yaml::Load(*this, stream, iFromPython)) == true ? eOK : eOpticalDataFileMissingValue;
}

EInuSensorsDBErrors CCalibrationDataImp::Save(const boost::filesystem::path& iCalibrationSetDir) const
{
    ofstream outFile((iCalibrationSetDir / OPTICAL_DATA_FILE_NAME).string());
    return (CCalibrationData2Yaml::Save(*this, outFile) == true) ? eOK : eSaveOpticalData;
}

void CCalibrationDataImp::UpdateSensorsID(const std::pair<int, int>& iStereoChannels, const std::pair<int, int>& iFeChannels, const int iRgbChannel)
{
    if (!mOldFormatNU4000B0)
    {
        return;
    }

    // Need to execute this code only once
    mOldFormatNU4000B0 = false;

    std::map<int, COneSensorData> newSensors;
    if (iStereoChannels.first != -1)
    {
        // replace the Stereo channels
        newSensors[iStereoChannels.first] = Sensors[DEFAULT_IR_RIGHT];
        newSensors[iStereoChannels.second] = Sensors[DEFAULT_IR_LEFT];
    }

    if (iFeChannels.first != -1)
    {
        newSensors[iFeChannels.first] = Sensors[DEFAULT_FE_RIGHT];
    }

    if (iFeChannels.second != -1)
    {
        newSensors[iFeChannels.second] = Sensors[DEFAULT_FE_LEFT];
    }

    if (iRgbChannel != -1)
    {
        newSensors[iRgbChannel] = Sensors[DEFAULT_RGB];
    }

    Sensors = newSensors;

    for (auto& elem : Baselines)
    {
        if ((elem.first.first == DEFAULT_IR_RIGHT) || (elem.first.second == DEFAULT_IR_RIGHT))
        {
            double val = elem.second;
            Baselines.erase(elem.first);
            Baselines[iStereoChannels] = val;
            break;
        }
    }
}

