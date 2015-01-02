#ifndef COIN_DETECTOR_H
#define COIN_DETECTOR_H

#include <opencv2/opencv.hpp>

class CoinDetector {
private:
	int debug;
	float scale_error_ratio;
	//Result will be contained in these variables
	cv::vector<cv::Vec3f> coin_positions;
	cv::vector<cv::Vec3f> corrected_positions;
	cv::vector<int> coin_classes;
	cv::vector<cv::Mat> coin_images;
	cv::Mat isolated_coin_input;

	void preprocess(cv::Mat image, cv::Mat &output_image);
	void find_circles(cv::Mat image, cv::Mat &output_image);
	void isolate_coins(cv::Mat image, cv::vector<cv::Mat> &output_coin_images);
	void draw_bounds(cv::Mat image, cv::Mat output_image);
	void correct_circles();

protected:
	 int min_circle_radius = 10;
	 int min_circle_radius_limit = 50;
	 int max_circle_radius = 70;
	 int max_circle_radius_limit = 150;

public:
	CoinDetector(int debug_mode = 1, float scale_error = 1.5);
	int detect(cv::Mat image, cv::Mat &result);
	cv::vector<cv::Mat> getCoins();
	cv::vector<cv::Point2d> getCoinPositions();
	cv::vector<double> getCoinRadii();
	cv::vector<int> getCoinClass();
	void calibrate(cv::Mat test_image);
	static void calibrate_cb(int, void* userdata);
	void CoinDetector::calibration_result();
	cv::Mat calibration_image;
};

#endif