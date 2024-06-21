
#ifndef INUMODELJNI_H
#define INUMODELJNI_H


#include <jni.h>
#include <InuModel.h>
#include "JniWrapper/JniWrapper.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}

class CInuModelJNI : public CJavaObject
{
public :

	CInuModelJNI(JNIEnv * env, std::string className, const InuCommon::CInuModel& nativemodel, jobject iClassLoader = NULL);
	CInuModelJNI(JNIEnv * env, jobject thiz, const InuCommon::CInuModel& nativemodel);

protected:
    void Copy(JNIEnv* env, const InuCommon::CInuModel& nativemodel);

	CJavaMember modelName;
//	CJavaMember imgStreamChannels;
    CJavaMember defaultVideoFPS;
    CJavaMember defaultWebCamFPS;
//	CJavaMember maxETvalue;
	CJavaMember FWEnumerator;

	CJavaMember defaultInterleavedMode;
//	CJavaMember defaultMirrorMode;
	CJavaMember defaultAutoControl;
//	CJavaMember multiChannelsIsSupported;
//	CJavaMember featuresTrackingSupport;

	CJavaMember defaultDepthResolution;
	CJavaMember defaultWebCamResolution;
//	CJavaMember defaultVideoFormat;
//	CJavaMember defaultWebcamFormat;
};

#endif
#endif //INUMODELJNI_H
