#ifndef COIN_IDENTIFIER_H
#define COIN_IDENTIFIER_H

#include <opencv2/opencv.hpp>

class CoinIdentifier {
private:
	int debug;
	int divisions;

	cv::vector<std::string> filelist;
	cv::vector<int> hyp_list;

	void unwrap_coin(cv::Mat image, cv::Mat &output_image);
	void preprocess(cv::Mat image, cv::Mat &output_image);
	void CoinIdentifier::convert_to_mask(cv::Mat image, cv::Mat &output);
	void offset_x(cv::Mat image, cv::Mat &output, int offset);

	cv::Vec3b interpolate(cv::Point2f pt, cv::Mat image);
	cv::Scalar get_gray_values(cv::Mat image, int levels);
	cv::Scalar abs_scalar(cv::Scalar m);
	cv::Mat load_coin_from_template( int n, std::string folder_name);
	double get_coin_val(int n);

public:
	CoinIdentifier(int ndebug = 0, int ndivisions = 3);
	void identify_coins(cv::vector<cv::Mat> coins, cv::vector<int> coin_class_groups);
	int identify(cv::Mat image, int coin_class_group);
	void train();
	void draw_coins(cv::Mat image, cv::Mat &output, cv::vector<cv::Vec3f> coin_positions, cv::Vector<int> hyp_list);
	bool getTemplateClass(int index);

	//Might need to move this function to a separate file to handle String operations
	std::string get_qualified_name(std::string folder_name, std::string file_name);
	cv::vector<int> getCoinClass();
};

#endif