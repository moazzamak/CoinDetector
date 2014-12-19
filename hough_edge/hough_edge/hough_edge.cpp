#include <iostream>
#include <sstream>
#include <opencv2\opencv.hpp>

#include "coin_detector.h"
#include "coin_identifier.h"
#include "image_stream.h"

using namespace std;

int main() {
	//Initializing objects

	cv::Mat frame, temp_frame, temp_coin;
	
	//Initializing ImageStream class
	//this handles images saved on file as an input stream
	ImageStream is("test_data", "test_", ".jpg", 200);

	//File and folder names
	string filename = "test_data/test_1.jpg";
	string folder_name = "training_data";
	string output_folder = "unwrapped";

	vector<string> filelist;
	filelist.push_back("euro_0-01_1.png"); 
	filelist.push_back("euro_0-02_0.png");
	filelist.push_back("euro_0-05_1.png");
	filelist.push_back("euro_0-10_1.png");
	filelist.push_back("euro_0-20_1.png");
	filelist.push_back("euro_0-50_1.png");
	filelist.push_back("euro_1-00_1.png");
	filelist.push_back("euro_2-00_1.png");

	////Loading resources
	//frame = cv::imread(filename);
	is >> frame;

	if(frame.empty()){
		cout << "Error: Could not find file " << filename << endl;
		return -1;
	}

	CoinDetector cd(0);
	
	//Processing
	cd.detect(frame, temp_frame);
	cv::vector<cv::Mat> coins = cd.getCoins();
	cout << "Detected " << coins.size() << " coins." << endl;
	
	for (int i = 0; i < filelist.size(); i++){	
		
		ostringstream ost;
		ost << folder_name << "/" << filelist[i];
		string qualified_coin_name = ost.str();

		ost.str("");

		ost << output_folder << "/" << filelist[i];
		string qualified_output_name = ost.str();
		
		cout << qualified_coin_name << endl
			<< qualified_output_name << endl;

		cv::Mat test_coin = cv::imread(qualified_coin_name);

		CoinIdentifier ci;
		ci.identify(test_coin, temp_coin);

		cv::imwrite(qualified_output_name, temp_coin);
	}

	return 0;
}

cv::Mat fetchFrame(){
}