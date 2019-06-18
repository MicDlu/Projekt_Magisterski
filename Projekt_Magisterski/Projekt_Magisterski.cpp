// Projekt_Magisterski.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"


int main()
{
	std::vector<cv::String> files = GetFiles();

	// OTSU 3 LEVEL PAPER THRESHOLDING
	if (OTSU_1)
		for (int i = 0; i < files.size(); i++)
		{
			cv::Mat imageInput = cv::imread(files[i]);
			cv::Mat imageFixSize = FixImageSize(imageInput);
			cv::Mat imageExtract = RemoveBackground(imageFixSize);
			cv::Mat imagePreOtsu = OtsuPreReduceVariety(imageExtract);
			OtsuN otsu(imagePreOtsu, 3);
			cv::Mat imageLevelAll = otsu.ReturnLeveledImage();
			std::vector<cv::Mat> imageLevels = otsu.ReturnImageLevels();
			//cv::imshow("image all levels", imageLevelAll);
			/*cv::imshow("image level 1", imageLevels[0]);
			cv::imshow("image level 2", imageLevels[1]);
			cv::imshow("image level 3", imageLevels[2]);
			cv::waitKey(0);*/

			std::vector<cv::Point2f> intersectionPoints = GetGridLevelIntersections(imageLevels[1]);
			cv::RNG rng(12345);
			cv::Mat imageIntersections = cv::Mat::zeros(IMAGE_SIZE, CV_8UC3);
			cv::copyTo(imageExtract, imageIntersections, cv::Mat::ones(IMAGE_SIZE, CV_8U));
			for (int i = 0; i < intersectionPoints.size(); i++)
			{
				circle(imageIntersections, intersectionPoints[i], 2, cv::Scalar(0, 0, 255), -1, 8, 0);
			}
			cv::imshow("Intersection Points", imageIntersections);
			cv::waitKey(0);

			cv::destroyAllWindows();
		}

	// Temporary deprecated
	/*
	//std::vector<cv::String> files = GetFiles();
	for (int i = 0; i < files.size(); i++)
	{
		cv::Mat imageInput = cv::imread(files[i]);
		cv::Mat imageFixSize = FixImageSize(imageInput);

		DocAreaLSD docAreaLSD(imageFixSize);
		std::vector<cv::Point> quads = docAreaLSD.GetQuadPoints();

		// DRAW  EDGE POINTS
		if (DRAW_EDGE)
		{
			for (int q = 0; q < 4; q++)
				cv::circle(imageFixSize, quads[q], 10, cv::Scalar(150, 150, 0), 5);
			cv::imshow("edges", imageFixSize);
			cv::waitKey(0);
		}

		// PERSPECTIVE TRANSFORM
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
		cv::warpPerspective(imageFixSize, imageWarped, transform, imageFixSize.size());
		cv::imshow("warped", imageWarped);
		cv::waitKey(0);

		// 3- level thresholding
		if (OTSU_2)
		{
			cv::Mat	imageExtract = ReduceVariety(imageWarped);
			OtsuN otsu(imageExtract, 3);
			cv::Mat imageLevelAll = otsu.ReturnLeveledImage();
			std::vector<cv::Mat> imageLevels = otsu.ReturnImageLevels();
			//cv::imshow("image all levels", imageLevelAll);
			//cv::imshow("image level 1", imageLevels[0]);
			//cv::imshow("image level 2", imageLevels[1]);
			//cv::imshow("image level 3", imageLevels[2]);
			//cv::waitKey(0);
			//cv::destroyAllWindows();
		}
	}
	*/
	return 0;
}


std::vector<cv::Point2f> GetGridLevelIntersections(cv::Mat imageGridLevel)
{
	int windowNo = 0;
	if (imageGridLevel.channels() > 1)
		cvtColor(imageGridLevel, imageGridLevel, cv::COLOR_RGB2GRAY);
	//cv::imshow(std::to_string(windowNo++), imageGridLevel);

	// Calculate Harris Corners
	// https://docs.opencv.org/3.0-beta/doc/py_tutorials/py_feature2d/py_features_harris/py_features_harris.html
	cv::Mat imageHarris = cv::Mat::zeros(imageGridLevel.size(), CV_8U);
	cv::cornerHarris(imageGridLevel, imageHarris, 4, 3, 0.05);
	//cv::imshow(std::to_string(windowNo++), imageHarris);

	// Binarize corner pixels
	cv::erode(imageHarris, imageHarris, cv::Mat::ones(cv::Size(3, 3), CV_8U));
	cv::imshow(std::to_string(windowNo++), imageHarris);
	cv::threshold(imageHarris, imageHarris, 0.01, 1, cv::ThresholdTypes::THRESH_BINARY);
	//cv::imshow(std::to_string(windowNo++), imageHarris);

	// Calculate centers of contours
	// https://www.pyimagesearch.com/2016/02/01/opencv-center-of-contour/
	// https://docs.opencv.org/2.4/doc/tutorials/imgproc/shapedescriptors/moments/moments.html
	imageHarris.convertTo(imageHarris, CV_8U, 255);
	std::vector<std::vector<cv::Point>> contours;
	cv::findContours(imageHarris, contours, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);
	cv::Mat imageCont = cv::Mat::zeros(imageGridLevel.size(), CV_8U);
	std::vector<cv::Moments> mu(contours.size());
	std::vector<cv::Point2f> mc(contours.size());
	for (int i = 0; i < contours.size(); i++)
	{
		mu[i] = moments(contours[i], true);
		mc[i] = cv::Point2f(mu[i].m10 / mu[i].m00, mu[i].m01 / mu[i].m00);
	}

	//cv::RNG rng(12345);
	//cv::Mat imageIntersections = cv::Mat::zeros(IMAGE_SIZE, CV_8UC3);
	//cv::copyTo(imageGridLevel, imageIntersections, cv::Mat::ones(IMAGE_SIZE, CV_8U));
	//for (int i = 0; i < mu.size(); i++)
	//{
	//	cv::Scalar colorRng = cv::Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
	//	drawContours(imageIntersections, contours, i, colorRng, 2, 8);
	//	circle(imageIntersections, mc[i], 2, cv::Scalar(0, 0, 255), -1, 8, 0);
	//}
	//cv::imshow(std::to_string(windowNo++), imageIntersections);

	return mc;
}

