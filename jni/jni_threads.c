#include "jni.h"
#include <string.h>
#include <utils/Log.h>

#include <pthread.h>

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

static jmethodID gOnNativeMessage = NULL;

static JavaVM* gVm = NULL;
static jobject gObj =NULL;
static void com_tyanbing_jni_threads_MainActivity_nativeInit(JNIEnv* env, jobject obj){
    if(NULL == gOnNativeMessage){
        jclass class = (*env) -> GetObjectClass(env,obj);

        gOnNativeMessage = (*env)->GetMethodID(env,class,"onNativeMessage","(Ljava/lang/String;)V");

        if(NULL == gOnNativeMessage){
            jclass exceptionClass = (*env) ->FindClass(env,"java/lang/RuntimeExcetion");
            (*env) -> ThrowNew(env,exceptionClass,"Unable to find method");
        }
    }
}
static void com_yanbing_jni_threads_MainActivity_nativeFree(JNIEnv* env, jobject obj){

}

static void com_yanbing_jni_threads_MainActivity_nativeWorker(JNIEnv* env, jobject obj,jint id ,jint iterations){
    for(jint i=0;i<iterations;i++){
        char message[26];
        sprintf(message, "Worker %d:Iteration %d",id,i);
        jstring messageString = (*env)->NewStringUTF(env,message);
        (*env) -> CallVoidMethod(env,obj, gOnNativeMessage,messageString);
        if(NULL != (*env) ->ExceptionOccurred(env)){
            break;
        }
        sleep(1);
    }
}

struct NativeWorkerArgs{
    jint id;
    jint iterations;
}



static void com_yanbing_jni_threads_MainActivity_posixThreads(JNIEnv* env, jobject obj,jint id ,jint iterations){


}

static JNINativeMethod methods[] = {
  {"stringFromJNI", "()Ljava/lang/String;", (void*)com_yanbing_jni_threads_MainActivity_stringFromJNI },
  {"nativeInit","()V",(void*)com_tyanbing_jni_threads_MainActivity_nativeInit},
  {"nativeFree","()V",(void*)com_yanbing_jni_threads_MainActivity_nativeFree},
  {"nativeWorker","(II)V",(void*)com_yanbing_jni_threads_MainActivity_nativeWorker},
  {"posixThreads","(II)V",(void*)com_yanbing_jni_threads_MainActivity_posixThreads},
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