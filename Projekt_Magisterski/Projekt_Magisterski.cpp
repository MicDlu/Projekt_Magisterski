// Projekt_Magisterski.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"


int main()
{
	std::vector<cv::String> files = GetFiles();
	for (int i = 0; i < files.size(); i++)
	{
		cv::Mat imageInput = cv::imread(files[i],cv::COLORMAP_HSV);
		cv::Mat imageFixSize = FixImageSize(imageInput);
		DocAreaLSD docAreaLSD(imageFixSize);
		std::vector<cv::Point2f> quads = docAreaLSD.GetQuadPoints();
		for (int q = 0; q < 4; q++)		
			cv::circle(imageFixSize, quads[q], 10, cv::Scalar(150, 150, 0), 5);

		cv::imshow("test", imageFixSize);
		cv::waitKey(0);
	}
	
	return 0;
}

