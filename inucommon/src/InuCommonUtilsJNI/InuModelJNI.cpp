/*
 * InuModelJNI.cpp
 *
 *  Created on: December 1, 2016
 *      Author: Olga
 */
#include "InuModelJNI.h"
#include "InuModel.h"
#ifdef __ANDROID__
#else
#include <cstring>
#include <iostream>
#endif
#include "JniWrapper/JniWrapper.h"
#include "Logger.h"

CInuModelJNI::CInuModelJNI( JNIEnv * env ,  jobject thiz, const InuCommon::CInuModel& nativemodel) :
		CJavaObject(env, thiz),
		modelName(this, "modelName", "Ljava/lang/String;"),
//		imgStreamChannels(this, "imgStreamChannels", "Ljava/lang/String;"),
		defaultVideoFPS(this, "defaultVideoFPS", "J"),
		defaultWebCamFPS(this, "defaultWebCamFPS", "J"),
//		maxETvalue(this, "maxETvalue", "J"),
		FWEnumerator(this, "FWEnumerator", "J"),
		defaultInterleavedMode(this, "defaultInterleavedMode", "Z"),
//		defaultMirrorMode(this, "defaultMirrorMode", "Z"),
		defaultAutoControl(this, "defaultAutoControl", "Z"),
//		multiChannelsIsSupported(this, "multiChannelsIsSupported", "Z"),
//		featuresTrackingSupport(this, "featuresTrackingSupport", "Z"),
		defaultDepthResolution(this, "defaultDepthResolution", "LInuCommon/ESensorResolution;"),
		defaultWebCamResolution(this, "defaultWebCamResolution", "LInuCommon/ESensorResolution;")
//		defaultVideoFormat(this, "defaultVideoFormat", "LInuCommon/ETiffFormat;"),
//		defaultWebcamFormat(this, "defaultWebcamFormat", "LInuCommon/ETiffFormat;")
{
	Copy(env, nativemodel);
}


CInuModelJNI::CInuModelJNI(JNIEnv * env, std::string className, const InuCommon::CInuModel& nativemodel, jobject iClassLoader) :
		CJavaObject(env, className, iClassLoader),
		modelName(this, "modelName", "Ljava/lang/String;"),
//		imgStreamChannels(this, "imgStreamChannels", "Ljava/lang/String;"),
		defaultVideoFPS(this, "defaultVideoFPS", "J"),
		defaultWebCamFPS(this, "defaultWebCamFPS", "J"),
//		maxETvalue(this, "maxETvalue", "J"),
		FWEnumerator(this, "FWEnumerator", "J"),
		defaultInterleavedMode(this, "defaultInterleavedMode", "Z"),
//		defaultMirrorMode(this, "defaultMirrorMode", "Z"),
		defaultAutoControl(this, "defaultAutoControl", "Z"),
//		multiChannelsIsSupported(this, "multiChannelsIsSupported", "Z"),
//		featuresTrackingSupport(this, "featuresTrackingSupport", "Z"),
		defaultDepthResolution(this, "defaultDepthResolution", "LInuCommon/ESensorResolution;"),
		defaultWebCamResolution(this, "defaultWebCamResolution", "LInuCommon/ESensorResolution;")
//		defaultVideoFormat(this, "defaultVideoFormat", "LInuCommon/ETiffFormat;"),
//		defaultWebcamFormat(this, "defaultWebcamFormat", "LInuCommon/ETiffFormat;")
{
	Copy(env, nativemodel);
}

void CInuModelJNI::Copy(JNIEnv* env, const InuCommon::CInuModel& nativemodel)
{
	LOGE("CInuModel JNI:copy.");

    jfieldID fid = env->GetFieldID(env->FindClass("InuCommon/InuModel"), "modelName", "Ljava/lang/String;");
    jstring javaString = env->NewStringUTF(nativemodel.GetModelName().c_str());
    env->SetObjectField(mActualObject,fid, javaString);
    env->DeleteLocalRef(javaString);

//    jfieldID imgStreamChannels = env->GetFieldID(env->FindClass("InuCommon/InuModel"), "imgStreamChannels", "Ljava/lang/String;");
//    javaString = env->NewStringUTF(nativemodel.GetImageStreamsChannels().c_str());
//    env->SetObjectField(mActualObject,imgStreamChannels, javaString);
//    env->DeleteLocalRef(javaString);

	(*this)["defaultVideoFPS"]               = (long long)nativemodel.GetDefaultFPS(-1);
	(*this)["defaultWebCamFPS"]               = (long long)nativemodel.GetDefaultFPS(-1);
//	(*this)["maxETvalue"]               = (long long)nativemodel.GetExposureTimeMaxValue();
	(*this)["FWEnumerator"]             = (long long)(0);//(long long)nativemodel.GetFWEnumerator();
	LOGE("CInuModel long values copied");

	//(*this)["defaultInterleavedMode"]   = (bool)nativemodel.GetDefaultInterleaved();
//	(*this)["defaultMirrorMode"]        = (bool)nativemodel.GetDefaultMirrorMode();
	(*this)["defaultAutoControl"]       = (bool)nativemodel.GetDefaultAutoControl();
//	(*this)["multiChannelsIsSupported"] = (bool)nativemodel.GetMultiChannelsIsSupported();
//	(*this)["featuresTrackingSupport"]  = (bool)nativemodel.GetFeaturesTrackingSupport();
	LOGE("CInuModel boolean values copied.");

	jclass res = (jclass)env->FindClass("InuCommon/ESensorResolution");
	jmethodID getMethod = env->GetStaticMethodID(res, "forValue", "(I)LInuCommon/ESensorResolution;");

	jclass tiff = (jclass)env->FindClass("InuCommon/ETiffFormat");
	jmethodID getMethodTiff = env->GetStaticMethodID(tiff, "forValue", "(I)LInuCommon/ETiffFormat;");

	fid = env->GetFieldID(env->FindClass("InuCommon/InuModel"), "defaultDepthResolution", "LInuCommon/ESensorResolution;");
    jint value = (int)(nativemodel.GetDefaultResolution(-1));
    jobject jres = env->CallStaticObjectMethod(res, getMethod, (int)value);
    env->SetObjectField(mActualObject,fid, (jobject)jres);

	fid = env->GetFieldID(env->FindClass("InuCommon/InuModel"), "defaultWebCamResolution", "LInuCommon/ESensorResolution;");
    value = (int)(nativemodel.GetDefaultResolution(-1));
    jres = env->CallStaticObjectMethod(res, getMethod, (int)value);
    env->SetObjectField(mActualObject,fid, (jobject)jres);

//	fid = env->GetFieldID(env->FindClass("InuCommon/InuModel"), "defaultVideoFormat", "LInuCommon/ETiffFormat;");
//    value = (int)(nativemodel.GetDefaultVideoformat());
//    jres = env->CallStaticObjectMethod(tiff, getMethodTiff, (int)value);
//    env->SetObjectField(mActualObject,fid, (jobject)jres);
//
//	fid = env->GetFieldID(env->FindClass("InuCommon/InuModel"), "defaultWebcamFormat", "LInuCommon/ETiffFormat;");
//    value = (int)(nativemodel.GetDefaultWebcamformat());
//    jres = env->CallStaticObjectMethod(tiff, getMethodTiff, (int)value);
//    env->SetObjectField(mActualObject,fid, (jobject)jres);

//	CJavaObject managedVideoRes(env, (jobject)(*this)["defaultDepthResolution"]["LInuCommon/ESensorResolution;"]);
//	managedVideoRes["mCode"] =  (int)nativemodel.GetDefaultDepthResolution();

//	CJavaObject managedWebCamRes(env, (jobject)(*this)["defaultWebCamResolution"]["LInuCommon/ESensorResolution;"]);
//	managedWebCamRes["mCode"] =  (int)nativemodel.GetDefaultRGBResolution();

//	CJavaObject managedVideoFormat(env, (jobject)(*this)["defaultVideoFormat"]["LInuCommon/ETiffFormat;"]);
//	managedVideoFormat["mCode"] =  (int)nativemodel.GetDefaultVideoformat();
//
//	CJavaObject managedCamFormat(env, (jobject)(*this)["defaultWebcamFormat"]["LInuCommon/ETiffFormat;"]);
//	managedCamFormat["mCode"] =  (int)nativemodel.GetDefaultWebcamformat();

	LOGE("CInuModel copy finished");
}


