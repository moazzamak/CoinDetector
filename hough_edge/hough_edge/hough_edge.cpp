#include <iostream>
#include <sstream>
#include <opencv2\opencv.hpp>

#include "coin_detector.h"
#include "coin_identifier.h"

using namespace std;

int main() {
	//Initializing objects

	cv::Mat frame, temp_frame;
	double min, max;
	
	//Loading resources
	frame = cv::imread("test_1.jpg");

	if(frame.empty())
		return -1;

	CoinDetector oCoin;
	
	oCoin.detect(frame);

	//Initializing environment
	cvNamedWindow("Edges", 1);
	
	//Processing
	//Convert to grayscale
	cv::cvtColor(frame, temp_frame, CV_RGB2GRAY);
	cv::minMaxIdx(temp_frame, &min, &max);
	cv::adaptiveThreshold(temp_frame, temp_frame, max, cv::ADAPTIVE_THRESH_GAUSSIAN_C, cv::THRESH_BINARY, 11, 1);

	//Output
	cv::imshow("Edges", temp_frame);
	cv::waitKey(0);

	return 0;
}