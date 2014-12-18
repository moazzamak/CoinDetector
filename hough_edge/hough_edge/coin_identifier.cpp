#include <opencv2/opencv.hpp>
#include <math.h>
#define PI (3.141592653589793)

#include <iostream>

#include "coin_identifier.h"


using namespace std;

void CoinIdentifier::preprocess(cv::Mat image, cv::Mat &output_image){
	//Initialize temporary variables
	double min, max;

	//Convert to grayscale
	cv::cvtColor(image, output_image, CV_RGB2GRAY);
	
	//Threshold image to highlight edges
	cv::minMaxIdx(output_image, &min, &max);
	//cv::GaussianBlur(output_image, output_image, cv::Size(3,3), 2, 2);
	
	//cv::dilate(output_image, output_image, cv::KERNEL_GENERAL);
	
	////Canny works better with estimating centres
	//cv::Canny(output_image, output_image, 150, 350);

	//Alternate to canny, more robust
	cv::adaptiveThreshold(output_image, output_image, max, cv::ADAPTIVE_THRESH_GAUSSIAN_C, cv::THRESH_BINARY_INV, 7, 7);

	if(debug){
		cvNamedWindow("Preprocessed");
		cv::imshow("Preprocessed", output_image);
		cv::waitKey(0);
		cvDestroyAllWindows();
	}
}

void CoinIdentifier::unwrap_coin(cv::Mat image, cv::Mat &output_image){
	cv::Vec2f center(image.cols/2, image.rows/2);

	//Calculate output image dimensions

	//Width would be equal to circumference of the image
	int width = 2*PI*center[0];
	int height = center[0];

	cv::Mat temp(height, width, image.type());

	for (int x = 0; x < width; x++){
		double theta = (x / (double) width) * 2 * PI;

		for (int y = 0; y < height; y++){
			double r = (double) y;
			float ix = ( r * cos(theta) ) + center[0];
			float iy = ( r * sin(theta) ) + center[1];

			if (temp.channels() != 1){
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

	cout <<  temp.rows << endl << temp.cols << endl;

	temp.copyTo(output_image);

	if(debug){
		cvNamedWindow("Unwrapped");
		cv::imshow("Unwrapped", output_image);
		cv::waitKey(0);
		cvDestroyAllWindows();
	}
}

cv::Vec3b CoinIdentifier::interpolate(cv::Point2f pt, cv::Mat image){
	cv::Mat patch;
	cv::getRectSubPix(image, cv::Size(1, 1), pt, patch);
	return patch.at<cv::Vec3b>(0,0);
}

//Public functions
CoinIdentifier::CoinIdentifier(){
	debug = 1;
}

void CoinIdentifier::identify(cv::Mat image, cv::Mat &output_image){
	CoinIdentifier::unwrap_coin(image, output_image);
	//CoinIdentifier::preprocess(output_image, output_image);
}