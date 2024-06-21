
#include "OSUtilities.h"
#include "ConsoleLogger.h"
#include <iostream>
#include <map>

using namespace InuCommon;
 
static std::map<CLogger::ESeverity, std::string> sSeverity2Color = {
    {CLogger::eDebug, "\033[1;34m"} ,  // Blue
    {CLogger::eInfo, "\033[32m"},    // Green
    {CLogger::eWarning, "\033[33m"}, // Yellow
    {CLogger::eError, "\033[1;31m"},   // Purpole 
    {CLogger::eStatus, "\033[37m"} };

void CConsoleLogger::Write(const std::pair<CLogger::ESeverity, std::string >& logMessage)
{
    std::cout << sSeverity2Color[logMessage.first] << logMessage.second << "\033[0m";
}

CConsoleLogger::CConsoleLogger()
{
    std::cout << "\033[0m";
}

CConsoleLogger::~CConsoleLogger() 
{
    std::cout << "\033[0m";
}
