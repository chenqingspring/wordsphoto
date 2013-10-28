#include <jni.h>
#include "processc.h"
extern "C" {

JNIEXPORT jboolean JNICALL Java_com_wordsphoto_object_model_bitmap_utils_LibImageProcess_imagePathFromJNI(

JNIEnv *env, jobject obj, jobject matrixList) {

	jboolean isProcessSuccess = wordPhotoProcess(env, obj, matrixList);

	return isProcessSuccess;
}

}

