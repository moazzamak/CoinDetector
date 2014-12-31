#ifndef FILE_STREAM_H
#define FILE_STREAM_H

#include <sstream>
#include <string>
#include <opencv2/opencv.hpp>

class ImageStream{
private:
	std::string folder_name;
	std::string file_pre;
	std::string file_extension;
	int total_frames;
	int counter;

public:
	ImageStream(std::string sfolder_name, std::string sfile_pre, std::string sfile_extension, int nFrames);
	void operator>>(cv::Mat &image);
	cv::Mat get_next_frame();
	std::string get_qualified_name();
};

#endif