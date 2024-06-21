
#include "DebugViewLogger.h"

#ifdef __ANDROID__
#include <android/log.h>
#elif __GNUC__
#include <syslog.h>
#endif

#include <sstream>

using namespace InuCommon;

#ifdef __ANDROID__
static std::map<CLogger::ESeverity, int> sSeverity2Android = {
    {CLogger::eDebug, ANDROID_LOG_DEBUG} ,
    {CLogger::eInfo, ANDROID_LOG_INFO},
    {CLogger::eWarning, ANDROID_LOG_WARN},
    {CLogger::eError, ANDROID_LOG_ERROR},
    {CLogger::eStatus, ANDROID_LOG_INFO} };

#elif __GNUC__
static std::map<CLogger::ESeverity, int> sSeverity2Linux = {
    {CLogger::eDebug, LOG_DEBUG} ,
    {CLogger::eInfo, LOG_INFO},
    {CLogger::eWarning, LOG_WARNING},
    {CLogger::eError, LOG_ERR},
    {CLogger::eStatus, LOG_NOTICE } };
#endif

CDebugViewLogger::CDebugViewLogger()
{
#if defined(__GNUC__) && !defined(__ANDROID__)
    prefix = COsUtilities::GetExeFileName();
    openlog (prefix.c_str(), LOG_CONS | LOG_PID | LOG_NDELAY , LOG_LOCAL0);
#endif
}

CDebugViewLogger::~CDebugViewLogger()
{
#if defined(__GNUC__) && !defined(__ANDROID__)
    closelog ();
#endif
}

// Implementation of CLogger abstract class
void    CDebugViewLogger::Write(const std::pair<CLogger::ESeverity, std::string >& logMessage)
{
#ifdef __ANDROID__
    __android_log_print(sSeverity2Android[logMessage.first], "InuDev", "%s", logMessage.second.c_str());
#elif __GNUC__
    syslog (sSeverity2Linux[logMessage.first], "%s", logMessage.second.c_str());
#else
    OutputDebugString(InuCommon::COsUtilities::ConvertEncoding(logMessage.second).c_str());
#endif
}

