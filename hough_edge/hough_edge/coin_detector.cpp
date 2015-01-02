#include <opencv2/opencv.hpp>
#include "coin_detector.h"
#include <opencv2/video/background_segm.hpp>

//Preprocessing image

void CoinDetector::calibrate(cv::Mat test_image){
	calibrating = true;
	cvNamedWindow("Calibration", CV_WINDOW_AUTOSIZE);
	test_image.copyTo(calibration_image);
	
	cv::createTrackbar("Min Radius", "Calibration", &min_circle_radius, min_circle_radius_limit);
	cv::createTrackbar("Max Radius", "Calibration", &max_circle_radius, max_circle_radius_limit);

	while(1) {
		cv::Mat temp = calibration_result();
		
		cv::imshow("Calibration", temp);
		int k = cv::waitKey(50);
		if(k == 27)
			break;
	}
	std::cout << "Finishing calibration" << std::endl;
	cvDestroyAllWindows();

	calibrating = false;
}

cv::Mat CoinDetector::calibration_result(){
	std::cout << min_circle_radius << std::endl;
	std::cout << max_circle_radius << std::endl << std::endl;

	cv::Mat temp;
	cv::vector<cv::Vec3f> circles;

	preprocess(calibration_image, temp);
	cv::HoughCircles(temp, circles, CV_HOUGH_GRADIENT, 2.0, 40, 200, 130, min_circle_radius, max_circle_radius);
	
	for (int i = 0; i < circles.size(); i++) {
		cv::Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
		int radius = cvRound(circles[i][2]);
		
		cv::circle(temp, center, radius, cv::Scalar(255, 255, 255), 2, 8, 0);
	}

	return temp;

}

void CoinDetector::preprocess(cv::Mat image, cv::Mat &output_image){

	double min, max;
	//Convert to grayscale
	cv::cvtColor(image, output_image, CV_RGB2GRAY);
	//Threshold image to highlight edges
	cv::minMaxIdx(output_image, &min, &max);
	cv::GaussianBlur(output_image, output_image, cv::Size(3, 3), 2, 2);
	cv::dilate(output_image, output_image, cv::KERNEL_GENERAL);
	//Alternate to canny, more robust
	//cv::adaptiveThreshold(output_image, output_image, max, cv::ADAPTIVE_THRESH_GAUSSIAN_C, cv::THRESH_BINARY_INV, 15, 15);
	cv::Canny(output_image, output_image, 75, 255, 3);
	
	//cv::threshold(output_image, output_image, 75, 255, CV_THRESH_BINARY);
	
	output_image.copyTo(isolated_coin_input);
	if (debug && !calibrating){
		cvNamedWindow("Preprocessed");
		cv::imshow("Preprocessed", output_image);
	
		cv::waitKey(0);
		cvDestroyAllWindows();
	}
}

//Finding circles inside image
void CoinDetector::find_circles(cv::Mat image, cv::Mat &output_image){
	output_image.create(image.size(), CV_8UC3);
	std::cout << "min circle radius" << min_circle_radius << std::endl;
	std::cout << "max circle radius" << max_circle_radius << std::endl;
	cv::HoughCircles(image, coin_positions, CV_HOUGH_GRADIENT, 2.0, 40, 200, 130, 20, 80);

	if (debug){
		image.copyTo(output_image);

		for (int i = 0; i < coin_positions.size(); i++) {
			cv::Point center(cvRound(coin_positions[i][0]), cvRound(coin_positions[i][1]));
			int radius = cvRound(coin_positions[i][2]);

			//cv::circle(isolated_coin_input, center, 3, cv::Scalar(0, 255, 0), -1, 8, 0);
			cv::circle(output_image, center, radius, cv::Scalar(255, 255, 255), 2, 8, 0);
		}

		cvNamedWindow("Circles");
		cv::imshow("Circles", output_image);
		cv::waitKey(0);
		cvDestroyAllWindows();
	}
}

void CoinDetector::isolate_coins(cv::Mat image, cv::vector<cv::Mat> &output_coin_images) {
	float ratio = scale_error_ratio;
	cv::Mat input_processed_image;
	isolated_coin_input.copyTo(input_processed_image);
	for (int i = 0; i < coin_positions.size(); i++) {
		input_processed_image.copyTo(isolated_coin_input);
		int x = coin_positions[i][0] - coin_positions[i][2] * ratio;
		int y = coin_positions[i][1] - coin_positions[i][2] * ratio;

		//Make sure ROI never goes outside image boundaries
		int w_max = (x > 0) ? coin_positions[i][2] * ratio : coin_positions[i][0];
		w_max = (coin_positions[i][0] + coin_positions[i][2] * ratio < image.cols) ? w_max : image.cols - coin_positions[i][0];

		int h_max = (y > 0) ? coin_positions[i][2] * ratio : coin_positions[i][1];
		h_max = (coin_positions[i][1] + coin_positions[i][2] * ratio < image.rows) ? h_max : image.rows - coin_positions[i][1];

		int width = 2 * w_max;
		int height = 2 * h_max;

		cv::Mat temp = image(cvRect(x, y, width, height));
		
		//to localize the coins we are finding contours from the binary image
		cv::Mat temp_binary = isolated_coin_input(cvRect(x, y, width, height));
		cvNamedWindow("temp_binary", CV_WINDOW_AUTOSIZE);
		
		cv::imshow("temp_binary", temp_binary);
		std::vector<std::vector<cv::Point> > contours;
		cv::vector<cv::Vec4i> hierarchy;
		//std::cout << "in devvrat" << std::endl;
		//cv::Point offset;
		//offset.x = coin_positions[i][0];
		//offset.y = coin_positions[i][1];
		//cv::cvtColor(temp_binary, temp_binary, CV_BGR2GRAY);
		//cv::threshold(temp_binary, temp_binary, 40, 255, CV_THRESH_BINARY);
		//cv::threshold(temp_binary, temp_binary, 0, 255, THRESH_BINARY_INV);
		cv::findContours(temp_binary, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
		
		//cv::imshow("temp_binary", temp_binary);
		//std::cout << "finding contours" << contours.size() << std::endl;
		//vector<vector<Point> > contours_poly(contours.size());
		//vector<Rect> boundRect(contours.size());
		cv::Rect bounding_rect;
		float max_area = 0.0;
		int max_area_idx = 0;
		for (int j = 0; j < contours.size(); j++) {
			//approxPolyDP(Mat(contours[j]), contours_poly[j], 3, true);
			//boundRect[j] = boundingRect(Mat(contours_poly[j]));
			//float perimieter = arcLength(contours[j], true);
			float area = contourArea(contours[j], false);
			if (area > max_area) {
				max_area = area;
				max_area_idx = j;
				bounding_rect = boundingRect(contours[j]);
			}
		}
		
		//Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
		//drawContours(temp_binary, contours, max_area_idx, cv::Scalar(255, 255, 255), 2, 8, hierarchy, 0, Point());
		//cv::imshow("temp_binary", temp_binary);
		//std::cout << "after devvrat" << std::endl;
		//cv::Point tl = boundRect[max_area_idx].tl();
		//cv::Point br = boundRect[max_area_idx].br();
		cv::Point tl = bounding_rect.tl();
		cv::Point br = bounding_rect.br();
		int lx, ly, isolated_width, isolated_height;
		lx = tl.x;
		ly = tl.y + 1;
		isolated_width = br.x - lx;
		isolated_height = br.y - ly;
		cv::Mat temp_new = temp(cvRect(lx, ly, isolated_width, isolated_height));

		output_coin_images.push_back(temp_new);
		contours.clear();
		hierarchy.clear();

		if (debug) {
			//Initializing environment
			cvNamedWindow("Isolated", 1);

			//Output
			cv::imshow("Isolated", temp_new);
			//cvNamedWindow("ISOLATED IMAGE", CV_WINDOW_AUTOSIZE);
			//cv::imshow("ISOLATED IMAGE", isolated_coin_input);
			cv::waitKey(0);
			cvDestroyAllWindows();
		}
	}
}

//Public functions

//Default Constructor
CoinDetector::CoinDetector(int debug_mode, float scale_error){
	debug = debug_mode;
	scale_error_ratio = scale_error;
	calibrating = false;

	min_circle_radius = 10;
	min_circle_radius_limit = 50;
	max_circle_radius = 70;
	max_circle_radius_limit = 150;
}

//Running detector on an image
int CoinDetector::detect(cv::Mat image, cv::Mat &output_image) {

	CoinDetector::preprocess(image, output_image);

	CoinDetector::find_circles(output_image, output_image);

	CoinDetector::isolate_coins(image, coin_images);
	output_image = coin_images[0];

	////TODO: implement this function so that it improves quality
	////Testing: Do not use! Reduces result quality
	//CoinDetector::correct_circles();

	CoinDetector::draw_bounds(image, output_image);

	return 1;
}

void CoinDetector::correct_circles() {
	cv::vector<cv::Vec3f> pos;

	for (int i = 0; i < coin_images.size(); i++) {
		cv::Mat temp = coin_images[i];
		CoinDetector::preprocess(temp, temp);
		cv::HoughCircles(temp, pos, CV_HOUGH_GRADIENT, 1.5, temp.rows / 2, 220, 140, 20, 150);
		corrected_positions.push_back(pos[0]);

		for (int i = 0; i < pos.size(); i++) {
			cv::Point center(cvRound(pos[i][0]), cvRound(pos[i][1]));
			int radius = cvRound(pos[i][2]);

			cv::circle(temp, center, 3, cv::Scalar(0, 255, 0), -1, 8, 0);
			cv::circle(temp, center, radius, cv::Scalar(255, 255, 255), 3, 8, 0);
		}

		if (debug) {
			//Initializing environment
			cvNamedWindow("Fixed", 1);

			//Output
			cv::imshow("Fixed", temp);
			cv::waitKey(0);
			cvDestroyAllWindows();
		}
	}
}

void CoinDetector::draw_bounds(cv::Mat image, cv::Mat output_image) {
	image.copyTo(output_image);

	float ratio = 1;

	for (int i = 0; i < coin_positions.size(); i++) {
		int x = coin_positions[i][0] - coin_positions[i][2] * ratio;
		int y = coin_positions[i][1] - coin_positions[i][2] * ratio;

		//Make sure ROI never goes outside image boundaries
		int w_max = (x > 0) ? coin_positions[i][2] * ratio : coin_positions[i][0];
		w_max = (coin_positions[i][0] + coin_positions[i][2] * ratio < image.cols) ? w_max : image.cols - coin_positions[i][0];

		int h_max = (y > 0) ? coin_positions[i][2] * ratio : coin_positions[i][1];
		h_max = (coin_positions[i][1] + coin_positions[i][2] * ratio < image.rows) ? h_max : image.rows - coin_positions[i][1];

		int width = 2 * w_max;
		int height = 2 * h_max;

		cv::rectangle(output_image, cv::Rect(x, y, width, height), cv::Scalar(0, 255, 0), 3);
	}

	if (debug) {
		//Initializing environment
		cvNamedWindow("Coins", 1);

		//Output
		cv::imshow("Coins", output_image);
		cv::waitKey(0);
		cvDestroyAllWindows();
	}
}

cv::vector<cv::Mat> CoinDetector::getCoins() {
	return coin_images;
}

//Returns positions of coin in image
cv::vector<cv::Point2d> CoinDetector::getCoinPositions() {}

//Returns radius of coins in image
cv::vector<double> CoinDetector::getCoinRadii() {}

//Returns classification of coins in image
cv::vector<int> CoinDetector::getCoinClass() {}