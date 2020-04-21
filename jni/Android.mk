#yanbing add
#
#Android.mk必须以LOCAL_PATH变量的定义开头
#android构建系统利用LOCAL_PATH来定位源文件，因为将该变量设置为硬编码值并不合适，
#所以android构建系统提供了一个名为 my-dir 的宏功能；将该变量设置为 my-dir 宏功能的返回值，可以将其放在当前目录下；
LOCAL_PATH := $(call my-dir)

#Android构建系统将CLEAR_VARS变量设置为clear-vars.mk片段的位置；
#包含Makefile片段可以清除除了LOCAL_PATH以外的LOCAL_<name>变量；例如LOCAL_MODULE与LOCAL_SRC_FILES等
include $(CLEAR_VARS)

#LOCAL_MODULE变量用来给模块设定一个唯一的名称
#hello-jni模块会生成一个共享文件且构建系统会将他命名为libhello-jni
LOCAL_MODULE_TAGS := jni-threads
LOCAL_MODULE := libjni_threads

#LOCAL_SRC_FILES变量定义用来建立和组装这个模块的源文件列表
LOCAL_SRC_FILES := jni_threads.c

LOCAL_SHARED_LIBRARIES := \
     liblog \
     libcutils \
     libandroid_runtime \
     libnativehelper \
     libbinder

LOCAL_C_INCLUDES += \
    $(JNI_H_INCLUDE)

LOCAL_MULTILIB := 64

LOCAL_CXX_STL := none
#为了建立可供主应用程序使用的模块，必须将该模块变成共享库
#Android NDK 构建系统将BUILD_SHARED_LIBRARY变量设置成build-shared-library.mk文件的保存位置。
#该Makefile片段包含了将源文件构建和组装成共享库的必要过程
include $(BUILD_SHARED_LIBRARY)
