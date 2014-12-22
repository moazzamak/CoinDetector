#include <iostream>
#include <sstream>
#include <opencv2\opencv.hpp>

//Import headers
#include "coin_detector.h"
#include "coin_identifier.h"
#include "image_stream.h"

using namespace std;

#define DEBUG 0

int main() {
	//Initializing objects

	cv::Mat frame, temp_frame, temp_coin;
	
	//Initializing ImageStream class
	//this handles images saved on file as an input stream
	ImageStream cap("test_data", "test_", ".jpg", 200);
	////Or alternately open webcam
	//cv::VideoCapture cap(0);

	CoinIdentifier ci(DEBUG);
	ci.train();

	////Loading resources
	cap >> frame;

	if(frame.empty()){
		cout << "Error: Could not find file " << cap.get_qualified_name() << endl;
		return -1;
	}

	CoinDetector cd(DEBUG);
	
	//Processing
	cd.detect(frame, temp_frame);
	cv::vector<cv::Mat> coins = cd.getCoins();
	cout << "Detected " << coins.size() << " coins." << endl;
	

	return 0;
}

cv::Mat fetchFrame(){
}