#include <fstream>
#include <string>
#include <sstream>
#include <iostream>
#include <memory>

#include "InuDevice.h"
#include "InuModel.h"
#include "ResolutionDataSet.h"
#include "TemperatureDataSet.h"
#include "CalibrationDataImp.h"
#include "OSUtilities.h"

#include <boost/filesystem/operations.hpp>

using namespace std;
using namespace InuCommon;

void PrintOpticalData(const CCalibrationData::COneSensorData::CCameraData &iCameraData, const string& iCameraName)
{
    cout << iCameraName << ".Extrinsic.Valid : " << iCameraData.Extrinsic.Valid << endl;
    if (iCameraData.Extrinsic.Valid)
    {
        cout << iCameraName << ".Extrinsic.Rotation : " << iCameraData.Extrinsic.Rotation[0] << endl;
        cout << iCameraName << ".Extrinsic.Translation : " << iCameraData.Extrinsic.Translation[0] << endl;
    }

    cout << iCameraName << ".Intrinsic.Valid : " << iCameraData.Intrinsic.Valid << endl;
    if (iCameraData.Intrinsic.Valid)
    {
        cout << iCameraName << ".Intrinsic.Alpha : " << iCameraData.Intrinsic.Alpha << endl;
        cout << iCameraName << ".Intrinsic.FocalLength : " << iCameraData.Intrinsic.FocalLength[0] << endl;
        cout << iCameraName << ".Intrinsic.Model : " << iCameraData.Intrinsic.Model << endl;
        cout << iCameraName << ".Intrinsic.OpticalCenter : " << iCameraData.Intrinsic.OpticalCenter[0] << endl;
        cout << iCameraName << ".Intrinsic.LensDistortion : ";
        for (auto elem : iCameraData.Intrinsic.LensDistortion)
        {
            cout << elem << " ";
        } 
        cout << endl;
    }
}

void PrintValueIMU(const string& iKey, const double* iValue, int iValueLen)
{
    cout << iKey;
    for (int i = 0; i < iValueLen; i++)
    {
        cout << iValue[i] << ",";
    }
    cout << endl;
}

void PrintOpticalData(const CCalibrationData& iDeviceData)
{
    cout << "Device Data, version: " << iDeviceData.Version << endl << "************************\n";
    for (const auto& elem : iDeviceData.Sensors)
    {
        cout << "Sensor ID: " << elem.first << endl << "-------------------\n";
        cout << "Description: " << elem.second.Description << endl;

        PrintOpticalData(elem.second.RealCamera, "Real");
        PrintOpticalData(elem.second.VirtualCamera, "Virtual");
    }

    if (iDeviceData.Imu.Accelerometer.Valid)
    {
        cout << "IMU Acc Valid:\n-------------------\n";
        PrintValueIMU("iDeviceData.Imu.Accelerometer.Bias : ", iDeviceData.Imu.Accelerometer.Bias, 3);
        PrintValueIMU("iDeviceData.Imu.Accelerometer.Misalignment : " , &(iDeviceData.Imu.Accelerometer.Misalignment[0][0]) , 9 );
        PrintValueIMU("iDeviceData.Imu.Accelerometer.Noise : ", iDeviceData.Imu.Accelerometer.Noise , 3);
        PrintValueIMU("iDeviceData.Imu.Accelerometer.Scale : ", &(iDeviceData.Imu.Accelerometer.Scale[0][0]), 9);
    }
    if (iDeviceData.Imu.AccelerometerTemperature.Valid)
    {
        PrintValueIMU("iDeviceData.Imu.Accelerometer.Reference : ", &iDeviceData.Imu.AccelerometerTemperature.Reference , 1);
        PrintValueIMU("iDeviceData.Imu.Accelerometer.Slope : ", iDeviceData.Imu.AccelerometerTemperature.Slope , 3);
    }

    if (iDeviceData.Imu.Gyroscope.Valid)
    {
        PrintValueIMU("iDeviceData.Imu.Gyroscope.Bias : ", iDeviceData.Imu.Gyroscope.Bias , 3);
        PrintValueIMU("iDeviceData.Imu.Gyroscope.Misalignment : ", &(iDeviceData.Imu.Gyroscope.Misalignment[0][0]), 9);
        PrintValueIMU("iDeviceData.Imu.Gyroscope.Noise : ", iDeviceData.Imu.Gyroscope.Noise , 3);
        PrintValueIMU("iDeviceData.Imu.Gyroscope.Scale : ", &(iDeviceData.Imu.Gyroscope.Scale[0][0]), 9);
    }
    if (iDeviceData.Imu.GyroscopeTemperature.Valid)
    {
        PrintValueIMU("iDeviceData.Imu.Gyroscope.Reference : ", &iDeviceData.Imu.GyroscopeTemperature.Reference , 1);
        PrintValueIMU("iDeviceData.Imu.Gyroscope.Slope : ", iDeviceData.Imu.GyroscopeTemperature.Slope , 3);
    }

    if (iDeviceData.Imu.ValidExterinsic)
    {
        PrintValueIMU("iDeviceData.Imu.Extrinsic : ", &(iDeviceData.Imu.Extrinsic[0][0]), 16);
    }
}

void PrintSensorInfo(const CInuDevice& sensor)
{
    cout << "Sensor Data: " << endl << "-----------------\n";
    cout << "Name: " << sensor.SensorID() << "\nType: " << sensor.GetSensorModelName()  
        << "\nRevision: " << sensor.GetRevisionID() << "\nHW Type: " << sensor.HwType()
        << "\nBoot ID: " << sensor.GetBootID() << "\nFW Version: " << sensor.GetDualSensorsMaster() << endl;

    cout << "LUT 0 exist: " << sensor.LutExist(0) << std::endl;
    cout << "LUT 2 exist: " << sensor.LutExist(2) << std::endl;
}

void PrintCalibrationData(shared_ptr<const CResolutionDataSet> calibrationData)
{
    PrintOpticalData(*calibrationData->GetCalibrationData());

    try
    {
        cout << "IB LUT Left - size: " << calibrationData->GetLUT(CLutsDataSet::eIntensityBalance, 0).size() << endl;
        cout << "D&R LUT Left - size: " << calibrationData->GetLUT(CLutsDataSet::eDistortionAndRectification, 0).size() << endl;
        cout << "IB LUT Right - size: " << calibrationData->GetLUT(CLutsDataSet::eIntensityBalance, 1).size() << endl;
        cout << "D&R LUT Right - size: " << calibrationData->GetLUT(CLutsDataSet::eDistortionAndRectification, 1).size() << endl;
    }
    catch(...) {
    }
         

    const vector < CRegistersSet::CRegisterData > & allReg = calibrationData->GetRegistersSet()->GetRegistersData();
    for (size_t i=0; i<allReg.size() ; i++)
    {
        cout << "Type: " << ((allReg[i].mType == CRegistersSet::eSensor) ? "SEN " : (allReg[i].mType == CRegistersSet::eRegister) ? "REG" : (allReg[i].mType == CRegistersSet::eI2C) ? "IC2" : "Wait");
        cout << hex << ", Address: " << allReg[i].mAddress << ", Data: " << allReg[i].mData << ", Eye: " 
            << ((allReg[i].mLeftEye) ? "L " : "R ")  << ", Size: " << allReg[i].mSize << ", Size: " << allReg[i].mPhase << endl;
    }
    cout << dec;
    cout << endl << "Config.ireg as string:" << calibrationData->GetRegistersSet()->GetRegistersDataAsString();

    cout << endl << endl;

}

void InuSensorTest(
    CInuDevice& sensor, 
    CTiffFile::ESensorResolution resolution, 
    int temperature,
    vector<int>  requiredTemp,
    const std::string& dirName=std::string())
{
    EInuSensorsDBErrors ec = sensor.Init();
    if (ec != InuCommon::eOK)
    {
        cout << "Failed to init InuCommon::CInuDevice - error: " << to_string(ec) << endl;
        return;
    }

    EInuSensorsDBErrors rc = sensor.LoadCalibration(requiredTemp, 0, 60, dirName,true);
    if (rc != eOK)
    {
        cout << "Failed to load calibration: " << rc << endl;
        return;
    }

    shared_ptr<const CResolutionDataSet>  calibrationData = sensor.GetTemperatureDataSet(temperature)->GetResolution(resolution);
    if (calibrationData == nullptr)
    {
        cout << "Failed to load calibration data" << endl;
        return;
    }
    else
    {
        PrintSensorInfo(sensor);
        PrintCalibrationData(calibrationData);
    }
}

/*/
void InuSensorDBTest(const string& sensorID, CTiffFile::ESensorResolution resolution)
{
    shared_ptr<CInuDeviceDB>  sensorsDB=CInuDeviceDB::GetTheInstance();
    
    InuSensorTest(sensorsDB->GetInuSensor(sensorID) , resolution);
}
*/

void GetFromTiff(const string& fileName)
{
    unsigned int w, h, bpp;
    CTiffFile::ETiffFormat format;
    CTiffFile::CMetaData metaData;

    cout << "Read from tiff\n\n";

    EInuSensorsDBErrors rc = eOK;
    if (CTiffFile::ReadHeader(fileName, w, h, bpp, format, metaData) == CTiffFile::eOK)
    {
        CInuDevice sensor(metaData.SensorID.Get(), metaData.SensorModel.Get(), metaData.HWType.Get());
        
        EInuSensorsDBErrors ec = sensor.Init();
        if (ec != InuCommon::eOK)
        {
            cout << "Failed to init InuCommon::CInuDevice - error: " << to_string(ec) << endl;
            return;
        }

        vector<int>  requiredTemp;
        rc = sensor.LoadCalibration(requiredTemp, 0, 60, "", false);
        if (rc != eOK)
        {
            cout << "Failed to load calibration: " << rc << endl;
        }
        else
        {
            shared_ptr<const CResolutionDataSet> calibrationData = sensor.GetTemperatureDataSet(CInuDevice::UNDEFIEND_CALIB_TEMPERATURE)->GetResolution(metaData.SensorResolution.Get());
            if (calibrationData == nullptr)
            {
                cout << "Failed to load calibration data from tiff file" << endl;
            }
            else
            {
                PrintSensorInfo(sensor);
                PrintCalibrationData(calibrationData);
            }
        }
    }
    else
    {
        cout << "Failed to read tiff header" << endl;
    }
}

void TestSensorDataFromCSV(const std::string& iModelName) 
{
    InuCommon::CInuModel test;

    //test.Init(iModelName);
}

int main (int argc, char** argv)
{
    int testType(0);
    string sensorID;
    CTiffFile::ESensorResolution  resolution;

    if (argc > 1)
    {
        ifstream   inputFile(argv[1]);
        while (inputFile.good() && !inputFile.eof())
        {
            char buffer[1024];
            memset(buffer, 0, 1024);
            inputFile.getline(buffer, 1024);
            if (buffer[0] == '#')
            {
                continue;
            }

            string line(buffer);

            size_t pos = line.find_first_of('=') + 1;
            string testType = line.substr(pos, line.size() - pos );
            int test = 0;
            try
            {
                test = std::stoul(testType);
            }
            catch (...)
            {
                continue;
            }

            memset(buffer, 0, 1024);

            cout << "\n///////////////////\n TEST no. " << test << "\n///////////////////\n\n";
            switch (test)
            {

            case 2:
            {
                inputFile.getline(buffer, 1024);
                line = string(buffer);
                pos = line.find_first_of('=') + 1;
                GetFromTiff(line.substr(pos, line.size() - pos));
            }
            break;
            case 3:
            {
                inputFile.getline(buffer, 1024);
                line = string(buffer);
                pos = line.find_first_of('=') + 1;
                string dirName = line.substr(pos, line.size() - pos);

                inputFile.getline(buffer, 1024);
                line = string(buffer);
                pos = line.find_first_of('=') + 1;
                string model = line.substr(pos, line.size() - pos);

                inputFile.getline(buffer, 1024);
                line = string(buffer);
                pos = line.find_first_of('=') + 1;
                string fwRevision = line.substr(pos, line.size() - pos);
                CTiffFile::EHWType hwType = CTiffFile::eNU4100A0;
                if (fwRevision == "NU4000C0")
                {
                    hwType = CTiffFile::eNU4000C0;
                }
                else if(fwRevision == "NU4000C1")
                {
                    hwType = CTiffFile::eNU4000C1;
                }

                CInuDevice  sensor(dirName, model, hwType);
                vector<int>  requiredTemp = { 3,25,45 };
                InuSensorTest(sensor, CTiffFile::ESensorResolution::eFull, 1000, requiredTemp, dirName);
            }
            break;
            case 4:
            {
                inputFile.getline(buffer, 1024);
                line = string(buffer);
                pos = line.find_first_of('=') + 1;
                TestSensorDataFromCSV(line.substr(pos, line.size() - pos));
            }
            break;
/*            case 5:
            {
                COpticalDataImp  tmp;
                EInuSensorsDBErrors rc = tmp.Load("TestOptical", CTiffFile::eB0, nullptr);
                if (rc != eOK)
                {
                    cout << "Failed to load optical data: " << rc << endl;
                }
                PrintOpticalData(tmp);

                CCalibrationDataImp deviceData(tmp);
                PrintOpticalData(deviceData);

#ifdef _DEBUG
                boost::system::error_code ec;
                if (!boost::filesystem::exists("Output", ec))
                {
                    boost::filesystem::create_directory("Output", ec);
                    if (ec != boost::system::errc::success)
                    {
                        cout << "Failed to create Output folder " << endl;
                    }
                }

                rc = tmp.Save("Output");
                if (rc != eOK)
                {
                    cout << "Failed to save optical data to Output folder:  " << rc << endl;
                }
                rc = deviceData.Save("Output");
                if (rc != eOK)
                {
                    cout << "Failed to save optical data to Output folder:  " << rc << endl;
                }

                if (!boost::filesystem::exists("OutputDeviceData", ec))
                {
                    boost::filesystem::create_directory("OutputDeviceData", ec);
                    if (ec != boost::system::errc::success)
                    {
                        cout << "Failed to create Output folder " << endl;
                    }
                }
                rc = COpticalDataImp(deviceData).Save("OutputDeviceData");
                if (rc != eOK)
                {
                    cout << "Failed to save optical data to Output folder:  " << rc << endl;
                }
#endif
                break;
            }
*/
            case 6:
            {
                CCalibrationDataImp  tmp;
                EInuSensorsDBErrors rc = tmp.Load("TestOpticalNew", CTiffFile::eNU4000B0, nullptr);
                if (rc != eOK)
                {
                    cout << "Failed to load optical data: " << rc << endl;
                }
                PrintOpticalData(tmp);

#ifdef _DEBUG
                boost::system::error_code ec;
                if (!boost::filesystem::exists("Output", ec))
                {
                    boost::filesystem::create_directory("Output", ec);
                    if (ec != boost::system::errc::success)
                    {
                        cout << "Failed to create Output folder " << endl;
                    }
                }

                rc = tmp.Save("Output");
                if (rc != eOK)
                {
                    cout << "Failed to save optical data to Output folder:  " << rc << endl;
                }
#endif
                break;
            }
            break;

            default:
            {
                inputFile.getline(buffer, 1024);
                line = string(buffer);
                string sensorID(buffer);

                memset(buffer, 0, 1024);
                inputFile.getline(buffer, 1024);
                line = string(buffer);
                string model(line.substr(pos, line.size() - pos));

                memset(buffer, 0, 1024);
                inputFile.getline(buffer, 1024);
                line = string(buffer);
                pos = line.find_first_of('=') + 1;
                CTiffFile::EHWType hwType= CTiffFile::EHWType(stoul(line.substr(pos, line.size() - pos)));

                memset(buffer, 0, 1024);
                inputFile.getline(buffer, 1024);
                line = string(buffer);
                pos = line.find_first_of('=') + 1;
                resolution = CTiffFile::ESensorResolution(stoul(line.substr(pos, line.size() - pos)));

                memset(buffer, 0, 1024);
                inputFile.getline(buffer, 1024);
                line = string(buffer);
                pos = line.find_first_of('=') + 1;
                int temperature = int(stoul(line.substr(pos, line.size() - pos)));

                if (test == 0)
                {
                    //InuSensorDBTest(sensorID, resolution);
                }
                else // (testType == 1)
                {
                    CInuDevice  sensor(sensorID, model, hwType);
                    vector<int>  requiredTemp = { 3,25,45 };
                    InuSensorTest(sensor, resolution, temperature, requiredTemp);
                }
            }
            break;
            }
        }
    }
    else
    {
        cout << "Error need to implement input from stdin";
    }

    char ch;
    cout << "Enter any key\n";

    cin >> ch;
}
