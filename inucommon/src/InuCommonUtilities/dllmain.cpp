// dllmain.cpp : Defines the entry point for the DLL application.
#include "OSUtilities.h"
#include "Logger.h"

#ifdef _WIN32
#include <windows.h>

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:

        break;

    case DLL_THREAD_ATTACH:

        break;

    case DLL_THREAD_DETACH:

        break;

    case DLL_PROCESS_DETACH:

        InuCommon::CLogger::sDestructorInvoked = true;

        //InuCommon::CLogger::DestroyInstance(); // Static objects are removed automatically on DLL unload!
        //Destroying the object here may cause a race and deadlock between the DLL Unload and the Process closing process.

        break;
    }
    return TRUE;
}
#elif defined(__ANDROID__) || defined(__linux__)

#if defined(__GNUC__)
__attribute__((destructor))
#endif
void on_unload(void)
{
    //InuCommon::CLogger::DestroyInstance(); Static objects are removed automatically on DLL unload!
        //Destroying the object here may cause a race and deadlock between the DLL Unload and the Process closing process.
}

#if defined(__GNUC__)
__attribute__((constructor))
#endif
void on_load(void)
{

}

#endif

