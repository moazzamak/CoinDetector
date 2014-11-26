#include <opencv2/opencv.hpp>
#include "coin_detector.h"


//Private functions

//Preprocessing image
void CoinDetector::preprocess(cv::Mat image){

}

//Public functions

//Default Constructor
CoinDetector::CoinDetector(){

}

//Detecting coins from an image
int detect(cv::Mat image);

//Returns positions of coin in image
cv::Vector<cv::Point2d> getCoinPositions(){}

//Returns radius of coins in image
cv::Vector<double> getCoinRadii(){}

//Returns classification of coins in image
cv::Vector<int> getCoinClass(){}