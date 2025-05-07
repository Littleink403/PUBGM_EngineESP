LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE    := Project
LOCAL_CPPFLAGS += -w -std=c++17 -Wno-error=c++11-narrowing
LOCAL_C_INCLUDES += $(LOCAL_PATH)/include

LOCAL_C_INCLUDES += $(LOCAL_PATH)/Utils
LOCAL_C_INCLUDES += $(LOCAL_PATH)/Engine
FILE_LIST += $(wildcard $(LOCAL_PATH)/*.c*)
LOCAL_SRC_FILES := Hook.cpp
LOCAL_SRC_FILES += syscall.s
LOCAL_LDLIBS := -lm -ldl -lz -llog -landroid 
include $(BUILD_SHARED_LIBRARY)