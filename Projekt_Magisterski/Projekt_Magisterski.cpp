// Projekt_Magisterski.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"


int main()
{
	std::vector<cv::String> files = GetFiles();
	for (int i = 0; i < files.size(); i++)
	{
		cv::Mat imageInput = cv::imread(files[i]);
		cv::Mat imageResized = ResizeImage(imageInput);
		cv::Mat imageThresholded = ThresholdImage(imageResized);
		cv::InputArray kernel = cv::Mat::ones(cv::Size(5, 5), CV_8U);
		cv::morphologyEx(imageThresholded, imageThresholded, cv::MORPH_CLOSE, kernel);
		cv::imshow("test", imageThresholded);
		cv::waitKey(0);
		DocAreaLSD::DocAreaLSD(imageThresholded);
	}



	cv::Mat imageInput = LoadImage();
	cv::Mat imageResized = ResizeImage(imageInput);
	cv::Mat imageThresholded = ThresholdImage(imageResized);
	cv::InputArray kernel = cv::Mat::ones(cv::Size(5, 5), CV_8U);
	cv::morphologyEx(imageThresholded, imageThresholded, cv::MORPH_CLOSE, kernel);
	cv::imshow("test", imageThresholded);
	cv::waitKey(0);
	DocAreaLSD::DocAreaLSD(imageThresholded);

	//std::vector<cv::String> files = GetFiles();
	//for (int i = 0; i < files.size(); i++)
	//{
	//	LineFeaturesTutorial(files[i]);
	//}
	
	return 0;
}

