
#ifndef __CALLBACKDISPATCHER_H__
#define __CALLBACKDISPATCHER_H__

#include "JniWrapper/JniWrapper.h"
#include "JniWrapper/JvmEnvironment.h"
#include "InuErrorJNI.h"

#include <boost/thread/tss.hpp>

template <class JniFrame, class NativeFrame>  
class CJniDispatcher
{
public:
    
    CJniDispatcher(JNIEnv* env, CJavaObject& iJavaStream, std::string eventName, std::string iFunctionInterface) :
        mFunctionInterface(iFunctionInterface),
        mEventName(eventName)
    {
    	jobject classLoaderObj;
    	CJavaObject streamClassTtype(env, (jobject)iJavaStream.mClassTypeReflector);
    	streamClassTtype["getClassLoader"]["()Ljava/lang/ClassLoader;"](&classLoaderObj);
    	//get a ClassLoader from the streams object (so it we are sure it has the InuDev namespace loaded)
    	mClassLoader.reset(new CJavaGlobalRef(env, classLoaderObj));

        mStream.reset(new CJavaWeakRef(env, iJavaStream));
    }

    inline void  CallBack( const NativeFrame& iFrame, InuDev::CInuError error) 
    {
        if (mLocalThreadEnv.get() == NULL && mListener)
        {
            mLocalThreadEnv.reset(new CJavaThreadEnv(CJvmEnvironment::sJavaVm));
        }
        JNIEnv* env = mLocalThreadEnv->mEnvironment;

#ifdef __ANDROID__
        JniFrame jniFrame(env, iFrame, mClassLoader->mGlobalRef);
        CInuErrorJNI jniError(env, error, mClassLoader->mGlobalRef);
#else
        JniFrame jniFrame(env, iFrame);
        CInuErrorJNI jniError(env, error);
#endif
        jobject javaStream = env->NewLocalRef(mStream->mLocalRef);
        if (javaStream != (jobject)NULL)
        {
        	env->CallVoidMethod(mListener->mGlobalRef, callbackMethod, (jobject)javaStream,  (jobject)jniFrame, (jobject) jniError);
        }

        env->DeleteLocalRef(javaStream);
        env->DeleteLocalRef(jniFrame);
        env->DeleteLocalRef(jniError);

        //Check for java exception and log it
//         jthrowable exc = env->ExceptionOccurred();
//         if (exc) {
// 
//             env->ExceptionClear();
// 
//             // Get the class
//             jclass exccls = env->GetObjectClass(exc);
// 
//             // Get method ID for methods 
//             jmethodID getCodeMeth = env->GetMethodID(exccls, "getCode", "()I");
// 
//             jmethodID getMsgMeth = env->GetMethodID(exccls, "toString", "()Ljava/lang/String;");
// 
//             jstring obj = (jstring)env->CallObjectMethod(exccls, getMsgMeth);
//             CJavaToString errString(env, obj);
// 
//         }
    }

    inline void setEventListener(JNIEnv* env, jobject iObj)
    {
        mListener.reset( new CJavaGlobalRef(env, iObj));
        mListenerClass.reset(new CJavaGlobalRef(env, CJavaObject(env, iObj).mClassTypeReflector));

    	callbackMethod = env->GetMethodID((jclass)mListenerClass->mGlobalRef, mEventName.c_str(), mFunctionInterface.c_str());
    }

    inline void removeEventListener()
    {
        mListener.release();
        mListenerClass.release();
        mStream.release();
    }

    std::string mEventName;
    std::string mFunctionInterface;

    std::unique_ptr<CJavaWeakRef>   mStream;
    std::unique_ptr<CJavaGlobalRef> mListener;
    std::unique_ptr<CJavaGlobalRef> mListenerClass;
    jmethodID 						callbackMethod;

    std::unique_ptr<CJavaGlobalRef> mClassLoader;

    boost::thread_specific_ptr<CJavaThreadEnv> mLocalThreadEnv;
};

#endif
