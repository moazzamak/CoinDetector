#ifndef COIN_DETECTOR_H
#define COIN_DETECTOR_H

#include <opencv2/opencv.hpp>

class CoinDetector {
private:
	int debug;
	float rescale_ratio;
	float excess_ratio;
	bool calibrating;
	//Result will be contained in these variables
	cv::vector<cv::Vec3f> coin_positions;
	cv::vector<cv::Vec3f> corrected_positions;
	cv::vector<int> coin_classes;
	cv::vector<cv::Mat> coin_images;
	cv::vector<cv::Mat> coin_images_final;

	void preprocess(cv::Mat image, cv::Mat &output_image);
	void find_circles(cv::Mat image, cv::Mat &output_image);
	void isolate_coins(cv::Mat image, cv::vector<cv::Mat> &output_coin_images);
	void draw_bounds(cv::Mat image, cv::Mat output_image);

protected:
	int min_circle_radius;
	int min_circle_radius_limit;
	int max_circle_radius;
	int max_circle_radius_limit;

	// Pre recorded radius samples of different coins
	cv::vector<double> rad_samples;
	cv::vector<int> rad_groups;

public:
	CoinDetector(int debug_mode = 1, float scale_error = 1.5, float calibration_error = 1.0f);
	int detect(cv::Mat image, cv::Mat &result);
	cv::vector<cv::Mat> getCoins();
	cv::vector<cv::Vec3f> getCoinPositions();
	cv::vector<double> getCoinRadii();
	cv::vector<int> getCoinClass();
	void calibrate(cv::Mat test_image);
	cv::Mat calibration_result();
	cv::Mat calibration_image;
};

#endif