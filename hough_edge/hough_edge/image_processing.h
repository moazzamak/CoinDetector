#ifndef IMAGE_PROCESSING_H
#define IMAGE_PROCESSING_H

#include <opencv2/opencv.hpp>
#include "opencv2/gpu/gpu.hpp"

static const char * windowName = "Hough Circle Detection Demo";

struct DetectionParameters
{
	int spatialWindowRadius;
	int colorWindowRadius;
	int sharpeningWeight; // x100
	int laplaccianScale;  // x100
	int cannyThreshold;
	int accumulatorThreshold;

	int houghResolution;  // x10
	int minRadius;
	int maxRadius;

	DetectionParameters()
		: spatialWindowRadius(10)
		, colorWindowRadius(10)
		, sharpeningWeight(40)
		, laplaccianScale(20)
		, cannyThreshold(40)
		, accumulatorThreshold(40)
		, houghResolution(10)
		, minRadius(5)
		, maxRadius(20)
	{
	}
};

class image_processing
{
public:
	image_processing(const cv::Mat_<cv::Vec3b>& image);
	cv::Mat show(const cv::Mat& colorImg) const;
	void process(const cv::Mat_<cv::Vec3b>& inputRgbImage, const DetectionParameters& settings);
	void run();

protected:

	void display();
	static void trackbarPropertyChanged(int, void* userdata);
	

private:
	cv::Mat image;
	DetectionParameters mSettings;
	//EggsDetectorAlgorithm mAlgorithm;
	cv::Mat filtered;
	cv::Mat grayImg;
	cv::Mat grayImgf;
	cv::Mat_<float> blurredf;
	cv::Mat_<float> laplaccian;
	cv::Mat sharpened;
	std::vector<cv::Vec3f> circles;
};

#endif