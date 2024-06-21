
#include "OSUtilities.h"
#include "TextFileLogger.h"
#include <sstream>
#include <time.h> 
#include <iostream>

#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/interprocess/permissions.hpp>
#include <map>

using namespace InuCommon;

 
std::string CTextFileLogger::sTextFileFolder;
//bool CTextFileLogger::sIsFileFolderChanged = true;
unsigned int CTextFileLogger::sMaxFileSize = MAX_FILE_SIZE;

CTextFileLogger::CTextFileLogger() : IBaseLogger(), 
    mCounter(0), 
    mFileInstance(0)
{
}


CTextFileLogger::~CTextFileLogger()
{
    mOutputFile.close();
}

void CTextFileLogger::OpenFile()
{
    //Create a log file for each DLL (the constructor will be called once for each singleton in each DLL)
//    std::string dllFilePath = InuCommon::COsUtilities::GetDLLFileName();

    std::string exeFilePath = InuCommon::COsUtilities::GetExeFileName();

    int pID = InuCommon::COsUtilities::ProcessID();

    // Use default folder if the user defined path is empty
    boost::filesystem::path folder = sTextFileFolder.empty() ? (InuCommon::COsUtilities::GetProgramDataFolder()) : boost::filesystem::path(sTextFileFolder);

    if (!folder.empty())
    {
        folder /= "log"; 
    }
    else
    {
        folder = "log";
    }

    boost::system::error_code ec;
    if (!boost::filesystem::exists(folder,ec))
    {
        boost::filesystem::create_directory(folder,ec);
        if (ec.value() != boost::system::errc::success)
        {
            return;
        }
        boost::filesystem::permissions(folder,boost::filesystem::all_all,ec);
        if (ec.value() != boost::system::errc::success)
        {
            return;
        }
   }

    std::string prefix =  exeFilePath + "_" ; //+ dllFilePath + "_";

    InuCommon::COsUtilities::CleanupFolder(folder.string(), prefix, MAX_NUMBER_OF_FILES);

    boost::filesystem::path filename(folder);
    filename /= prefix + boost::lexical_cast<std::string>(pID).c_str();
    if (mFileInstance > 0)
    { 
        filename += "_";
        filename += boost::lexical_cast<std::string>(mFileInstance).c_str();
    }
    filename += "-Log.txt";

    boost::system::error_code boostErr;
    if (boost::filesystem::exists(filename, boostErr))
    {
    	mOutputFile.open (filename.c_str(), std::ios::app);
    }
    else
    {
    	mOutputFile.open (filename.c_str(), std::ios::out);
    }
}


void CTextFileLogger::Write(const std::pair<CLogger::ESeverity, std::string >& logMessage)
{
    mCounter += (unsigned int)(logMessage.second.size());
    if (mCounter > sMaxFileSize || mOutputFile.fail() ) //|| sIsFileFolderChanged)
    {
        mCounter = 0;
        mFileInstance++;
        mOutputFile.close();
        OpenFile();
        //sIsFileFolderChanged = false;
    }

    if (mOutputFile.good())
    {
        mOutputFile << logMessage.second;
    }
}

std::string CTextFileLogger::GetFileLoggerPath()
{
    return sTextFileFolder;
}

void CTextFileLogger::SetFileLoggerPath(const std::string& textFileFolder)
{
    boost::system::error_code ec;
    if (boost::filesystem::exists(textFileFolder, ec) && (sTextFileFolder != textFileFolder))
    {
        //sIsFileFolderChanged = true;
        sTextFileFolder = textFileFolder;
    }
}

