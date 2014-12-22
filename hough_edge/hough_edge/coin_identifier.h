#ifndef COIN_IDENTIFIER_H
#define COIN_IDENTIFIER_H

#include <opencv2/opencv.hpp>

class CoinIdentifier {
private:
	int debug;
	cv::vector<std::string> filelist;
	
	void unwrap_coin(cv::Mat image, cv::Mat &output_image);
	void preprocess(cv::Mat image, cv::Mat &output_image);
	cv::Vec3b interpolate(cv::Point2f pt, cv::Mat image);
	void CoinIdentifier::convert_to_mask(cv::Mat image, cv::Mat &output);
	
public:
	CoinIdentifier(int ndebug = 0);
	void identify_coins(cv::vector<cv::Mat> coins, cv::Mat image, cv::Mat &output_image);
	int identify(cv::Mat image);
	void train();
	void offset_x(cv::Mat image, cv::Mat output, int offset);

	//Might need to move this function to a separate file to handle String operations
	std::string get_qualified_name(std::string folder_name, std::string file_name);
};

#endif