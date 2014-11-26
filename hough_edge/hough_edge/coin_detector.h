#ifndef COIN_DETECTOR_H
#define COIN_DETECTOR_H

#include <opencv2/opencv.hpp>

class CoinDetector {
private:
	//Result will be contained in these variables
	cv::Vector<cv::Point2d> coin_positions;
	cv::Vector<double> coin_radii;
	cv::Vector<int> coin_classes;

	cv::Mat temp_image;

	void preprocess(cv::Mat image);

public:
	CoinDetector();
	int detect(cv::Mat image);
	cv::Vector<cv::Point2d> getCoinPositions();
	cv::Vector<double> getCoinRadii();
	cv::Vector<int> getCoinClass();
};

#endif