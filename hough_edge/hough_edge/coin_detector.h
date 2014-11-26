#ifndef COIN_DETECTOR_H
#define COIN_DETECTOR_H

#include <opencv2/opencv.hpp>

class CoinDetector {
private:
	int debug;
	float scale_error_ratio;
	//Result will be contained in these variables
	cv::vector<cv::Vec3f> coin_positions;
	cv::vector<int> coin_classes;
	cv::vector<cv::Mat> coin_images;

	void preprocess(cv::Mat image, cv::Mat &output_image);
	void find_circles(cv::Mat image, cv::Mat &output_image);
	void isolate_coins(cv::Mat image, cv::vector<cv::Mat> &output_coin_images);
public:
	CoinDetector();
	int detect(cv::Mat image, cv::Mat &result);
	cv::vector<cv::Mat> getCoins();
	cv::vector<cv::Point2d> getCoinPositions();
	cv::vector<double> getCoinRadii();
	cv::vector<int> getCoinClass();
};

#endif