cmake_minimum_required(VERSION 3.18.1)

project("native-lib")

# Include OpenCV
include_directories(${OpenCV_DIR}/jni/include)
add_library( lib_opencv SHARED IMPORTED )
set_target_properties(lib_opencv PROPERTIES IMPORTED_LOCATION ${OpenCV_DIR}/libs/${ANDROID_ABI}/libopencv_java4.so)

# Add our native library
add_library(
    native-lib
    SHARED
    native-lib.cpp
    EdgeDetector.cpp
    GLRenderer.cpp
)

# Search for required libraries
find_library(
    log-lib
    log
)

find_library(
    android-lib
    android
)

find_library(
    gles-lib
    GLESv2
)

find_library(
    egl-lib
    EGL
)

# Link libraries
target_link_libraries(
    native-lib
    lib_opencv
    ${log-lib}
    ${android-lib}
    ${gles-lib}
    ${egl-lib}
)

