// Projekt_Magisterski.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"


int main()
{

	std::vector<cv::String> files = GetFiles();
	for (int i = 0; i < files.size(); i++)
	{
		cv::Mat imageInput = cv::imread(files[i]);
		cv::Mat imageFixSize = FixImageSize(imageInput);
		DocAreaLSD docAreaLSD(imageFixSize);
		std::vector<cv::Point> quads = docAreaLSD.GetQuadPoints();
		//for (int q = 0; q < 4; q++)
		//	cv::circle(imageFixSize, quads[q], 10, cv::Scalar(150, 150, 0), 5);
		//cv::imshow("test", imageFixSize);
		//cv::waitKey(0);
		cv::Mat imageMask = cv::Mat::zeros(imageFixSize.size(), CV_8UC3);
		cv::fillConvexPoly(imageMask, quads, cv::Scalar(255, 255, 255));

		cv::Mat imageMasked;
		cv::bitwise_and(imageFixSize, imageMask, imageMasked);
		//cv::imshow("test", imageMasked);
		//cv::waitKey(0);

		std::vector<cv::Point2f> quadSource;
		for (int q = 0; q < 4; q++)
			quadSource.push_back(cv::Point2f(quads[q].x, quads[q].y));

		std::vector<cv::Point2f> quadDestination;
		quadDestination.push_back(cv::Point2f(0, 0));
		quadDestination.push_back(cv::Point2f(0, imageFixSize.size().height));
		quadDestination.push_back(cv::Point2f(imageFixSize.size().width, imageFixSize.size().height));
		quadDestination.push_back(cv::Point2f(imageFixSize.size().width, 0));

		cv::Mat transform = cv::getPerspectiveTransform(quadSource, quadDestination);
		cv::Mat imageWarped;
		cv::warpPerspective(imageMasked, imageWarped, transform, imageFixSize.size());


		//cv::imshow("test", imageWarped);
		//cv::waitKey(0);
		Otsu3(imageWarped);
		//OtsuN(imageWarped,2);
		continue;
	}
	
	return 0;
}

