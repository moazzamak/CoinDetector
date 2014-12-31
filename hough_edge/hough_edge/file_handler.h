#ifndef FILE_HANDLER_H
#define FILE_HANDLER_H

#include <opencv2/opencv.hpp>

//Import 3rd party headers
#include "dirent.h"

class FileHandler{
private:
public:
	cv::vector<std::string> list_files(std::string directory);
};

#endif