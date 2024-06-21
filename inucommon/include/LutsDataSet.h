
#ifndef __LUTSDATASET_H__
#define __LUTSDATASET_H__

#include "InuSensorDBDefs.h"
#include "TiffFile.h"


#include <string>
#include <utility>
#include <vector>

namespace boost { namespace filesystem { class path; } }

namespace InuCommon
{
    ////////////////////////////////////////////////////////////////////////
    /// \brief    All LUTs of one calibration set  
    ///
    /// Role: Represents all LUTs that should be loaded to chip
    ///
    /// Responsibilities: 
    ///      1. Knows how to load/save from LUT files
    ///      2. Access to all LUTs
    ////////////////////////////////////////////////////////////////////////
    class INUSENSORSDB_API CLutsDataSet 
    {
    public:

        /// \brief  Different types of LUT
        enum ELutType
        {
            eIntensityBalance = 0,
            eDistortionAndRectification,
            eBPC,
            eFIR,
            eNumberOfTypes
        };
        
        CLutsDataSet() {}
        virtual ~CLutsDataSet() {}

        /// \brief    brief Get calibration LUT, pair of binary buffer and its size.
        /// 
        /// If LUT has not been loaded yet from DB, then it loads it and returns the loaded data 
        /// \param[in] iType    LUT type.
        /// \param[in] iLeftEye    Which eye (left/right).
        /// \param[in] iLutIndex   The Lut index.
        /// \return const std::pair<unsigned char*, unsigned int>&    If it fails then LUT size is 0.
        const std::vector<uint8_t>& GetLUT(ELutType iType, uint32_t iSensorNumber, uint32_t iLutIndex = 0) const;
       
        EInuSensorsDBErrors Load(const boost::filesystem::path& iCalibrationSetDir, InuCommon::CTiffFile::EHWType iHwType);

        EInuSensorsDBErrors Save(const boost::filesystem::path& iCalibrationSetDir);

    private:


        /// \brief  All different types of LUTS. Value is null and size is 0, if LUT has not been loaded yet.
#ifdef _MSC_VER
#pragma warning(disable : 4251)
#endif
        struct CLutData {
            CLutData(ELutType iType, uint32_t iSensorNumber, uint32_t iIndex) : Type(iType), Index(iIndex), SensorNumber(iSensorNumber) {}
            bool operator==(const CLutData& input) const {
                return int(Type) == int(input.Type) && Index == input.Index && SensorNumber == input.SensorNumber; 
            }
            //bool operator<(const CLutData& input) const {
            //    return int(Type) < int(input.Type) && Index < input.Index && SensorNumber < input.SensorNumber;
            //}
            ELutType Type;
            uint32_t Index;
            uint32_t SensorNumber;
            std::vector<uint8_t> Data;
        };
        std::vector<CLutData>  mLuts;

        static std::map<ELutType, std::string> sLutNames ;

#ifdef _MSC_VER
#pragma warning(default : 4251)
#endif

        EInuSensorsDBErrors LoadLUT(const std::string& iCalibrationSetDir, CLutData& iLutData);
    };

    inline const std::vector<uint8_t>& CLutsDataSet::GetLUT(ELutType iType, unsigned int iSensorNumber, uint32_t iLutIndex) const {
        for (const auto& lut : mLuts)
        {
            if (CLutData(iType, iSensorNumber, iLutIndex) == lut) {
                return lut.Data;
            }
        }
        throw("missing LUT: " + std::to_string(iType) + " " + std::to_string(iSensorNumber) + " " + std::to_string(iLutIndex));
    }

}
#endif
