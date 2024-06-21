#ifndef __CALIBRAIONDATAIMP_H__
#define __CALIBRAIONDATAIMP_H__

#include "InuSensorDBDefs.h"
#include "CalibrationData.h"
#include "TiffFile.h"

#ifdef _MSC_VER
#pragma warning(disable : 4275)
#pragma warning(disable : 4251)
#endif

namespace boost { namespace filesystem { class path; } }
namespace YAML { class Node;  }

namespace InuCommon
{
    // It is not defined in CCalibrationDataImp because of strange compilation error in gcc 4.9 
    static const int DEFAULT_IR_RIGHT = 0;
    static const int DEFAULT_IR_LEFT = 1;
    static const int DEFAULT_RGB = 2;
    static const int DEFAULT_FE_RIGHT = 3;
    static const int DEFAULT_FE_LEFT = 4;

    ////////////////////////////////////////////////////////////////////////
    /// \brief    Persistency of Optical Data   
    ///
    /// Role: Implementation of persistent behavior of Optical Data. Inherits COpticalData
    ///
    /// Responsibilities: 
    ///      1. Knows how to load/save from files
    ////////////////////////////////////////////////////////////////////////
    class COMMONUTILITIES_API CCalibrationDataImp : public CCalibrationData
    {
    public:

        // These are the sensors IDs of NU4000B0 as written in the optical data file (ImageParameters.ini) 


        CCalibrationDataImp();
        CCalibrationDataImp(const CCalibrationData& input) : CCalibrationData(input) {}
        virtual ~CCalibrationDataImp() {}

        /// \brief  Load optical data from file system
        /// \brief  Param[In]: iKeepImuData - Saves IMU data from original Optical Data
        EInuSensorsDBErrors Load(const boost::filesystem::path& iCalibrationSetDir, InuCommon::CTiffFile::EHWType iHwType, const CCalibrationData* iOrigCalibrationData, bool iFromPython = false);

        /// \brief  Save optical data to file system
        EInuSensorsDBErrors Save(const boost::filesystem::path& iCalibrationSetDir) const;

        /// \brief  Update the sensors IDs of the optical data according to current IDs of connected sensor
        ///
        ///         Backward compatibility: NU4000B0 with old optical data file (ImageParameters.ini), should be removed someday
        /// \brief  Param[In]: iStereoChannels - IR right and left sensors ID (of connected model)
        /// \brief  Param[In]: iFeChannels - FE right and left sensors ID (of connected model)
        /// \brief  Param[In]: iRgbChannel - RGB sensors ID (of connected model)
        void UpdateSensorsID(
            const std::pair<int, int>& iStereoChannels,
            const std::pair<int, int>& iFeChannels,
            const int iRgbChannel);

        // load optical data from storage (iFromPython = file that was created by Python version of calibration)
        EInuSensorsDBErrors LoadFromYaml(const boost::filesystem::path& iFileName, bool iFromPython = false);

    private:
       
        static const std::string OPTICAL_DATA_FILE_NAME;

        EInuSensorsDBErrors Load(const boost::filesystem::path& iCalibrationSetDir);

        // New or old format of optical data file
        bool mOldFormatNU4000B0;
    };
}

#ifdef _MSC_VER
#pragma warning(default : 4275)
#pragma warning(default : 4251)
#endif

#endif
