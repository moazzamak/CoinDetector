#ifndef COIN_IDENTIFIER_H
#define COIN_IDENTIFIER_H

#include <opencv2/opencv.hpp>

class CoinIdentifier {
private:
	int debug;
	
	void unwrap_coin(cv::Mat image, cv::Mat &output_image);
	void preprocess(cv::Mat image, cv::Mat &output_image);
	cv::Vec3b interpolate(cv::Point2f pt, cv::Mat image);
	
public:
	CoinIdentifier();

	void identify(cv::Mat image, cv::Mat &output_image);
};

#endif