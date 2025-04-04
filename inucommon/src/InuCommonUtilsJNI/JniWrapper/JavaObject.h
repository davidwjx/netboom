#ifndef JAVAOBJECT_H
#define JAVAOBJECT_H

// Java Object wrapper
// encapsulates reflection calls to java object's members and functions

#include <string>
#include <cstdarg>

class CJavaObject;

class CJavaMember
{
public:
    CJavaMember(JNIEnv * env,jclass classTypeReflector,jobject iObject, std::string name) : 
        mEnvironment(env) , mClassTypeReflector(classTypeReflector), mObject(iObject), mName(name)
    {

    }

    CJavaMember(JNIEnv * env,jclass classTypeReflector,jobject iObject, std::string name, std::string javaObjectType) : 
        mEnvironment(env) , mClassTypeReflector(classTypeReflector), mObject(iObject), mName(name), mType(javaObjectType)
    {

    }

    CJavaMember(CJavaObject* javaobj, std::string name, std::string javaObjectType); //implemented at the bottom under CJavaObject definition

    CJavaMember(CJavaObject* javaobj, std::string name); //implemented at the bottom under CJavaObject definition



    CJavaMember operator [](std::string  fieldAndType) 
    { 
        return CJavaMember(mEnvironment, mClassTypeReflector,mObject,  mName, fieldAndType); 
    }

    //////////////////////////////////////////////////////////////////////////
    //Set fields

    template <typename ASSIGN_TYPE>
    CJavaMember& operator = (ASSIGN_TYPE value)
    {
        static_assert(sizeof(ASSIGN_TYPE) == -1, "CJavaMember assignment type not implemented");
        return *this;
    }

    //////////////////////////////////////////////////////////////////////////
    //Get fields (cast)

    template <typename CAST_TYPE>
    operator CAST_TYPE() const
    { 
        return cast<CAST_TYPE>();
    }

    template  <typename CAST_TYPE>
    CAST_TYPE cast() const
    {
        static_assert(sizeof(CAST_TYPE) == -1, "CJavaMember cast not implemented");
    }

    //////////////////////////////////////////////////////////////////////////
    // Functions

    template <typename... ARGS>
    void operator() (ARGS... args )
    {
       	jmethodID mid = mEnvironment->GetMethodID(mClassTypeReflector, mName.c_str(), mType.c_str());
    	JavaCallFunctionToList(mid, args...);
    }

    template <typename JAVA_RETURN_TYPE, typename... ARGS>
    void operator() (JAVA_RETURN_TYPE* returnValue, ARGS... args)
    { 
    	jmethodID mid = mEnvironment->GetMethodID(mClassTypeReflector, mName.c_str(), mType.c_str());
        *returnValue = JavaCallFunctionToList<JAVA_RETURN_TYPE>(mid, args...);
    }

    JNIEnv * mEnvironment;
    jclass mClassTypeReflector;
    jobject mObject;
    std::string mName;
    std::string mType;

private:
    template <typename JAVA_RETURN_TYPE>
    JAVA_RETURN_TYPE JavaCallFunction (jmethodID  mid, va_list iParams)
    {
        static_assert(sizeof(JAVA_RETURN_TYPE) == -1, "Java Function call not implemented for this type");
        return JAVA_RETURN_TYPE();
    }

    template <typename JAVA_RETURN_TYPE>
    JAVA_RETURN_TYPE JavaCallFunctionToList (jmethodID  mid, ...)
    {
    	JAVA_RETURN_TYPE ret;
        va_list vaList;
        va_start (vaList, mid);
        ret  = JavaCallFunction<JAVA_RETURN_TYPE>(mid, vaList);
        va_end(vaList);
        return ret;
    }

    void JavaCallFunctionToList (jmethodID  mid, ...);
};

class CJavaObject
{
public:

    CJavaObject(JNIEnv * env, std::string typeName, jobject iClassLoader = NULL): mEnvironment(env), mClassLoader(iClassLoader)
    {
    	constructObject(env, typeName, iClassLoader,  "()V");
    }

    template <typename... Args>
    CJavaObject(JNIEnv * env, std::string typeName, std::string constructorSignature, Args... args): mEnvironment(env)
    {
    	constructObject(env, typeName, NULL, constructorSignature,args...);
    }

    template <typename... Args>
    CJavaObject(JNIEnv * env, std::string typeName, jobject iClassLoader , std::string constructorSignature, Args... args): mEnvironment(env), mClassLoader(iClassLoader)
    {
    	constructObject(env, typeName, iClassLoader, constructorSignature, args...);
    }

    CJavaObject(JNIEnv * env, jobject objectReference) : mEnvironment(env), mActualObject(objectReference), mClassLoader(NULL), clearClass(true)
    {
    	mClassTypeReflector = mEnvironment->GetObjectClass(mActualObject);
    }

    CJavaObject(JNIEnv * env, jclass classType) : mEnvironment(env), mClassTypeReflector(classType), mClassLoader(NULL) ,clearClass(false)
    {
        jmethodID classConstructor = env->GetMethodID(mClassTypeReflector, "<init>", "()V");
        mActualObject = env->NewObject(mClassTypeReflector, classConstructor);
    }

    virtual ~CJavaObject()
    {
    	if (clearClass)
    	{
    		mEnvironment->DeleteLocalRef(mClassTypeReflector);
    	}
    }

    CJavaMember operator [](std::string memberName) { return CJavaMember(this, memberName); }

    // member name and member object type pair
    CJavaMember operator [](std::pair<std::string /*memberName*/, std::string /*javaObjectType*/> fieldAndType) 
    { 
        return CJavaMember(this, fieldAndType.first, fieldAndType.second); 
    }

    operator jobject() 
    {
        return mActualObject;
    }

    JNIEnv * mEnvironment;
    jclass mClassTypeReflector;
    jobject mActualObject;
    jobject mClassLoader;

private:
    template <typename... Args>
    void constructObject(JNIEnv * env, std::string typeName, jobject iClassLoader , std::string constructorSignature, Args... args);
    bool clearClass;
};

//Include specializations after declaration
#include "JavaObjectSpec.h"

#endif
