// dllmain.cpp : Defines the entry point for the DLL application.


#include "config.h"

#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN    // Exclude rarely-used stuff from Windows headers
#include <tchar.h>
#include <windows.h>
#include "Shlwapi.h"
#endif


#if defined(__GNUC__)
__attribute__((destructor))
#endif
void on_unload(void)
{

}

#if defined(__GNUC__)
__attribute__((constructor)) void on_load(void)
#else
void on_load(HMODULE hModule)
#endif
{

}

#if defined(_WIN32)

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:

        on_load(hModule);
        break;

    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:

        break;

    case DLL_PROCESS_DETACH:
        on_unload();

        break;
    }
    return TRUE;
}

#endif
