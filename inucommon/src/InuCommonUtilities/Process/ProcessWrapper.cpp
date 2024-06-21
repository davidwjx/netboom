#include "ProcessWrapper.h"
#include <chrono>
#include <array>
#include <string>
#include <iterator>
#include <sstream>
#include <memory>
#include <stdio.h>
#include <stdlib.h>
#include <iomanip>
#include <boost/filesystem.hpp>
#include <boost/circular_buffer.hpp>
#include <Logger.h>

#ifdef _WIN32
#include <windows.h> 
#include <tchar.h>
#include <stdio.h> 
#include <strsafe.h>

#define ENV_BUFSIZE 4096
#define PATH_VARNAME TEXT("Path")
#endif

//#include <boost/format.hpp>

// TODO: port logger utilities from InuCalibration to InuCommon
#define COMMON_LOGGER_PREFIX "Common"
#define COMMON_LOG_INFO(msg)  CLogger::Write((msg), CLogger::eInfo,  COMMON_LOGGER_PREFIX)
#define COMMON_LOG_ERROR(msg) CLogger::Write((msg), CLogger::eError, COMMON_LOGGER_PREFIX)

using namespace InuCommon;

#ifndef _WIN32
inline int _pipe(int fildes[2], unsigned psize, int textmode) {
    return pipe(fildes);
}
inline FILE* _popen(const char* command, const char* type) {
    return popen(command, type);
}
inline int _pclose(FILE* file) {
    return pclose(file);
}
#else
//////////////////////////////////////////////////////////////////////////
// Windows implementation
//////////////////////////////////////////////////////////////////////////


CProcessWrapper::EResultCodes CProcessWrapper::RunProcessWindows(const std::string & iCmd, bool iKeepOutput)
{
    HANDLE g_hChildStd_OUT_Rd = NULL;
    HANDLE g_hChildStd_OUT_Wr = NULL;
    HANDLE g_hInputFile = NULL;

    SECURITY_ATTRIBUTES saAttr;

    // Set the bInheritHandle flag so pipe handles are inherited. 

    saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
    saAttr.bInheritHandle = TRUE;
    saAttr.lpSecurityDescriptor = NULL;

    // Create a pipe for the child process's STDOUT. 
    BOOL result = CreatePipe(&g_hChildStd_OUT_Rd, &g_hChildStd_OUT_Wr, &saAttr, 0);
    if (result == 0) {
        COMMON_LOG_ERROR("[Process] Failed to create pipe");
        return EResultCodes::eERROR_FAILED_TO_CREATE_PIPE;
    }

    // Ensure the read handle to the pipe for STDOUT is not inherited.
    result = SetHandleInformation(g_hChildStd_OUT_Rd, HANDLE_FLAG_INHERIT, 0);
    if (result == 0) {
        COMMON_LOG_ERROR("[Process] Failed to set handle info");
        return EResultCodes::eERROR_FAILED_TO_SET_HANDLE_INFO;
    }

    //////////////////////////////////////////////////////////////////////////
    // Create the child process
    //////////////////////////////////////////////////////////////////////////
    std::wstring wstr(iCmd.begin(), iCmd.end()); //convert to wstring

    //TCHAR szCmdline[] = wstr.c_str();
    PROCESS_INFORMATION piProcInfo;
    STARTUPINFO siStartInfo;
    BOOL bSuccess = FALSE;

    // Set up members of the PROCESS_INFORMATION structure. 

    ZeroMemory(&piProcInfo, sizeof(PROCESS_INFORMATION));

    // Set up members of the STARTUPINFO structure. 
    // This structure specifies the STDIN and STDOUT handles for redirection.

    ZeroMemory(&siStartInfo, sizeof(STARTUPINFO));
    siStartInfo.cb = sizeof(STARTUPINFO);
    siStartInfo.hStdError = g_hChildStd_OUT_Wr;
    siStartInfo.hStdOutput = g_hChildStd_OUT_Wr;
    //siStartInfo.hStdInput = g_hChildStd_IN_Rd;
    siStartInfo.dwFlags |= STARTF_USESTDHANDLES;

    // Create the child process. 

    //std::vector<wchar_t> bufs(iCmd.begin(), iCmd.end());
    std::vector<wchar_t> buf(wstr.begin(), wstr.end());
    buf.push_back(0);

    //////////////////////////////////////////////////////////////////////////

    DWORD dwFlags = CREATE_NO_WINDOW;
#ifdef UNICODE
    dwFlags |= CREATE_UNICODE_ENVIRONMENT;
#endif

    bSuccess = CreateProcess(
        NULL,             // App name
        buf.data(),       // command line 
        NULL,             // process security attributes 
        NULL,             // primary thread security attributes 
        TRUE,             // handles are inherited 
        dwFlags,          // creation flags 
        NULL,             // (LPVOID)chNewEnv, // NULL, // use parent's environment 
        NULL,             // use parent's current directory 
        &siStartInfo,     // STARTUPINFO pointer 
        &piProcInfo);     // receives PROCESS_INFORMATION 

    // We gave this handle to the process, we must close it (otherwise FileRead might get stuck)
    CloseHandle(g_hChildStd_OUT_Wr);

    if (bSuccess == 0) {
        COMMON_LOG_ERROR("[Process] Error creating child process " + GetLastError());
        return EResultCodes::eERROR_FAILED_TO_CREATE_CHILD_PROCESS;
    }

    //////////////////////////////////////////////////////////////////////////
    // Read From Pipe
    //////////////////////////////////////////////////////////////////////////

    // Read output from the child process's pipe for STDOUT
    // and write to the parent process's pipe for STDOUT. 
    // Stop when there is no more data. 
    const unsigned int BUFSIZE = 2048;

    DWORD dwRead;
    CHAR chBuf[BUFSIZE + 1] = {};
    bSuccess = FALSE;
    HANDLE hParentStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
        
    mProcessOutput.clear();

    boost::circular_buffer<char> cb(BUFSIZE*3);
    //std::deque<char> cb(BUFSIZE);

    // TODO: this is a fast fix, strings between buffer may split
    for (;;)
    {
        bSuccess = ReadFile(g_hChildStd_OUT_Rd, chBuf, BUFSIZE, &dwRead, NULL);
        if (!bSuccess || dwRead == 0) break;

        cb.insert(std::end(cb), std::begin(chBuf), std::begin(chBuf) + dwRead);

        auto cb_begin_it = std::begin(cb);
        auto cb_newline_it = std::begin(cb);

        // split by new line and print all whole lines

        while ((cb_newline_it = std::find(cb_begin_it, std::end(cb), '\n')) != std::end(cb))
        {
            // copy and strip non ASCII characters that may crash our logger (and also \r)
            std::string msg; // (b, it);
            std::copy_if(cb_begin_it, cb_newline_it, std::back_inserter(msg), [](char c) { return c >= 0 && c != '\r'; });
            cb_begin_it = cb_newline_it + 1;  // skip the newline
            
            COMMON_LOG_INFO((std::string) " [External] " + msg);

            if (iKeepOutput)
            {
                mProcessOutput.push_back(msg);
            }
        }

        // clear printed characters
        cb.erase(std::begin(cb), cb_begin_it);

        if (!bSuccess) break;
    }

    // Wait until child process exits.
    DWORD exit_code;
    WaitForSingleObject(piProcInfo.hProcess, INFINITE);
    GetExitCodeProcess(piProcInfo.hProcess, &exit_code);

    mProcessExitCode = static_cast<int>(exit_code);

    CloseHandle(piProcInfo.hProcess);
    CloseHandle(piProcInfo.hThread);

    return (mProcessExitCode == 0)
        ? EResultCodes::eSUCCESS
        : EResultCodes::eERROR_PROCESS_FINISED_NON_ZERO_RETURN_CODE;
}
#endif


CProcessWrapper::EResultCodes CProcessWrapper::RunScript(
    const std::string & iInterpeter, 
    const std::string & iScript, 
    const std::string & iArguments,
    bool iKeepOutput /* = false */)
{

    if (!COsUtilities::FileExists(iInterpeter)) {
        COMMON_LOG_ERROR((std::string) "[Process] Cannot find interpreter: " + iInterpeter);
        return EResultCodes::eERROR_FAILED_TO_LOCATE_INTERPRETER;
    }

    if (!COsUtilities::FileExists(iScript)) {
        COMMON_LOG_ERROR((std::string) "[Process] Cannot find script: " + iScript);
        return EResultCodes::eERROR_FAILED_TO_LOCATE_SCRIPT;
    }

    using namespace ProcessUtils; 

    std::stringstream cmd;
    cmd << Q(iInterpeter) << " " << Q(iScript) << " " << iArguments;

    auto retval = RunCommand(cmd.str(), iKeepOutput);

    return retval;
}

//////////////////////////////////////////////////////////////////////////

CProcessWrapper::EResultCodes CProcessWrapper::RunProcess(
    const std::string & iExecutable,
    const std::string & iArguments,
    bool iKeepOutput /* = false */)
{
    using namespace ProcessUtils;

    if (!COsUtilities::FileExists(iExecutable)) {
        COMMON_LOG_ERROR((std::string) "[Process] Cannot find executable: " + iExecutable);
        return EResultCodes::eERROR_FAILED_TO_RUN_EXTERNAL_PROCESS;
    }

    std::stringstream cmd;
    cmd << Q(iExecutable) << " " << iArguments;

    return RunCommand(cmd.str(), iKeepOutput);
}

//////////////////////////////////////////////////////////////////////////

CProcessWrapper::EResultCodes CProcessWrapper::RunCommand(const std::string & iCmd, bool iKeepOutput, bool iAlwaysUsePopen)
{

    COMMON_LOG_INFO((std::string) "[Process] Running external process: " + iCmd);

    mProcessOutput.clear();

    auto start_time = std::chrono::high_resolution_clock::now();

#ifdef _WIN32
    auto result = 
        iAlwaysUsePopen 
          ? RunProcess_popen(iCmd, iKeepOutput)
          : RunProcessWindows(iCmd, iKeepOutput);
#else
    auto result = RunProcess_popen(iCmd, iKeepOutput);
#endif

    auto end_time = std::chrono::high_resolution_clock::now();

    std::chrono::duration<float> fsec = end_time - start_time;
    std::stringstream ss;

    ss << "[Process] -- Finished with code " << mProcessExitCode << ", elappsed time "
        << std::fixed << std::setprecision(2) << fsec.count() << " seconds";

    COMMON_LOG_INFO(ss.str());

    return result;
}

//////////////////////////////////////////////////////////////////////////

CProcessWrapper::EResultCodes CProcessWrapper::RunProcess_popen(const std::string & iCmd, bool iKeepOutput)
{
    std::array<char, 128> buffer = {};  // zero initialization

    int exitCode = -1;

    // scope for unique_ptr, we want deleter set exitCode before we return from this method
    {
        auto pcloseDeleter = [&](FILE * f) { exitCode = _pclose(f);  };
        std::unique_ptr<FILE, decltype(pcloseDeleter)> pipe(_popen(iCmd.c_str(), "r"), pcloseDeleter);

        FILE *p = pipe.get();
        if (p == nullptr) {
            COMMON_LOG_ERROR((std::string) "[Process] Cannot run external process, popen() failed");
            return EResultCodes::eERROR_FAILED_TO_CREATE_PIPE;
        }

        // blocking approach
        while (std::fgets(buffer.data(), (int) buffer.size() - 1, pipe.get()) != nullptr)
        {
            std::string str{ buffer.data() };
            if (!str.empty() && str.back() == '\n') {
                str.pop_back(); // remove trailing \n
            }

            mProcessOutput.push_back(std::move(str));
            COMMON_LOG_INFO((std::string) "[External] " + buffer.data());
        }
    }

    COMMON_LOG_INFO((std::string) "[Process] -- Finished with code " + std::to_string(exitCode));
    mProcessExitCode = exitCode;

    return (mProcessExitCode == 0) 
        ? EResultCodes::eSUCCESS
        : EResultCodes::eERROR_PROCESS_FINISED_NON_ZERO_RETURN_CODE;
}

