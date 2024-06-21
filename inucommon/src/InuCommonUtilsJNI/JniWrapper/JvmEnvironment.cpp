#include "JvmEnvironment.h"
#include <map>

#include "Locks.h"
#include "OSUtilities.h"

JavaVM* CJvmEnvironment::sJavaVm = nullptr;
jobject CJvmEnvironment::gClassLoader = 0;
jmethodID CJvmEnvironment::gFindClassMethod = 0;

void CJvmEnvironment::Init( JavaVM* iVm )
{
	sJavaVm = iVm;
	
#ifdef __ANDROID__
	auto env = getEnv();
	// any class from InuDev package will do
	auto randomClass = env->FindClass("InuDev/Point3D");
	jclass classClass = env->GetObjectClass(randomClass);
	auto classLoaderClass = env->FindClass("java/lang/ClassLoader");
	auto getClassLoaderMethod = env->GetMethodID(classClass, "getClassLoader", "()Ljava/lang/ClassLoader;");
	gClassLoader = env->NewGlobalRef(env->CallObjectMethod(randomClass, getClassLoaderMethod));
	gFindClassMethod = env->GetMethodID(classLoaderClass, "findClass", "(Ljava/lang/String;)Ljava/lang/Class;");
#endif
}

static BLock mapLock;
static std::map<int,std::weak_ptr<CJavaThreadEnv>>  sLocalThreadEnv;

/*
This function enables to release JVM, currently it is not in use, but we may need it in the future
static void callback_threadDetacher(void * env)
{
	CJvmEnvironment::sJavaVm->DetachCurrentThread();
}
*/

std::shared_ptr<CJavaThreadEnv> CJvmEnvironment::GetThreadEnv()
{
    std::shared_ptr<CJavaThreadEnv> javaEnvPtr;
    int threadId = InuCommon::COsUtilities::ThreadID();

    WriteLock wLock(mapLock);
    javaEnvPtr = sLocalThreadEnv[threadId].lock();

    if (javaEnvPtr != nullptr)
    {
        return javaEnvPtr;
    }

    //else we need to create a new object
    javaEnvPtr = std::make_shared<CJavaThreadEnv>(sJavaVm);
    sLocalThreadEnv[threadId] = javaEnvPtr;

    return javaEnvPtr;
}
