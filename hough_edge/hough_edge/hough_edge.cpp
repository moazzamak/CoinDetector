#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <sstream>
#include <opencv2\opencv.hpp>


//Import headers
#include "coin_detector.h"
#include "coin_identifier.h"
#include "image_stream.h"

using namespace std;
using namespace cv;

#define DEBUG 1

int main() {
	//Initializing objects

	cv::Mat frame, temp_frame, temp_coin;

	//Initializing ImageStream class
	//this handles images saved on file as an input stream
	ImageStream cap("test_data", "image_", ".jpg", 200);
	string isolated_output_folder = "isolated_images";
	////Or alternately open webcam
	//cv::VideoCapture cap(0);

	//TODO: I'm putting this to 1.0f for now Dev you need to fix the circle localization
	//so we can put the extra region back
	CoinDetector cd(DEBUG);
	//CoinIdentifier ci(DEBUG);

	//ci.train();
	//for (;;){
	////Loading resources
	cap >> frame;

	if (frame.empty()) {
		cout << "Error: Could not find file "  << endl;
		return -1;
	}
<<<<<<< HEAD
	//cd.calibrate(frame);
	cout << "Done calibration" << endl;
=======
	
	cd.calibrate(frame);
	cout << "Done calibration" << endl;
	
>>>>>>> origin/coinDetector
	//Detecting coins in image
	cd.detect(frame, temp_frame);
	cv::vector<cv::Mat> coins = cd.getCoins();
	cout << "Detected " << coins.size() << " coin candidates." << endl;
	for (int i = 0; i < coins.size(); i++){
		ostringstream ost;
		ost << isolated_output_folder << "/isolated_image" << i + 1 << ".png";
		string isolated_coin_name = ost.str();
		cv::imwrite(isolated_coin_name, coins[i]);
	}
	//Identifying coins in image
	//ci.identify_coins(coins, frame, temp_frame);
	//if (waitKey(30) >= 0) break;   // you can increase delay to 2 seconds here
	//}
	return 0;
}
