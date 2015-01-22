#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <sstream>
#include <opencv2\opencv.hpp>
#include <queue>

//Import headers
#include "coin_detector.h"
#include "coin_identifier.h"
#include "image_stream.h"

using namespace std;
using namespace cv;

#define DEBUG 0

int main() {
	//Initializing objects and variables
	string isolated_output_folder = "isolated_images";
	cv::Mat frame, frame_buf, temp_frame, temp_coin;

	std::queue<cv::Mat> frame_buffer;
	int frame_buffer_length = 5;

	//Initializing ImageStream class
	//this handles images saved on file as an input stream
	//ImageStream cap("test_data_2", "image_", ".jpg", 200);

	////Or alternately open webcam
	cv::VideoCapture cap(0);

	if(!cap.isOpened()) // check if we succeeded
		return -1;

	CoinDetector cd(DEBUG);
	CoinIdentifier ci(DEBUG);

	ci.train();

	cap >> frame_buf;

	frame = frame_buf.clone();

	cd.calibrate(frame);
	cout << "Done calibration" << endl;
	
	bool update_coins = false, changed = false, first_frame = true;
	float threshold = 10;

	int n = 0;


	cvNamedWindow("CoinDetector");

	while (cv::waitKey(100) != 27) {
		////Loading resources
		cap >> frame_buf;

		frame = frame_buf.clone();

		if (frame.empty()) {
			cout << "Error: Failed to load stream! "  << endl;
			return -1;
		}

		frame_buffer.push(frame);
		frame.copyTo(temp_frame);

		if (update_coins || first_frame) {
			first_frame = false;
			update_coins = false;

			//Detecting coins in image
			cd.detect(temp_frame, temp_frame);
			cv::vector<cv::Mat> coins = cd.getCoins();
			cv::vector<cv::Vec3f> coin_pos = cd.getCoinPositions();
			cv::vector<int> coin_class_groups = cd.getCoinClass();

			cout << "Detected " << coins.size() << " coin candidates." << endl;

			// Writing coins isolated
			if(DEBUG) {
				for (int i = 0; i < coins.size(); i++){
					ostringstream ost;
					ost << isolated_output_folder << "/isolated_image" << i + 1 << ".jpg";
					string isolated_coin_name = ost.str();
					cv::imwrite(isolated_coin_name, coins[i]);
				}
			}

			// Identifying coins in image
			ci.identify_coins(coins, coin_class_groups);
			cv::vector<int> coin_hyp = ci.getCoinClass();
			ci.draw_coins(temp_frame, temp_frame, coin_pos, coin_hyp);
		}
		else if(!changed)
		{
			cv::vector<cv::Vec3f> coin_pos = cd.getCoinPositions();
			cv::vector<int> coin_hyp = ci.getCoinClass();
			ci.draw_coins(temp_frame, temp_frame, coin_pos, coin_hyp);
		}

		cv::imshow("CoinDetector", temp_frame);

		cv::Mat temp;
		cv::Scalar s;

		if ( frame_buffer.size() > frame_buffer_length) {
			frame_buffer.pop();
		}

		//cout << frame_buffer.size() << endl;

		cv::subtract(frame_buffer.front(), frame_buffer.back(), temp);
		cvtColor(temp, temp, CV_BGR2GRAY);
		temp.convertTo(temp, CV_32F, 1.0/255.0/255.0);
		s = sum(sum(temp));
		//cout << s << endl;

		if(changed) {
			// Wait for a few seconds after change to update frame
			cout << n << endl;
			n++;

			if(n > frame_buffer_length) {
				n = 0;
				update_coins = true;
				changed = false;
			}
		}
		

		if(abs(s[0]) > threshold) {
			n = 0;
			changed = true;
			update_coins = false;
		}

		// Show output
		cv::imshow("CoinDetector", temp_frame);
		//cv::waitKey(0);
	}

	cv::destroyAllWindows();
	return 0;
}