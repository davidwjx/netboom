
#ifndef __MISSINGCALIBRATIONSALG_H__
#define __MISSINGCALIBRATIONSALG_H__

#include "InuSensorDBDefs.h"
#include "TemperatureDataSet.h"
#include "CalibrationDataAll.h"

#include <vector>
#include <thread>

namespace InuCommon
{

    ////////////////////////////////////////////////////////////////////////
    /// \brief    One calibration data of InuSensor
    ///
    /// Role: Represents one calibration data (there might be different calibrations per InuSensor)
    ///
    /// Responsibilities: 
    ///      1. Knows how to load/save from calibration folder 
    ///      2. Access to calibration data members (Optical data, LUTs and Initials registers values)
    ////////////////////////////////////////////////////////////////////////
    class CMissingCalibrationsAlg
    {

    public:

        CMissingCalibrationsAlg(std::shared_ptr<CCalibrationDataAll> iCalibrationData, const std::vector<int>& iRequiredTemperatures, int iSaturationMin, int iSaturationMax);

        virtual ~CMissingCalibrationsAlg();

        /// \brief  Load calibration data from file system
        EInuSensorsDBErrors Calculate();

        void InterpulationCalculation(int32_t iCalibCold, int32_t iCalibWarm,
            float iParamsCalibCold, float iParamsCalibWarm, float& ioParamsCalibNew, int32_t ioNewTemperature) const;
        
    private:

        static const std::string DSR_FOLDER_NAME;
        static const std::string DSR_FILE_NAME;
        static const std::string FOLDER_INITIAL;
        static const std::string TEMPORARY_CALCULATED_FOLDER;
        static const std::string OVERWRITTEN_FOLDER;

        /// \brief  Original cold calibration data set
        std::weak_ptr<CCalibrationDataAll> mCalibationData;
        
        std::vector<int> mRequiredTemperatures;

        std::vector<int> mExistingTemperatures;

        /// \brief  InuCalibration version from which DSR implementation was changed to MATLAB as received from calibration team
        static const std::string DSR_MATLAB_VERSION;

        /// \brief  Original cold calibration data set
        int mColdCalibration;

        /// \brief  Original warm calibration data set
        int mWarmCalibration;

        /// \brief  Minimal saturation temp for extrapulation
        int mMinSaturation;

        /// \brief  Maximal saturation temp for extrapulation
        int mMaxSaturation;

        std::thread mCalculationThread;

        bool mStopCalcuationThread;

        void CalculationThreadFunc();

        EInuSensorsDBErrors Interpulation(std::shared_ptr<CTemperatureDataSet> ioTemperatureDataSet) const;

        EInuSensorsDBErrors CalculateDataset(std::shared_ptr<CTemperatureDataSet> ioTemperatureDataSet, const boost::filesystem::path& iYamlFile) const;

        EInuSensorsDBErrors SaveConfigIregAndOpticalData(std::shared_ptr<CTemperatureDataSet> iNewCalibration) const;

        EInuSensorsDBErrors CreateTemporaryFolderForDSR(std::shared_ptr<CTemperatureDataSet> iCalibration) const;
        EInuSensorsDBErrors RenameTempFolderByTemperature(std::shared_ptr<CTemperatureDataSet> iCalibration, const boost::filesystem::path& iOriginalPath) const;

        EInuSensorsDBErrors RunDSROverOriginalData(std::shared_ptr<CTemperatureDataSet>& ioCalibration) const;

        std::string IntVersionToString(uint32_t iVersion) const;

        EInuSensorsDBErrors SaveToDiskAndAddCalibration(const std::shared_ptr<CTemperatureDataSet>& iCalib) const;

        EInuSensorsDBErrors RunDSROverOriginalData(
            std::shared_ptr<CTemperatureDataSet> ioOriginalData, 
            std::shared_ptr<CCalibrationDataAll> iCalibationDataShared,
            const std::map<int, std::shared_ptr<CTemperatureDataSet>>& calibrationDataSets,
            bool iColdTemperature);

    };
}
#endif
