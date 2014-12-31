#ifndef COIN_DETECTOR_H
#define COIN_DETECTOR_H

#include <opencv2/opencv.hpp>


class CoinDetector {
private:
	int debug;
	float scale_error_ratio;
	//Result will be contained in these variables
	cv::vector<cv::Vec3f> coin_positions;
	cv::vector<cv::Vec3f> isolated_coin_positions;
	cv::vector<cv::Vec3f> corrected_positions;
	cv::vector<int> coin_classes;
	cv::vector<cv::Mat> coin_images;
	cv::Mat isolated_coin_input;
	

	void preprocess(cv::Mat image, cv::Mat &output_image);
	void find_circles(cv::Mat image, cv::Mat &output_image);
<<<<<<< HEAD
	void isolate_coins(cv::Mat image, cv::Mat isolated_image_input, cv::vector<cv::Mat> &output_coin_images);
=======
	void isolate_coins(cv::Mat image, cv::vector<cv::Mat> &output_coin_images);
>>>>>>> 00d0ab83fccddebf4db15c8bed5522c0f0c01843
	void draw_bounds(cv::Mat image, cv::Mat output_image);
	void correct_circles();
	void hitmiss(cv::Mat& src, cv::Mat& dst, cv::Mat& kernel);
	void find_contours(cv::Mat image);
public:
	CoinDetector(int debug_mode = 1, float scale_error = 1.3);
	int detect(cv::Mat image, cv::Mat &result);
	cv::vector<cv::Mat> getCoins();
	cv::vector<cv::Point2d> getCoinPositions();
	cv::vector<double> getCoinRadii();
	cv::vector<int> getCoinClass();
};

#endif