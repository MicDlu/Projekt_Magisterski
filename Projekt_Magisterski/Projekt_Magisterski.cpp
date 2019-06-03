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
			imageExtract = ReduceVariety(imageExtract);
			OtsuN otsu(imageExtract, 3);
			cv::Mat imageLevelAll = otsu.ReturnLeveledImage();
			cv::Mat imageLevel1 = otsu.ReturnLevelImage(0);
			cv::Mat imageLevel2 = otsu.ReturnLevelImage(1);
			cv::Mat imageLevel3 = otsu.ReturnLevelImage(2);
			cv::imshow("image all levels", imageLevelAll);
			cv::imshow("image level 1", imageLevel1);
			cv::imshow("image level 2", imageLevel2);
			cv::imshow("image level 3", imageLevel3);
			cv::waitKey(0);
		}

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
			cv::Mat imageLevel1 = otsu.ReturnLevelImage(0);
			cv::Mat imageLevel2 = otsu.ReturnLevelImage(1);
			cv::Mat imageLevel3 = otsu.ReturnLevelImage(2);
			cv::imshow("image all levels", imageLevelAll);
			cv::imshow("image level 1", imageLevel1);
			cv::imshow("image level 2", imageLevel2);
			cv::imshow("image level 3", imageLevel3);
			cv::waitKey(0);
			cv::destroyAllWindows();
		}

		// HOUGH
		if (HOUGH)
		{
			cv::Mat houghInput;
			cv::Canny(imageFixSize, houghInput, 50, 200);
			imshow("houghp", houghInput);
			cv::waitKey(0);
			std::vector<cv::Vec2f> lines;

			cv::HoughLines(houghInput, lines, 1, CV_PI / 180, 100, 0, 0);

			cv::Mat houghTemp = cv::Mat::zeros(houghInput.size(), CV_8U);
			for (size_t i = 0; i < lines.size(); i++)
			{
				float rho = lines[i][0], theta = lines[i][1];
				cv::Point pt1, pt2;
				double a = cos(theta), b = sin(theta);
				double x0 = a * rho, y0 = b * rho;
				pt1.x = cvRound(x0 + 500 * (-b));
				pt1.y = cvRound(y0 + 500 * (a));
				pt2.x = cvRound(x0 - 500 * (-b));
				pt2.y = cvRound(y0 - 500 * (a));
				cv::line(houghTemp, pt1, pt2, cv::Scalar(0, 0, 255), 3, cv::LINE_AA);
				cv::circle(houghTemp, pt1, 2, cv::Scalar(255, 0, 0), 2);
				cv::circle(houghTemp, pt2, 2, cv::Scalar(255, 0, 0), 2);
			}
			imshow("hough", houghTemp);
			cv::waitKey(0);
		}
	}
	return 0;
}

