#!/bin/bash

g++ grab-sharpness.cpp -o grab-sharpness `mysql_config --cflags --libs` -std=c++11  \
-I/usr/include/opencv2 \
-I/opt/pylon5/include \
-I/usr/include/tesseract \
-L/usr/local/lib \
-I/usr/local/include/opencv -I/usr/local/include -L/usr/local/lib -lopencv_shape -lopencv_stitching -lopencv_objdetect -lopencv_superres \
-lopencv_video -lopencv_photo -lopencv_ml -lopencv_imgproc -lopencv_flann -lopencv_core \
-L/usr/local/lib -ltesseract \
-Wl,--enable-new-dtags -Wl,-rpath,/opt/pylon5/lib64 \
-L/opt/pylon5/lib64 -Wl,-E -lpylonbase -lpylonutility -lGenApi_gcc_v3_0_Basler_pylon_v5_0 -lGCBase_gcc_v3_0_Basler_pylon_v5_0 \
-lzbar \
-lboost_system -lboost_regex -lboost_filesystem \
-lpthread
