#include <opencv2/opencv.hpp>
#include "coin_detector.h"
#include <opencv2/video/background_segm.hpp>

//Preprocessing image

void CoinDetector::calibrate(cv::Mat test_image){
	calibrating = true;
	cvNamedWindow("Calibration", CV_WINDOW_AUTOSIZE);
	test_image.copyTo(calibration_image);
	
	cv::createTrackbar("Min Radius", "Calibration", &min_circle_radius, min_circle_radius_limit);
	cv::createTrackbar("Max Radius", "Calibration", &max_circle_radius, max_circle_radius_limit);

	while(1) {
		cv::Mat temp = calibration_result();
		
		cv::imshow("Calibration", temp);
		int k = cv::waitKey(50);
		if(k == 27)
			break;
	}
	std::cout << "Finishing calibration" << std::endl;
	cvDestroyAllWindows();

	calibrating = false;
}

cv::Mat CoinDetector::calibration_result(){
	cv::Mat temp;
	cv::vector<cv::Vec3f> circles;

	preprocess(calibration_image, temp);
	cv::HoughCircles(temp, circles, CV_HOUGH_GRADIENT, 2.0, 40, 200, 130, min_circle_radius, max_circle_radius);
	
	for (int i = 0; i < circles.size(); i++) {
		cv::Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
		int radius = cvRound(circles[i][2]);
		
		cv::circle(temp, center, radius, cv::Scalar(255, 255, 255), 2, 8, 0);
		
		rescale_ratio = radius / rad_samples[3];
	}

	return temp;

}

void CoinDetector::preprocess(cv::Mat image, cv::Mat &output_image){

	double min, max;
	//Convert to grayscale
	cv::cvtColor(image, output_image, CV_RGB2GRAY);
	//Threshold image to highlight edges
	cv::minMaxIdx(output_image, &min, &max);
	cv::GaussianBlur(output_image, output_image, cv::Size(3, 3), 2, 2);
	cv::dilate(output_image, output_image, cv::KERNEL_GENERAL);
	//Alternate to canny, more robust
	//cv::adaptiveThreshold(output_image, output_image, max, cv::ADAPTIVE_THRESH_GAUSSIAN_C, cv::THRESH_BINARY_INV, 15, 15);
	cv::Canny(output_image, output_image, 75, 255, 3);
	
	//cv::threshold(output_image, output_image, 75, 255, CV_THRESH_BINARY);
	
	if (debug && !calibrating){
		cvNamedWindow("Preprocessed");
		cv::imshow("Preprocessed", output_image);
	
		cv::waitKey(0);
		cvDestroyAllWindows();
	}
}

//Finding circles inside image
void CoinDetector::find_circles(cv::Mat image, cv::Mat &output_image){
	output_image.create(image.size(), CV_8UC3);
	
	cv::HoughCircles(image, coin_positions, CV_HOUGH_GRADIENT, 2.0, 40, 200, 130, 10, 70);

	if (debug){
		image.copyTo(output_image);

		for (int i = 0; i < coin_positions.size(); i++) {
			cv::Point center(cvRound(coin_positions[i][0]), cvRound(coin_positions[i][1]));
			int radius = cvRound(coin_positions[i][2]);

			cv::circle(output_image, center, radius, cv::Scalar(255, 255, 255), 2, 8, 0);
		}

		cvNamedWindow("Circles");
		cv::imshow("Circles", output_image);
		cv::waitKey(0);
		cvDestroyAllWindows();
	}
}

void CoinDetector::isolate_coins(cv::Mat image, cv::vector<cv::Mat> &output_coin_images) {
	float ratio = excess_ratio;

	output_coin_images.clear();
	coin_classes.clear();

	for (int i = 0; i < coin_positions.size(); i++) {
		float rad_new = coin_positions[i][2] * ratio;// + 28 ;
		int x = coin_positions[i][0] - rad_new;
		int y = coin_positions[i][1] - rad_new;

		//Make sure ROI never goes outside image boundaries
		int w_max = (x > 0) ? rad_new : coin_positions[i][0];
		w_max = (coin_positions[i][0] + rad_new < image.cols) ? w_max : image.cols - coin_positions[i][0];

		int h_max = (y > 0) ? rad_new : coin_positions[i][1];
		h_max = (coin_positions[i][1] + rad_new < image.rows) ? h_max : image.rows - coin_positions[i][1];

		int width = 2 * w_max;
		int height = 2 * h_max;

		cv::Mat temp = image(cvRect(x, y, width, height));

		if (debug) {
			//Initializing environment
			cvNamedWindow("Before Isolation");

			//Output
			cv::imshow("Before Isolation", temp);
			cv::waitKey(0);
			cvDestroyAllWindows();
		}


		cv::vector<cv::Vec3f> circles;

		cv::Mat temp2;
		preprocess( temp, temp2 );


		bool found_coin = false;

		for (int j = rad_samples.size() - 1; j >= 0; j--) {
			double rad = rad_samples[j];
			cv::HoughCircles(temp2, circles, CV_HOUGH_GRADIENT, 2.0, 400, 200, 130, rad - 2, rad + 10);
			if ( circles.size() > 0 ){
				float rad_new2 = circles[0][2];
				int x2 = circles[0][0] - rad_new2;
				int y2 = circles[0][1] - rad_new2;

				//Make sure ROI never goes outside image boundaries
				int w_max2 = (x2 > 0) ? rad_new2 : circles[0][0];
				w_max2 = (circles[0][0] + rad_new2 < temp.cols) ? w_max2 : temp.cols - circles[0][0];

				int h_max2 = (y2 > 0) ? rad_new2 : circles[0][1];
				h_max2 = (circles[0][1] + rad_new2 < temp.rows) ? h_max2 : temp.rows - circles[0][1];

				int width2 = 2 * w_max2;
				int height2 = 2 * h_max2;

				cv::Mat temp_new = temp(cvRect(x2, y2, width2, height2));
				
				output_coin_images.push_back(temp_new);
				coin_classes.push_back(j);

				found_coin = true;
				break;
			}
		}

		if ( !found_coin ) {
			coin_classes.push_back(-1);
		}

		else if (debug) {
			//Initializing environment
			cvNamedWindow("Isolated");

			for (int k = 0; k < coin_positions.size(); k++) {
				cv::Point center(cvRound(circles[k][0]), cvRound(circles[k][1]));
				int radius = cvRound(circles[k][2]);

				// Can we draw the circle on the image or does it go out of frame
				if ( (circles[k][0] - radius < 0 || circles[k][0] + radius > temp.cols) ||
					(circles[k][1] - radius < 0 || circles[k][1] + radius > temp.rows)) {
						break;
				}

				cv::circle(temp2, center, radius, cv::Scalar(255, 255, 255), 2, 8, 0);
			}
			
			//Output
			cv::imshow("Isolated", temp2);
			cv::waitKey(0);
			cvDestroyAllWindows();
		}
	}

	if (debug){
			for (int i = 0; i < output_coin_images.size(); i++) {
				cvNamedWindow("Isolated Final");
				cv::imshow("Isolated Final", output_coin_images[i]);
				cv::waitKey(0);
				cvDestroyAllWindows();
			}
	}
}

//Public functions

//Default Constructor
CoinDetector::CoinDetector(int debug_mode, float scale_error, float calibration_error){
	debug = debug_mode;
	excess_ratio = scale_error;
	calibrating = false;

	min_circle_radius = 10;
	min_circle_radius_limit = 200;
	max_circle_radius = 70;
	max_circle_radius_limit = 200;
	
	////
	//rad_samples.push_back( 40.0 ); // 1 cent
	//rad_samples.push_back( 47.0 ); // 2 cent
	//rad_samples.push_back( 52.0 ); // 5 cent
	//rad_samples.push_back( 49.0 ); // 10 cent
	//rad_samples.push_back( 55.0 ); // 20 cent
	//rad_samples.push_back( 58.0 ); // 50 cent
	//rad_samples.push_back( 58.0 ); // 1 euro
	//rad_samples.push_back( 63.0 ); // 2 euro


	rad_samples.push_back( 40.0 ); // 1 cent
	rad_samples.push_back( 47.0 ); // 2 cent and 10 cent
	rad_samples.push_back( 52.0 ); // 5 cent and 20 cent
	rad_samples.push_back( 58.0 ); // 50 cent and 1 euro
	rad_samples.push_back( 63.0 ); // 2 euro


	// Specify which group the coins belong to
	//		- 2 cent and 10 cent are similar size
	//		- 5 cent and 20 cent are similar size
	//		- 50 cent and 1 euro are similar size
	rad_groups.push_back( 0 ); // 1 cent
	rad_groups.push_back( 1 ); // 2 cent
	rad_groups.push_back( 2 ); // 5 cent
	rad_groups.push_back( 1 ); // 10 cent
	rad_groups.push_back( 2 ); // 20 cent
	rad_groups.push_back( 3 ); // 50 cent
	rad_groups.push_back( 3 ); // 1 euro
	rad_groups.push_back( 4 ); // 2 euro

}

//Running detector on an image
int CoinDetector::detect(cv::Mat image, cv::Mat &output_image) {

	CoinDetector::preprocess(image, output_image);

	CoinDetector::find_circles(output_image, output_image);

	CoinDetector::isolate_coins(image, coin_images_final);

	CoinDetector::draw_bounds(image, output_image);

	return 1;
}

void CoinDetector::draw_bounds(cv::Mat image, cv::Mat output_image) {
	image.copyTo(output_image);

	float ratio = 1;

	for (int i = 0; i < coin_positions.size(); i++) {
		if (coin_positions[i][2] == 0)
			continue;
		int x = coin_positions[i][0] - coin_positions[i][2] * ratio;
		int y = coin_positions[i][1] - coin_positions[i][2] * ratio;

		//Make sure ROI never goes outside image boundaries
		int w_max = (x > 0) ? coin_positions[i][2] * ratio : coin_positions[i][0];
		w_max = (coin_positions[i][0] + coin_positions[i][2] * ratio < image.cols) ? w_max : image.cols - coin_positions[i][0];

		int h_max = (y > 0) ? coin_positions[i][2] * ratio : coin_positions[i][1];
		h_max = (coin_positions[i][1] + coin_positions[i][2] * ratio < image.rows) ? h_max : image.rows - coin_positions[i][1];

		int width = 2 * w_max;
		int height = 2 * h_max;

		cv::rectangle(output_image, cv::Rect(x, y, width, height), cv::Scalar(0, 255, 0), 3);
	}

	if (debug) {
		//Initializing environment
		cvNamedWindow("Coins", 1);

		//Output
		cv::imshow("Coins", output_image);
		cv::waitKey(0);
		cvDestroyAllWindows();
	}
}

cv::vector<cv::Mat> CoinDetector::getCoins() {
	return coin_images_final;
}

//Returns positions of coin in image
cv::vector<cv::Vec3f> CoinDetector::getCoinPositions() {
	return coin_positions;
}

//Returns radius of coins in image
cv::vector<double> CoinDetector::getCoinRadii() {}

//Returns classification of coins in image
cv::vector<int> CoinDetector::getCoinClass() { return coin_classes; }