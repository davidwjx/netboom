#include "LutsDataSet.h"
#include "OSUtilities.h"

#include <boost/filesystem/path.hpp>

#include <fstream>

#include <boost/tokenizer.hpp>
#include <boost/assign/list_of.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/foreach.hpp>
#include "boost/lexical_cast.hpp"
#include <boost/filesystem/operations.hpp>


using namespace InuCommon;
using namespace std;

std::map<CLutsDataSet::ELutType, std::string> CLutsDataSet::sLutNames = {
    {eIntensityBalance, "_ib"},
    {eDistortionAndRectification, "_dsr"},
    {eBPC, "_bpc"},
    {eFIR, "_fir"} };

EInuSensorsDBErrors CLutsDataSet::Load(const boost::filesystem::path& iCalibrationSetDir, InuCommon::CTiffFile::EHWType iHwType)
{
    vector<boost::filesystem::directory_entry> allFiles;
    copy(boost::filesystem::directory_iterator(iCalibrationSetDir.string()), boost::filesystem::directory_iterator(), back_inserter(allFiles));
    boost::system::error_code boostErr;
    for (const auto& binaryFile : allFiles)
    {
        if ( boost::filesystem::is_regular_file(binaryFile, boostErr) &&
            (boost::system::errc::success == boostErr.value()))
        {
            if (binaryFile.path().extension() == ".bin")
            {
                auto tmp = binaryFile.path();
                std::string name = tmp.leaf().replace_extension().string();
                auto pos = name.find_first_of('_');
                if (pos != std::string::npos)
                {
                    uint32_t sensorNumber(name[pos + 1] - '0');
                    if (name[pos + 2] == '_')
                    {
                        uint32_t fileIndex(name[pos + 3] - '0');
                        if (name[pos + 4] == '_')
                        {
                            std::string lutTypeStr = name.substr(pos + 4, name.size() - 1);
                            for (auto iType : sLutNames)
                            {
                                if (iType.second == lutTypeStr)
                                {
                                    mLuts.push_back(CLutData(iType.first, sensorNumber, fileIndex));
                                    LoadLUT(binaryFile.path().string(), mLuts[mLuts.size()-1]);
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    return eOK;
}

EInuSensorsDBErrors CLutsDataSet::LoadLUT(const std::string& iFileName, CLutData& iLutData)
{
    ifstream  lutFile(InuCommon::COsUtilities::ConvertEncoding(iFileName), ios_base::binary);
    if (lutFile.good())
    {
        boost::system::error_code ec;
        uintmax_t fileSize = boost::filesystem::file_size(InuCommon::COsUtilities::ConvertEncoding(iFileName), ec);

        if ((ec.value() == boost::system::errc::success) && (fileSize > 0))
        {
            iLutData.Data.resize(fileSize);

            lutFile.read((char*)(&(iLutData.Data[0])), fileSize);
            if (lutFile.fail())
            {
                iLutData.Data.clear();
            }
        }
    }

    return eOK;
}

EInuSensorsDBErrors CLutsDataSet::Save(const boost::filesystem::path& iCalibrationSetDir)
{
    std::vector<uint8_t>  tempLut;
    for (auto& lut : mLuts)
    {
        std::string lutFileName = "sensor_" + std::to_string(lut.SensorNumber) +
            "_ " + std::to_string(lut.SensorNumber) +
            "_ " + sLutNames.at(lut.Type) + ".bin";

            ofstream output_file(lutFileName, ios_base::binary);
            output_file.write((char*)(&(lut.Data[0])), lut.Data.size());
            output_file.close();
    }

    return eOK;
}

