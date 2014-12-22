#include <sstream>
#include <string>
#include <opencv2/opencv.hpp>

#include "image_stream.h"

using namespace std;

ImageStream::ImageStream(std::string sfolder_name, std::string sfile_pre, std::string sfile_extension, int nFrames) {
	folder_name = sfolder_name;
	file_pre = sfile_pre;
	file_extension = sfile_extension;
	total_frames = nFrames;

	counter = 0;
}

void ImageStream::operator>>(cv::Mat &image) {
	image = get_next_frame();
}

cv::Mat ImageStream::get_next_frame() {
	counter = counter + 1;

	if ( counter >= total_frames )
		counter = 1;
	
	std::string qualified_file_name = get_qualified_name();
	cv::Mat image = cv::imread(qualified_file_name);


	return image;
}

std::string ImageStream::get_qualified_name() {
	ostringstream ost;
	ost << folder_name << "/" << file_pre << counter << file_extension;
	
	return ost.str();
}