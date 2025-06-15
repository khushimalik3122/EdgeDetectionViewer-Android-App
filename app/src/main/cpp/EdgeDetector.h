#ifndef EDGEDETECTOR_H
#define EDGEDETECTOR_H

#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>

class EdgeDetector {
public:
    EdgeDetector();
    ~EdgeDetector();
    
    cv::Mat processFrame(const cv::Mat& inputFrame);
    void setCannyThresholds(double lowThreshold, double highThreshold);
    void setGaussianBlurKernel(int kernelSize);
    
private:
    double m_lowThreshold;
    double m_highThreshold;
    int m_gaussianKernelSize;
    
    cv::Mat applyCannyEdgeDetection(const cv::Mat& inputFrame);
    cv::Mat applyGaussianBlur(const cv::Mat& inputFrame);
    cv::Mat convertToGrayscale(const cv::Mat& inputFrame);
};

#endif // EDGEDETECTOR_H

