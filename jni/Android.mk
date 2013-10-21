LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

include ../includeOpenCV.mk

ifeq ("$(wildcard $(OPENCV_MK_PATH))","")

include ../../OpenCV-2.3.1/share/OpenCV/OpenCV.mk

else

include $(OPENCV_MK_PATH)

endif

LOCAL_MODULE    := ProcessImage
LOCAL_SRC_FILES := ProcessImage.cpp

include $(BUILD_SHARED_LIBRARY)