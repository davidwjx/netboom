//
// File : COsUtilities.cpp - Win32 and Linux Implementation
//
#ifdef _WIN32
#include "OSUtilities.h"
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <boost/filesystem.hpp>

using namespace std;
using namespace InuCommon;

#include <thread>
#include <Shlwapi.h>
#include <Sddl.h>
#include <Aclapi.h>
#include <shlobj.h>
#include <Psapi.h>
#include <windows.h>
#include <Mmsystem.h>
#include <tlhelp32.h>
#include <tchar.h>
#include <iostream>

#include <boost/filesystem.hpp>

using namespace std;
using namespace InuCommon;

const char COsUtilities::PATH_SEPARATOR('\\');
const string  COsUtilities::INUITIVE_PATH("INUITIVE_PATH");

#define MAX_KEY_LENGTH 255
#define MAX_VALUE_NAME 16383

//
// Mutex Functions
//.

bool COsUtilities::WaitForThreadTermination(pthread_t& iThread, DWORD& exitCode, unsigned int iTimeOut)
{
    ::WaitForSingleObject(iThread, iTimeOut);
    if (!::GetExitCodeThread(iThread, &exitCode))
    {
        return false ;
    }

    if (exitCode != STILL_ACTIVE)
    {
        CloseHandle(iThread);
        iThread = NULL;
        return true;
    }
    return false ;
}

bool COsUtilities::IsThreadAlive(pthread_t& iThread)
{
   return  !WaitForThreadTermination(iThread, 0); //checks thread state
}

bool COsUtilities::WaitForThreadTermination(pthread_t& iThread, unsigned int iTimeOut)
{
    DWORD   exitCode;
    return WaitForThreadTermination(iThread, exitCode, iTimeOut);
}

bool COsUtilities::WaitForThreadTermination(std::thread& iThread)
{
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
    return true;
}

bool  COsUtilities::ThreadsAreEqual(HANDLE handle)
{
    return InuCommon::COsUtilities::ThreadID() == ::GetThreadId(handle);
}

unsigned long 	COsUtilities::GetCurrentSessionID(bool& isIsolatedSession)
{
    DWORD sessID = 0;

    // get the current ID , no special treat for session 0 isolation

    ::ProcessIdToSessionId(::GetCurrentProcessId(),&sessID);
    if (sessID != 0 || !IsSessionZeroIsolate()){ // if no isolation return it
        return sessID;
    }

    DWORD	actualLen = 0;
    HANDLE	TokenHandle = 0;
    // try to get the thread (impersonate by Microfoft in lsass) token, hopefully Microsoft will have no bug
    if (!OpenThreadToken(GetCurrentThread(), TOKEN_QUERY ,FALSE, &TokenHandle)){
        return sessID;
    }
    //read the impersonate session ID
    if( !GetTokenInformation( TokenHandle, TokenSessionId, &sessID, sizeof(DWORD), &actualLen ) ){
        CloseHandle(TokenHandle);
        return sessID; // return what we have from ProcessIdToSessionId, (it will be 0)
    }
    // mark the isolated flag, so sharedMemoryCreate will append the session to the SM name
    isIsolatedSession = true;
    CloseHandle(TokenHandle);
    return sessID;
}

bool  COsUtilities::FileCopy(const std::string& source, const std::string& dest)
{
    try {
        // check if the directory already exists
        return (::CopyFile(COsUtilities::ConvertEncoding(source).c_str(), COsUtilities::ConvertEncoding(dest).c_str(), false) == TRUE);
    }
    catch(...) {
        return false;
    }
}

bool COsUtilities::CreateDir(const std::string& name)// , std::string kk)
{
    //printf("%s\n", kk.c_str());
    try {
        HANDLE          hFind;
        WIN32_FIND_DATA FindFileData;

        // check if the directory already exists
        hFind = ::FindFirstFile(COsUtilities::ConvertEncoding(name).c_str(), &FindFileData);

        if (hFind == INVALID_HANDLE_VALUE) {
            return (::CreateDirectory(COsUtilities::ConvertEncoding(name).c_str(), NULL) == TRUE);
        }
        ::FindClose(hFind);
    }
    catch (...) {
        return false;
    }
    return true;
}



bool COsUtilities::SaveRaw(const unsigned char *Buffer, int iBufferSize, const std::string& iFileName, uint64_t iIndex )
{
    string fileName(BuildFileName(iFileName, ".raw", iIndex));

    // Now we can open a file to save to (using windows functions but it doesn't matter what file I/O functions you use of course)
    HANDLE file = CreateFile (COsUtilities::ConvertEncoding(fileName).c_str() , GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
    if ( NULL == file )
    {
        CloseHandle (file);//this is a potential bug - should not pass NULL to CloseHandle!
        return false;
    }

    // and finally the image data:
    unsigned long bwritten;
    if ( WriteFile ( file, Buffer, iBufferSize, &bwritten, NULL ) == false )
    {
        CloseHandle (file);
        return false;
    }

    // Now we can close our function with
    CloseHandle (file);
    return true;
}


bool COsUtilities::SaveBMP(const unsigned char* Buffer, int width, int height, int bitsPerPixel, long sizeInBytes, const std::string& baseFileName, uint64_t index)
{
    // add index and extension to filename
    std::string fileName(BuildFileName(baseFileName, ".bmp", index));

    // Buffer is an array that contains the image data, width and height are the dimensions of the image to save, and paddedsize
    // is the size of Buffer in bytes. bmp file is the filename to save to.
    //
    // First we declare the header structs and clear them:

    BITMAPFILEHEADER bmfh;
    BITMAPINFOHEADER info;

    memset (&bmfh, 0, sizeof(BITMAPFILEHEADER));
    memset (&info, 0, sizeof(BITMAPINFOHEADER));

    //RGB 565 bit mask
    static const int MASK_SIZE=3;
    DWORD dwMask[MASK_SIZE];
    dwMask[0] = 0x0000F800; //red
    dwMask[1] = 0x000007E0; //green
    dwMask[2] = 0x0000001F; //blue

    // fill the file header with data:
    bmfh.bfType             = 0x4d42;       // 0x4d42 = 'BM'
    bmfh.bfReserved1        = 0;
    bmfh.bfReserved2        = 0;

    //save 565
    if (bitsPerPixel == 16)
    {
        //add the bit mask
        bmfh.bfOffBits          = 0x36 + sizeof(DWORD)*MASK_SIZE;
        bmfh.bfSize             = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + sizeInBytes + sizeof(DWORD)*MASK_SIZE;
    }
    else
    {
        bmfh.bfOffBits          = 0x36;
        bmfh.bfSize             = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + sizeInBytes;
    }

    // and the info header:
    info.biSize             = sizeof(BITMAPINFOHEADER);
    info.biWidth            = width;
    info.biHeight           = -height;   // If it is not negative the image is vertically flipped
    info.biPlanes           = 1;
    info.biBitCount         = bitsPerPixel;
    info.biCompression      = (bitsPerPixel == 16) ? BI_BITFIELDS : BI_RGB;
    info.biSizeImage        = 0;
    info.biXPelsPerMeter    = 0x0ec4;
    info.biYPelsPerMeter    = 0x0ec4;
    info.biClrUsed          = 0;
    info.biClrImportant     = 0;

    // We want to save as a 24 bit RGB image, so we have to set biCompression to BI_RGB, biBitCount to 24 and biPlanes to 1.
    // In 24 bit images we can set the biSizeImage value to 0 since it is ignored.
    // For PelsPerMeter i simply use the values that Paint uses when saving bitmaps.
    // Since we have no palette, we set the biClrUsed to 0, and biClrImportant being zero means that all colors are important.

    //Now we can open a file to save to (Using windows functions but it doesn't matter what file I/O functions you use of course)
    HANDLE file = CreateFile(COsUtilities::ConvertEncoding(fileName).c_str() , GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

    if (file == NULL)
    {
        CloseHandle(file);//this is a potential bug - should not pass NULL to CloseHandle!
        return false;
    }

    // Write the file header and info header:
    unsigned long bwritten;


    if (WriteFile(file, &bmfh, sizeof(bmfh), &bwritten, NULL ) == false )
    {
        CloseHandle(file);
        return false;
    }

    if (WriteFile(file, &info, sizeof(info), &bwritten, NULL ) == false )
    {
        CloseHandle(file);
        return false;
    }

    //save the RGB 565 bitmask
    if (bitsPerPixel == 16)
    {
        if (WriteFile(file, &dwMask, sizeof(dwMask), &bwritten, NULL ) == false )
        {
            CloseHandle(file);
            return false;
        }
    }


    // and finally the image data:
    if (WriteFile(file, Buffer, sizeInBytes, &bwritten, NULL ) == false )
    {
        CloseHandle(file);
        return false;
    }

    // Now we can close our function with
    CloseHandle(file);
    return true;
}

unsigned char* COsUtilities::LoadBMP (int& width, int& height, long& size, const std::string& baseFileName, uint64_t index)
{
    BITMAPFILEHEADER bmpheader;
    BITMAPINFOHEADER bmpinfo;
    DWORD bytesread;

    // add index and extension to filename
    string fileName(baseFileName);
    if (index != -1)
    {
        ostringstream  outputFileName;
        outputFileName << baseFileName << "_" << index << ".bmp" << ends;
        fileName = outputFileName.str();
    }

    // Note that we take three pointers as parameters for width, height and size, since we will return
    // the image dimensions and size in these variables. bmp file is of course the filename of the bitmap,
    // and the return value of the function will be a pointer to the image data.
    // First lets try to open the file:

    HANDLE file = CreateFile(COsUtilities::ConvertEncoding(fileName).c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL);

    if (file == NULL)
    {
        return NULL;
    }

    // Just a quick note here: it's useful to write if ( NULL == file ) instead of if ( file == NULL ) to
    // prevent bugs, since on accidentally typing if ( file = NULL ) the compiler will not complain but assign
    // NULL to the file handle. if ( NULL = file ) will spawn a compiler error, so you can prevent bugs
    // easily this way.
    //
    // Back to the topic: now we opened the file and can read the file header. On error we will close the file
    // and return from the function.

    if (ReadFile(file, &bmpheader, sizeof ( BITMAPFILEHEADER ),  &bytesread, NULL ) == false )
    {
        CloseHandle (file);
        return NULL;
    }

    // Now we can read the info header:
    if (ReadFile(file, &bmpinfo, sizeof ( BITMAPINFOHEADER ), &bytesread, NULL ) == false )
    {
        CloseHandle (file);
        return NULL;
    }

    // Since we are only going to load 24bit .bmp files here we now do some checking of the header contents.
    // First check if the file is actually a bitmap:

    if (bmpheader.bfType != 'MB')
    {
        CloseHandle(file);
        return NULL;
    }

    // Check if it's uncompressed

    if ( bmpinfo.biCompression != BI_RGB && bmpinfo.biCompression != BI_BITFIELDS)
    {
        CloseHandle(file);
        return NULL;
    }

    // check if it's 24bit
    if ( (bmpinfo.biBitCount != 24) && (bmpinfo.biBitCount != 32) && (bmpinfo.biBitCount != 16))
    {
        CloseHandle(file);
        return NULL;
    }

    // When we are here we actually have a 24 bit bmp, so lets get its size and dimensions. We'll store them in
    // the supplied variables:

    width   = bmpinfo.biWidth;
    height  = abs ( bmpinfo.biHeight );
    size = bmpheader.bfSize - bmpheader.bfOffBits;

    // To be independent of the type of info header, we compute the image data size as the whole file size minus
    // the distance from file origin to start of image data.
    //
    // Now we create a buffer to hold the data

    BYTE *Buffer = (BYTE*)COsUtilities::AllocteBuffer(size);

    // Again, to be independent of info header version, we set the file pointer to the start of image data as
    // told by the bfOffBits:

    SetFilePointer(file, bmpheader.bfOffBits, NULL, FILE_BEGIN );

    // And now we can read in the data. We make sure that on error the Buffer gets deleted so we don't create
    // memory leaks:

    if (ReadFile(file, Buffer, size, &bytesread, NULL) == false )
    {
        COsUtilities::FreeBuffer((char*)Buffer);
        Buffer=NULL;
        CloseHandle(file);
        return NULL;
    }

    // and finish the function

    CloseHandle(file);
    return Buffer;
}


SM_HANDLE  COsUtilities::SharedMemoryCreate(const std::string& name, unsigned long size, bool& createdHere)
{
    createdHere = false;

    ostringstream newNameStream;

    bool isIsolatedSession = false;
    string newName = name;
    if (name.size()) {
        newNameStream << "Local\\Inuitive>" << name;
        int currSessId = GetCurrentSessionID(isIsolatedSession);
        if (isIsolatedSession){ // if it is Vista+ , we and a service, we add the session ID to the SM name
            newNameStream << hex << currSessId;
        }
        newName = newNameStream.str();
    }
    HANDLE hMMFile = ::OpenFileMapping(FILE_MAP_WRITE, FALSE, COsUtilities::ConvertEncoding(newName).c_str());

    // if this object exists, return the handle
    if (hMMFile) {
        return hMMFile;
    }

    createdHere = true;


    // otherwise, we need to create the object
    SID_IDENTIFIER_AUTHORITY siaWorld = SECURITY_WORLD_SID_AUTHORITY;
    PSID psidEveryone = NULL;
    int nSidSize ;
    int nAclSize ;
    PACL paclNewDacl = NULL;
    SECURITY_DESCRIPTOR sd ;
    SECURITY_ATTRIBUTES sa ;

    try{
        // Create the everyone sid
        if (!AllocateAndInitializeSid(&siaWorld, 1, SECURITY_WORLD_RID, 0,
            0, 0, 0, 0, 0, 0, &psidEveryone))
        {
            psidEveryone = NULL ;
            throw "";
        }

        nSidSize = GetLengthSid(psidEveryone) ;
        nAclSize = nSidSize * 2 + sizeof(ACCESS_ALLOWED_ACE) + sizeof(ACCESS_DENIED_ACE) + sizeof(ACL) ;
        paclNewDacl = (PACL) LocalAlloc( LPTR, nAclSize ) ;
        if( !paclNewDacl )
            throw "";
        if(!InitializeAcl( paclNewDacl, nAclSize, ACL_REVISION ))
            throw "";

        if(!AddAccessDeniedAce( paclNewDacl, ACL_REVISION, WRITE_DAC | WRITE_OWNER, psidEveryone ))
            throw "";
        // I am using GENERIC_ALL here so that this very code can be applied to
        // other objects.  Specific access should be applied when possible.
        if(!AddAccessAllowedAce( paclNewDacl, ACL_REVISION, GENERIC_ALL, psidEveryone ))
            throw "";
        if(!InitializeSecurityDescriptor( &sd, SECURITY_DESCRIPTOR_REVISION ))
            throw "";
        if(!SetSecurityDescriptorDacl( &sd, TRUE, paclNewDacl, FALSE ))
            throw "";
        sa.nLength = sizeof( sa ) ;
        sa.bInheritHandle = FALSE ;
        sa.lpSecurityDescriptor = &sd ;

        hMMFile = ::CreateFileMapping (INVALID_HANDLE_VALUE,
            &sa,
            PAGE_READWRITE,
            0,
            size,
            COsUtilities::ConvertEncoding(newName).c_str());


        // set lower integrity so other proc can join this SM
        if (hMMFile && IsSessionZeroIsolate()){
            bool stat = SetObjectToLowIntegrity(hMMFile);
        }

    }catch(...){
        if( !paclNewDacl )
            LocalFree( paclNewDacl ) ;
        if( !psidEveryone )
            FreeSid( psidEveryone ) ;//this is a potential bug - should not pass NULL to FreeSid!
    }

    return hMMFile ;
}

bool  COsUtilities::SharedMemoryRelease(SM_HANDLE hSMHandle)
{
    return ::CloseHandle((HANDLE)hSMHandle) == TRUE ? true : false;
}

void*  COsUtilities::SharedMemoryAddress(SM_HANDLE hSMHandle)
{
    void* address = ::MapViewOfFile((HANDLE)hSMHandle,
        FILE_MAP_WRITE,
        0,
        0,
        0);
    return address;
}

bool  COsUtilities::SharedMemoryAddressRelease(void* ptr)
{
    return ::UnmapViewOfFile(ptr) == TRUE ? true : false;
}


bool  COsUtilities::SetObjectToLowIntegrity(HANDLE hObject)
{
    // The LABEL_SECURITY_INFORMATION SDDL SACL to be set for low integrity
    LPCWSTR LOW_INTEGRITY_SDDL_SACL_W = L"S:(ML;;NW;;;LW)";

    bool bRet = false;
    DWORD dwErr = ERROR_SUCCESS;
    PSECURITY_DESCRIPTOR pSD = NULL;
    PACL pSacl = NULL;
    BOOL fSaclPresent = FALSE;
    BOOL fSaclDefaulted = FALSE;

    if ( ConvertStringSecurityDescriptorToSecurityDescriptorW (
        LOW_INTEGRITY_SDDL_SACL_W, SDDL_REVISION_1, &pSD, NULL ) )	{
            if ( GetSecurityDescriptorSacl (
                pSD, &fSaclPresent, &pSacl, &fSaclDefaulted ) )	{
                    dwErr = SetSecurityInfo (
                        hObject, SE_KERNEL_OBJECT, LABEL_SECURITY_INFORMATION,
                        NULL, NULL, NULL, pSacl );

                    bRet = (ERROR_SUCCESS == dwErr);
            }

            LocalFree ( pSD );
    }

    return bRet;
}

std::string  COsUtilities::GetRoamingUserPath()
{
    TCHAR szPath[MAX_PATH];
    if ( SUCCEEDED(SHGetFolderPath( NULL, CSIDL_APPDATA ,NULL, 0, szPath ) ) )
    {
        boost::filesystem::path file(szPath);
        file /= "Inuitive";
        return file.string();
    }
    return "";
}

std::string  COsUtilities::GetLocalUserPath()
{
    TCHAR szPath[MAX_PATH];
    if ( SUCCEEDED(SHGetFolderPath( NULL, CSIDL_LOCAL_APPDATA ,NULL, 0, szPath ) ) )
    {
        boost::filesystem::path file(szPath);
        file /= "Inuitive";
        return file.string();
    }
    return "";
}

std::string  COsUtilities::GetTempFolder()
{
    TCHAR szPath[MAX_PATH];
    GetTempPath(MAX_PATH, szPath);
    boost::filesystem::path path(szPath);
    path /= "Inuitive";

    boost::system::error_code err;
    if (!boost::filesystem::exists(path,err))
    {
        boost::filesystem::create_directory(path,err);
        if (err != boost::system::errc::success)
        {
            return "";
        }
        boost::filesystem::permissions(path,boost::filesystem::all_all,err);
        if (err != boost::system::errc::success)
        {
            return "";
        }
    }

    return path.string();
}

std::string  COsUtilities::GetProgramDataFolder()
{
    TCHAR szPath[MAX_PATH];
    if ( SUCCEEDED(SHGetFolderPath( NULL, CSIDL_COMMON_APPDATA ,NULL, 0, szPath ) ) )
    {
        boost::filesystem::path path(szPath);
        path /= "Inuitive";

        boost::system::error_code err;
        if (!boost::filesystem::exists(path,err))
        {
            boost::filesystem::create_directory(path,err);
            if (err != boost::system::errc::success)
            {
                return "";
            }
            boost::filesystem::permissions(path,boost::filesystem::all_all,err);
            if (err != boost::system::errc::success)
            {
                return "";
            }
        }
        return path.string();
    }
    return "";
}

std::string  COsUtilities::GetExeFullPathFileName(const string& iProcName)
{
    PROCESSENTRY32 entry;
    entry.dwSize = sizeof(PROCESSENTRY32);
    std::string oFullPath;

    std::wstring tempConverted = COsUtilities::ConvertEncoding(iProcName);
    const wchar_t* procName = tempConverted.c_str();

    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

    if (Process32First(snapshot, &entry) == TRUE)
    {
        while (Process32Next(snapshot, &entry) == TRUE)
        {
            if (wcscmp(procName, entry.szExeFile) == 0)
            {
                HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, entry.th32ProcessID);

                wchar_t   filePath[MAX_PATH] = {0};
                GetModuleFileName((HINSTANCE)&__ImageBase, filePath, MAX_PATH);
                boost::filesystem::path dllFilePath = filePath;

                oFullPath = dllFilePath.parent_path().string();

                CloseHandle(hProcess);
            }
        }
    }

    CloseHandle(snapshot);
    return oFullPath;
}

bool COsUtilities::IsProcessRunning(const std::string& procName)
{
  HANDLE hProcessSnap;
  PROCESSENTRY32 pe32;
  bool foundProcess = false;
  // Take a snapshot of all processes in the system.
  hProcessSnap = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0 );
  if( hProcessSnap == INVALID_HANDLE_VALUE )
  {
    return false;
  }

  pe32.dwSize = sizeof( PROCESSENTRY32 );

  // Retrieve information about the first process,
  // and exit if unsuccessful
  if( !Process32First( hProcessSnap, &pe32 ) )
  {
    CloseHandle( hProcessSnap );          // clean the snapshot object
    return foundProcess;
  }

  // Now walk the snapshot of processes, and
  // display information about each process in turn
  std::wstring tempConverted = COsUtilities::ConvertEncoding(procName);
  const wchar_t* procNameCStr = tempConverted.c_str();
  do
  {
      if (wcscmp(procNameCStr,pe32.szExeFile) == 0)
      {
          foundProcess = true;
          break;
      }

  } while( Process32Next( hProcessSnap, &pe32 ) );

  CloseHandle( hProcessSnap );
  return foundProcess ;
}

bool COsUtilities::IsProcessRunning( uint32_t procID )
{
    HANDLE hProcessSnap;
    PROCESSENTRY32 pe32;
    bool foundProcess = false;
    // Take a snapshot of all processes in the system.
    hProcessSnap = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0 );
    if( hProcessSnap == INVALID_HANDLE_VALUE )
    {
        return false;
    }

    pe32.dwSize = sizeof( PROCESSENTRY32 );

    // Retrieve information about the first process,
    // and exit if unsuccessful
    if( !Process32First( hProcessSnap, &pe32 ) )
    {
        CloseHandle( hProcessSnap );          // clean the snapshot object
        return foundProcess;
    }

    // Now walk the snapshot of processes, and
    // display information about each process in turn
    do
    {
        if ((pe32.th32ProcessID) == procID)
        {
            foundProcess = true;
            break;
        }

    } while( Process32Next( hProcessSnap, &pe32 ) );

    CloseHandle( hProcessSnap );
    return foundProcess ;
}

uint32_t COsUtilities::GetProcessID(const std::string& procName)
{
    HANDLE hProcessSnap;
    PROCESSENTRY32 pe32;
    uint32_t oPID = -1;
    // Take a snapshot of all processes in the system.
    hProcessSnap = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0 );
    if( hProcessSnap == INVALID_HANDLE_VALUE )
    {
        return false;
    }

    pe32.dwSize = sizeof( PROCESSENTRY32 );

    // Retrieve information about the first process,
    // and exit if unsuccessful
    if( !Process32First( hProcessSnap, &pe32 ) )
    {
        CloseHandle( hProcessSnap );          // clean the snapshot object
        return oPID;
    }

    // Now walk the snapshot of processes, and
    // display information about each process in turn
    std::wstring tempConverted = COsUtilities::ConvertEncoding(procName);
    const wchar_t* procNameCStr = tempConverted.c_str();
    do
    {
        if (wcscmp(procNameCStr,pe32.szExeFile) == 0)
        {
            oPID = pe32.th32ProcessID;
            break;
        }

    } while( Process32Next( hProcessSnap, &pe32 ) );

    CloseHandle( hProcessSnap );
    return oPID ;
}

long COsUtilities::InterlockedCompareExchange( volatile long* iTaget, long iValue, long iComparand)
{
    return ::InterlockedCompareExchange(iTaget, iValue, iComparand);
}

std::string COsUtilities::GetDLLFileName()
{
    boost::filesystem::path dllPath = COsUtilities::GetDLLFullPathFileName();

    return dllPath.stem().string();
}

std::string  COsUtilities::GetDLLFileNameImp(HINSTANCE instance)
{
    wchar_t   filePath[MAX_PATH] = {0};
    if (GetModuleFileName(instance, filePath, MAX_PATH) == 0)
    {
        return std::string();
    }

    boost::filesystem::path dllFilePath = filePath;

    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> convert;
    return convert.to_bytes(dllFilePath.wstring());
}

std::string COsUtilities::GetExeFileName()
{
    wchar_t   filePath[MAX_PATH] = {0};
    GetModuleFileName(NULL, filePath, MAX_PATH);
    boost::filesystem::path exeFilePath = filePath;

    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> convert;
    return convert.to_bytes(exeFilePath.filename().wstring());
}


bool SearchRegistryInstallationPath(LPTSTR uninstallPath, std::string& installationPath)
{
    HKEY hKey;
    TCHAR    achKey[MAX_KEY_LENGTH];   // buffer for subkey name
    DWORD    cbName;                   // size of name string
    DWORD    cSubKeys = 0;               // number of subkeys
    DWORD    cbMaxSubKey;              // longest subkey size
    DWORD    i, retCode;
    DWORD    cchValue = MAX_VALUE_NAME;
    LONG lResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, uninstallPath, 0, KEY_READ, &hKey);

    // get all subkey names
    retCode = RegQueryInfoKey(hKey, NULL, NULL, NULL, &cSubKeys, &cbMaxSubKey, NULL, NULL, NULL, NULL, NULL, NULL);
    if (cSubKeys)
    {
        for (i = 0; i < cSubKeys; i++)
        {
            cbName = MAX_KEY_LENGTH;
            retCode = RegEnumKeyEx(hKey, i, achKey, &cbName, NULL, NULL, NULL, NULL);
            if (retCode == ERROR_SUCCESS)
            {                                // open subkey and try to find InuDev in InstallLocation
                HKEY hSubKey;
                std::wstring fullPath = std::wstring(uninstallPath) + achKey;
                lResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, fullPath.c_str(), 0, KEY_READ, &hSubKey);

                if (lResult == ERROR_SUCCESS)
                {
                    WCHAR szBuffer[2048];
                    DWORD dwBufferSize = sizeof(szBuffer);
                    ULONG nError;
                    nError = RegQueryValueExW(hSubKey, L"InstallLocation", 0, NULL, (LPBYTE)szBuffer, &dwBufferSize);
                    if (ERROR_SUCCESS == nError)
                    {
                        std::wstring ReqPath(szBuffer);
                        if (ReqPath.length() != 0)
                        {
                            if (ReqPath.find(L"InuDev") != string::npos)
                            {
                                installationPath = COsUtilities::ConvertEncodingBack(ReqPath);
                                RegCloseKey(hSubKey);
                                return true;

                            }
                        }
                    }
                    RegCloseKey(hSubKey);
                }
            }
        }
    }
    return false;
}

/// \brief    scan Uninstall registry key to fine the installation path
void COsUtilities::GetInstallationPath(std::string& installationPath)
{

    // open Uninstall key for read
    LPTSTR  uninstallPath = L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\";

    bool foundPath = SearchRegistryInstallationPath(uninstallPath, installationPath);
    if (!foundPath)
    {
        //try Wow6432Node
        LPTSTR  uninstallPath = L"SOFTWARE\\Wow6432Node\\\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\";
        foundPath = SearchRegistryInstallationPath(uninstallPath, installationPath);
    }

}

bool COsUtilities::IsDebuggerPresent()
{
    return ::IsDebuggerPresent() != 0;
}


bool COsUtilities::WaitForEventOrThreadTermination(pthread_t& iThread, void* iEvent, unsigned int iTimeOut, unsigned int iInretval)
{
    EWaitResults ret(eTimeout);
    unsigned int elapsedTime = 0;
    bool isThreadAlive = IsThreadAlive(iThread);

    while (ret == eTimeout && isThreadAlive && elapsedTime < iTimeOut)
    {
        ret = EventWait(iEvent, iInretval);
        elapsedTime += iInretval;
        isThreadAlive = IsThreadAlive(iThread);
    }
    return ((ret == eEvent) || (!isThreadAlive));
}

std::string COsUtilities::GetVersion(const std::string fileName)
{
    std::ostringstream verString("");

    DWORD  verHandle = NULL;
    UINT   size = 0;
    LPBYTE lpBuffer = NULL;
    const char *szVersionFile = fileName.c_str();

    DWORD  verSize = GetFileVersionInfoSizeA(szVersionFile, &verHandle);

    char *verData = nullptr;

    if (verSize != 0)
    {
        verData = new char[verSize];

        if (true
            && GetFileVersionInfoA(szVersionFile, verHandle, verSize, verData)
            && VerQueryValue(verData, TEXT("\\"), (VOID FAR* FAR*)&lpBuffer, &size)
            && size
            )
        {
            VS_FIXEDFILEINFO *verInfo = (VS_FIXEDFILEINFO *)lpBuffer;

            if (verInfo->dwSignature == 0xfeef04bd)
            {
                // Doesn't matter if you are on 32 bit or 64 bit,
                // DWORD is always 32 bits, so first two revision numbers
                // come from dwFileVersionMS, last two come from dwFileVersionLS

                verString << std::setfill('0')
                    << ((verInfo->dwFileVersionMS >> 16) & 0xffffu) << "."
                    << std::setw(2) << ((verInfo->dwFileVersionMS >> 0) & 0xffffu) << "."
                    << std::setw(4) << ((verInfo->dwFileVersionLS >> 16) & 0xffffu) << "."
                    << ((verInfo->dwFileVersionLS >> 0) & 0xffffu)
                    ;

            }
        }
    }

    delete[] verData;

    return verString.str();
}

std::string COsUtilities::getenv(const std::string& varname)
{
    size_t len;

    auto ss = InuCommon::COsUtilities::ConvertEncoding(varname);

    errno_t rc = _wgetenv_s(&len, nullptr, 0, ss.c_str());

    if (rc || !len)
    {
        return std::string();
    }

    std::wstring str;
    str.resize(len);

    rc = _wgetenv_s(&len, const_cast<wchar_t*>(str.data()), len, ss.c_str());

    if (rc)
    {
        return std::string();
    }

    str.pop_back(); // take of last char (NUL)

    return ConvertEncodingBack(str);
}

bool  COsUtilities::putenv(const std::string& varname, const std::string& value)
{
    auto varnameW = InuCommon::COsUtilities::ConvertEncoding(varname);
    auto valueW = InuCommon::COsUtilities::ConvertEncoding(value);

    errno_t rc = _wputenv_s(varnameW.c_str(), valueW.c_str());

    return !rc;
}

bool COsUtilities::ConfigureOMP(char* const* argv)
{
    auto ompPolicy = getenv("OMP_WAIT_POLICY");
    if (ompPolicy != "PASSIVE")
    {
        if (IsDebuggerPresent())
        {
            // can't execute from debugger
            return false;
        }

        if (putenv("OMP_WAIT_POLICY", "PASSIVE") == false)
        {
            return false;
        }

        // (old) NOTE: Processing input parameters to add '"' to each parameter with space in it.
        // Reason is Windows *concatenates* argv, so we have a single line, and original '"'
        // Are removed(!). See discussion here: https://bugs.python.org/issue436259

        // Getting the command line directly, as Windows messes up with characters like ","
        // making original command line reconstruction impossible. Luckily, we can get
        // the original command line, "as-is".

        LPSTR cmdLine = GetCommandLineA();

        //std::cout << "Before _spawnl " << std::endl;
        intptr_t rc = _spawnl(_P_NOWAIT, argv[0], cmdLine, nullptr);
        //std::cout << "After _spawnl, rc: " << rc << std::endl;

        exit(0);
    }

    return true;
}

#endif
