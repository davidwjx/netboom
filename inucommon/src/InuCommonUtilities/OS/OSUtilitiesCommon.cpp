//
// File : COsUtilities.cpp - Win32 and Linux Implementation 
//

#include "OSUtilities.h"

#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/tokenizer.hpp>
#include <boost/foreach.hpp>

using namespace InuCommon;

//#include <BoostFix/wait_fix.hpp>

#if  defined(__ANDROID__) || (defined(_MSC_VER) && ((_MSC_VER < 1800) || (_MSC_VER == 1900)))
#include <boost/thread/condition_variable.hpp>
#include <boost/atomic.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/thread.hpp>
using namespace boost;
#else
#include <atomic>
#include <condition_variable>
#include <mutex>
#include <thread>
#include <time.h>
#include <map>
using namespace std;
#endif


//Common OS code
#ifdef _DEBUG
const int COsUtilities::DEADLOCK_TIMEOUT = 10000;
#else
const int COsUtilities::DEADLOCK_TIMEOUT = 1000;
#endif

struct CInuEvent
{
    CInuEvent() : mIsSignaled(false) , mIsCanceled(false), mIsWaiting(false) {}
    atomic<bool> mIsSignaled;
    atomic<bool> mIsCanceled;
    atomic<bool> mIsWaiting;
    condition_variable mCondition;
    mutex mLock;
};

COsUtilities::EWaitResults COsUtilities::EventWait(void* hEvent, unsigned long timeout)
{
    EWaitResults ret(eEvent);

#if defined(__ANDROID__) || (defined(_MSC_VER) && (_MSC_VER < 1700))
    boost::chrono::system_clock::time_point chronoTimeout = boost::chrono::system_clock::now() + boost::chrono::milliseconds(timeout);
#else
    std::chrono::system_clock::time_point chronoTimeout = std::chrono::system_clock::now() + std::chrono::milliseconds(timeout);
#endif

    CInuEvent* inuEvent = (CInuEvent*)hEvent;
    unique_lock<mutex> wLock(inuEvent->mLock);

    inuEvent->mIsWaiting = true;

#ifndef _DEBUG
    // This code confirms that there is no change in system clock
    bool signalArrived = false;
    do 
    {
        signalArrived = inuEvent->mCondition.wait_until(wLock,chronoTimeout,
            [&inuEvent]()
        {
            //check conditions when signaled
            return (inuEvent->mIsCanceled || inuEvent->mIsSignaled);
        } );

        if (!signalArrived)
        {
#if defined(__ANDROID__) || (defined(_MSC_VER) && (_MSC_VER < 1700))
            auto timeAfter = boost::chrono::system_clock::now();
            if (boost::chrono::duration_cast<boost::chrono::milliseconds>(timeAfter - chronoTimeout).count() < std::max(timeout * 2 , timeout + 5000))
#else
            auto timeAfter = std::chrono::system_clock::now();
            if (std::chrono::duration_cast<std::chrono::milliseconds>(timeAfter - chronoTimeout).count() < std::max(timeout * 2, timeout + 5000))
#endif
            {
                // It's really a timeout and not chaning of the system clock
                break;
            }
        }


    } while (!signalArrived);

#else
    // During debug we might have breakpoints for a long time so we will not check a change in system clock
    bool signalArrived = inuEvent->mCondition.wait_until(wLock, chronoTimeout,
        [&inuEvent]()
        {
            //check conditions when signaled
            return (inuEvent->mIsCanceled || inuEvent->mIsSignaled);
        });
#endif


    //Lock continues after wait
    inuEvent->mIsWaiting = false;

    if (!signalArrived)
    {
        ret = eTimeout;
    }
    else if (inuEvent->mIsCanceled)
    {
        inuEvent->mIsCanceled = false;
        ret = eCancel;
    }
    else if (inuEvent->mIsSignaled)
    {
        inuEvent->mIsSignaled = false;
        ret = eEvent;
    }

    return ret;
}

void* COsUtilities::EventCreate(bool useManualReset)
{
    void* hEvent = new CInuEvent();
    return hEvent ;
}

bool COsUtilities::EventDestroy(void* hEvent)
{
    CInuEvent* inuEvent = (CInuEvent*)hEvent;
    EventCancel(hEvent);
    delete inuEvent;
    return true;
}

bool COsUtilities::EventSet(void* hEvent)
{
    if (hEvent == nullptr)
    {
        return false;
    }
    CInuEvent* inuEvent = (CInuEvent*)hEvent;
    unique_lock<mutex> lck(inuEvent->mLock);
    inuEvent->mIsSignaled = true;
    if (inuEvent->mIsWaiting == true)
    {
        inuEvent->mCondition.notify_all(); //only notify if there's someone waiting. (avoid internal exception when there are no waiters)
    }

    return true;
}

bool COsUtilities::EventReset(void* hEvent)
{
    if (hEvent == nullptr)
    {
        return false;
    }
    CInuEvent* inuEvent = (CInuEvent*)hEvent;
    unique_lock<mutex> lck(inuEvent->mLock);
    inuEvent->mIsSignaled = false;

    return true;
}

bool COsUtilities::EventCancel(void* hEvent)
{
    if (hEvent == nullptr)
    {
        return false;
    }
    CInuEvent* inuEvent = (CInuEvent*)hEvent;
    unique_lock<mutex> lck(inuEvent->mLock);
    inuEvent->mIsCanceled = true;
    if (inuEvent->mIsWaiting == true)
    {
        inuEvent->mCondition.notify_all(); //only notify if there's someone waiting. (avoid internal exception when there are no waiters)
    }

    return true;

}

bool COsUtilities::FileExists(const std::string& name)
{
    boost::system::error_code boostErr;
    return boost::filesystem::exists(InuCommon::COsUtilities::ConvertEncoding(name), boostErr) && (boostErr.value() == boost::system::errc::success);
}

void COsUtilities::CleanupFolder(const std::string& folder, const std::string& iContainsString, int iNumOfFilesToLeave)
{
    std::map<std::time_t, std::string> timeToFileMap;

    boost::filesystem::path dir(folder);
    if (!boost::filesystem::exists(dir))
    {
        boost::filesystem::create_directories(dir);
    }
    boost::filesystem::directory_iterator dirIter(dir), dirIterEnd;
    while (dirIter != dirIterEnd)
    {
        // List all the files by timestamp
        if (boost::filesystem::exists(*dirIter) && boost::filesystem::is_regular_file(dirIter->status()))
        {
            if (dirIter->path().filename().string().find(iContainsString) != std::string::npos)
            {
                std::time_t t = boost::filesystem::last_write_time(*dirIter);
                timeToFileMap[t] = dirIter->path().string();
            }
        }
        ++dirIter;
    }

     int i = 0;
    // loop in reverse order. from new files to old
    for (auto reverseIterator = timeToFileMap.rbegin(); reverseIterator != timeToFileMap.rend(); reverseIterator++)
    {
        i++;
        //the map is sorted by the time stamp. keep only 20 last files.
        if (i > iNumOfFilesToLeave)
        {
            boost::system::error_code boostErr;
            boost::filesystem::remove(reverseIterator->second, boostErr);
        }
    }
}

bool COsUtilities::ListAllFiles(const std::string& iDirName, const std::string& iExtension, RecordingFileList& oList, const std::string& iPrefix, bool iFullPath)
{
    bool foundFiles(false);

    boost::system::error_code boostErr;
    if (FileExists(iDirName) && 
        boost::filesystem::is_directory(InuCommon::COsUtilities::ConvertEncoding(iDirName), boostErr) &&
        (boostErr.value() == boost::system::errc::success) ) {
        boost::filesystem::directory_iterator end_iter;
        for ( boost::filesystem::directory_iterator dir_itr( iDirName ); dir_itr != end_iter; ++dir_itr )
        {
            if ( boost::filesystem::is_regular_file(*dir_itr, boostErr) && (boostErr.value() == boost::system::errc::success) )
            {
                std::string fileName(dir_itr->path().filename().string());

                if (!iPrefix.empty() && ( (fileName.size() < iPrefix.size()) || (fileName.substr(0, iPrefix.size()) != iPrefix) ) )
                {
                    continue;
                }
                if (fileName.substr(fileName.size() - iExtension.size(), iExtension.size()) != iExtension)
                {
                    continue;
                }
                if (iFullPath)
                {
                    fileName = iDirName + PATH_SEPARATOR + fileName;
                }

                //remove file extension
                size_t dot = fileName.find_last_of(".");
                std::string noExt = fileName.substr(0,dot);

                //remove second file extension(if exists)
                size_t dot2 = 0;
                dot2 = noExt.find_last_of(".");
                if (dot2 != 0)
                {
                    noExt = noExt.substr(0, dot2);
                }

                //the number is in the end of the file, so reverse and find where number ends
                std::string rev = std::string(noExt.rbegin(),noExt.rend());
                size_t firstLetter = rev.find_first_not_of("0123456789");

                std::string reversedNumber = rev.substr(0,firstLetter);

                uint64_t index = 0;
                try
                {
                    index = boost::lexical_cast<uint64_t>(std::string(reversedNumber.rbegin(), reversedNumber.rend()));
                }
                catch (...)
                {
                }

                FileIndexPair tmp = {fileName,index };

                foundFiles = true;
                oList.push_back( tmp );
            }
        }
        if (foundFiles)
        {
            sort(oList.begin(), oList.end(), FileIndexPair::TimeStampCompare);
        }
    }
       
    return foundFiles;
}

std::string  COsUtilities::GetCommonConfigPath()
{
    
    // Check in Inuitive path
    std::string tmp(COsUtilities::getenv(COsUtilities::INUITIVE_PATH));
    boost::filesystem::path inuitiveConfigPath(tmp);
    inuitiveConfigPath /= "config"; 
    
    boost::system::error_code boostErr;
    if (!tmp.empty() && boost::filesystem::exists(inuitiveConfigPath, boostErr) && (boostErr.value() == boost::system::errc::success))
    {
        return inuitiveConfigPath.string();
    }

    std::string installationPath;
    COsUtilities::GetInstallationPath(installationPath); 
    boost::filesystem::path devPath =installationPath;
    boost::filesystem::path configDevPath = devPath  / "config";
    if (boost::filesystem::exists(configDevPath, boostErr) && (boostErr.value() == boost::system::errc::success) )
    {
        return configDevPath.string();
    }

    return std::string("..//config");
}

std::string  COsUtilities::BuildFileName(
        const std::string& iInputFileName,
        const std::string& iExtension,
        uint64_t iIndex,
        const std::string& iFileNameSuffix/* = std::string("")*/
)
{
    std::string fileName(iInputFileName);
    std::string::size_type pos=iInputFileName.find_last_of('.');
    if ( (pos == std::string::npos) || (iInputFileName.substr(pos, iInputFileName.size() - pos) != iExtension) )
    {
        if (iIndex != std::numeric_limits<uint64_t>::max())
        {
            std::ostringstream  outputFileName;
            outputFileName << iInputFileName;
            if (iInputFileName[iInputFileName.size()-1] != '_')
            {
                outputFileName << "_"; 
            }
            outputFileName << iIndex;
            fileName = outputFileName.str();
        }
        if (!iFileNameSuffix.empty())
        {
            fileName += iFileNameSuffix;
        }
        if (fileName[fileName.size()-1] == '_')
        {
            fileName.resize(fileName.size()-1);
        }
        fileName += iExtension;    
    }
    return fileName;
}


int COsUtilities::VersionsStringCompare(const std::string& iVersionStrFirst, const std::string& iVersionStrSecond)
{
    std::vector<unsigned long> tokens1, tokens2;
    boost::char_separator<char> sep(".");

    boost::tokenizer<boost::char_separator<char>> tokensBoost1(iVersionStrFirst, sep);
    boost::tokenizer<boost::char_separator<char>> tokensBoost2(iVersionStrSecond, sep);

    BOOST_FOREACH(const std::string& t, tokensBoost1)
    {
        tokens1.push_back(std::stoul(t));
    }

    BOOST_FOREACH(const std::string& t, tokensBoost2)
    {
        tokens2.push_back(std::stoul(t));
    }

    auto iter1 = tokens1.begin();
    auto iter2 = tokens2.begin();
    for (; 	iter1 != tokens1.end() && iter2 != tokens2.end() ;)
    { 
        if (*iter1 > *iter2)
        {
            return -1;
        }
        if (*iter1 < *iter2)
        {
            return 1;
        }
        iter1++;
        iter2++;
    }

    if (iter1 != tokens1.end())
    {
        return -1;
    }

    if (iter2 != tokens2.end())
    {
        return 1;
    }

    return 0;
}
