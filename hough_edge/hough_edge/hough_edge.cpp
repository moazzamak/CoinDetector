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
	//Initializing objects and variables
	string isolated_output_folder = "isolated_images";
	cv::Mat frame, temp_frame, temp_coin;

	//Initializing ImageStream class
	//this handles images saved on file as an input stream
	ImageStream cap("test_data", "image_", ".jpg", 200);

	////Or alternately open webcam
	//cv::VideoCapture cap(0);

	//TODO: I'm putting this to 1.0f for now Dev you need to fix the circle localization
	//so we can put the extra region back
	CoinDetector cd(DEBUG);
	//CoinIdentifier ci(DEBUG);

	//ci.train();

	////Loading resources
	cap >> frame;

	if (frame.empty()) {
		cout << "Error: Could not find file "  << endl;
		return -1;
	}
	
	cd.calibrate(frame);
	cout << "Done calibration" << endl;
	
	//Detecting coins in image
	cd.detect(frame, temp_frame);
	cv::vector<cv::Mat> coins = cd.getCoins();
	cv::vector<cv::Vec3f> coin_pos = cd.getCoinPositions();
	cout << "Detected " << coins.size() << " coin candidates." << endl;

	// Writing coins isolated
	if(DEBUG || 1) {
		for (int i = 0; i < coins.size(); i++){
			ostringstream ost;
			ost << isolated_output_folder << "/isolated_image" << i + 1 << ".jpg";
			string isolated_coin_name = ost.str();
			cv::imwrite(isolated_coin_name, coins[i]);
		}
	}
	// Identifying coins in image
	ci.identify_coins(coins);
	cv::vector<int> coin_hyp = ci.getCoinClass();
	ci.draw_coins(frame, frame, coin_pos, coin_hyp);

	// Show output
	cvNamedWindow("Frame");
	cv::imshow("Frame", frame);
	cv::waitKey(0);
	cv::destroyAllWindows();
	return 0;
}