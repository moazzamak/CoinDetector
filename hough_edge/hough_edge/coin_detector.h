#ifndef COIN_DETECTOR_H
#define COIN_DETECTOR_H

#include <opencv2/opencv.hpp>

class CoinDetector {
private:
	//Result will be contained in these variables
	cv::Vector<cv::Point2d> coin_positions;
	cv::Vector<double> coin_radii;
	cv::Vector<int> coin_classes;

	void preprocess(cv::Mat image, cv::Mat &result);

public:
	CoinDetector();
	int detect(cv::Mat image, cv::Mat &result);
	cv::Vector<cv::Point2d> getCoinPositions();
	cv::Vector<double> getCoinRadii();
	cv::Vector<int> getCoinClass();
};

#endif