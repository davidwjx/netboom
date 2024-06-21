// InuStreamsJNI.cpp : Defines the exported functions for the DLL application.
//

#include <iostream>
#include "InuModelDB.h"
#include "InuModelJNI.h"
#include "InuModelDBJNI.h"
#include "Logger.h"

JNIEXPORT jboolean JNICALL Java_InuCommon_InuModelDB_init
		(JNIEnv * env, jobject thiz)
{
	std::shared_ptr<InuCommon::CInuModelDB> inuModelDB = InuCommon::CInuModelDB::GetTheInstance();

	if (!inuModelDB->Load())
	{
		LOGE("Can't  Load SensorsDB.");
		return false;
	}

	LOGE("SensorsDB loaded.");
	const std::map<std::pair<std::string,std::string>, InuCommon::CInuModel>& inuModels = inuModelDB->GetAllModels();
	std::map<std::pair<std::string,std::string>, InuCommon::CInuModel>::const_iterator itr;

	jclass mapClass = (jclass)env->FindClass("java/util/HashMap");
	if(mapClass == NULL)
	{
		LOGE("JNI:Can't find HashMap class.");
		return false;
	}

	// HashMap
	jmethodID initm = env->GetMethodID(mapClass, "<init>", "()V");//HashMap()
	jobject hashMap = env->NewObject(mapClass, initm);
	jmethodID put = env->GetMethodID(mapClass, "put", "(Ljava/lang/Object;Ljava/lang/Object;)Ljava/lang/Object;");
	jclass modclass = (jclass) env->FindClass("InuCommon/InuModel");
	jmethodID constructor = env->GetMethodID(modclass, "<init>", "()V");

	// Iterate on the models and initialize the Java properties
	for(itr = inuModels.begin(); itr != inuModels.end(); itr++)
	{
        auto msg = itr->first.first.c_str();
#ifdef __ANDROID__
		LOGE("%s", itr->first.first.c_str());
#else
        LOGE(itr->first.first.c_str());
#endif
		jobject model = env->NewObject(modclass,constructor );
		jstring name = env->NewStringUTF(itr->first.first.c_str());
		CInuModelJNI managedModel(env, model, itr->second);

		env->CallObjectMethod(hashMap, put, name, (jobject)model);
		env->DeleteLocalRef(name);
		env->DeleteLocalRef(model);
	}
	LOGE("SensorsDB set up.");

    jfieldID fid = env->GetFieldID(env->FindClass("InuCommon/InuModelDB"), "mInuModels", "Ljava/util/HashMap;");
    env->SetObjectField(thiz,fid, (jobject)hashMap);
	LOGE("SensorsDB assigned");

	//env->DeleteLocalRef(model);
	return true;

}

