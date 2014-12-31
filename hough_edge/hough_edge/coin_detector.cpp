#include <opencv2/opencv.hpp>
#include "coin_detector.h"
<<<<<<< HEAD
#include <sstream>
using namespace cv;
RNG rng(12345);
=======

>>>>>>> 00d0ab83fccddebf4db15c8bed5522c0f0c01843
//Private functions

//Preprocessing image
void CoinDetector::preprocess(cv::Mat image, cv::Mat &output_image) {
	//Initialize temporary variables
	double min, max;

<<<<<<< HEAD
	//Convert to grayscale
	cv::cvtColor(image, output_image, CV_RGB2GRAY);

	//Threshold image to highlight edges
	cv::minMaxIdx(output_image, &min, &max);
	cv::GaussianBlur(output_image, output_image, cv::Size(3, 3), 2, 2);
	//cv::Laplacian(output_image, output_image, CV_16S, 3, 1, 0, BORDER_DEFAULT);
	//convertScaleAbs(output_image,output_image);
	cv::dilate(output_image, output_image, cv::KERNEL_GENERAL);

=======
	image.copyTo(output_image);

	if(image.channels() > 1) {
		//Convert to grayscale
		cv::cvtColor(output_image, output_image, CV_RGB2GRAY);
	}

	//Threshold image to highlight edges
	cv::minMaxIdx(output_image, &min, &max);
	cv::GaussianBlur(output_image, output_image, cv::Size(3,3), 2, 2);
	
	cv::dilate(output_image, output_image, cv::KERNEL_GENERAL);
	
>>>>>>> 00d0ab83fccddebf4db15c8bed5522c0f0c01843
	////Canny works better with estimating centres
	//cv::Canny(output_image, output_image, 150, 350);

	//Alternate to canny, more robust
	cv::adaptiveThreshold(output_image, output_image, max, cv::ADAPTIVE_THRESH_GAUSSIAN_C, cv::THRESH_BINARY_INV, 15, 15);
<<<<<<< HEAD
	//output_image.copyTo(isolated_coin_input);
	output_image.copyTo(isolated_coin_input);
	//isolated_coin_input = output_image.clone();
	if (debug){
=======

	if(debug) {
>>>>>>> 00d0ab83fccddebf4db15c8bed5522c0f0c01843
		cvNamedWindow("Preprocessed");
		cv::imshow("Preprocessed", output_image);
		cv::waitKey(0);
		cvDestroyAllWindows();
	}
}

void CoinDetector::find_contours(cv::Mat image){
	cv::Mat contourImage = image.clone();
	std::vector<std::vector<cv::Point> > contours;
	vector<Vec4i> hierarchy;
	//std::cout << "in devvrat" << std::endl;
	cv::findContours(contourImage, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
	std::cout << "finding contours" << std::endl;
	vector<vector<Point> > contours_poly(contours.size());
	vector<Rect> boundRect(contours.size());
	float max_area = 0.0;
	int max_area_idx = 0;
	for (int j = 0; j < contours.size(); j++) {
		float area = contourArea(contours[j], false);
		if (area > 300) {
			drawContours(contourImage, contours, j, Scalar(0, 0, 255), 2, 8, hierarchy);
		}
		//approxPolyDP(Mat(contours[j]), contours_poly[j], 3, true);
		//boundRect[j] = boundingRect(Mat(contours_poly[j]));
		//rectangle(image, boundRect[j].tl(), boundRect[j].br(), Scalar(255, 255, 255), 3, 8, 0);
	}
	//for (int k = 0; k< contours.size(); k++)
	//{
	//Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
	//drawContours(temp_binary, contours_poly, i, color, 1, 8, vector<Vec4i>(), 0, Point());
	
	//circle(drawing, center[i], (int)radius[i], color, 2, 8, 0);
	cvNamedWindow("contours", CV_WINDOW_AUTOSIZE);
	cv::imshow("contours", contourImage);
	//}

}
//Finding circles inside image
<<<<<<< HEAD
void CoinDetector::find_circles(cv::Mat image, cv::Mat &output_image){
	output_image.create(image.size(), CV_8UC3);
	//isolated_coin_input.create(image.size(), CV_8UC3);

	cv::HoughCircles(image, coin_positions, CV_HOUGH_GRADIENT, 1.5, 40, 200, 130, 15, 100);
	
	if (debug){
=======
void CoinDetector::find_circles(cv::Mat image, cv::Mat &output_image) {

	output_image.create(image.size(), CV_8UC3);
	
	cv::HoughCircles(image, coin_positions, CV_HOUGH_GRADIENT, 1.5, 40, 200, 130, 20, 100);

	if(debug) {
>>>>>>> 00d0ab83fccddebf4db15c8bed5522c0f0c01843
		image.copyTo(output_image);

		for (int i = 0; i < coin_positions.size(); i++) {
			cv::Point center(cvRound(coin_positions[i][0]), cvRound(coin_positions[i][1]));
			int radius = cvRound(coin_positions[i][2]);

			cv::circle(output_image, center, 3, cv::Scalar(0, 255, 0), -1, 8, 0);
			cv::circle(output_image, center, radius, cv::Scalar(255, 255, 255), 3, 8, 0);
		}

		cvNamedWindow("Circles");
		cv::imshow("Circles", output_image);
		cv::waitKey(0);
		cvDestroyAllWindows();
	}
}

<<<<<<< HEAD
void CoinDetector::isolate_coins(cv::Mat image, cv::Mat isolated_image_input, cv::vector<cv::Mat> &output_coin_images){
	//cvNamedWindow("isolatedinput", CV_WINDOW_AUTOSIZE);
	//cv::imshow("isolatedinput", isolated_image_input);
	//cv::waitKey(0);
	//cvDestroyAllWindows();
	float ratio = scale_error_ratio;
	//cv::Mat iso_image;
	//isolated_image_input.create(isolated_image_input.size(), CV_8UC3);
	
	for (int i = 0; i < coin_positions.size(); i++) 
	{
		int x = coin_positions[i][0] - coin_positions[i][2] * ratio;
		int y = coin_positions[i][1] - coin_positions[i][2] * ratio;

		//Make sure ROI never goes outside image boundaries
		int w_max = (x > 0) ? coin_positions[i][2] * ratio : coin_positions[i][0];
		w_max = (coin_positions[i][0] + coin_positions[i][2] * ratio < image.cols) ? w_max : image.cols - coin_positions[i][0];

		int h_max = (y > 0) ? coin_positions[i][2] * ratio : coin_positions[i][1];
		h_max = (coin_positions[i][1] + coin_positions[i][2] * ratio < image.rows) ? h_max : image.rows - coin_positions[i][1];

		int width = 2 * w_max;
		int height = 2 * h_max;

		//cv::Mat temp_ori = image(cvRect(x, y, width, height));
		cv::Mat temp = image(cvRect(x, y, width, height));
		std::cout << "before devvrat" << std::endl;
		//devvrat
		cv::Mat temp_binary = isolated_coin_input(cvRect(x, y, width, height));
		cvNamedWindow("temp_binary", CV_WINDOW_AUTOSIZE);
		//cv::imshow("temp_binary", temp_binary);
		std::vector<std::vector<cv::Point> > contours;
		vector<Vec4i> hierarchy;
		std::cout << "in devvrat" << std::endl;
		cv::findContours(temp_binary, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
		cv::imshow("temp_binary", temp_binary);
		std::cout << "finding contours" << std::endl;
		vector<vector<Point> > contours_poly(contours.size());
		vector<Rect> boundRect(contours.size());
		float max_area = 0.0;
		int max_area_idx = 0;
		for (int j = 0; j < contours.size(); j++) {
			approxPolyDP(Mat(contours[j]), contours_poly[j], 3, true);
			boundRect[j] = boundingRect(Mat(contours_poly[j]));
			float area = contourArea(contours[j], false);
			if (area > max_area) {
				max_area = area;
				max_area_idx = j;

				//drawContours(temp_binary, contours, j, Scalar(255, 255, 255), 3, 8);
				//std::cout << "drawing contours" << std::endl;
				//cvNamedWindow("contours", CV_WINDOW_AUTOSIZE);
				//cv::imshow("contours", temp_binary);
			}
		}
		float area = contourArea(contours[max_area_idx], false);
		if (area > 300) {
			drawContours(temp_binary, contours_poly, max_area_idx, Scalar(255, 255, 255), 3, 8, vector<Vec4i>(), 0, Point());
			rectangle(temp_binary, boundRect[max_area_idx].tl(), boundRect[max_area_idx].br(), Scalar(255, 255, 255), 3, 8, 0);
			//circle(drawing, center[i], (int)radius[i], color, 2, 8, 0);
			cvNamedWindow("contours", CV_WINDOW_AUTOSIZE);
			cv::imshow("contours", temp_binary);
			//cv::waitKey(0);
			//cvDestroyAllWindows();
		}
		//for (int k = 0; k< contours.size(); k++)
		//{
			//Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
			////drawContours(temp_binary, contours_poly, max_area_idx, Scalar(255, 255, 255), 3, 8, vector<Vec4i>(), 0, Point());
			////rectangle(temp_binary, boundRect[max_area_idx].tl(), boundRect[max_area_idx].br(), Scalar(255, 255, 255), 3, 8, 0);
			//circle(drawing, center[i], (int)radius[i], color, 2, 8, 0);
			////cvNamedWindow("contours", CV_WINDOW_AUTOSIZE);
			////cv::imshow("contours", temp_binary);
		//}
		std::cout << "after devvrat" << std::endl;
		cv::Point tl = boundRect[max_area_idx].tl();
		cv::Point br = boundRect[max_area_idx].br();
		int lx, ly, isolated_width, isolated_height;
		lx = tl.x;
		ly = tl.y + 1;
		isolated_width = br.x - lx;
		isolated_height = br.y - ly;
		std::cout << "points x:" << tl.x << std::endl;
		std::cout << "points y:" << tl.y << std::endl;
		std::cout << "points x:" << br.x << std::endl;
		std::cout << "points x:" << br.y << std::endl;
		cv::Size s = temp.size();
		std::cout << "size image" << s.width << std::endl;
		std::cout << "size image" << s.height << std::endl;
		cv::Mat temp_new = temp(cvRect(lx, ly, isolated_width, isolated_height));

		//devvrat*/
		//hitmiss(temp, temp, b);
		output_coin_images.push_back(temp_new);
		contours.clear();
		hierarchy.clear();
		contours_poly.clear();
		boundRect.clear();
		
=======
void CoinDetector::isolate_coins(cv::Mat image, cv::vector<cv::Mat> &output_coin_images) {
	float ratio = scale_error_ratio;
	for (int i = 0; i < coin_positions.size(); i++) {
		int x = coin_positions[i][0]-coin_positions[i][2]*ratio;
		int y = coin_positions[i][1]-coin_positions[i][2]*ratio;

		//Make sure ROI never goes outside image boundaries
		int w_max = (x > 0) ? coin_positions[i][2]*ratio : coin_positions[i][0];
		w_max = (coin_positions[i][0]+coin_positions[i][2]*ratio < image.cols) ? w_max : image.cols - coin_positions[i][0];

		int h_max = (y > 0) ? coin_positions[i][2]*ratio : coin_positions[i][1];
		h_max = (coin_positions[i][1]+coin_positions[i][2]*ratio < image.rows) ? h_max : image.rows - coin_positions[i][1];

		int width = 2*w_max;
		int height = 2*h_max;

		cv::Mat temp = image( cvRect(x , y,	width, height) );
		output_coin_images.push_back(temp);
>>>>>>> 00d0ab83fccddebf4db15c8bed5522c0f0c01843

		if (debug) {
			//Initializing environment
			cvNamedWindow("Isolated", 1);

			//Output
			cv::imshow("Isolated", temp_new);
			cv::waitKey(0);
			cvDestroyAllWindows();
		}
	}
}

//Public functions

//Default Constructor
CoinDetector::CoinDetector(int debug_mode, float scale_error) {
	debug = debug_mode;
	scale_error_ratio = scale_error;
}

//Running detector on an image
int CoinDetector::detect(cv::Mat image, cv::Mat &output_image) {
	
	CoinDetector::preprocess(image, output_image);
	//CoinDetector::find_contours(output_image);
	CoinDetector::find_circles(output_image, output_image);
<<<<<<< HEAD

	CoinDetector::isolate_coins(image,isolated_coin_input, coin_images);
=======
	
	CoinDetector::isolate_coins(image, coin_images);
>>>>>>> 00d0ab83fccddebf4db15c8bed5522c0f0c01843
	output_image = coin_images[0];

	////TODO: implement this function so that it improves quality
	////Testing: Do not use! Reduces result quality
	//CoinDetector::correct_circles();

	CoinDetector::draw_bounds(image, output_image);

	return 1;
}
void CoinDetector::hitmiss(cv::Mat& src, cv::Mat& dst, cv::Mat& kernel)
{
	CV_Assert(src.type() == CV_8U && src.channels() == 1);

	cv::Mat k1 = (kernel == 1) / 255;
	cv::Mat k2 = (kernel == -1) / 255;

	cv::normalize(src, src, 0, 1, cv::NORM_MINMAX);

	cv::Mat e1, e2;
	cv::erode(src, e1, k1, cv::Point(-1, -1), 1, cv::BORDER_CONSTANT, cv::Scalar(0));
	cv::erode(1 - src, e2, k2, cv::Point(-1, -1), 1, cv::BORDER_CONSTANT, cv::Scalar(0));
	dst = e1 & e2;
}

void CoinDetector::correct_circles() {
	cv::vector<cv::Vec3f> pos;

	for (int i = 0; i < coin_images.size(); i++) {
		cv::Mat temp = coin_images[i];
		CoinDetector::preprocess(temp, temp);
		cv::HoughCircles(temp, pos, CV_HOUGH_GRADIENT, 1.5, temp.rows/2, 220, 140, 20, 150);
		corrected_positions.push_back(pos[0]);
		
		for (int i = 0; i < pos.size(); i++) {
			cv::Point center( cvRound(pos[i][0]), cvRound(pos[i][1]) );
			int radius = cvRound( pos[i][2] );
		
			cv::circle(temp, center, 3, cv::Scalar(0, 255, 0), -1, 8, 0);
			cv::circle(temp, center, radius, cv::Scalar(255, 255, 255), 3, 8, 0);
		}

		if(debug) {
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
		int x = coin_positions[i][0]-coin_positions[i][2]*ratio;
		int y = coin_positions[i][1]-coin_positions[i][2]*ratio;

		//Make sure ROI never goes outside image boundaries
		int w_max = (x > 0) ? coin_positions[i][2]*ratio : coin_positions[i][0];
		w_max = (coin_positions[i][0]+coin_positions[i][2]*ratio < image.cols) ? w_max : image.cols - coin_positions[i][0];

		int h_max = (y > 0) ? coin_positions[i][2]*ratio : coin_positions[i][1];
		h_max = (coin_positions[i][1]+coin_positions[i][2]*ratio < image.rows) ? h_max : image.rows - coin_positions[i][1];

		int width = 2*w_max;
		int height = 2*h_max;

		cv::rectangle(output_image, cv::Rect(x, y, width, height), cv::Scalar(0, 255, 0), 3);
	}

	if(debug) {
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
cv::vector<cv::Point2d> getCoinPositions() {}

//Returns radius of coins in image
cv::vector<double> getCoinRadii() {}

//Returns classification of coins in image
cv::vector<int> getCoinClass() {}
