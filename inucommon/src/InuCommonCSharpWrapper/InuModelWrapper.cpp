// This is the main DLL file.

#include <msclr\marshal_cppstd.h>
#include "Storage.h"
#include "InuModel.h"
#include "InuModelWrapper.h"
#include <codecvt>

using namespace InuCommon;

InuModelWrapper::InuModelWrapper()
{

}

bool InuModelWrapper::Init(System::String^ iSensorName, System::String^ iHWType)
{
    CInuModel sensorModel;
    try{

        msclr::interop::marshal_context context;
        std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> convert;
        std::string nativeName = convert.to_bytes(context.marshal_as<std::wstring>(iSensorName));
        std::string nativeHwType = convert.to_bytes(context.marshal_as<std::wstring>(iHWType));

        if (sensorModel.Init(nativeName, nativeHwType) != eOK)
        {
            // Failed to load model information
            return false;
        }

        return Init(sensorModel);
    }
    catch(...)
    {
        return false;
    }
}

bool InuModelWrapper::Init(const CInuModel& sensorModel)
{
    DefaultDepthFPS = float(sensorModel.GetDefaultFPS(DEFAULT_CHANNEL_ID));
	DefaultRGBFPS = float(sensorModel.GetDefaultFPS(DEFAULT_CHANNEL_ID));
	ModelName = gcnew System::String(sensorModel.GetModelName().c_str());
    HwType = gcnew System::String(sensorModel.GetHwTypeAsString().c_str());
    FWEnumerator = 0; //  sensorModel.GetFWEnumerator();
	DefaultDepthResolution = (ESensorResolution)sensorModel.GetDefaultResolution(DEFAULT_CHANNEL_ID);
	DefaultRGBResolution = (ESensorResolution)sensorModel.GetDefaultResolution(DEFAULT_CHANNEL_ID);
    DefaultAutoControl       = sensorModel.GetDefaultAutoControl();
	ImageStreamsChannels = gcnew System::String(sensorModel.GetImageStreamsChannels().c_str());

    ChannelsActiveSensors = gcnew Dictionary<System::String^, System::String^>();
    std::map<std::string, std::string> m = sensorModel.GetChannelsInterleaveMode();

    for (auto it = m.begin(); it != m.end(); it++)
    {
        ChannelsActiveSensors->Add(
                gcnew System::String(it->first.c_str()), 
                gcnew System::String(it->second.c_str())
            );
    }

    DepthCrop = System::Drawing::Size(0, 0);

    return true;
}
