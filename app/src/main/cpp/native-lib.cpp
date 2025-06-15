#include <jni.h>
#include <string>
#include <android/log.h>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include "EdgeDetector.h"

#define LOG_TAG "EdgeDetectionViewer"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

EdgeDetector edgeDetector;

extern "C" JNIEXPORT jbyteArray JNICALL
Java_com_edgedetection_viewer_MainActivity_processFrame(
        JNIEnv *env,
        jobject /* this */,
        jbyteArray inputArray,
        jint width,
        jint height,
        jboolean enableProcessing) {

    // Convert jbyteArray to cv::Mat
    jbyte* inputBytes = env->GetByteArrayElements(inputArray, nullptr);
    jsize inputLength = env->GetArrayLength(inputArray);
    
    // Create OpenCV Mat from input data (assuming YUV420 format)
    cv::Mat yuvMat(height + height/2, width, CV_8UC1, (unsigned char*)inputBytes);
    cv::Mat rgbMat;
    cv::cvtColor(yuvMat, rgbMat, cv::COLOR_YUV2RGB_I420);
    
    cv::Mat outputMat;
    
    if (enableProcessing) {
        // Apply edge detection
        outputMat = edgeDetector.processFrame(rgbMat);
    } else {
        // Return original frame
        outputMat = rgbMat.clone();
    }
    
    // Convert back to byte array
    std::vector<uchar> outputBuffer;
    cv::imencode(".jpg", outputMat, outputBuffer);
    
    jbyteArray outputArray = env->NewByteArray(outputBuffer.size());
    env->SetByteArrayRegion(outputArray, 0, outputBuffer.size(), (jbyte*)outputBuffer.data());
    
    // Release input array
    env->ReleaseByteArrayElements(inputArray, inputBytes, JNI_ABORT);
    
    return outputArray;
}

extern "C" JNIEXPORT jstring JNICALL
Java_com_edgedetection_viewer_MainActivity_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {
    std::string hello = "Hello from C++ with OpenCV " + cv::getVersionString();
    return env->NewStringUTF(hello.c_str());
}

