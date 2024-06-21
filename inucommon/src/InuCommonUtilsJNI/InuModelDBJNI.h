/*
 * InuModelDBJNI.h
 *
 *  Created on: December 7, 2016
 *      Author: Olga
 */

#ifndef INUMODELDBJNI_H_
#define INUMODELDBJNI_H_

#include <jni.h>

#ifdef __cplusplus
extern "C" {
#endif

	//InuCommon::InuModelDB

	JNIEXPORT jboolean JNICALL Java_InuCommon_InuModelDB_init
		(JNIEnv *, jobject);


#ifdef __cplusplus
}



#endif
#endif /* INUMODELDBJNI_H_ */
