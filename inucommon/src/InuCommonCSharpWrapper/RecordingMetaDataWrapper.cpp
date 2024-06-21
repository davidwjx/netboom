// This is the main DLL file.

#include <msclr/marshal_cppstd.h>

#include "RecordingMetaDataWrapper.h"
#include "RecordingMetaData.h"
#include "Storage.h"


using namespace InuCommon;

RecordingMetaData::RecordingMetaData()
{
    CRecordingMetaData metaDataC;
    CopyMembers(metaDataC);
}

bool  RecordingMetaData::Load(System::String^ iDirName, System::String^ iFileName)
{
    CRecordingMetaData metaDataC;


    msclr::interop::marshal_context context;
    std::string nativeFileName = context.marshal_as<std::string>(iFileName);
    std::string nativeDirName = context.marshal_as<std::string>(iDirName);

    if (metaDataC.Load(IStorage::CreateStorage(IStorage::eXML, nativeDirName, nativeFileName)))
    {
        CopyMembers(metaDataC);
    }

    return true;
}


bool  RecordingMetaData::Save(System::String^ iDirName, System::String^ iFileName)
{
    CRecordingMetaData metaDataC;
    msclr::interop::marshal_context context;
    std::string nativeFileName = context.marshal_as<std::string>(iFileName);
    std::string nativeDirName = context.marshal_as<std::string>(iDirName);

    metaDataC.Save(InuCommon::IStorage::CreateStorage(InuCommon::IStorage::eXML, nativeDirName, nativeFileName));

    return true;

}

void RecordingMetaData::CopyMembers(const CRecordingMetaData& input)
{
    SensorID = gcnew System::String(input.mSensorID.c_str());
    SensorName = gcnew System::String(input.mSensorName.c_str());
    Date = gcnew System::String(input.mDate.c_str());
    Time = gcnew System::String(input.mTime.c_str());
    SWVersion = gcnew System::String(input.mSWVersion.c_str());
    Registers = gcnew System::String(input.mRegisters.c_str());


    FPS = input.mFPS;
    CalibrationRev=input.mCalibrationRev;

    if (input.mOpticalData.Sensors.size() > 2)
    {
        // Support only NU3000, need to update for NU4000B0
        FocalR_X = float(input.mOpticalData.Sensors.at(0).VirtualCamera.Intrinsic.FocalLength[0]);
        FocalR_Y = float(input.mOpticalData.Sensors.at(0).VirtualCamera.Intrinsic.FocalLength[1]);
        FocalL_X = float(input.mOpticalData.Sensors.at(1).VirtualCamera.Intrinsic.FocalLength[0]);
        FocalL_Y = float(input.mOpticalData.Sensors.at(1).VirtualCamera.Intrinsic.FocalLength[0]);
        Distance = float(input.mOpticalData.Sensors.at(1).VirtualCamera.Extrinsic.Translation[0]);

        CenterR_X = float(input.mOpticalData.Sensors.at(0).VirtualCamera.Intrinsic.OpticalCenter[0]);
        CenterR_Y = float(input.mOpticalData.Sensors.at(0).VirtualCamera.Intrinsic.OpticalCenter[1]);

        CenterL_X = float(input.mOpticalData.Sensors.at(1).VirtualCamera.Intrinsic.OpticalCenter[0]);
        CenterL_Y = float(input.mOpticalData.Sensors.at(1).VirtualCamera.Intrinsic.OpticalCenter[1]);
    }
}



















