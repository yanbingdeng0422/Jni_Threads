#include <jni.h>
#include <string.h>
#include <utils/Log.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>

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

struct NativeWorkerArgs{
    jint id;
    jint iterations;
};

static jstring com_yanbing_jni_threads_MainActivity_stringFromJNI(JNIEnv* env, jobject obj)
{
    return (*env)->NewStringUTF(env,"Hello from C++");
}

static jmethodID gOnNativeMessage = NULL;

static JavaVM* gVm = NULL;
static jobject gObj =NULL;
//互斥实例
static pthread_mutex_t mutex;

static void com_tyanbing_jni_threads_MainActivity_nativeInit(JNIEnv* env, jobject obj){
    //初始化互斥锁
    if(0!= pthread_mutex_init(&mutex,NULL)){
        jclass exceptionClass = (*env)->FindClass("java/lang/RuntimeException");
        (*env) -> ThrowNew(env,exceptionClass,"Unable to initalize mutex");
        goto exit;
    }
    //如果对象的全局引用为设置
    if(NULL == gObj){
        //为对象创建一个新的全局引用
        gObj = (*env)->NewGlobalRef(env,obj);
        if(NULL == gObj){
            goto exit;
        }
    }

    if(NULL == gOnNativeMessage){
        jclass class = (*env) -> GetObjectClass(env,obj);

        gOnNativeMessage = (*env)->GetMethodID(env,class,"onNativeMessage","(Ljava/lang/String;)V");

        if(NULL == gOnNativeMessage){
            jclass exceptionClass = (*env) ->FindClass(env,"java/lang/RuntimeExcetion");
            (*env) -> ThrowNew(env,exceptionClass,"Unable to find method");
        }
    }

    exit:
        return;
}
static void com_yanbing_jni_threads_MainActivity_nativeFree(JNIEnv* env, jobject obj){
    //删除全局引用
    if(NULL != gObj){
        (*env) -> DeleteGlobalRef(env,gObj);
        gObj = NULL;
    }

    if(0!=pthread_mutex_destroy(&mutex)){
        //获取异常类
        jclass exceptionClass =(*env)->FindClass(env,"java/lang/RuntimeException");
        //抛出异常
        (*env)->ThrowNew(env,exceptionClass,"Unable to destroy mutex");
    }
}



static void com_yanbing_jni_threads_MainActivity_nativeWorker(JNIEnv* env, jobject obj,jint id ,jint iterations){
    //锁定互斥锁
    if(0 != pthread_mutex_lock(&mutex)){
        //获取异常类
        jclass exceptionClass =(*env)->FindClass(env,"java/lang/RuntimeException");
        //抛出异常
        (*env)->ThrowNew(env,exceptionClass,"Unable to lock mutex");
        goto exit;
    }

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
    //解锁互斥锁
    if(0 != pthread_mutex_unlock(&mutex)){
        //获取异常类
        jclass exceptionClass =(*env)->FindClass(env,"java/lang/RuntimeException");
        //抛出异常
        (*env)->ThrowNew(env,exceptionClass,"Unable to unlock mutex");
        goto exit;
    }

    exit:
        return;
}

static void* nativeWorkerThread(void* args){
    JNIEnv* env = NULL;
    //将当前线程附加到Java虚拟机上
    //并且获得JNIEnv接口指针
    if(0 == (*gVm)->AttachCurrentThread(gVm,&env,NULL)){
        //获取原生worker线程参数
        struct NativeWorkerArgs* nativeWorkerArgs = (struct NativeWorkerArgs*) args;
        //在线程上下文中运行原生worker
        com_yanbing_jni_threads_MainActivity_nativeWorker(env,gObj,nativeWorkerArgs->id,nativeWorkerArgs->iterations);

        //释放原生worker线程参数
        //delete nativeWorkerArgs;
        //从java虚拟机中分离当前线程
        (*gVm) -> DetachCurrentThread(gVm);
    }
    return (void *)1;
}

static void com_yanbing_jni_threads_MainActivity_posixThreads(JNIEnv* env, jobject obj,jint threads ,jint iterations){

    pthread_t handles[threads];

    for(jint i=0;i<threads;i++){
        struct NativeWorkerArgs* nativeWorkerArgs =  (struct NativeWorkerArgs*)malloc(sizeof(struct NativeWorkerArgs));
        nativeWorkerArgs->id = i;
        nativeWorkerArgs->iterations =iterations;

       // pthread_t thread;
        int result = pthread_create(&handles[i],NULL,nativeWorkerThread,(void*)nativeWorkerArgs);
        if(0!= result){
            //获取异常类
            jclass execptionClass = (*env)->FindClass(env,"java/lang/RuntimeException");

            //抛出异常
            (*env)->ThrowNew(env,execptionClass,"Unable to create thread");
            goto exit;
        }
    }

    for(jint i=0;i<threads;i++){
        void* result =NULL;

        if(0 !=pthread_join(handles[i],&result)){
            //获取异常类
            jclass exectionClass = (*env)->FindClass(env,"java/lang/RuntimeException");
            (*env)->ThrowNew(env,exectionClass,"Unable to join thread");
        }else{
            char message[26];
            sprintf(message,"Worker %d returned %d",i,result);
            jstring messageString =(*env)->NewStringUTF(env,message);

            (*env) -> CallVoidMethod(env,obj,gOnNativeMessage,messageString);

            if(NULL != (*env) ->ExceptionOccurred(env)){
                goto exit;
            }
        }
    }

    exit:
        return;
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

    gVm = vm;

    result = JNI_VERSION_1_4;

bail:
    return result;
}