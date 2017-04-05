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

set(SOURCES src/logger.hpp src/logger.cpp src/BlurDetection.hpp
        src/BlurDetection.cpp src/Thinning.cpp src/Thinning.hpp src/Smoothing.cpp src/Smoothing.hpp
        src/ColorBalance.cpp src/ColorBalance.hpp src/Warp.cpp src/Warp.hpp
        src/Deskew.cpp src/Deskew.hpp src/Cropping.cpp src/Cropping.hpp src/Binarization.cpp
        src/Binarization.hpp src/Denoising.cpp src/Denoising.hpp src/SauvolaBinarization.cpp
        src/SauvolaBinarization.h src/Deblur.cpp src/Deblur.hpp src/Rotate.cpp src/Rotate.hpp
        src/FormatConvert.cpp src/FormatConvert.hpp src/Compatibility.hpp src/Resize.cpp
        src/Resize.hpp)



set(CMAKE_BUILD_TYPE Debug)


include_directories( ${CMAKE_BINARY_DIR} ${PROJECT_SOURCE_DIR}/src)

add_library(prlib SHARED ${SOURCES})

find_package(OpenCV)

target_link_libraries(prlib
        ${OpenCV_LIBS}
        -llept
        -fPIC
        #liblibjpeg.a
        #[[-lzlib
        -ltiff.a
        -lippicv.a
        liblibwebp.a
        liblibjasper.a

        -ljpeg
        -lpng
        -lgif

        -lgomp
        -fPIC
        -lpthread]])

#[[target_link_libraries(prlib
        ${OpenCV_LIBSS}
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
        -lpthread)]]



