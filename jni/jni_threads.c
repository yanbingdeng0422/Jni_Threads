#include "jni.h"
#include <string.h>
#include <utils/Log.h>

#include <stdio.h>

#define LOG_TAG "threads-jni"
/*JNIEXPORT jstring JNICALL
Java_com_yanbing_hello_jni_MainActivity_stringFromJNI(
        JNIEnv* env,
        jobject thiz) {
    //std::string hello = "Hello from C++";
    return (*env)->NewStringUTF(env,"Hello from C++");
}*/
static const char *classPathName = "com.yanbing.jni_threads.MainActivity";

static jstring com_yanbing_jni_threads_MainActivity_stringFromJNI(JNIEnv* env, jobject obj)
{
    return (*env)->NewStringUTF(env,"Hello from C++");
}

static JNINativeMethod methods[] = {
  {"stringFromJNI", "()Ljava/lang/String;", (void*)com_yanbing_jni_threads_MainActivity_stringFromJNI },
};

/*
 * Register several native methods for one class.
 */
static int registerNativeMethods(JNIEnv* env, const char* className,
    JNINativeMethod* gMethods, int numMethods)
{
    jclass clazz;

    clazz = (*env)->FindClass(env,className);
    if (clazz == NULL) {
        ALOGE("Native registration unable to find class '%s'", className);
        return JNI_FALSE;
    }
    if ( (*env)->RegisterNatives(env,clazz, gMethods, numMethods) < 0) {
        ALOGE("RegisterNatives failed for '%s'", className);
        return JNI_FALSE;
    }

    return JNI_TRUE;
}

/*
 * Register native methods for all classes we know about.
 *
 * returns JNI_TRUE on success.
 */
static int registerNatives(JNIEnv* env)
{
  if (!registerNativeMethods(env, classPathName,
                 methods, sizeof(methods) / sizeof(methods[0]))) {
    return JNI_FALSE;
  }

  return JNI_TRUE;
}

typedef union {
    JNIEnv* env;
    void* venv;
} UnionJNIEnvToVoid;

jint JNI_OnLoad(JavaVM* vm, void* reserved)
{
    UnionJNIEnvToVoid uenv;
    uenv.venv = NULL;
    jint result = -1;
    JNIEnv* env = NULL;

    ALOGI("JNI_OnLoad");

    if ((*vm)->GetEnv(vm,&uenv.venv, JNI_VERSION_1_4) != JNI_OK) {
        ALOGE("ERROR: GetEnv failed");
        goto bail;
    }
    env = uenv.env;

    if (registerNatives(env) != JNI_TRUE) {
        ALOGE("ERROR: registerNatives failed");
        goto bail;
    }

    result = JNI_VERSION_1_4;

bail:
    return result;
}