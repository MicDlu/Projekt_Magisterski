// Projekt_Magisterski.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"


int main()
{
	// std::vector<cv::String> files = GetFiles();
	std::vector<cv::String> files = GetFiles2();
	
	// OTSU 3 LEVEL PAPER THRESHOLDING
	if (OTSU_1)
		for (int i = 1; i < files.size(); i++)
		{
			cv::Mat imageInput = cv::imread(files[i]);
			cv::Mat imageFixSize = FixImageSize(imageInput);
			// Calc quad
			DocAreaLSD docAreaLSD(imageFixSize);
			std::vector<cv::Point> quads = docAreaLSD.GetQuadPoints();
			// Crop to page			
			cv::Mat maskBkgd = GetBackgroundMask(imageFixSize);
			char pageSide = CalcPageSide(maskBkgd);
			cv::Mat maskPage = RemoveOtherPage(maskBkgd, pageSide, quads);
			cv::Mat imagePage;
			cv::copyTo(imageFixSize, imagePage, maskPage);
			// Thresholding
			cv::Mat imagePreOtsu = OtsuPreReduceVariety(imagePage,maskPage);
			OtsuN otsu(imagePreOtsu, 3);
			cv::Mat imageLevelAll = otsu.ReturnLeveledImage();
			std::vector<cv::Mat> imageLevels = otsu.ReturnImageLevels();
			cv::imshow("page mask", maskPage);
			cv::imshow("page image", imagePage);
			cv::imshow("image all levels", imageLevelAll);
			cv::imshow("image level 1", imageLevels[0]);
			cv::imshow("image level 2", imageLevels[1]);
			cv::imshow("image level 3", imageLevels[2]);
			cv::waitKey(0);

			// Intersection points
			std::vector<cv::Point2f> intersectionPoints = GetGridLevelIntersections(imageLevels[1]);

			// Draw points
			cv::Mat imageIntersectPoints = imagePage.clone();
			for (int i = 0; i < intersectionPoints.size(); i++)
			{
				circle(imageIntersectPoints, intersectionPoints[i], 0, cv::Scalar(0,0,255), 4);
			}
			cv::imshow("imageIntersections", imageIntersectPoints);

			// Draw lines
			//PointLines pointLines(intersectionPoints,quads);
			//std::vector<std::vector<cv::Point2f>> vertLines = pointLines.GetVerticalLines();
			//cv::RNG rng(12345);
			//cv::Mat imageIntersectLines = imagePage.clone();
			//for (int i = 0; i < vertLines.size(); i++)
			//{
			//	cv::Scalar colorRng = cv::Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
			//	for (int j = 0; j < vertLines[i].size(); j++)
			//	{
			//		circle(imageIntersectLines, vertLines[i][j], 0, colorRng, 5);
			//	}
			//}
			//cv::imshow("Intersections lines", imageIntersectLines);
			cv::waitKey(0);
			cv::destroyAllWindows();
		}

	return 0;
}

void OldCode_DrawingAndPerspectiveTransform()
{
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
		cv::Mat	image = ReduceVariety(imageWarped);
		OtsuN otsu(image, 3);
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
}

cv::Point2f GetNearestPointIdx(std::vector<cv::Point2f> &intersectionPoints, cv::Point2f &pnt)
{
	// https://stackoverflow.com/questions/9825959/find-closest-neighbors-opencv
	cv::flann::KDTreeIndexParams indexParams;
	cv::flann::Index kdtree(cv::Mat(intersectionPoints).reshape(1), indexParams);
	std::vector<float> query;
	query.push_back(pnt.x);
	query.push_back(pnt.y);
	std::vector<int> indices;
	std::vector<float> dists;
	kdtree.knnSearch(query, indices, dists, 2);
	// Reject entry point
	for (int i = 0; i < 2; i++)	
		if (dists[i] != 0)
			return intersectionPoints[indices[i]];	
}

cv::Mat RemoveOtherPage(cv::Mat image, char pageSide, std::vector<cv::Point> quads)
{
	std::vector<cv::Point> polyPoints;
	if (pageSide == 'L')
	{
		LineExtender notebookSpine(quads[2], quads[3]);
		cv::Vec4f spineExtend = notebookSpine.GetLineExtends(IMAGE_FIX_SIZE);
		polyPoints.push_back(cv::Point(spineExtend[0], spineExtend[1]));
		polyPoints.push_back(cv::Point(spineExtend[2], spineExtend[3]));
		polyPoints.push_back(cv::Point(IMAGE_FIX_SIZE.width - 1, 0));
		polyPoints.push_back(cv::Point(IMAGE_FIX_SIZE.width - 1, IMAGE_FIX_SIZE.height - 1));
	}
	else if (pageSide == 'R')
	{
		LineExtender notebookSpine(quads[0], quads[1]);
		cv::Vec4f spineExtend = notebookSpine.GetLineExtends(IMAGE_FIX_SIZE);
		polyPoints.push_back(cv::Point(spineExtend[0], spineExtend[1]));
		polyPoints.push_back(cv::Point(spineExtend[2], spineExtend[3]));
		polyPoints.push_back(cv::Point(0, 0));
		polyPoints.push_back(cv::Point(0, IMAGE_FIX_SIZE.height - 1));
	}
	cv::Mat imageCut = image.clone();
	cv::fillConvexPoly(imageCut, polyPoints, cv::Scalar(0));
	//cv::imshow("image poly removed", imageCut);
	return imageCut;
}

char CalcPageSide(cv::Mat imageNoBkgd)
{
	// Convert to double type
	cv::Mat img64 = imageNoBkgd;
	if (imageNoBkgd.channels() > 1)
		cv::cvtColor(imageNoBkgd, img64, cv::COLOR_BGR2GRAY);
	img64.convertTo(img64, CV_64FC3);

	// Calculate columns sum histogram
	// https://stackoverflow.com/questions/12833657/column-sum-of-opencv-matrix-elements
	// https://docs.opencv.org/2.4/modules/core/doc/operations_on_arrays.html#reduce
	cv::Mat matHist = cv::Mat::zeros(cv::Size(img64.cols, 1), CV_64FC1);
	cv::reduce(img64, matHist, 0, cv::REDUCE_SUM);

	// Calculate minimum value of column sum
	cv::Mat MatHistMin;
	cv::reduce(matHist, MatHistMin, 1, cv::REDUCE_MIN);
	double histMin = MatHistMin.at<double>(0);

	// Calculate average position of minimum value
	int sum = 0;
	int count = 0;
	for (int i = 0; i < matHist.cols; i++)
	{
		if (matHist.at<double>(i) == histMin)
		{
			sum += i;
			count += 1;
		}
	}

	// Return result
	//if (count == 0)
	//	return '?';
	double histMinPosAvg = sum / count;
	if (histMinPosAvg < img64.cols / 2)
		return 'L';
	else
		return 'R';
}


std::vector<cv::Point2f> GetGridLevelIntersections(cv::Mat imageGridLevel)
{
	// Parameters
	int harrisBlock = 3;
	int harrisKSize = 3;
	int harrisK = 0.05; // whatever
	int erodeSize = 4;

	int windowNo = 0;
	if (imageGridLevel.channels() > 1)
		cvtColor(imageGridLevel, imageGridLevel, cv::COLOR_RGB2GRAY);
	//cv::imshow(std::to_string(windowNo++), imageGridLevel);

	// Calculate Harris Corners
	// https://docs.opencv.org/3.0-beta/doc/py_tutorials/py_feature2d/py_features_harris/py_features_harris.html
	cv::Mat imageHarris = cv::Mat::zeros(imageGridLevel.size(), CV_8U);
	cv::cornerHarris(imageGridLevel, imageHarris, harrisBlock, harrisKSize, harrisK);
	//cv::imshow(std::to_string(windowNo++), imageHarris);

	// Binarize corner pixels
	cv::erode(imageHarris, imageHarris, cv::Mat::ones(cv::Size(erodeSize, erodeSize), CV_8U));
	//cv::imshow(std::to_string(windowNo++), imageHarris);
	cv::threshold(imageHarris, imageHarris, 0.01, 1, cv::ThresholdTypes::THRESH_BINARY);
	//cv::imshow(std::to_string(windowNo++), imageHarris);

	// Calculate contours
	// https://www.pyimagesearch.com/2016/02/01/opencv-center-of-contour/
	// https://docs.opencv.org/2.4/doc/tutorials/imgproc/shapedescriptors/moments/moments.html
	imageHarris.convertTo(imageHarris, CV_8U, 255);
	std::vector<std::vector<cv::Point>> contours;
	cv::findContours(imageHarris, contours, cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE);

	// Calc average contour rectangle boundary size
	cv::Size avgRectSize = cv::Size(0, 0);
	for (int i = 0; i < contours.size(); i++)
	{
		avgRectSize += cv::boundingRect(contours[i]).size();
	}
	avgRectSize.width /= contours.size();
	avgRectSize.height /= contours.size();

	// Reject invalid contours and calculate centers
#define REJECT_FACTOR 3
	std::vector<cv::Point2f> mc;
	for (int i = 0; i < contours.size(); i++)
	{
		cv::Rect contourRect = cv::boundingRect(contours[i]);
		if ((contourRect.width <= (REJECT_FACTOR * avgRectSize.width)) && (contourRect.height <= (REJECT_FACTOR * avgRectSize.height))) {
			cv::Moments mu = moments(contours[i], true);
			if (mu.m00 != 0)
				mc.push_back(cv::Point2f(mu.m10 / mu.m00, mu.m01 / mu.m00));
			else if ((contourRect.width > avgRectSize.width) && (contourRect.height > avgRectSize.height))
				mc.push_back(cv::Point2f(contourRect.x + contourRect.width / 2, contourRect.y + contourRect.height / 2));
		}
	}

	// Visualize
	////cv::RNG rng(12345);
	//cv::Mat imageIntersectPoints = imageGridLevel.clone();
	//cv::cvtColor(imageIntersectPoints, imageIntersectPoints, cv::COLOR_GRAY2BGR);
	//// Visualize contours
	//for (int i = 0; i < contours.size(); i++)
	//{
	//	cv::Scalar colorRng;// = cv::Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
	//	cv::Rect contourRect = cv::boundingRect(contours[i]);
	//	if ((contourRect.width <= (REJECT_FACTOR * avgRectSize.width)) && (contourRect.height <= (REJECT_FACTOR * avgRectSize.height)))
	//		colorRng = cv::Scalar(200, 0, 0);
	//	else// if ((contourRect.width > avgRectSize.width) && (contourRect.height > avgRectSize.height))
	//		colorRng = cv::Scalar(0, 0, 255);
	//	drawContours(imageIntersectPoints, contours, i, colorRng, 3, 8);
	//}
	//// Visualize points
	//for (int i = 0; i < mc.size(); i++)
	//{
	//	circle(imageIntersectPoints, mc[i], 0, cv::Scalar(0, 255, 0), 2);
	//}
	//cv::imshow("Contours and centers", imageIntersectPoints);
	//cv::waitKey(0);

	return mc;
}

