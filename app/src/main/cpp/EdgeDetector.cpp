#include "EdgeDetector.h"
#include <android/log.h>

#define LOG_TAG "EdgeDetector"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

EdgeDetector::EdgeDetector() 
    : m_lowThreshold(50.0)
    , m_highThreshold(150.0)
    , m_gaussianKernelSize(5) {
    LOGI("EdgeDetector initialized");
}

EdgeDetector::~EdgeDetector() {
    LOGI("EdgeDetector destroyed");
}

cv::Mat EdgeDetector::processFrame(const cv::Mat& inputFrame) {
    if (inputFrame.empty()) {
        LOGE("Input frame is empty");
        return cv::Mat();
    }
    
    try {
        // Convert to grayscale
        cv::Mat grayFrame = convertToGrayscale(inputFrame);
        
        // Apply Gaussian blur to reduce noise
        cv::Mat blurredFrame = applyGaussianBlur(grayFrame);
        
        // Apply Canny edge detection
        cv::Mat edgeFrame = applyCannyEdgeDetection(blurredFrame);
        
        // Convert back to 3-channel for display
        cv::Mat outputFrame;
        cv::cvtColor(edgeFrame, outputFrame, cv::COLOR_GRAY2RGB);
        
        return outputFrame;
        
    } catch (const cv::Exception& e) {
        LOGE("OpenCV exception in processFrame: %s", e.what());
        return inputFrame.clone();
    } catch (const std::exception& e) {
        LOGE("Standard exception in processFrame: %s", e.what());
        return inputFrame.clone();
    }
}

void EdgeDetector::setCannyThresholds(double lowThreshold, double highThreshold) {
    m_lowThreshold = lowThreshold;
    m_highThreshold = highThreshold;
    LOGI("Canny thresholds set to: low=%.1f, high=%.1f", lowThreshold, highThreshold);
}

void EdgeDetector::setGaussianBlurKernel(int kernelSize) {
    // Ensure kernel size is odd and positive
    if (kernelSize % 2 == 0) kernelSize++;
    if (kernelSize < 3) kernelSize = 3;
    
    m_gaussianKernelSize = kernelSize;
    LOGI("Gaussian blur kernel size set to: %d", kernelSize);
}

cv::Mat EdgeDetector::convertToGrayscale(const cv::Mat& inputFrame) {
    cv::Mat grayFrame;
    
    if (inputFrame.channels() == 3) {
        cv::cvtColor(inputFrame, grayFrame, cv::COLOR_RGB2GRAY);
    } else if (inputFrame.channels() == 4) {
        cv::cvtColor(inputFrame, grayFrame, cv::COLOR_RGBA2GRAY);
    } else {
        grayFrame = inputFrame.clone();
    }
    
    return grayFrame;
}

cv::Mat EdgeDetector::applyGaussianBlur(const cv::Mat& inputFrame) {
    cv::Mat blurredFrame;
    cv::GaussianBlur(inputFrame, blurredFrame, 
                     cv::Size(m_gaussianKernelSize, m_gaussianKernelSize), 
                     0, 0);
    return blurredFrame;
}

cv::Mat EdgeDetector::applyCannyEdgeDetection(const cv::Mat& inputFrame) {
    cv::Mat edgeFrame;
    cv::Canny(inputFrame, edgeFrame, m_lowThreshold, m_highThreshold);
    return edgeFrame;
}

