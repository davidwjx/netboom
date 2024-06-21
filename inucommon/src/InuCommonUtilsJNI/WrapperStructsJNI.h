#ifndef WRAPPERSTRUCTSJNI_H
#define WRAPPERSTRUCTSJNI_H

#include "JniWrapper/JniWrapper.h"

class CPoint2DJNI : public CJavaObject
{
public:

    CPoint2DJNI(JNIEnv* env, jclass iClassLoader = NULL) :
        CJavaObject(env, "InuDev/Point2D", iClassLoader),
        x(this, "x", "I"),
        y(this, "y", "I")
    {
   
    }

    CPoint2DJNI(JNIEnv* env, jobject obj) : 
        CJavaObject(env, obj),
        x(this, "x", "I"),
        y(this, "y", "I")
    {

    }

    CJavaMember x;
    CJavaMember y;
};

class CPoint3DJNI : public CJavaObject
{
public:

    CPoint3DJNI(JNIEnv* env, jclass iClassLoader = NULL) :
        CJavaObject(env, "InuDev/Point3D", iClassLoader),
        x(this, "x", "F"),
        y(this, "y", "F"),
        z(this, "z", "F")
    {

    }

    CPoint3DJNI(JNIEnv* env, jobject obj) : 
        CJavaObject(env, obj),
        x(this, "x", "F"),
        y(this, "y", "F"),
        z(this, "z", "F")
    {

    }

    CJavaMember x;
    CJavaMember y;
    CJavaMember z;
};

class CPoint4DJNI : public CJavaObject
{
public:

    CPoint4DJNI(JNIEnv* env, jclass iClassLoader = NULL) :
        CJavaObject(env, "InuDev/Point4D", iClassLoader),
        x(this, "x", "I"),
        y(this, "y", "I"),
        z(this, "z", "I"),
        w(this, "w", "I")
    {

    }

    CPoint4DJNI(JNIEnv* env, jobject obj) : 
        CJavaObject(env, obj),
        x(this, "x", "I"),
        y(this, "y", "I"),
        z(this, "z", "I"),
        w(this, "w", "I")
    {

    }

    CJavaMember x;
    CJavaMember y;
    CJavaMember z;
    CJavaMember w;
};

class CRect2DJNI : public CJavaObject
{
public:

	CRect2DJNI(JNIEnv* env, jclass iClassLoader = NULL) :
        CJavaObject(env, "InuDev/Rect2D", iClassLoader),
        x(this, "x", "I"),
        y(this, "y", "I"),
		height(this, "height", "I"),
		width(this, "width", "I")
    {

    }

	CRect2DJNI(JNIEnv* env, jobject obj) :
        CJavaObject(env, obj),
        x(this, "x", "I"),
        y(this, "y", "I"),
		height(this, "height", "I"),
		width(this, "width", "I")
    {

    }

    CJavaMember x;
    CJavaMember y;
    CJavaMember height;
    CJavaMember width;
};

//class CExposureParamsJNI : public CJavaObject
//{
//public:
//
//	CExposureParamsJNI(JNIEnv* env, bool isloader, jobject iClassLoader = NULL) :
//        CJavaObject(env, "InuDev/ExposureParams", iClassLoader),
//		digitalGainRight(this, "digitalGainRight", "I"),
//		digitalGainLeft(this, "digitalGainLeft", "I"),
//		analogGainRight(this, "analogGainRight", "I"),
//		analogGainLeft(this, "analogGainLeft", "I"),
//		exposureTimeLeft(this, "exposureTimeLeft", "I"),
//		exposureTimeRight(this, "exposureTimeRight", "I")
//    {
//
//    }
//
//	CExposureParamsJNI(JNIEnv* env, jobject obj, int i) :
//        CJavaObject(env, obj),
//		digitalGainRight(this, "digitalGainRight", "I"),
//		digitalGainLeft(this, "digitalGainLeft", "I"),
//		analogGainRight(this, "analogGainRight", "I"),
//		analogGainLeft(this, "analogGainLeft", "I"),
//		exposureTimeLeft(this, "exposureTimeLeft", "I"),
//		exposureTimeRight(this, "exposureTimeRight", "I")
//    {
//
//    }
//
//    CJavaMember digitalGainRight;
//    CJavaMember digitalGainLeft;
//    CJavaMember analogGainRight;
//    CJavaMember analogGainLeft;
//    CJavaMember exposureTimeLeft;
//    CJavaMember exposureTimeRight;
//
//};

class CDepthFilterParamsJNI : public CJavaObject
{
public:

	CDepthFilterParamsJNI(JNIEnv* env, jclass iClassLoader = NULL) :
        CJavaObject(env, "InuDev/DepthFilterParams", iClassLoader),
		filterType(this, "filterType", "LInuDev/EDepthInputTypes;"),
		colorType(this, "colorType", "LInuDev/EDepthColorTypes;")
    {

    }

	CDepthFilterParamsJNI(JNIEnv* env, jobject obj) :
        CJavaObject(env, obj),
		filterType(this, "filterType", "LInuDev/EDepthInputTypes;"),
		colorType(this, "colorType", "LInuDev/EDepthColorTypes;")
    {

    }

    CJavaMember filterType;
    CJavaMember colorType;
};

class CImageFrameParamsJNI : public CJavaObject
{
public:

	CImageFrameParamsJNI(JNIEnv* env, jclass iClassLoader = NULL) :
        CJavaObject(env, "InuDev/ImageFrameParams", iClassLoader),
		channelId(this, "channelId", "I"),
		frameStartX(this, "frameStartX", "I"),
		frameStartY(this, "frameStartY", "I"),
		frameWidth(this, "frameWidth", "I"),
		frameHeight(this, "frameHeight", "I")
    {

    }

	CImageFrameParamsJNI(JNIEnv* env, jobject obj) :
        CJavaObject(env, obj),
		channelId(this, "channelId", "I"),
		frameStartX(this, "frameStartX", "I"),
		frameStartY(this, "frameStartY", "I"),
		frameWidth(this, "frameWidth", "I"),
		frameHeight(this, "frameHeight", "I")
    {

    }

    CJavaMember channelId;
    CJavaMember frameStartX;
    CJavaMember frameStartY;
    CJavaMember frameWidth;
    CJavaMember frameHeight;
};

class CVideoInitParamsJNI : public CImageFrameParamsJNI
{
public:

	CVideoInitParamsJNI(JNIEnv* env, jclass iClassLoader = NULL) :
		CImageFrameParamsJNI(env, iClassLoader),
		isInterleaved(this, "isInterleaved", "Z")
    {

    }

	CVideoInitParamsJNI(JNIEnv* env, jobject obj) :
		CImageFrameParamsJNI(env, obj),
		isInterleaved(this, "isInterleaved", "Z")
    {

    }

    CJavaMember isInterleaved;
};

class CEntityVersionJNI : public CJavaObject
{
public:

	CEntityVersionJNI(JNIEnv* env, jclass iClassLoader = NULL) :
        CJavaObject(env, "InuDev/EEntitiesID", iClassLoader),
		id(this, "id", "LInuDev/EEntitiesID;"),
		name(this, "name", "Ljava/lang/String;"),
		versionNum(this, "versionNum", "I"),
		versionString(this, "versionString", "Ljava/lang/String;")
    {
    }

	CEntityVersionJNI(JNIEnv* env, jobject obj) :
        CJavaObject(env, obj),
		id(this, "id", "LInuDev/EEntitiesID;"),
		name(this, "name", "Ljava/lang/String;"),
		versionNum(this, "versionNum", "I"),
		versionString(this, "versionString", "Ljava/lang/String;")
    {

    }

    CJavaMember id;
    CJavaMember name;
    CJavaMember versionNum;
    CJavaMember versionString;
};

class CAgcParamsJNI : public CJavaObject
{
public:

	CAgcParamsJNI(JNIEnv* env, jclass iClassLoader = NULL) :
        CJavaObject(env, "InuDev/AgcParams", iClassLoader),
		intensityAlg(this, "intensityAlg", "LInuDev/EIntensityAlg;"),
		foregroundLevel(this, "foregroundLevel", "LInuDev/EIntensityLevel;"),
		backgroundLevel(this, "backgroundLevel", "LInuDev/EIntensityLevel;"),
		autoExposure(this, "autoExposure", "Z"),
		autoLumaTarget(this, "autoLumaTarget", "Z"),
		lumaTarget(this, "lumaTarget", "B"),
		eliminateFlickering(this, "eliminateFlickering", "Z"),
		powerGridRate(this, "powerGridRate", "LInuDev/EPowerGridRate;"),
		digitalGainLeft(this, "digitalGainLeft", "I"),
		analogGainLeft(this, "analogGainLeft", "I"),
		digitalGainRight(this, "digitalGainRight", "I"),
		analogGainRight(this, "analogGainRight", "I"),
		shutterLeft(this, "shutterLeft", "I"),
		shutterRight(this, "shutterRight", "I")
    {
    }

	CAgcParamsJNI(JNIEnv* env, jobject obj) :
        CJavaObject(env, obj),
		intensityAlg(this, "intensityAlg", "LInuDev/EIntensityAlg;"),
		foregroundLevel(this, "foregroundLevel", "LInuDev/EIntensityLevel;"),
		backgroundLevel(this, "backgroundLevel", "LInuDev/EIntensityLevel;"),
		autoExposure(this, "autoExposure", "Z"),
		autoLumaTarget(this, "autoLumaTarget", "Z"),
		lumaTarget(this, "lumaTarget", "B"),
		eliminateFlickering(this, "eliminateFlickering", "Z"),
		powerGridRate(this, "powerGridRate", "LInuDev/EPowerGridRate;"),
		digitalGainLeft(this, "digitalGainLeft", "I"),
		analogGainLeft(this, "analogGainLeft", "I"),
		digitalGainRight(this, "digitalGainRight", "I"),
		analogGainRight(this, "analogGainRight", "I"),
		shutterLeft(this, "shutterLeft", "I"),
		shutterRight(this, "shutterRight", "I")
    {

    }
    CJavaMember	autoExposure;
    CJavaMember	intensityAlg;
    CJavaMember	foregroundLevel;
    CJavaMember	backgroundLevel;
    CJavaMember	autoLumaTarget;
    CJavaMember	lumaTarget;
    CJavaMember	eliminateFlickering;
    CJavaMember	powerGridRate;
    CJavaMember	digitalGainLeft;
    CJavaMember	analogGainLeft;
    CJavaMember	digitalGainRight;
    CJavaMember	analogGainRight;
    CJavaMember	shutterLeft;
    CJavaMember	shutterRight;
};

#endif
