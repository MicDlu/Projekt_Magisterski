#include "pch.h"

cv::Mat LoadImage()
{
	cv::String directoryPath = "C:/Users/micha/source/Repos/Projekt_Magisterski/Projekt_Magisterski/Images/";
	cv::String fileName = "IMG_20190228_200009.jpg";
	return cv::imread(directoryPath + fileName);
}

cv::Mat FixImageSize(cv::Mat input)
{
	cv::Mat output;
	cv::Size resizeSize = cv::Size(800, 600);
	cv::resize(input, output, resizeSize);
	return output;
}

cv::Mat ThresholdImage(cv::Mat input)
{
	cv::Mat output;
	cv::Mat inputGray;
	cvtColor(input, inputGray, cv::COLOR_RGB2GRAY);
	cv::adaptiveThreshold(inputGray, output, 255,cv::AdaptiveThresholdTypes::ADAPTIVE_THRESH_GAUSSIAN_C,cv::ThresholdTypes::THRESH_BINARY,11,2);
	return output;
}

std::vector<cv::String> GetFiles()
{
	std::vector<cv::String> files;
	cv::String directory = "C:/Users/micha/Pictures/Images/";
	files.push_back(directory + "IMG_20190228_195957.jpg");
	files.push_back(directory + "IMG_20190228_200009.jpg");
	files.push_back(directory + "IMG_20190228_200028.jpg");
	files.push_back(directory + "IMG_20190228_200046.jpg");
	files.push_back(directory + "IMG_20190228_200050.jpg");
	files.push_back(directory + "IMG_20190228_200058.jpg");
	return files;
}