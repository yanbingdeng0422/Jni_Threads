LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE_TAGS := optional

LOCAL_SRC_FILES := \
	$(call all-java-files-under, src)

LOCAL_PACKAGE_NAME := JNI_Threads
LOCAL_CERTIFICATE := platform

#LOCAL_SHARED_LIBRARIES := hello-jni

LOCAL_STATIC_ANDROID_LIBRARIES := \
        android-support-v4


LOCAL_JNI_SHARED_LIBRARIES := libjni_threads

LOCAL_PROGUARD_ENABLED := disabled
LOCAL_SDK_VERSION := current

LOCAL_DEX_PREOPT := false
include $(BUILD_PACKAGE)

LOCAL_CPPFLAGS += -std=c++11

include $(call all-makefiles-under,$(LOCAL_PATH))
