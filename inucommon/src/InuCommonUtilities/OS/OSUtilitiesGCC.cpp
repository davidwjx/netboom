
//
// File : COsUtilities.cpp - Win32 and Linux Implementation
//
#ifdef __GNUC__

#include "OSUtilities.h"
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <thread>

#include <boost/filesystem.hpp>

using namespace std;
using namespace InuCommon;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <sys/types.h>
#include <sys/ipc.h>
#ifndef __ANDROID__
    #include <sys/shm.h>
    #include <sys/sem.h>
#else
    #include "pwd.h"
#ifdef __ANDROID__
    //#include <cpu-features.h>
#endif
#endif
#include <sys/stat.h>
#include <signal.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <memory>
#include <dirent.h>
#include <limits.h>
#include <pthread.h>

#ifndef __ANDROID__
#include <atomic>
#include <condition_variable>
#include <mutex>
#else
#include <boost/thread/condition_variable.hpp>
#include <boost/atomic.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/chrono.hpp>
#endif

#include <iostream>
#include <fstream>

//#ifdef __APPLE__
#include <dlfcn.h>
//#endif

using namespace std;

//
// Mutex Functions
//

const char COsUtilities::PATH_SEPARATOR('/');
const string  COsUtilities::INUITIVE_PATH("INUITIVE_PATH");
extern char *__progname; //holds the programName  on Linux based system (libc)

// one semaphore is used for Fetch&Inc/Decrement operations

void* COsUtilities::LoadTheLibrary(const std::string& name)
{
    void* tmp = (void*)dlopen(name.c_str(), RTLD_NOW | RTLD_GLOBAL);
    //char* err = dlerror();  // For debug
    return tmp;
}

int COsUtilities::FreeTheLibrary(void* iLibHandle)
{
	return dlclose(iLibHandle);
}

void* COsUtilities::GetFunctionEntryPoint(void* iLibHandle, const std::string& iFuncName)
{
    return dlsym(iLibHandle, iFuncName.c_str());
}

std::string  COsUtilities::GetTempFolder()
{
    std::string tempDir = getenv(INUITIVE_PATH);
    if (tempDir.empty())
    {
#ifdef ANDROID_PERFORMANCE
        tempDir = "/data/local/tmp";
#else
        if (tempDir.empty())
        {
#if __ANDROID__
            tempDir = "/storage/emulated/0/Inuitive/tmp";
#else
            tempDir = "/tmp/Inuitive";
#endif
        }
        else
        {
            tempDir += "/tmp";
        }
#endif
    }
    else
    {
        tempDir += "/Inuitive";
    }

    boost::filesystem::path path(tempDir);


    boost::system::error_code err;
    if (!boost::filesystem::exists(path,err))
    {
        boost::filesystem::create_directory(path,err);
        if (err.value() != boost::system::errc::success)
        {
        return "";
        }

#ifndef __ANDROID__
        boost::filesystem::permissions(path,boost::filesystem::all_all,err);
        if (err.value() != boost::system::errc::success)
        {
            return "";
        }
#endif
    }

    return path.string();
}



void  COsUtilities::GetInstallationPath(std::string& installationPath)
{
    installationPath = getenv("INUITIVE_PATH");
    if (installationPath.empty())
    {
#ifdef  __ANDROID__
        installationPath = "/storage/emulated/0/Inuitive/InuDev";
#elif __arm__ //rpi or tizen
        installationPath = ".";
#else //linux
        installationPath = "/opt/Inuitive/InuDev";
#endif
    }
}

long  COsUtilities::InterlockedCompareExchange( volatile long* iTaget, long iValue, long iComparand)
{
    return __sync_val_compare_and_swap(iTaget, iComparand, iValue);
}


bool COsUtilities::IsProcessRunning( uint32_t procID )
{
    boost::system::error_code err;
    boost::filesystem::path dir("/proc");
    if (boost::filesystem::exists(dir, err) && (err.value() == boost::system::errc::success))
    {
        vector<boost::filesystem::directory_entry> subDir;
        copy(boost::filesystem::directory_iterator(COsUtilities::ConvertEncoding(dir.string())), boost::filesystem::directory_iterator(), back_inserter(subDir));
        //All names from here are UTF16 (If used from iterators)
        FOR_EACH (boost::filesystem::directory_entry iter , subDir)
        {
            boost::filesystem::path  procDir(iter);
            try
            {
                int id = atoi(procDir.filename().string().c_str());
                if (id == int(procID))
                {
                    return true;
                }
            }
            catch (...)
            {
                continue;
            }
        }
    }
    return false;
}

uint32_t COsUtilities::GetProcessID(const std::string& procName)
{
    uint32_t result = UINT_MAX;

    // Open the /proc directory
    boost::system::error_code err;
    boost::filesystem::path dir("/proc");
    if (boost::filesystem::exists(dir, err) && (err.value() == boost::system::errc::success))
    {
        vector<boost::filesystem::directory_entry> subDir;
        copy(boost::filesystem::directory_iterator(COsUtilities::ConvertEncoding(dir.string())), boost::filesystem::directory_iterator(), back_inserter(subDir));
        //All names from here are UTF16 (If used from iterators)
        FOR_EACH (boost::filesystem::directory_entry iter , subDir)
        {
            boost::filesystem::path  procDir(iter);
            try
            {
                uint32_t id = atoi(procDir.filename().string().c_str());
                if (id <= 0) // || id == currProcId)
                {
                    continue;
                }
                procDir /= "cmdline";
                ifstream cmdFile(procDir.string().c_str());
                string cmdLine;
                getline(cmdFile, cmdLine);
                if (!cmdLine.empty())
                {
                    // Keep first cmdline item which contains the program path
                    size_t pos = cmdLine.find('\0');
                    if (pos != string::npos)
                    {
                        cmdLine = cmdLine.substr(0, pos);
                    }
                    // Keep program name only, removing the path
                    pos = cmdLine.rfind('/');
                    if (pos != string::npos)
                    {
                        cmdLine = cmdLine.substr(pos + 1);
                    }
                    string processName(procName);
                    pos = processName.rfind('/');
                    if (pos != string::npos)
                    {
                        processName = processName.substr(pos + 1);
                    }
                    // Compare against requested process name
                    if (processName == cmdLine)
                    {
                        result = id;
                        break;
                    }
                }
            }
            catch (...)
            {
                continue;
            }
        }
    }
    return result;
}

bool COsUtilities::IsProcessRunning(const std::string& procName)
{
    uint32_t procId = GetProcessID(procName);
    return ((procId != UINT_MAX) && (procId != ProcessID()));
}

bool COsUtilities::SaveRaw(const unsigned char *Buffer, int iBufferSize, const std::string& iFileName, uint64_t iIndex )
{
    string fileName(BuildFileName(iFileName, ".raw", iIndex));

    ofstream  file(fileName, ios::binary);
    file.write((const char*)Buffer, iBufferSize);
    return file.good();
}


bool COsUtilities::SaveBMP(const unsigned char* Buffer, int width, int height, int bitsPerPixel, long sizeInBytes, const std::string& baseFileName, uint64_t index)
{
    // This code is based on StackOverflow sample code + some modifications to make it identical to Windows version

    static const int SIGNATURE_SIZE=2;

#pragma pack(push,1)

    struct CFileheader
    {
        uint8_t signature[SIGNATURE_SIZE];
        uint32_t filesize;
        uint32_t reserved;
        uint32_t fileoffset_to_pixelarray;
    } ;

    struct CBitmapinfoheader
    {
        uint32_t dibheadersize;
        uint32_t width;
        uint32_t height;
        uint16_t planes;
        uint16_t bitsperpixel;
        uint32_t compression;
        uint32_t imagesize;
        uint32_t ypixelpermeter;
        uint32_t xpixelpermeter;
        uint32_t numcolorspallette;
        uint32_t mostimpcolor;
    } ;

    struct CBitmap
    {
        CFileheader fileheader;
        CBitmapinfoheader bitmapinfoheader;
    } ;

    //RGB 565 bit mask
    static const int MASK_SIZE=3;
    uint32_t dwMask[MASK_SIZE];
    dwMask[0] = 0x0000F800; //red
    dwMask[1] = 0x000007E0; //green
    dwMask[2] = 0x0000001F; //blue

#pragma pack(pop)

    // add index and extension to filename
    std::string fileName(BuildFileName(baseFileName, ".bmp", index));

    ofstream fp(fileName, ios::binary);
    if (!fp.good())
    {
        return false;
    }

    CBitmap bitmap;

    bitmap.fileheader.signature[0] = 'B';
    bitmap.fileheader.signature[1] = 'M';
    bitmap.fileheader.filesize = sizeof(CBitmap) + sizeInBytes;
    bitmap.fileheader.fileoffset_to_pixelarray = sizeof(CBitmap);
    bitmap.bitmapinfoheader.dibheadersize = sizeof(CBitmapinfoheader);
    bitmap.bitmapinfoheader.width = width;
    bitmap.bitmapinfoheader.height = -height;
    bitmap.bitmapinfoheader.planes = 1;
    bitmap.bitmapinfoheader.bitsperpixel = bitsPerPixel;
    bitmap.bitmapinfoheader.compression = 0;
    bitmap.bitmapinfoheader.imagesize = 0;
    bitmap.bitmapinfoheader.ypixelpermeter = 0x0ec4 ;
    bitmap.bitmapinfoheader.xpixelpermeter = 0x0ec4 ;
    bitmap.bitmapinfoheader.numcolorspallette = 0;
    bitmap.bitmapinfoheader.mostimpcolor = 0;

    if (bitsPerPixel == 16)
    {
        bitmap.fileheader.filesize += sizeof(uint32_t)*MASK_SIZE;
        bitmap.fileheader.fileoffset_to_pixelarray += sizeof(uint32_t)*MASK_SIZE;
        bitmap.bitmapinfoheader.compression = 3;
    }

    fp.write((const char*)&bitmap, sizeof(CBitmap));

    //save the RGB 565 bitmask
    if (bitsPerPixel == 16)
    {
        fp.write( (const char*)&dwMask, sizeof(dwMask));
        if (!fp.good())
        {
            return false;
        }
    }

    // Write the buffer
    fp.write((const char*)Buffer, sizeInBytes);
    if (!fp.good())
    {
        return false;
    }

    fp.close();

    return true;
}

std::string  COsUtilities::GetLocalUserPath()
{
    std::string homeDir = COsUtilities::getenv("HOME");
    if (homeDir.empty())
    {
        int bufsize = sysconf(_SC_GETPW_R_SIZE_MAX);
        if (bufsize != -1)
        {
            struct passwd *result = NULL;
#ifndef __ANDROID__
            char buffer[_SC_GETPW_R_SIZE_MAX];
            struct passwd userInfo;
            if (result && (getpwuid_r(getuid(), &userInfo,buffer,bufsize,&result) != 0) )
            {
                homeDir = userInfo.pw_dir;
            }
#else
            if ((result = getpwuid(getuid())) != NULL)
            {
                homeDir = result->pw_dir;
            }
#endif


        }
    }

    boost::filesystem::path localPath = homeDir;
    localPath /= ".Inuitive";

    boost::system::error_code err;
    if (!boost::filesystem::exists(localPath,err))
    {
        boost::filesystem::create_directory(localPath,err);
        if (err.value() != boost::system::errc::success)
        {
            return "";
        }
    }

    return localPath.string();
}

unsigned char* COsUtilities::LoadBMP (int& width, int& height, long& size, const std::string& baseFileName, uint64_t index)
{
    return nullptr;
}


bool COsUtilities::SSE4IsSupported(unsigned int iWidth, EOptimizationLevel iRequestedLevel)
{
    if ( (iWidth % 16 != 0) || (iRequestedLevel < eSSE) )
    {
        return false;
    }

#if !defined(__arm__) && !defined(__aarch64__)

    static int IsAVXSupported(-1);
    if (IsAVXSupported == -1)
    {
#ifdef CPUID_NOT_SUPPORTED
        unsigned int eax,ebx,ecx(0),edx;

        __get_cpuid(1,&eax,&ebx,&ecx,&edx);

        IsAVXSupported = (ecx & (1 << 19));
#else
        IsAVXSupported = __builtin_cpu_supports("sse4.1");
#endif
    }

    return (IsAVXSupported != 0);

#else

#if defined(__aarch64__)
    return true; // ARMv8-A has mandatory NEON support
#else
    // 32 bit machine.
#if defined(__ANDROID__)
    //uint64_t cpuFeatures = android_getCpuFeatures();
    return true;//cpuFeatures & ANDROID_CPU_ARM_FEATURE_NEON;
#else
    return false;

#endif
#endif
#endif

}


bool COsUtilities::AVXIsSupported(unsigned int iWidth, EOptimizationLevel iRequestedLevel)
{
#if  !defined(__arm__) && !defined(__aarch64__)
    if ( (iWidth % AVX_PADDING != 0) || (iRequestedLevel < eAVX) )
    {
        return false;
    }

    static int IsAVXSupported(-1);
    if (IsAVXSupported == -1)
    {
#ifdef CPUID_NOT_SUPPORTED

        unsigned int eax,ebx,ecx(0),edx;

        __get_cpuid(7,&eax,&ebx,&ecx,&edx);

        IsAVXSupported = (ebx & (1 << 5));
#else
    IsAVXSupported = __builtin_cpu_supports("avx2");
#endif
    }

    return (IsAVXSupported != 0);

#else
    return false;
#endif
}

inline bool syscommand(string aCommand, string & result)
{
    FILE * f;
    if ( !(f = popen( aCommand.c_str(), "r" )) ) {
        cout << "Can not open file" << endl;
        return false;
    }
    const int BUFSIZE = 4096;
    char buf[ BUFSIZE ];
    if (fgets(buf,BUFSIZE,f)!=NULL) {
        result = buf;
    }
    pclose( f );
    return true;
}


std::string COsUtilities::GetDLLFullPathFileName()
{
    return std::string();
}
std::string COsUtilities::GetDLLFullPathFileName(const std::string& dllName)
{
    return std::string();
}

std::string COsUtilities::GetDLLFileName()
{
    void* address = __builtin_return_address(0);
    Dl_info symbolInfo;
    if (dladdr(address,&symbolInfo) && symbolInfo.dli_fname != nullptr)
    {
        std::string dllName(symbolInfo.dli_fname);
        size_t last_pos = dllName.find_last_not_of(" \n\r\t") + 1;
        if (last_pos != string::npos)
        {
            dllName.erase(last_pos);
        }
        boost::filesystem::path path(dllName);
        return path.filename().string();
    }

    return "";
}

std::string COsUtilities::GetExeFileName()
{
    return std::string(__progname);
}

static std::string  GetExeFullPathFileName(pid_t iProcpid, const string& iAppName)
{
#ifdef __ANDROID__
    // On Android it is not allowed to access /proc.
    return std::string();
#endif

    string fRes="";
    stringstream command;
    command <<  "readlink /proc/" << iProcpid << "/exe | sed \"s/\\(\\/" << iAppName << "\\)$//\"";
    if (syscommand(command.str(),fRes) == false || fRes.empty())
    {
        return "";
    }
    size_t last_pos = fRes.find_last_not_of(" \n\r\t") + 1;
    if (last_pos != string::npos)
    {
        fRes.erase(last_pos);
    }

    boost::filesystem::path fullPath(fRes);
    fullPath /= iAppName;
    return fullPath.string();
}

std::string  COsUtilities::GetExeFullPathFileName()
{
#ifdef __ANDROID__
    // On Android it is not allowed to access /proc. We use procid instead
    boost::filesystem::path fullPath = boost::filesystem::path(".") / GetExeFileName();
    return fullPath.string();
#endif

    return ::GetExeFullPathFileName(pid_t(getpid()), GetExeFileName());
}

std::string COsUtilities::GetExeFullPathFileName(const std::string& iProcName)
{
    std::string result;
    uint32_t procId = GetProcessID(iProcName);
    if (procId != UINT_MAX)
    {
        result = ::GetExeFullPathFileName(pid_t(procId),iProcName);
    }

    return result;
}

bool COsUtilities::IsDebuggerPresent()
{
    return false;

//     char buf[1024];
//     int debugger_present = 0;
//
//     int status_fd = open("/proc/self/status", O_RDONLY);
//     if (status_fd == -1)
//         return 0;
//
//     size_t num_read = read(status_fd, buf, sizeof(buf));
//
//     if (num_read > 0)
//     {
//         static const char TracerPid[] = "TracerPid:";
//         char *tracer_pid;
//
//         buf[num_read] = 0;
//         tracer_pid    = strstr(buf, TracerPid);
//         if (tracer_pid)
//             debugger_present = !!atoi(tracer_pid + sizeof(TracerPid) - 1);
//     }
//
//     return debugger_present;
}

pthread_t COsUtilities::CreateThread(THREAD_FUNC threadFunc, void * params)
{
    pthread_t tid;

    pthread_attr_t attr;

    // In the future we may want to create detachable threads, currently all are joinable
    int rc = pthread_attr_init(&attr);
    rc = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    if (rc)
    {
        return (pthread_t)NULL;
    }

    rc = pthread_create(&tid, NULL, threadFunc, params);
    if (rc || tid == (pthread_t)NULL)
    {
        return (pthread_t)NULL;
    }
    return tid;
}


bool COsUtilities::WaitForEventOrThreadTermination(pthread_t& iThread, void* iEvent, unsigned int iTimeOut, unsigned int iInretval)
{
    EWaitResults ret(eTimeout);
    // There is a bug on Linux that in order to clean the memory joinable threads there is a need to wait for at least 1 ms
    unsigned int elapsedTime = 1;
    bool isThreadAlive = IsThreadAlive(iThread);

    while (ret == eTimeout && isThreadAlive && elapsedTime < iTimeOut)
    {
        ret = EventWait(iEvent, iInretval);
        // There is a bug on Linux that in order to clean the memory joinable threads there is a need to wait for at least 1 ms
        elapsedTime += (iInretval + 1);
        isThreadAlive = IsThreadAlive(iThread);
    }

    return ((ret == eEvent) || (!isThreadAlive));
}

bool COsUtilities::WaitForThreadTermination(pthread_t& iThread, DWORD& exitCode, unsigned int iTimeOut) //timeout in miliseconds
{
    int res = 0;

#ifdef __ANDROID__
    void *status;
    res = pthread_join(iThread, &status);
    exitCode = (long long)status;
#else
    if (iTimeOut == INFINITE_TIMEOUT)
    {
        void *status;
        res = pthread_join(iThread, &status);
        exitCode = (DWORD)status;
    }
    else
    {
        timespec linuxTimeout;
        clock_gettime(CLOCK_REALTIME, &linuxTimeout);
        linuxTimeout.tv_sec += iTimeOut / 1000;
        linuxTimeout.tv_nsec += (iTimeOut % 1000) * 1000000;
        void *status;
        res = pthread_timedjoin_np(iThread, &status, &linuxTimeout);
        exitCode = (DWORD)status;
    }
#endif
    return (res == 0); //0 means success
}

bool COsUtilities::WaitForThreadTermination(std::thread& iThread)
{
#ifdef __ANDROID__
    if (iThread.joinable())
    {
        // With NDK 10e the function thread:id returns invalid value. This workaround was justed in stackoverflow
        std::ostringstream ostream;
        ostream << iThread.get_id() << std::ends;

        if (std::stoul(ostream.str()) == (unsigned long)pthread_self())
        {
            iThread.detach();
        }
        else
        {
            iThread.join();
        }
    }
#else
    if (iThread.joinable() &&
        (iThread.get_id() != std::thread::id()))
    {
        if (iThread.get_id() != std::this_thread::get_id())
        {
            iThread.join();
        }
        else
        {
            iThread.detach();
        }
    }
#endif

    return true;
}

std::string COsUtilities::getenv(const std::string& varname)
{
    //override InuitivePath for android direct mode (service)
    char* aTmp = ::getenv(varname.c_str());
    if (aTmp != NULL)
    {
        return std::string(aTmp, strlen(aTmp));
    }
    return "";
}

bool  COsUtilities::putenv(const std::string& varname, const std::string& value)
{
    return setenv(varname.c_str(), value.c_str(), 1) == 0;
}

bool COsUtilities::ConfigureOMP(char* const* argv)
{
    auto ompPolicy = getenv("OMP_WAIT_POLICY");
    if (ompPolicy != "PASSIVE")
    {
        if (putenv("OMP_WAIT_POLICY", "PASSIVE") == false)
        {
            return false;
        }

        execv(argv[0], (char* const*)argv);

        // We will get here only if the execv failed
        return false;
    }

    return true;
}

#endif
