#include "ResolutionDataSet.h"
#include "Logger.h"
#include "OSUtilities.h"
#include "CalibrationDataImp.h"


#include <boost/filesystem/path.hpp>
#include <boost/filesystem.hpp>

#include <fstream>

using namespace InuCommon;
using namespace std;


InuCommon::CResolutionDataSet::CResolutionDataSet(int iActualTemperature,const string& iResolutionName) :
    mCalibrationData(new CCalibrationDataImp()),
    mRegistersSet(nullptr),
    mCalibrationDirectory(),
    mActualTemperature(iActualTemperature),
    mResolutionName(iResolutionName)
{
}

std::shared_ptr<CCalibrationData> CResolutionDataSet::GetOpticalDataAndUpdateSensorsID(
    const std::pair<int, int>& iStereoChannels,
    const std::pair<int, int>& iFeChannels,
    const int iRgbChannel)
{
    // Backward compatibility: NU4000B0 with ImageParameters.ini, should be removed someday
#ifdef __ANDROID__
	// Bug in llvm - dynmic cast cause to crash
	reinterpret_cast<CCalibrationDataImp*>(mCalibrationData.get())->UpdateSensorsID(iStereoChannels, iFeChannels, iRgbChannel);
#else
    dynamic_pointer_cast<CCalibrationDataImp>(mCalibrationData)->UpdateSensorsID(iStereoChannels, iFeChannels, iRgbChannel);
#endif

    return mCalibrationData;
}

InuCommon::CResolutionDataSet::CResolutionDataSet(const CResolutionDataSet& iInput) :
    mCalibrationData(new CCalibrationDataImp(*dynamic_pointer_cast<CCalibrationDataImp>(iInput.mCalibrationData))),
    mRegistersSet(iInput.mRegistersSet->Clone()),
    mCalibrationDirectory(iInput.mCalibrationDirectory),
    mLutsDataSet(iInput.mLutsDataSet),
    mResolutionName(iInput.mResolutionName)
{
}

EInuSensorsDBErrors CResolutionDataSet::Load(const boost::filesystem::path& iCalibrationSetDir, InuCommon::CTiffFile::EHWType iHwType, bool iLoadOdpReg, const CCalibrationData* originalOptical)
{

    EInuSensorsDBErrors rc = reinterpret_cast<CCalibrationDataImp*>(mCalibrationData.get())->Load(iCalibrationSetDir, iHwType, originalOptical);

    // For modules flashed with bypass & without optical data(QBX)
    if (rc == eOpticalDataFileNotFound)
    {
        InuCommon::CLogger::Write("Optical data not found - trying to run Bypass mode", InuCommon::CLogger::eError, "OpticalData");
        rc = eOK;
    }

    mCalibrationDirectory = iCalibrationSetDir.string();

    if (rc == eOK)
    {
        mRegistersSet = unique_ptr<CRegistersSet>(CRegistersSet::CreateObject(iHwType));
        rc = mRegistersSet->Load(iCalibrationSetDir, mCalibrationData->Version);
        if (rc != eOK)
        {
            return eRegistersFileNotFound;
        }
        if (rc == eOK)
        {
            rc = mLutsDataSet.Load(iCalibrationSetDir, iHwType);
        }
    }    

    return rc;
}

EInuSensorsDBErrors CResolutionDataSet::SaveConfigIregAndOpticalData(const boost::filesystem::path& iCalibrationSetDir)
{
    EInuSensorsDBErrors rc = eOK;
    boost::filesystem::path resolutionPath(iCalibrationSetDir);
    resolutionPath /= GetResolutionName();
    resolutionPath /= "HW";
    rc = mRegistersSet.get()->Save(resolutionPath);
    if (rc != eOK)
    {
        return rc;
    }
    rc = reinterpret_cast<CCalibrationDataImp*>(mCalibrationData.get())->Save(resolutionPath);

    return rc;
}


EInuSensorsDBErrors CResolutionDataSet::Save(const boost::filesystem::path& iCalibrationSetDir)
{
    EInuSensorsDBErrors rc = eOK;

    boost::filesystem::path resolutionPath(iCalibrationSetDir);
    resolutionPath /= GetResolutionName();

    boost::system::error_code boostErr;
    if (COsUtilities::FileExists(resolutionPath.string()) != true)
    {
        boost::filesystem::remove_all(resolutionPath, boostErr);
        if (boostErr.value() != boost::system::errc::success)
        {
            return eFailedToCreateFolder;
        }
    }

    if (!boost::filesystem::create_directory(resolutionPath, boostErr) || (boostErr.value() != boost::system::errc::success))
    {
        return eFailedToCreateFolder;
    }

    resolutionPath /= "HW";
    if (!boost::filesystem::create_directory(resolutionPath, boostErr) || (boostErr.value() != boost::system::errc::success))
    {
        return eFailedToCreateFolder;
    }
    rc = reinterpret_cast<CCalibrationDataImp*>(mCalibrationData.get())->Save(resolutionPath);
    if (rc != eOK)
    {
        return rc;
    }
    rc = mRegistersSet.get()->Save(resolutionPath);
    if (rc != eOK)
    {
        return rc;
    }
    rc = mLutsDataSet.Save(resolutionPath);
    if (rc != eOK)
    {
        return rc;
    }
    
    return eOK;
}

