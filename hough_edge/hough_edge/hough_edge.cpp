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
	
	char filename[] = "test_data/test_1.jpg";
	//Loading resources
	frame = cv::imread(filename);

	if(frame.empty()){
		cout << "Error: Could not find file " << filename << endl;
		return -1;
	}

	CoinDetector oCoin;
	
	//Processing
	oCoin.detect(frame, temp_frame);
	cv::vector<cv::Mat> coins = oCoin.getCoins();
	
	cout << "Detected " << coins.size() << " coins." << endl;
	return 0;
}