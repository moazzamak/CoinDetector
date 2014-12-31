#include <opencv2/opencv.hpp>
#include <math.h>
#define PI (3.141592653589793)

#include <iostream>

#include "shift.hpp"
#include "coin_identifier.h"
#include "file_handler.h"

using namespace std;

void CoinIdentifier::preprocess(cv::Mat image, cv::Mat &output_image) {
	//Initialize temporary variables
	double min, max;

	image.copyTo(output_image);

	if(image.channels() > 1) {
		//Convert to grayscale
		cv::cvtColor(output_image, output_image, CV_RGB2GRAY);
	}

	//Threshold image to highlight edges
	cv::minMaxIdx(output_image, &min, &max);
	//cv::GaussianBlur(output_image, output_image, cv::Size(3,3), 2, 2);
	
	//Alternate to canny, more robust
	cv::adaptiveThreshold(output_image, output_image, max, cv::ADAPTIVE_THRESH_GAUSSIAN_C, cv::THRESH_BINARY_INV, 7, 7);

	if(debug) {
		cvNamedWindow("Preprocessed");
		cv::imshow("Preprocessed", output_image);
		cv::waitKey(0);
		cvDestroyAllWindows();
	}
}

void CoinIdentifier::unwrap_coin(cv::Mat image, cv::Mat &output_image) {
	cv::Vec2f center(image.cols/2, image.rows/2);

	//Calculate output image dimensions

	//Width would be equal to circumference of the image
	int width = 2*PI*center[0];
	int height = center[0];

	cv::Mat temp(height, width, image.type());

	for (int x = 0; x < width; x++) {
		double theta = (x / (double) width) * 2 * PI;

		for (int y = 0; y < height; y++) {
			double r = (double) y;
			float ix = ( r * cos(theta) ) + center[0];
			float iy = ( r * sin(theta) ) + center[1];

			if (temp.channels() != 1) {
				cv::Vec3b src, dst;
				src = image.at<cv::Vec3b>(iy, ix);
				
				cv::Point2f p(ix, iy);
				src = CoinIdentifier::interpolate(p, image);
				
				for (int i = 0; i < temp.channels(); i++) {
					dst[i] = src[i];
				}

				temp.at<cv::Vec3b>(y, x) = dst;
			}
			else if(temp.channels() == 1) {
				temp.at<uchar>(y, x) = image.at<uchar>(iy, ix);
			}
		}
	}

	temp.copyTo(output_image);

	if(debug) {
		cvNamedWindow("Unwrapped");
		cv::imshow("Unwrapped", output_image);
		cv::waitKey(0);
		cvDestroyAllWindows();
	}
}

void CoinIdentifier::convert_to_mask(cv::Mat image, cv::Mat &output){
	if(image.channels() != 1)
		cv::cvtColor(image, image, CV_BGR2GRAY);

	image.convertTo(image, CV_32F, 1/255.0);
	image.copyTo(output);
}

cv::Vec3b CoinIdentifier::interpolate(cv::Point2f pt, cv::Mat image) {
	cv::Mat patch;
	cv::getRectSubPix(image, cv::Size(1, 1), pt, patch);
	return patch.at<cv::Vec3b>(0,0);
}

//Public functions
CoinIdentifier::CoinIdentifier(int ndebug) {
	debug = ndebug;
}

void CoinIdentifier::identify_coins(cv::vector<cv::Mat> coins, cv::Mat image, cv::Mat &output_image) {
	for(int i = 0; i < coins.size(); i++) {
		int coin_hypothesis = identify(coins[i]);


		
	}
}

int CoinIdentifier::identify(cv::Mat image) {
		cv::Mat temp;
	
		//Extracting features from current coin contender image
		CoinIdentifier::unwrap_coin(image, image);
		CoinIdentifier::preprocess(image, image);
	
		convert_to_mask(image, image);	
		
		std::string folder_name = "features";

		int best_match;
		double best_score;

		cv::Mat best_image;

		//Matching coin contender with templates
		for (int i = 0; i < filelist.size(); i++) {
			std::string training_name = get_qualified_name(folder_name, filelist[i]);
			cv::Mat temp = cv::imread( training_name );

			cv::Mat new_image;
			cv::resize(image, new_image, temp.size());

			convert_to_mask(temp, temp);

			double local_best;

			for (int j = 0; j < new_image.rows; j++) {
				offset_x(new_image, new_image, j);

				cv::Mat overlap;
				cv::subtract(new_image, temp, overlap);
				cv::Scalar s = sum(sum(overlap));
				
				if(j==0 || abs(s[0]) < local_best)
					local_best = abs(s[0]);
			}

			if(i==0 || local_best < best_score) {
					best_score = local_best;
					best_match = i;
					temp.copyTo(best_image);
			}
		}

		if(1){
			cvNamedWindow("candidate");
			cvNamedWindow("template");

			cv::imshow("candidate", image);
			cv::imshow("template", best_image);
			cv::waitKey(0);
		}

		cout << best_match << "\t" << best_score << endl;
}

void CoinIdentifier::train() {
	cout << "Extracting features from training data." << endl;

	//File and folder names
	std::string training_folder_name = "training_data";
	std::string output_folder_name = "features";

	//Fetch list of files from the training folder
	FileHandler fh;
	filelist = fh.list_files(training_folder_name);

	if( filelist.size() == 0 ) {
		cout << "Error: Training files could not be found!" << endl;
		exit(-2);
	}
	
	//Generate features from training data
	for (int i = 0; i < filelist.size(); i++) {	
		if(debug)
			cout << filelist[i] << endl;

		std::string training_name = get_qualified_name(training_folder_name, filelist[i]);
		cv::Mat image = cv::imread( training_name );
		cv::Mat temp;

		CoinIdentifier::unwrap_coin(image, temp);
		CoinIdentifier::preprocess(temp, temp);
		
		std::string features_name = get_qualified_name(output_folder_name, filelist[i]);
		cv::imwrite(features_name, temp);
	}

	cout << "Generating features complete." << endl;
}

std::string CoinIdentifier::get_qualified_name(std::string folder_name, std::string file_name) {
	ostringstream ost;
	ost << folder_name << "/" << file_name;
	
	return ost.str();
}

void CoinIdentifier::offset_x(cv::Mat image, cv::Mat output, int offset){
	shift(image, image, cv::Point2f(offset, 0));
}