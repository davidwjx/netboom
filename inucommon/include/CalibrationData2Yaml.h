#ifndef __CALIBRATIONDATA2YAML_H__
#define __CALIBRATIONDATA2YAML_H__

#include "CommonUtilitiesDefs.h"
#include "CalibrationData.h"

namespace YAML { class Node;  }

namespace InuCommon
{
    ////////////////////////////////////////////////////////////////////////
    /// \brief    Persistency of CCalibrationData (Utilities class)  
    ///
    /// Role: Implementation of persistent behavior of Optical Data. Inherits COpticalData
    ///
    /// Responsibilities: 
    ///      1. Knows how to load/save from files
    ////////////////////////////////////////////////////////////////////////
    class COMMONUTILITIES_API CCalibrationData2Yaml
    {
    public:


        /// \brief  Load optical data from stream
        /// \brief  Param[In]: iStream - input stream
        static bool Load(CCalibrationData& oCalibration, std::istream& iStream, bool iFromPython = false);

        /// \brief  Save optical data to output stream system
        static bool Save(const CCalibrationData& iCalibration, std::ostream& oStream);

    private:
       
        // iFromPython = file that was created by Python version of calibration
        static bool LoadYaml(CCalibrationData& oCalibration, YAML::Node &ymlFile, bool iFromPython);

        static bool ReadBasline(const  YAML::Node &elem, CCalibrationData &oCalibration);

        // load camera data from storage
        static bool ReadCameraFromYaml(const YAML::Node& iCameraNode, CCalibrationData::COneSensorData::CCameraData &oCameraData);

    };
}

#endif
