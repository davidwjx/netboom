
#include "CalibrationData2Yaml.h"
#include "IniFile.h"
#include "OSUtilities.h"

#include <yaml-cpp/yaml.h>

using namespace InuCommon;
using namespace std;


bool CCalibrationData2Yaml::Load(CCalibrationData& oCalibration, std::istream& iStream,bool iFromPython)
{
    oCalibration = CCalibrationData();

    YAML::Node ymlFile = YAML::Load(iStream);

    return LoadYaml(oCalibration, ymlFile, iFromPython);
}

// load camera data from storage
template<class Type, int NumberOfElements>
bool ReadYamlSequence(const YAML::Node& iSequence, Type oData[NumberOfElements], Type iNullValue)
{
    if (!iSequence.IsSequence() || iSequence.size() != NumberOfElements)
    {
        return false;
    }

    for (int i = 0; i < NumberOfElements; i++)
    {
        try
        {
#ifdef __ANDROID__
            // Bug in Android ndk23b, there is an exception when reading the type failed 
            // and the exception cause to Seg Fault. Therefore we convert it by our own
            std::istringstream  stream(iSequence[i].as<std::string>());
            Type tmp = iNullValue;
            stream >> tmp;
            oData[i] = tmp;
#else
            oData[i] = iSequence[i].as<Type>();
#endif
        }
        catch (...)
        {
            oData[i] = iNullValue;
        }
    }

    return true;
}

template<class Type, int NumberOfElementsX, int NumberOfElementsY >
bool ReadYamlSequence(const YAML::Node& iSequence, Type oData[NumberOfElementsX][NumberOfElementsY], Type iNullValue)
{
    if (!iSequence.IsSequence() || iSequence.size() != NumberOfElementsX * NumberOfElementsY)
    {
        return false;
    }

    for (int i = 0; i < NumberOfElementsX; i++)
    {
        for (int j = 0; j < NumberOfElementsY; j++)
        {
            try
            {
#ifdef __ANDROID__
            	std::istringstream  stream(iSequence[i * NumberOfElementsY + j].as<std::string>());
                Type tmp = iNullValue;
                stream >> tmp;
                oData[i][j] = tmp;
#else
                oData[i][j] = iSequence[i * NumberOfElementsY + j].as<Type>();
#endif
            }
            catch (...)
            {
                oData[i][j] = iNullValue;
            }
        } 
    }


    return true;
}

bool CCalibrationData2Yaml::ReadCameraFromYaml(const YAML::Node& iCameraNode, CCalibrationData::COneSensorData::CCameraData &oCameraData)
{
    bool ret = true;
    for (YAML::const_iterator iter = iCameraNode.begin(); iter != iCameraNode.end(); iter++)
    {
        string section = iter->first.Scalar();
        if (ret && section == "extrinsic")
        {
            // Read Extrinsic parameters 
            YAML::Node extrinsic = iter->second;
            for (YAML::const_iterator iterExt = extrinsic.begin(); iterExt != extrinsic.end(); iterExt++)
            {
                if (iterExt->first.Scalar() == "T")
                {
                    ret = ret && ReadYamlSequence<double, 3>(iterExt->second, oCameraData.Extrinsic.Translation,0);
                }

                else if (iterExt->first.Scalar() == "om")
                {
                    ret = ret && ReadYamlSequence<double, 3>(iterExt->second, oCameraData.Extrinsic.Rotation,0);
                }
            }
            oCameraData.Extrinsic.Valid = ret;
        }

        else if (ret && section == "intrinsic")
        {
            // Read Extrinsic parameters 
            YAML::Node intrinsic = iter->second;
            for (YAML::const_iterator iterInt = intrinsic.begin(); iterInt != intrinsic.end(); iterInt++)
            {
                if (iterInt->first.Scalar() == "alphac")
                {
                    try
                    {
                        oCameraData.Intrinsic.Alpha = iterInt->second[0].as<double>();
                    }
                    catch (...)
                    {
                        // When the value is 0 it doesn't recognize it as double
                        oCameraData.Intrinsic.Alpha = std::stof(iterInt->second.Scalar());
                    }
                }
                else if (iterInt->first.Scalar() == "cc")
                {
                    ret = ret && ReadYamlSequence<double, 2>(iterInt->second, oCameraData.Intrinsic.OpticalCenter,0);
                }
                else if (iterInt->first.Scalar() == "fc")
                {
                    ret = ret && ReadYamlSequence<double, 2>(iterInt->second, oCameraData.Intrinsic.FocalLength,0);
                }
                else if (iterInt->first.Scalar() == "kc")
                {
                    if (!iterInt->second.IsSequence())
                    {
                        ret = false;
                    }

                    oCameraData.Intrinsic.LensDistortion.clear();
                    for (size_t i = 0; i < iterInt->second.size() ; i++)
                    {
                        oCameraData.Intrinsic.LensDistortion.push_back(iterInt->second[i].as<double>());
                    }
                }
                else if (iterInt->first.Scalar() == "model")
                {
                    int model = int(std::stoul(iterInt->second.Scalar()));
                    oCameraData.Intrinsic.Model = CCalibrationData::COneSensorData::CCameraData::CIntrinsicData::ECameraModel(model);
                }
                else if (iterInt->first.Scalar() == "rc")
                {
                    oCameraData.Intrinsic.Rc.clear();
                    for (size_t i = 0; i < iterInt->second.size(); i++)
                    {
                        oCameraData.Intrinsic.Rc.push_back(iterInt->second[i].as<double>());
                    }
                }
            }
            oCameraData.Intrinsic.Valid = ret;
        }

        else if (ret && section == "sensor")
        {
            // Read Extrinsic parameters 
            YAML::Node sensorData = iter->second;
            for (YAML::const_iterator iterData = sensorData.begin(); iterData != sensorData.end(); iterData++)
            {
                if (iterData->first.Scalar() == "hor_pix_size")
                {
                    oCameraData.PixelSize[0] = iterData->second.IsNull() ? 0 : std::stof(iterData->second.Scalar());
                }
                else if (iterData->first.Scalar() == "hor_res")
                {
                    oCameraData.Resolution[0] = iterData->second.IsNull() ? 0 : uint32_t(std::stoul(iterData->second.Scalar()));
                }
                else if (iterData->first.Scalar() == "ver_pix_size")
                {
                    oCameraData.PixelSize[1] = iterData->second.IsNull() ? 0 : std::stof(iterData->second.Scalar());
                }
                else if (iterData->first.Scalar() == "ver_res")
                {
                    oCameraData.Resolution[1] = iterData->second.IsNull() ? 0 : uint32_t(std::stoul(iterData->second.Scalar()));
                }
            }
        }
    }
    
    return ret;
}

void ImuTemperatureFromYaml(const YAML::Node& iImuIntrinsic, CCalibrationData::CImuData::CTemperatureData& oImuTempData)
{
    bool result = false;
    bool empty = true;

    for (YAML::const_iterator iter = iImuIntrinsic.begin(); iter != iImuIntrinsic.end(); iter++)
    {
        string section = iter->first.Scalar();
        if (section == "temp_reference")
        {
            oImuTempData.Reference = std::stof(iter->second.Scalar());
            result = true;
            empty = false;
        }
        else if (section == "temp_slope")
        {
        	result = ReadYamlSequence<double, 3>(iter->second, oImuTempData.Slope,0);
            if (result == false)
            {
                break;
            }
            empty = false;
        }
        else
        {
            // Unrecognized section
        }
    }

    oImuTempData.Valid = result && !empty;
}

void ImuIntrinsicFromYaml(const YAML::Node& iImuIntrinsic, CCalibrationData::CImuData::CIntrinsicData& oImuIntrinsic)
{
    bool result = true;
    bool empty = true;

    for (YAML::const_iterator iter = iImuIntrinsic.begin(); iter != iImuIntrinsic.end(); iter++)
    {
        string section = iter->first.Scalar();
        if (section == "bias")
        {
            result = ReadYamlSequence<double, 3>(iter->second, oImuIntrinsic.Bias,0);
            if (result == false)
            {
                break;
            }
            empty = false;
        }
        else if (section == "noise")
        {
            result = ReadYamlSequence<double, 3>(iter->second, oImuIntrinsic.Noise,0);
            if (result == false)
            {
                break;
            }
            empty = false;
        }
        else if (section == "misalignment")
        {
            result = ReadYamlSequence<double, 3, 3>(iter->second, oImuIntrinsic.Misalignment,0);
            if (result == false)
            {
                break;
            }
            empty = false;
        }
        else if (section == "scale")
        {
            result = ReadYamlSequence<double, 3, 3>(iter->second, oImuIntrinsic.Scale,0);
            if (result == false)
            {
                break;
            }
            empty = false;
        }
        else
        {
            // Unrecognized section
        }
    }

    oImuIntrinsic.Valid = result && !empty;
}

void ImuFromYaml(const YAML::Node& iImuYaml, CCalibrationData::CImuData& oImu)
{
    for (YAML::const_iterator iter = iImuYaml.begin(); iter != iImuYaml.end(); iter++)
    {
        string section = iter->first.Scalar();
        if (section == "accelerometer")
        {
            ImuIntrinsicFromYaml(iter->second, oImu.Accelerometer);
            ImuTemperatureFromYaml(iter->second, oImu.AccelerometerTemperature);
        }
        else if (section == "gyroscope")
        {
            ImuIntrinsicFromYaml(iter->second, oImu.Gyroscope);
            ImuTemperatureFromYaml(iter->second, oImu.GyroscopeTemperature);
        }
        else if (section == "extrinsic")
        {
            oImu.ValidExterinsic = ReadYamlSequence<double, 4, 4>(iter->second, oImu.Extrinsic,0);
        }
        else
        {
            // Unrecognized section
        }
    }
}

bool CCalibrationData2Yaml::LoadYaml(CCalibrationData& oCalibration, YAML::Node &ymlFile, bool iFromPython)
{
    if (!iFromPython && ymlFile.size() < 2)
    {
        return false;
    }

    for (const auto& elem : ymlFile)
    {

        if (elem.first.Scalar() == "version")
        {
            oCalibration.Version = elem.second.as<string>();
        }

        else if (elem.first.Scalar() == "imagers")
        {
            // Read "Imagers"
            YAML::Node allSensors = elem.second;

            // Iterate and read all sensors
            for (YAML::const_iterator iter = allSensors.begin(); iter != allSensors.end(); iter++)
            {
                string sensorName = iter->first.Scalar();
                if (iFromPython && sensorName == "fer")
                {
                    // Yaml that was created by Python and not be Calibration
                    continue;
                }
                size_t pos = sensorName.find_last_of('_');
                if (pos == string::npos)
                {
                    return false;
                }
                int index = int(std::stoul(sensorName.substr(pos + 1, sensorName.size() - pos - 1)));

                CCalibrationData::COneSensorData& sensorData = oCalibration.Sensors[index];

                // Populate all defined cameras
                bool result = true;
                YAML::Node allCameras = iter->second;
                for (YAML::const_iterator iterCamera = allCameras.begin(); iterCamera != allCameras.end(); iterCamera++)
                {
                    string cameraName = iterCamera->first.Scalar();
                    if (cameraName == "real")
                    {
                        result = result && ReadCameraFromYaml(iterCamera->second, sensorData.RealCamera);
                        sensorData.RealCamera.Valid = true;
                    }
                    else if (cameraName == "virtual")
                    {
                        result = result && ReadCameraFromYaml(iterCamera->second, sensorData.VirtualCamera);
                        sensorData.VirtualCamera.Valid = true;
                    }
                    else if (cameraName == "type")
                    {
                        sensorData.Description = iterCamera->second.Scalar();
                    }
                }

                if (!result)
                {
                    return false;
                }
            }
        }

        else if (elem.first.Scalar() == "stereo")
        {
            if (!ReadBasline(elem.second, oCalibration))
            {
                return false;
            }
        }

        // Read "IMU"
        else if (elem.first.Scalar() == "imu")
        {
            ImuFromYaml(elem.second, oCalibration.Imu);
        }
    }

    iFromPython = oCalibration.Baselines.empty();

    return !oCalibration.Sensors.empty();
}



// load camera data from storage
template<class Type, int NumberOfElements>
void SaveYamlSequence(YAML::Node& oSequence, const string& iKey, const Type iData[NumberOfElements], Type iNullValue , Type iEpsilon = std::numeric_limits<Type>::epsilon())
{
    for (int i = 0; i < NumberOfElements; i++)
    {
        Type tmp = std::abs(iData[i]) < iEpsilon ? iNullValue : iData[i];
        oSequence[iKey].push_back(tmp);
    }
}

YAML::Node CameraToYaml(const CCalibrationData::COneSensorData::CCameraData& iCamera)
{
    YAML::Node outYamlNode;

    if (iCamera.Extrinsic.Valid)
    {
        YAML::Node extrinsicNode;
        SaveYamlSequence<double,3>(extrinsicNode, "T", iCamera.Extrinsic.Translation, 0);
        SaveYamlSequence<double, 3>(extrinsicNode, "om", iCamera.Extrinsic.Rotation, 0);

        outYamlNode.force_insert("extrinsic", extrinsicNode);
    }

    if (iCamera.Intrinsic.Valid)
    {
        YAML::Node intrinsicNode;
        SaveYamlSequence<double, 1>(intrinsicNode, "alphac", &iCamera.Intrinsic.Alpha, 0);
        SaveYamlSequence<double, 2>(intrinsicNode, "cc", iCamera.Intrinsic.OpticalCenter, 0);
        SaveYamlSequence<double, 2>(intrinsicNode, "fc", iCamera.Intrinsic.FocalLength, 0);
        for (const auto& elem : iCamera.Intrinsic.LensDistortion)
        {
            intrinsicNode["kc"].push_back(std::abs(elem) < std::numeric_limits<double>::epsilon() ? 0 : elem);
        }        
        for (const auto& elem : iCamera.Intrinsic.Rc)
        {
            intrinsicNode["rc"].push_back(std::abs(elem) < std::numeric_limits<double>::epsilon() ? 0 : elem);
        }
        intrinsicNode.force_insert("model", int(iCamera.Intrinsic.Model));

        outYamlNode.force_insert("intrinsic", intrinsicNode);
    }

    YAML::Node sesnorNode;
    sesnorNode["hor_pix_size"] = std::abs(iCamera.PixelSize[0]) < std::numeric_limits<double>::epsilon() ? 0 : iCamera.PixelSize[0];
    sesnorNode["hor_res"] = iCamera.Resolution[0];
    sesnorNode["ver_pix_size"] = std::abs(iCamera.PixelSize[1]) < std::numeric_limits<double>::epsilon() ? 0 : iCamera.PixelSize[1];
    sesnorNode["ver_res"] = iCamera.Resolution[1];
    outYamlNode.force_insert("sensor", sesnorNode);
    return outYamlNode;
}

YAML::Node OneSensorsToYaml(const CCalibrationData::COneSensorData& iSensor)
{
    YAML::Node outYamlNode;

    if (iSensor.RealCamera.Valid)
    {
        outYamlNode.force_insert("real", CameraToYaml(iSensor.RealCamera));
    }

    outYamlNode.force_insert("type", iSensor.Description);

    if (iSensor.VirtualCamera.Valid)
    {
        outYamlNode.force_insert("virtual", CameraToYaml(iSensor.VirtualCamera));
    }

    return outYamlNode;
}

void ImuTemperatureToYaml(YAML::Node& oYamlNode, const CCalibrationData::CImuData::CTemperatureData& iImuTemperature)
{
    if (iImuTemperature.Valid == false)
    {
        return;
    }

    oYamlNode.force_insert("temp_reference", iImuTemperature.Reference);
    SaveYamlSequence<double, 3>(oYamlNode, "temp_slope", iImuTemperature.Slope, 0);
}

void ImuIntrinsicToYaml(YAML::Node& oYamlNode, const CCalibrationData::CImuData::CIntrinsicData& iImuIntrinsic)
{
    if (iImuIntrinsic.Valid == false)
    {
        return;
    }
    SaveYamlSequence<double, 3>(oYamlNode, "bias", iImuIntrinsic.Bias, 0);
    SaveYamlSequence<double, 3>(oYamlNode, "noise", iImuIntrinsic.Noise, 0);
    for (int i = 0; i < 3; i++)
    {
        SaveYamlSequence<double, 3>(oYamlNode, "misalignment", iImuIntrinsic.Misalignment[i], 0);
    }
    for (int i = 0; i < 3; i++)
    {
        SaveYamlSequence<double, 3>(oYamlNode, "scale", iImuIntrinsic.Scale[i], 0);
    }
}

YAML::Node ImuToYaml(const CCalibrationData::CImuData& iImu)
{
    YAML::Node outYamlNode;

    if (iImu.Accelerometer.Valid || iImu.AccelerometerTemperature.Valid)
    {
        YAML::Node sectionNode;
        ImuIntrinsicToYaml(sectionNode, iImu.Accelerometer);
        ImuTemperatureToYaml(sectionNode, iImu.AccelerometerTemperature);
        outYamlNode.force_insert("accelerometer", sectionNode);
    }

    if (iImu.Gyroscope.Valid || iImu.GyroscopeTemperature.Valid)
    {
        YAML::Node sectionNode;
        ImuIntrinsicToYaml(sectionNode, iImu.Gyroscope);
        ImuTemperatureToYaml(sectionNode, iImu.GyroscopeTemperature);
        outYamlNode.force_insert("gyroscope", sectionNode);
    }

    if (iImu.ValidExterinsic)
    {
	    for (int i = 0; i < 4; i++)
	    {
	        SaveYamlSequence<double, 4>(outYamlNode, "extrinsic", iImu.Extrinsic[i], 0);
	    }
    }

    return outYamlNode;
}

bool CCalibrationData2Yaml::Save(const CCalibrationData& iCalibration, std::ostream& oStream)
{
    // Build YAML from this
    YAML::Node outYaml;

    // Insert Version
    outYaml.force_insert("version", iCalibration.Version);

    if (iCalibration.Sensors.size() > 0)
    {
        YAML::Node allSensorsNode;
        for (const auto& elem : iCalibration.Sensors)
        {
            allSensorsNode.force_insert("sensor_" + to_string(elem.first), OneSensorsToYaml(elem.second));
        }
        outYaml.force_insert("imagers", allSensorsNode);

        // Insert baselines
        int index = 0;
        YAML::Node stereo;
        for (const auto& elem : iCalibration.Baselines)
        {
            YAML::Node baseline;
            baseline["imagers"].push_back("sensor_" + to_string(elem.first.first));
            baseline["imagers"].push_back("sensor_" + to_string(elem.first.second));
            baseline.force_insert("val",elem.second);

            stereo.force_insert("baseline_" + to_string(index++), baseline);
        }

        outYaml.force_insert("stereo", stereo);

        // Insert IMU
        YAML::Node imuData = ImuToYaml(iCalibration.Imu);
        if (imuData.size())
        {
            outYaml.force_insert("imu", imuData);
        }
    }
    YAML::Emitter emitter(oStream);
    emitter.SetSeqFormat(YAML::Flow);
    emitter << outYaml;

    return oStream.good() ? true : false;
}

bool InuCommon::CCalibrationData2Yaml::ReadBasline(const YAML::Node& iBaselines, CCalibrationData& oCalibration)
{
    // Iterate and read all baselines
    for (YAML::const_iterator baselineIter = iBaselines.begin(); baselineIter != iBaselines.end(); baselineIter++)
    {
        YAML::const_iterator iter = baselineIter->second.begin();

        double value(0);
        if (iter->first.Scalar() != "imagers")
        {
            return false;
        }

        string sensors[2];
        if (!ReadYamlSequence<string, 2>(iter->second, sensors,""))
        {
            return false;
        }

        pair<int, int> baselineSensors;
        for (size_t i = 0; i < 2; i++)
        {
            const auto& elem = sensors[i];
            size_t pos = elem.find_last_of('_');
            if (pos == string::npos)
            {
                return false;
            }
            string indexStr = elem.substr(pos + 1, elem.size() - pos - 1);
            int index;
            try
            {
                index = int(std::stoul(indexStr));
            }
            catch (...)
            {
                return false;
            }
            if (i == 0)
            {
                baselineSensors.first = index;
            }
            else
            {
                baselineSensors.second = index;
            }
        }

        iter++;
        if (iter->first.Scalar() != "val")
        {
            return false;
        }
        value = iter->second.as<double>();

        oCalibration.Baselines[baselineSensors] = value;
    }

    return true;
}

