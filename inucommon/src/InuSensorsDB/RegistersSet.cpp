#include "RegistersSet.h"

#include "IniFile.h"
#include "IregFile.h"

#include <boost/filesystem/path.hpp>
#include <boost/filesystem.hpp>
#include <sstream>

using namespace InuCommon;
using namespace std;

std::string CRegistersSet::GetRegistersDataAsString() const
{
    ostringstream result;
    for (const auto& elem : mRegistersData)
    {
        result << elem;
    }
    return result.str();
}

InuCommon::CRegistersSet* InuCommon::CRegistersSet::CreateObject(InuCommon::CTiffFile::EHWType iHwType)
{
    if (iHwType == InuCommon::CTiffFile::eNU4000C0 || iHwType == InuCommon::CTiffFile::eNU4000C1 || iHwType == InuCommon::CTiffFile::eNU4100A0 )
    {
        return new CRegistersSet4000();
    }
    return nullptr;
}
CRegistersSet* InuCommon::CRegistersSet::Clone() const
{
    if (dynamic_cast<const CRegistersSet4000*>(reinterpret_cast<const CRegistersSet*>(this)) != nullptr)
    {
        return new CRegistersSet4000(*dynamic_cast<const CRegistersSet4000*>(reinterpret_cast<const CRegistersSet*>(this)));
    }
    return nullptr;
}

EInuSensorsDBErrors CRegistersSet::Save(const boost::filesystem::path& iCalibrationSetDir)
{
    // CIregFile iregFile;
    boost::filesystem::path resolutionPath(iCalibrationSetDir);
    resolutionPath /= "config.ireg";
    unsigned int imageFormat = 1;
    CIregFile::Write(mRegistersData, resolutionPath.string(), mVideoSize, mDepthSize, imageFormat, mShiftY);
    return eOK;
}

EInuSensorsDBErrors CRegistersSet::LoadRegisterSetFile(const boost::filesystem::path& iFileName, std::vector<CRegisterData>& oRegistersData)
{
    EInuSensorsDBErrors rc = eOK;

    std::string configFileName;
    // Used to load external ireg files, LoadPreset from InuSpector for example.
    if (boost::filesystem::is_regular_file(iFileName))
    {
        unsigned int imageFormat(0), shiftY(0);
        std::string  configIrgVer;
        std::pair<unsigned int, unsigned int> webcamSize, depthSize, videoSize;
        if (CIregFile::Read(oRegistersData, iFileName.string(), videoSize, depthSize, webcamSize, imageFormat, shiftY, configIrgVer, false) != CIregFile::eOK)
        {
            return eFailedToReadConfigIreg;
        }
    }

    return rc;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                      CRegistersSet4000

EInuSensorsDBErrors CRegistersSet4000::Load(const boost::filesystem::path& iCalibrationSetDir, const std::string& iVersion)
{
    CIniFile file;
    // Currently it's hard coded for 4000, should be replaced in the future
    mShiftY = 2;

    if (iVersion >= "4.06" || iVersion.empty())
    {
        // In the new calibration version, the size are read from FW and not from this ini file
        // Empty iVersion can be generated only by manual calibration created during the integration
        return eOK;
    }

    if (!file.Load((iCalibrationSetDir / "nu4k_calibration_params.ini").string()))
    {
        std::string configVer;
        unsigned int imageFormat;
        if (CIregFile::Read(mRegistersData, (iCalibrationSetDir / "config.ireg").string(),
            mVideoSize, mDepthSize, mWebcamSize,
            imageFormat, mShiftY,
            configVer, false, std::numeric_limits<unsigned int>::max())
            != CIregFile::eOK)
        {
            // This is special case for calibration workflow, the registers and image size is still read from config.ireg
            return eRegistersFileNotFound;
        }
        return eOK;
    }

    unsigned int width, height;
    int numberOfElements = 1;
    if (!file.GetValue("LeftSensorParameters.LeftDsrOutputHorizontalSize", &width, numberOfElements) )
    {
        return eMissingKeyInRegistersFile;
    }

    mVideoSize.first = width;
    mDepthSize.first = width;

    if (!file.GetValue("LeftSensorParameters.LeftDsrOutputVerticalSize", &height, numberOfElements))
    {
        return eMissingKeyInRegistersFile;
    }

    mVideoSize.second = height;
    mDepthSize.second = height;

    return eOK;
}

