
#include "JniWrapper/JvmEnvironment.h"

#ifdef WIN32
// dllmain.cpp : Defines the entry point for the DLL application.
#include <windows.h>

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}
#endif


extern "C" {
    jint JNI_OnLoad(JavaVM* vm, void* reserved)
    {

        JNIEnv* env;
        if (vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_4) != JNI_OK)
        {
            return -1;
        }

        CJvmEnvironment::Init(vm);

        return JNI_VERSION_1_4;
    }
}
