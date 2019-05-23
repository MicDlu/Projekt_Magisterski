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
		DocAreaLSD::DocAreaLSD(imageFixSize);
	}
	
	return 0;
}

