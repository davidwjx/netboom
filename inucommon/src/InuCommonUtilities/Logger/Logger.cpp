
#include "OSUtilities.h"
#include "Logger.h"
#include "DebugViewLogger.h"
#include "TextFileLogger.h"
#include "ConsoleLogger.h"
#include "Version.h"

#include <boost/filesystem.hpp>

#if (!defined(_MSC_VER) || (_MSC_VER > 1600))
#include <chrono>
#endif
 
using namespace InuCommon;


//static LogInitializer InitializeLog;

// The static logger instance must NOT be lazy initiated. This guarantees that it will be created when the binary loads.
// Order of destruction is the inverse order of creation, so if it's created first, it's destructed last.
// (And lets other classes use the log in the static destruction)
// http://en.wikibooks.org/wiki/More_C%2B%2B_Idioms/Nifty_Counter

std::shared_ptr<CLogger>	CLogger::sTheInstance;
BLock CLogger::sConstructionLocker;
bool CLogger::sDestructorInvoked;
std::map<std::string,CLogger::ESeverity> CLogger::sSeverity;
const std::string CLogger::SEVERITY_DEFAULT_PREFIX("_DefaultPrefix_");

int CLogger::sLogType(CLogger::eAuto);

const int MAX_LOG_QUEUE_SIZE = 1000;

void CLogger::CreateTheInstance(int iLogType) 
{
    WriteLock wLock(sConstructionLocker);

    sDestructorInvoked = false;

    // Check again (after lock) that the object has not been created yet.
    if (sTheInstance == nullptr) 
    {
        SetSeverity(SEVERITY_DEFAULT_PREFIX, CLogger::eError);
        sTheInstance = std::shared_ptr<CLogger>(new CLogger());
        sTheInstance->Init(iLogType);
    }
}

bool CLogger::Init(int iLogType) 
{
    if (iLogType == eAuto)
    {
#if _MSC_VER
        if (InuCommon::COsUtilities::IsProcessRunning(std::string("Dbgview.exe")) || 
            InuCommon::COsUtilities::IsProcessRunning(std::string("DebugView++.exe")) || 
            IsDebuggerPresent() )
        {
            sLogType = eDebugView;
        }
        else
        {
            sLogType = eFile;
        }
#else
        sLogType = eDebugView;
#endif
    }
    else
    {
        sLogType = iLogType;
    }

#if _MSC_VER
    if (sLogType & eDebugView)
    {
        if (!InuCommon::COsUtilities::IsProcessRunning(std::string("Dbgview.exe")) &&
            !InuCommon::COsUtilities::IsProcessRunning(std::string("DebugView++.exe"))&&
            !IsDebuggerPresent() )
        {
            sLogType = eFile | sLogType;
        }
    }
#endif

    if ((sLogType & eConsole) && (mLoggers.find(eConsole) == mLoggers.end()) )
    {
        mLoggers[eConsole] = new CConsoleLogger();
    }
    if ((sLogType & eFile) && (mLoggers.find(eFile) == mLoggers.end()))
    {
        mLoggers[eFile] = new CTextFileLogger();
    }
    if ((sLogType & eDebugView) && (mLoggers.find(eDebugView) == mLoggers.end()))
    {
        mLoggers[eDebugView] = new CDebugViewLogger();
    }

    std::string dllFilePath = InuCommon::COsUtilities::GetDLLFileName();
    std::string exeFilePath = InuCommon::COsUtilities::GetExeFileName();

    SingleWrite("Logger for " + exeFilePath + "/" + dllFilePath + ", version number: " + INUCOMMON_VERSION_STR, eInfo, "");
    
    mThreadFinishedEvent.reset(InuCommon::COsUtilities::EventCreate() , InuCommon::COsUtilities::EventDestroy);
    mLoggingThread = InuCommon::COsUtilities::CreateThread(WriteLogsThread, this);
    mLoggingThreadIsRunning = true;

    return true;
}

CLogger::CLogger() :  
    mLoggingThread((pthread_t)NULL),
    mLogMessageEvent(InuCommon::COsUtilities::EventCreate(), InuCommon::COsUtilities::EventDestroy),
    mLoggingThreadIsRunning(false)
{
}

CLogger::~CLogger()
{
    Stop(false);
    
    for (auto& elem : mLoggers)
    {
        delete elem.second;
    }
    mLoggers.clear();

    mLogMessageEvent = nullptr;
    mThreadFinishedEvent = nullptr;
}

void CLogger::Stop(bool iWriteLog)
{
    if (mLoggingThreadIsRunning &&  mThreadFinishedEvent != nullptr)
    {
        InuCommon::COsUtilities::EventCancel(mLogMessageEvent.get());

        if (sDestructorInvoked)
        {
            InuCommon::COsUtilities::sleep(1000);
        }
        else
        {
            InuCommon::COsUtilities::WaitForEventOrThreadTermination(mLoggingThread, mThreadFinishedEvent.get());
        }

        mThreadFinishedEvent = nullptr;

        if (iWriteLog && mLoggingThreadIsRunning == false)
        {
            WriteAllLogs();  // Write leftover logs
        }
    }
}

void CLogger::SetLogType(int iLogType, bool iForce)
{
    if (iForce || (sTheInstance == nullptr) || (sLogType == eAuto && iLogType != eAuto) )
    {
        DestroyInstance();
        CreateTheInstance(iLogType);
    }
}

std::string CLogger::GetFileLoggerPath()
{
    return CTextFileLogger::GetFileLoggerPath();
}

void CLogger::SetFileLoggerPath(const std::string& textFileFolder)
{
    if (CTextFileLogger::GetFileLoggerPath() != textFileFolder)
    {
        CTextFileLogger::SetFileLoggerPath(textFileFolder);
        DestroyInstance();
    }
}

ThreadFuncReturnType CLogger::WriteLogsThread(void* iParent)
{
    CLogger* parent = reinterpret_cast<CLogger*>(iParent);

    while (InuCommon::COsUtilities::EventWait( parent->mLogMessageEvent.get()) !=  InuCommon::COsUtilities::eCancel)
    {
         parent->WriteAllLogs();
    }

    std::shared_ptr<void> autoSignalEvent(parent->mThreadFinishedEvent.get(), InuCommon::COsUtilities::EventSet); //guarantee signaling at end of scope (end of thread)
    parent->mLoggingThreadIsRunning = false;

    return 0;

}

void CLogger::WriteAllLogs()
{
    decltype(mMessagesQueue) localQueue;
    {
        WriteLock wlock(mQueueLock);
        localQueue.swap(mMessagesQueue);
    }

    // Write all waiting messages to all different logs
    for (const auto& logMessage : localQueue)
    {
        for (const auto& elem :  mLoggers)
        {
            elem.second->Write(logMessage);
        }
    }

    // Flush all different logs
    if (localQueue.size() > 0)
    {
        for (auto& elem : mLoggers)
        {
            elem.second->Flush();
        }
    }
}

void CLogger::SingleWrite(const std::string& log, ESeverity iSeverity , const std::string& logPrefix)
{
    try 
    {
        typedef decltype(mMessagesQueue) QueueType;

        std::ostringstream logStream;
        logStream << logPrefix <<  " ("  << int(iSeverity) << "): " << InuCommon::COsUtilities::ThreadID() << " ";

        // 32 LSB are enough, we need just the interval between each message and 4M seconds are enough
        uint32_t timeInMilli = uint32_t(InuCommon::COsUtilities::GetTimeHighRes(InuCommon::COsUtilities::eMilli));
        logStream << timeInMilli << " ";

#ifndef __ANDROID__
#if !defined(__arm__) || defined(__aarch64__)
        // gcc 4.9 that is used in our arm32 and Android ndk 10e compilation doesn't support std::put_time
        if (sLogType & eFile)
        {
            // Add current time to log file
            std::time_t now_c = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
            logStream << std::put_time(std::localtime(&now_c), "%T");
        }
#endif
#endif
        logStream << std::string(" ; ") << log;

        std::string logText = logStream.str();

        if (logText[logText.size()-1] == '\0')
        {
            // This case is for FW messages which ends with \0 and Linux editors don't like it
            logText[logText.size()-1] = '\n';
        }
        else if ((logText[logText.size()-1] != '\n') && (logText[logText.size()-1] != '\r') )
        {
            logText += '\n';
        }

#ifdef SYNC_LOGGING
        std::cout << logText <<std::endl;
#else
        bool setEvent = false;
        {
            WriteLock wlock(mQueueLock);
            if (mMessagesQueue.size() < MAX_LOG_QUEUE_SIZE)
            {
                mMessagesQueue.push_back(QueueType::value_type(std::make_pair(iSeverity,logText)));
                setEvent = true;
            }
        }
        if (setEvent)
        {
            InuCommon::COsUtilities::EventSet(mLogMessageEvent.get());
        }
#endif
    }
    catch(...) {
    }
}

void InuCommon::CLogger::DestroyInstance()
{
    WriteLock locker(sConstructionLocker);
    if (sTheInstance != nullptr)
    {
        sTheInstance->Stop(true);
        sTheInstance.reset();
    }
}
