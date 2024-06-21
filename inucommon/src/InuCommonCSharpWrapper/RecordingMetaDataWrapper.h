
#pragma once


using namespace System;
using namespace System::Collections::Generic;

namespace InuCommon 
{

    struct  CRecordingMetaData;


    ///////////////////////////////////////////////////////////////////////
    /// \brief RecordingMetaDataWrapper implementation class
    ///
    /// Role: Represents RecordingMetaDataWrapper
    ///
    /// Responsibilities: 
    ///      1. Implements the functions of RecordingMetaDataWrapper
    ///
    ///////////////////////////////////////////////////////////////////////
    public ref class RecordingMetaData
    {
    public:

        RecordingMetaData();

        property System::String^ SensorID;
        property System::String^ SensorName; 
        property System::String^ Date;
        property System::String^ Time;
        property System::String^ SWVersion;
        property System::String^ Registers;

        property unsigned int   FPS;
        property unsigned int   CalibrationRev;

        property array<int, 2>^ Gain;
        property array<int, 2>^ Exposure;


        property float FocalR_X;
        property float FocalR_Y;
        property float FocalL_X;
        property float FocalL_Y;
        property float Distance;
        property float CenterR_X;
        property float CenterR_Y;
        property float CenterL_X;
        property float CenterL_Y;
        property float EffectiveSize_X;
        property float EffectiveSize_Y;



        bool  Load(System::String^ iDirName, System::String^ iFileName);
        bool  Save(System::String^ iDirName, System::String^ iFileName);

    private:

        void CopyMembers(const CRecordingMetaData& input);

    };


}


