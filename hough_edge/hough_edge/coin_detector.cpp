#include <opencv2/opencv.hpp>
#include "coin_detector.h"


//Private functions

//Preprocessing image
void CoinDetector::preprocess(cv::Mat image, cv::Mat &result){
	//Initialize temporary variables
	double min, max;
	cv::Mat result;

	//Convert to grayscale
	cv::cvtColor(image, result, CV_RGB2GRAY);
	
	//Threshold image to highlight edges
	cv::minMaxIdx(result, &min, &max);
	cv::adaptiveThreshold(result, result, max, cv::ADAPTIVE_THRESH_GAUSSIAN_C, cv::THRESH_BINARY, 11, 1);
}

//Public functions

//Default Constructor
CoinDetector::CoinDetector(){

}

//Running detector on an image
int CoinDetector::detect(cv::Mat image, cv::Mat &result){
	CoinDetector::preprocess(image, result);
	
	return 1;
}

//Returns positions of coin in image
cv::Vector<cv::Point2d> getCoinPositions(){}

//Returns radius of coins in image
cv::Vector<double> getCoinRadii(){}

//Returns classification of coins in image
cv::Vector<int> getCoinClass(){}