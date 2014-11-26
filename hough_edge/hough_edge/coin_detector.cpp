#include <opencv2/opencv.hpp>
#include "coin_detector.h"


//Private functions

//Preprocessing image
void CoinDetector::preprocess(cv::Mat image, cv::Mat &output_image){
	//Initialize temporary variables
	double min, max;

	//Convert to grayscale
	cv::cvtColor(image, output_image, CV_RGB2GRAY);
	
	//Threshold image to highlight edges
	cv::minMaxIdx(output_image, &min, &max);
	cv::GaussianBlur(output_image, output_image, cv::Size(3,3), 2, 2);
	
	
	//Canny works better in estimating edges
	cv::Canny(output_image, output_image, 150, 350);

	//Alternate to canny
	//cv::adaptiveThreshold(output_image, output_image, max, cv::ADAPTIVE_THRESH_GAUSSIAN_C, cv::THRESH_BINARY_INV, 15, 15);

	cv::dilate(output_image, output_image, cv::KERNEL_GENERAL);
}

//Finding circles inside image
void CoinDetector::find_circles(cv::Mat image, cv::Mat &output_image){

	//output_image.create(image.size(), CV_8UC3);
	image.copyTo(output_image);

	cv::vector<cv::Vec3f> circles;
	cv::HoughCircles(image, circles, CV_HOUGH_GRADIENT, 1.5, 50, 10, 130, 20, 130);

	for (int i = 0; i < circles.size(); i++) {
		cv::Point center( cvRound(circles[i][0]), cvRound(circles[i][1]) );
		int radius = cvRound( circles[i][2] );
		
		cv::circle(output_image, center, 3, cv::Scalar(0, 255, 0), -1, 8, 0);
		cv::circle(output_image, center, radius, cv::Scalar(255, 255, 255), 3, 8, 0);
	}
}

//Public functions

//Default Constructor
CoinDetector::CoinDetector(){

}

//Running detector on an image
int CoinDetector::detect(cv::Mat image, cv::Mat &output_image){
	CoinDetector::preprocess(image, output_image);
	CoinDetector::find_circles(output_image, output_image);
	return 1;
}

//Returns positions of coin in image
cv::Vector<cv::Point2d> getCoinPositions(){}

//Returns radius of coins in image
cv::Vector<double> getCoinRadii(){}

//Returns classification of coins in image
cv::Vector<int> getCoinClass(){}