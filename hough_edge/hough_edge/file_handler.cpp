#include <opencv2\opencv.hpp>
#include <iostream>
#include "dirent.h"
#include "file_handler.h"

cv::vector<std::string> FileHandler::list_files(std::string directory){
	cv::vector<std::string> output;
	
	DIR *pDIR;
    struct dirent *entry;
	if( pDIR=opendir(directory.c_str()) ){
            while(entry = readdir(pDIR)){
                    if( strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0 ){
						output.push_back(entry->d_name);
					}
            }
            closedir(pDIR);
    }
	return output;
}