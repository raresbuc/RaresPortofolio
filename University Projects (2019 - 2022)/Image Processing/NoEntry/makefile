Linux := $(findstring Linux, $(shell uname -s))

default: face

%: %.cpp
		g++ $@.cpp /usr/lib64/libopencv_core.so.2.4 /usr/lib64/libopencv_highgui.so.2.4 /usr/lib64/libopencv_imgproc.so.2.4 /usr/lib64/libopencv_objdetect.so.2.4 -std=c++11