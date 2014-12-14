#include <opencv2/opencv.hpp>
#include "coin_detector.h"
#include <sstream>
using namespace cv;
//Private functions

//Preprocessing image
void CoinDetector::preprocess(cv::Mat image, cv::Mat &output_image){
	//Initialize temporary variables
	double min, max;
	cv::Mat gaussian_op;
	//cv::pyrMeanShiftFiltering(image, output_image, 10, 10, 1);
	//Convert to grayscale
	cv::cvtColor(image, output_image, CV_RGB2GRAY);
	
	//Threshold image to highlight edges
	
	cv::GaussianBlur(output_image, gaussian_op, cv::Size(3, 3), 2, 2);
	//cv::addWeighted(gaussian_op, 1.5, output_image, -0.5, 0, output_image, -1);
	cv::Laplacian(gaussian_op, output_image, CV_16S, 3, 1, 0, BORDER_DEFAULT);
	convertScaleAbs(output_image,output_image);
	//cv::dilate(output_image, output_image, cv::KERNEL_GENERAL);
	//cv::adaptiveThreshold(output_image, output_image, max, cv::ADAPTIVE_THRESH_GAUSSIAN_C, cv::THRESH_BINARY_INV, 3, 15);
	////Canny works better with estimating centres
	cv::Canny(output_image, output_image, 70, 210);
	cv::minMaxIdx(output_image, &min, &max);
	//cv::threshold(output_image, output_image, 50, max, cv::THRESH_BINARY);
	//cv::adaptiveThreshold(output_image, output_image, max, cv::ADAPTIVE_THRESH_GAUSSIAN_C, cv::THRESH_BINARY_INV, 3, 15);
	//cv::threshold(output_image,output_image,);
	//Alternate to canny, more robust
	
	
	if(debug){
		cvNamedWindow("Preprocessed");
		cv::imshow("Preprocessed", output_image);
		cv::waitKey(0);
		cvDestroyAllWindows();
	}
}

//Finding circles inside image
void CoinDetector::find_circles(cv::Mat image, cv::Mat &output_image){
	/*
	//output_image.create(image.size(), CV_8UC3);
	std::cout << "Detecting circles "<< std::endl;
	//declare and initialize both parameters that are subjects to change
	int cannyThreshold = 200;
	int accumulatorThreshold = 150;
	int minDist = 55;
	int maxRadii = 75;
	int minRadii = 20;
	cv::namedWindow("Hough Circle Detection Demo", CV_WINDOW_AUTOSIZE); //create a window called "Control"
	cv::namedWindow("Circles Checking", CV_WINDOW_AUTOSIZE); //create a window called "Control"

	
	//std::string windowName = "Hough Circle Detection Demo";
	//Create trackbars in "Control" window
	cvCreateTrackbar("Canny threshold", "Hough Circle Detection Demo", &cannyThreshold, maxCannyThreshold); //Hue (0 - 179)
	cvCreateTrackbar("Accumulator Threshold", "Hough Circle Detection Demo", &accumulatorThreshold, maxAccumulatorThreshold);
	cvCreateTrackbar("Minimum Dist", "Hough Circle Detection Demo", &minDist, 100);
	cvCreateTrackbar("Max Radii Threshold", "Hough Circle Detection Demo", &maxRadii, 200);
	cvCreateTrackbar("Minimum Radii", "Hough Circle Detection Demo", &minRadii, 40);

	int key = 0;
	while (true)
	{
		//output_image.create(image.size(), CV_8UC3);
		std::cout << "in loop" << std::endl;
		coin_positions.clear();
		// those paramaters cannot be =0
		// so we must check here
		//cannyThreshold = std::max(cannyThreshold, 1);
		//accumulatorThreshold = std::max(accumulatorThreshold, 1);
		std::cout << "whats taking time" << std::endl;
		std::vector<Vec3f> circles;
		// runs the actual detection
		Mat display = output_image.clone();
		HoughCircles(display, coin_positions, CV_HOUGH_GRADIENT, 1.5, minDist, cannyThreshold, accumulatorThreshold, minRadii, maxRadii);
		std::cout << "Hough circle done" << std::endl;
		// clone the colour, input image for displaying purposes
		//	Mat display = output_image.clone();
		for (size_t i = 0; i < coin_positions.size(); i++)
		{
			std::cout << "in for loop again" << std::endl;
			Point center(cvRound(coin_positions[i][0]), cvRound(coin_positions[i][1]));
			int radius = cvRound(coin_positions[i][2]);
			// circle center
			circle(display, center, 3, Scalar(0, 255, 0), -1, 8, 0);
			// circle outline
			circle(display, center, radius, Scalar(255, 255, 255), 3, 8, 0);
		}

		// shows the results
		imshow("Circles Checking", display);
		if (waitKey(30) == 27) //wait for 'esc' key press for 30ms. If 'esc' key is pressed, break loop
		{
			std::cout << "esc key is pressed by user" << std::endl;
			break;
		}

		// get user key
		//key = waitKey(10);
	}
	*/
	cv::HoughCircles(output_image, coin_positions, CV_HOUGH_GRADIENT, 1.5, 55, 200, 130, 15, 75);

	if(debug){
		image.copyTo(output_image);

		for (int i = 0; i < coin_positions.size(); i++) {
			cv::Point center( cvRound(coin_positions[i][0]), cvRound(coin_positions[i][1]) );
			int radius = cvRound( coin_positions[i][2] );
		
			cv::circle(output_image, center, 3, cv::Scalar(0, 255, 0), -1, 8, 0);
			cv::circle(output_image, center, radius, cv::Scalar(255, 255, 255), 3, 8, 0);
		}

		cvNamedWindow("Circles");
		cv::imshow("Circles", output_image);
		cv::waitKey(0);
		cvDestroyAllWindows();
	}
}

void CoinDetector::isolate_coins(cv::Mat image, cv::Mat &output_image, cv::vector<cv::Mat> &output_coin_images){
	float ratio = scale_error_ratio;
	for (int i = 0; i < coin_positions.size(); i++) {
		//int x = coin_positions[i][0]-coin_positions[i][2]*ratio;
		//int y = coin_positions[i][1]-coin_positions[i][2]*ratio;
		int x = coin_positions[i][0] - coin_positions[i][2]-10;
		int y = coin_positions[i][1] - coin_positions[i][2]-10;

		//Make sure ROI never goes outside image boundaries
		/*int w_max = (x > 0) ? coin_positions[i][2]*ratio : coin_positions[i][0];
		w_max = (coin_positions[i][0]+coin_positions[i][2]*ratio < image.cols) ? w_max : image.cols - coin_positions[i][0];

		int h_max = (y > 0) ? coin_positions[i][2]*ratio : coin_positions[i][1];
		h_max = (coin_positions[i][1]+coin_positions[i][2]*ratio < image.rows) ? h_max : image.rows - coin_positions[i][1];
*/		
		std::string s = std::to_string(i);
		String isolated_image_name = "isolated_images/isolated_image_" + s + ".png";
		
		std::cout << isolated_image_name << std::endl;
		
		int w_max = coin_positions[i][2] + 10;
		int h_max = coin_positions[i][2] + 10;
		int width = 2*w_max;
		int height = 2*h_max;

		cv::Mat temp = image( cvRect(x , y,	width, height) );
		//devvrat
		cv::Mat temp_binary = output_image(cvRect(x, y, width, height));
		
		vector <vector<Point> > contours; // Vector for storing contour
		vector<Vec4i> hierarchy;
		findContours(temp_binary, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

		for (int i = 0; i < contours.size(); i++) {
			float area = contourArea(contours[i], false);
			if (area > 300) {
				drawContours(temp, contours, i, Scalar(0, 0, 255), 2, 8, hierarchy);
			}
		}
		//devvrat
		output_coin_images.push_back(temp);
		cv::imwrite(isolated_image_name, temp);
		

		if(debug) {
			//Initializing environment
			cvNamedWindow("Isolated", CV_WINDOW_AUTOSIZE);
			//cvResizeWindow("Isolated", 10, 20);

			//Output
			cv::imshow("Isolated", temp);
			cv::waitKey(0);
			cvDestroyAllWindows();
		}
	}
}

//Public functions

//Default Constructor
CoinDetector::CoinDetector(int debug_mode, float scale_error){
	debug = debug_mode;
	scale_error_ratio = scale_error;
}

//Running detector on an image
int CoinDetector::detect(cv::Mat image, cv::Mat &output_image){
	
	CoinDetector::preprocess(image, output_image);
	
	CoinDetector::find_circles(output_image, output_image);
	
	CoinDetector::isolate_coins(image, output_image, coin_images);


	if (coin_images.size() > 0)
	{
		output_image = coin_images[0];
		if (debug) {
			//Initializing environment
			cvNamedWindow("output_image", 1);

			//Output
			cv::imshow("output_image", output_image);
			cv::waitKey(0);
			cvDestroyAllWindows();
		}
		CoinDetector::draw_bounds(image, output_image);
	}
	else
	{
		string text = "No coin detected.";
		int fontFace = FONT_HERSHEY_SCRIPT_SIMPLEX;
		double fontScale = 2;
		int thickness = 3;
		cv::Point textOrg(3, 4);
		cv::putText(output_image, text, textOrg, fontFace, fontScale, Scalar::all(255), thickness, 8);
		cvNamedWindow("Coins", 1);

		//Output
		cv::imshow("Coins", output_image);
		cv::waitKey(0);
		cvDestroyAllWindows();
	}

	////TODO: implement this function so that it improves quality
	////Testing: Do not use! Reduces result quality
	//CoinDetector::correct_circles();
	

	return 1;
}

void CoinDetector::correct_circles(){
	cv::vector<cv::Vec3f> pos;

	for (int i = 0; i < coin_images.size(); i++){
		cv::Mat temp = coin_images[i];
		CoinDetector::preprocess(temp, temp);
		cv::HoughCircles(temp, pos, CV_HOUGH_GRADIENT, 1.5, temp.rows/2, 220, 140, 20, 150);
		corrected_positions.push_back(pos[0]);
		
		for (int i = 0; i < pos.size(); i++) {
			cv::Point center( cvRound(pos[i][0]), cvRound(pos[i][1]) );
			int radius = cvRound( pos[i][2] );
		
			cv::circle(temp, center, 3, cv::Scalar(0, 255, 0), -1, 8, 0);
			cv::circle(temp, center, radius, cv::Scalar(255, 255, 255), 3, 8, 0);
		}

		if(debug) {
			//Initializing environment
			cvNamedWindow("Fixed", 1);

			//Output
			cv::imshow("Fixed", temp);
			cv::waitKey(0);
			cvDestroyAllWindows();
		}
	}
}

void CoinDetector::draw_bounds(cv::Mat image, cv::Mat output_image){
	image.copyTo(output_image);

	float ratio = 1;
	
	for (int i = 0; i < coin_positions.size(); i++) {
		int x = coin_positions[i][0]-coin_positions[i][2]*ratio;
		int y = coin_positions[i][1]-coin_positions[i][2]*ratio;

		//Make sure ROI never goes outside image boundaries
		int w_max = (x > 0) ? coin_positions[i][2]*ratio : coin_positions[i][0];
		w_max = (coin_positions[i][0]+coin_positions[i][2]*ratio < image.cols) ? w_max : image.cols - coin_positions[i][0];

		int h_max = (y > 0) ? coin_positions[i][2]*ratio : coin_positions[i][1];
		h_max = (coin_positions[i][1]+coin_positions[i][2]*ratio < image.rows) ? h_max : image.rows - coin_positions[i][1];

		int width = 2*w_max;
		int height = 2*h_max;

		cv::rectangle(output_image, cv::Rect(x, y, width, height), cv::Scalar(0, 255, 0), 3);
	}

	if(debug) {
		//Initializing environment
		cvNamedWindow("Coins", 1);

		//Output
		cv::imshow("Coins", output_image);
		cv::waitKey(0);
		cvDestroyAllWindows();
	}
}

cv::vector<cv::Mat> CoinDetector::getCoins(){
	return coin_images;
}

//Returns positions of coin in image
//cv::vector<cv::Point2d> getCoinPositions(){}

//Returns radius of coins in image
//cv::vector<double> getCoinRadii(){}

//Returns classification of coins in image
//cv::vector<int> getCoinClass(){}