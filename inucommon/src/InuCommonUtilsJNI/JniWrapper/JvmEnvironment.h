#ifndef JVMENVIRONMENT_H
#define JVMENVIRONMENT_H

#include "JniWrapper.h"
#include <memory>

class CJvmEnvironment
{
public:

    static void Init( JavaVM* iVm);

    //Get the Java Enviroment object associated with this thread
    static std::shared_ptr<CJavaThreadEnv> GetThreadEnv();

#ifdef __ANDROID__
	static jclass findClass(const char* name) {
        return static_cast<jclass>(getEnv()->CallObjectMethod(gClassLoader, gFindClassMethod, getEnv()->NewStringUTF(name)));
    }

    static JNIEnv* getEnv() {
        JNIEnv *env;
        int status = sJavaVm->GetEnv((void**)&env, JNI_VERSION_1_6);
        if(status < 0) {
            status = sJavaVm->AttachCurrentThread(&env, NULL);
            if(status < 0) {
                return nullptr;
            }
        }
        return env;
    }
#endif

    static JavaVM* sJavaVm;
    static jobject gClassLoader;
    static jmethodID gFindClassMethod;
};

#endif
