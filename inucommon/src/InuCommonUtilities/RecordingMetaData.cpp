
#include  "OSUtilitiesGCC.h"
#include "RecordingMetaData.h"
#include "Storage.h"

//#include <boost/date_time.hpp>
#include "boost/date_time/posix_time/posix_time_types.hpp"
#include <iomanip>

using namespace InuCommon;
using namespace std;

bool  CRecordingMetaData::Load(const std::shared_ptr<InuCommon::IStorage>& storage)
{
    bool ret = storage->SelectTable("General");
    if (ret)
    {
        ret = ret && storage->GetValue<string>(mSensorID, string("SensorID"));
        ret = ret && storage->GetValue<string>(mSensorName, string("SensorName"));
        ret = ret && storage->GetValue<string>(mDate, string("Date"));
        ret = ret && storage->GetValue<string>(mTime, string("Time"));
        ret = ret && storage->GetValue<string>(mSWVersion, string("SWVersion"));
        ret = ret && storage->GetValue<unsigned int>(mFPS, string("FPS"));
    }

    if (ret)
    {
        ret = storage->SelectTable("Calibration");
        if (ret)
        {
            ret = ret && storage->GetValue<unsigned int>(mCalibrationRev, string("Revision"));
            string tmp;
            ret = ret && storage->GetValue<string>(tmp, string("Resolution"));

            mResoltion = (tmp == "Binning" ? CTiffFile::eBinning :  tmp == "Full" ? CTiffFile::eFull :  tmp ==  "Alternate"?  CTiffFile::eAlternate : CTiffFile::eVerticalBinning);

        }
    }

    if (ret)
    {
        ret = storage->SelectTable("OpticalData");
        if (ret)
        {
            ret = storage->GetValue<string>(mOpticalDataAsString, string("DeviceData"));
        }
    }

    if (ret)
    {
        ret = storage->SelectTable("Registers");
        if (ret)
        {
            ret = ret && storage->GetValue<string>(mRegisters, string("ConfigIreg"));
        }
    }
    return ret;
}

bool  CRecordingMetaData::Save(const std::shared_ptr<InuCommon::IStorage>& storage) 
{
    // Define time and date
    namespace pt = boost::posix_time;
    pt::ptime now = pt::second_clock::local_time();

    std::ostringstream strStream;
    strStream.fill('0');
    strStream << setw(2) << now.date().day() << "/" << setw(2) << static_cast<int>(now.date().month()) << "/" << setw(2) << now.date().year();
    mDate = strStream.str();

    strStream.clear();
    strStream.fill('0');
    strStream << setw(2) << now.time_of_day().hours() << ":" << setw(2) << now.time_of_day().minutes() << ":" << setw(2) << now.time_of_day().seconds();
    mTime = strStream.str();


    bool ret = storage->SelectTable("General");
    if (!ret)
    {
        ret = storage->AddTable("General");
        ret = ret && storage->SelectTable("General");
    }

    if (ret)
    {
        ret = ret && storage->SetValue<string>(mSensorID, string("SensorID"));
        ret = ret && storage->SetValue<string>(mSensorName, string("SensorName"));
        ret = ret && storage->SetValue<string>(mDate, string("Date"));
        ret = ret && storage->SetValue<string>(mTime, string("Time"));
        ret = ret && storage->SetValue<string>(mSWVersion, string("SWVersion"));
        ret = ret && storage->SetValue<unsigned int>(mFPS, string("FPS"));
    }

    if (ret)
    {
        ret = storage->SelectTable("Calibration");
        if (!ret)
        {
            ret = storage->AddTable("Calibration");
            ret = ret && storage->SelectTable("Calibration");
        }
        if (ret)
        {
            ret = ret && storage->SetValue<unsigned int>(mCalibrationRev, string("Revision"));
            string tmp = (mResoltion == CTiffFile::eBinning ? "Binning" : mResoltion == CTiffFile::eFull ? "Full" : mResoltion == CTiffFile::eAlternate ?  "Alternate" : "VerticalBinning");
            ret = ret && storage->SetValue<string>(tmp, string("Resolution"));
        }
    }

    if (ret)
    {
        ret = storage->SelectTable("OpticalData");
        if (!ret)
        {
            ret = storage->AddTable("OpticalData");
            ret = ret && storage->SelectTable("OpticalData");
        }
        if (ret)
        {
            ret = ret && storage->SetValue<string>(mOpticalDataAsString, string("DeviceData"));
        }
    }

    if (ret)
    {
        ret = storage->SelectTable("Registers");
        if (!ret)
        {
            ret = storage->AddTable("Registers");
            ret = ret && storage->SelectTable("Registers");
        }
        if (ret)
        {
            ret = ret && storage->SetValue<string>(mRegisters, string("ConfigIreg"));
    }
    }

    return storage->Commit();
}


