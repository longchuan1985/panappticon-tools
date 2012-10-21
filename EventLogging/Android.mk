LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE_TAGS := optional

LOCAL_STATIC_JAVA_LIBRARIES := \
        android-support-v13 \
        android-support-v4 \

LOCAL_SRC_FILES := \
        $(call all-java-files-under, src) 

LOCAL_JNI_SHARED_LIBRARIES := libget_time

LOCAL_PACKAGE_NAME := EventLogging



include $(BUILD_PACKAGE)

include $(call all-makefiles-under, &(LOCAL_PATH))
