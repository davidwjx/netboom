#ifndef JNIWRAPPER_H
#define JNIWRAPPER_H

#include "jni.h"
#include "JavaClass.h"

#include "JavaObject.h"


class CJavaToString
{
public:
    CJavaToString(JNIEnv * env, jstring iJavaString) : mEnvironment(env), mJString(iJavaString)
    {
        mString = env->GetStringUTFChars(iJavaString, NULL);
    }

    ~CJavaToString()
    {
        mEnvironment->ReleaseStringUTFChars( mJString, mString);
    }

    operator const char*()
    {
        return mString;
    }

    JNIEnv *    mEnvironment;
    jstring     mJString;
    const char* mString;
};

class CJavaGlobalRef
{
public:
    CJavaGlobalRef(JNIEnv * env, const jobject iObject) : mEnvironment(env)
    {
        mGlobalRef = env->NewGlobalRef(iObject);
    }

    ~CJavaGlobalRef()
    {
        mEnvironment->DeleteGlobalRef( mGlobalRef );
    }

    operator jobject()
    {
        return mGlobalRef;
    }

    JNIEnv *    mEnvironment;
    jobject     mGlobalRef;
};

class CJavaWeakRef
{
public:
    CJavaWeakRef(JNIEnv * env, const jobject iObject) : mEnvironment(env)
    {
        mLocalRef = env->NewWeakGlobalRef(iObject);
    }

    ~CJavaWeakRef()
    {
        mEnvironment->DeleteWeakGlobalRef(mLocalRef);
    }

    operator jobject()
    {
        return mEnvironment->NewLocalRef(mLocalRef);
    }

    JNIEnv *    mEnvironment;
    jweak     mLocalRef;
};

class CJavaThreadEnv
{
public:
    CJavaThreadEnv(JavaVM* iVM) : mVM(iVM)
    {
#ifdef __ANDROID__
        mVM->AttachCurrentThread(&mEnvironment,NULL);
#else
        mVM->AttachCurrentThread((void**)&mEnvironment,NULL);
#endif
    }

    ~CJavaThreadEnv()
    {
        mVM->DetachCurrentThread();
    }

    operator JNIEnv*()
    {
        return mEnvironment;
    }

    JNIEnv*    mEnvironment;
    JavaVM*     mVM;
};

#endif

