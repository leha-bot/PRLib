cmake_minimum_required(VERSION 3.6.0)
project(prlib)

#set( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -O2 -g -fsanitize=address -fno-omit-frame-pointer -shared-libasan" )
set( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS}" )

include_directories(include/opencv include/opencv/opencv2 include/spdlog)

link_directories(lib/opencv lib/libjpeg lib/libpng lib/libtiff lib/zlib lib/libjasper
        lib/libwebp lib/libippcv lib/libprotobuf lib/leptonica lib/libgif)


set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_CXX_STANDARD 14)

set(OpenCV_LIBSS libopencv_ximgproc.a libopencv_xphoto.a libopencv_shape.a libopencv_imgcodecs.a libopencv_ts.a
        libopencv_videostab.a libopencv_photo.a libopencv_calib3d.a
        libopencv_video.a libopencv_stitching.a
        libopencv_videoio.a libopencv_superres.a
        libopencv_ml.a libopencv_objdetect.a  libopencv_features2d.a
        libopencv_flann.a libopencv_imgproc.a libopencv_highgui.a libopencv_core.a)

set(SOURCES src/core/GeneralImage.cpp src/core/GeneralImage.hpp src/core/cvmatandqimage.cpp src/core/cvmatandqimage.h src/logger.hpp src/logger.cpp src/core/BlurDetection.hpp
    src/core/BlurDetection.cpp src/core/Thinning.cpp src/core/Thinning.hpp src/core/Smoothing.cpp src/core/Smoothing.hpp src/core/ColorBalance.cpp src/core/ColorBalance.hpp src/core/Warp.cpp src/core/Warp.hpp 
    src/core/Deskew.cpp src/core/Deskew.hpp src/core/Cropping.cpp src/core/Cropping.hpp src/core/Binarization.cpp src/core/Binarization.hpp src/SystemTray.cpp src/SystemTray.hpp src/core/Denoising.cpp src/core/Denoising.hpp)



set(CMAKE_BUILD_TYPE Debug)


include_directories( ${CMAKE_BINARY_DIR} ${PROJECT_SOURCE_DIR}/src)

add_library(prlib SHARED ${SOURCES})

target_link_libraries(prlib ${OpenCV_LIBSS}
        libleptonica.a

        #liblibjpeg.a
        libzlib.a
        liblibtiff.a
        libippicv.a
        liblibwebp.a
        liblibjasper.a

        libjpeg.a
        liblibpng.a
        libgif.a

        -lgomp
        -fPIC
        -lpthread)


