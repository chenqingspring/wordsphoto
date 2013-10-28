LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

include OpenCV-2.3.1/share/OpenCV/OpenCV.mk


LOCAL_MODULE    := ProcessImage
LOCAL_SRC_FILES := ProcessImage.cpp \
					processc.cpp
LOCAL_LDLIBS := -llog

include $(BUILD_SHARED_LIBRARY)