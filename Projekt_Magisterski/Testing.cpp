#include "pch.h"
#include "Testing.h"


Testing::Testing()
{
}


Testing::~Testing()
{
}

int currHarrisBlockSize = 1;
int currHarrisKSize = 1;
int convHarrisKSize = 1;
double currHarrisK = 0.05;
int convHarrisK = currHarrisK *100;
int currErodeSize = 3;
double currThresh = 0.01;
int convThresh = currThresh*100;
double currThreshMax = 1;
int convThreshMax = 1;
cv::Mat image;

void TrackbarIntersectionPoints(cv::Mat imageIn)
{
	image = imageIn;
	std::string winName = "Testing";

	cv::namedWindow(winName, 1);
	cv::createTrackbar("BlockSize", winName, &currHarrisBlockSize, 10, OnHarrisBlockSizeChange);
	cv::createTrackbar("KSize", winName, &currHarrisKSize, 20, OnHarrisBlockSizeChange);
	cv::createTrackbar("K", winName, &convHarrisK, 100, OnHarrisBlockSizeChange);
	cv::createTrackbar("ErodeSize", winName, &currErodeSize, 10, OnHarrisBlockSizeChange);
	cv::createTrackbar("Thresh", winName, &convThreshMax, 100, OnHarrisBlockSizeChange);

	TestGetGridLevelIntersections(image);
	cv::waitKey(0);
}

void OnHarrisBlockSizeChange(int, void*)
{
	if (!currHarrisBlockSize)
		currHarrisBlockSize = 1;
	currHarrisKSize = (currHarrisKSize % 2) ? currHarrisKSize : currHarrisKSize + 1;
	double currHarrisK = convHarrisK / 100.0;
	double currThresh = convThresh / 100.0;
	TestGetGridLevelIntersections(image);
}

std::vector<cv::Point2f> TestGetGridLevelIntersections(cv::Mat imageGridLevel)
{
	cv::Mat imageGridLevelCopy;
	cv::copyTo(imageGridLevel, imageGridLevelCopy, cv::Mat::ones(IMAGE_FIX_SIZE, CV_8U));

	int windowNo = 0;
	if (imageGridLevelCopy.channels() > 1)
		cvtColor(imageGridLevelCopy, imageGridLevelCopy, cv::COLOR_RGB2GRAY);
	//cv::imshow(std::to_string(windowNo++), imageGridLevelCopy);

	// Calculate Harris Corners
	// https://docs.opencv.org/3.0-beta/doc/py_tutorials/py_feature2d/py_features_harris/py_features_harris.html
	cv::Mat imageHarris = cv::Mat::zeros(imageGridLevelCopy.size(), CV_8U);
	cv::cornerHarris(imageGridLevelCopy, imageHarris, currHarrisBlockSize, currHarrisKSize, currHarrisK);
	cv::imshow("harris", imageHarris);

	// Binarize corner pixels
	cv::Mat kernel = cv::getStructuringElement(cv::MORPH_CROSS, cv::Size(currErodeSize, currErodeSize));
	cv::erode(imageHarris, imageHarris, kernel);
	cv::imshow("erode", imageHarris);
	cv::threshold(imageHarris, imageHarris, currThresh, currThreshMax, cv::ThresholdTypes::THRESH_BINARY);
	cv::imshow("thresh", imageHarris);

	// Calculate centers of contours
	// https://www.pyimagesearch.com/2016/02/01/opencv-center-of-contour/
	// https://docs.opencv.org/2.4/doc/tutorials/imgproc/shapedescriptors/moments/moments.html
	imageHarris.convertTo(imageHarris, CV_8U, 255);
	std::vector<std::vector<cv::Point>> contours;
	cv::findContours(imageHarris, contours, cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE);
	cv::Mat imageCont = cv::Mat::zeros(imageGridLevelCopy.size(), CV_8U);
	std::vector<cv::Moments> mu(contours.size());
	std::vector<cv::Point2f> mc(contours.size());
	for (int i = 0; i < contours.size(); i++)
	{
		mu[i] = moments(contours[i], true);
		mc[i] = cv::Point2f(mu[i].m10 / mu[i].m00, mu[i].m01 / mu[i].m00);
	}

	cv::RNG rng(12345);
	cv::Mat imageIntersections = cv::Mat::zeros(IMAGE_FIX_SIZE, CV_8UC3);
	//cv::copyTo(imageGridLevelCopy, imageIntersections, cv::Mat::ones(IMAGE_FIX_SIZE, CV_8U));
	cvtColor(imageGridLevelCopy, imageGridLevelCopy, cv::COLOR_GRAY2BGR);
	for (int i = 0; i < mu.size(); i++)
	{
		cv::Scalar colorRng = cv::Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
		drawContours(imageIntersections, contours, i, colorRng, 2, 8);
		circle(imageIntersections, mc[i], 2, cv::Scalar(0, 0, 255), -1, 8, 0);
		circle(imageGridLevelCopy, mc[i], 2, cv::Scalar(0, 0, 255), -1, 8, 0);
	}
	cv::imshow("points", imageIntersections);
	cv::imshow("Testing", imageGridLevelCopy);

	return mc;
}

/////////////////////////////

void TrackbarHarris(cv::Mat imageIn)
{
	if (imageIn.channels() > 1)
		cvtColor(imageIn, imageIn, cv::COLOR_RGB2GRAY);
	image = imageIn;

	std::string winName = "HARRIS";
	cv::namedWindow(winName, 1);
	cv::createTrackbar("BlockSize", winName, &currHarrisBlockSize, 20, HarrisOnChange);
	cv::createTrackbar("KSize/2-1", winName, &convHarrisKSize, 10, HarrisOnChange);
	cv::createTrackbar("K", winName, &convHarrisK, 100, HarrisOnChange);

	HarrisRecalculate();
	cv::waitKey(0);
}

void HarrisOnChange(int, void*)
{
	if (!currHarrisBlockSize)
		currHarrisBlockSize = 1;
	currHarrisKSize = convHarrisKSize * 2 - 1;
	double currHarrisK = convHarrisK / 100.0;
	HarrisRecalculate();
}

void HarrisRecalculate()
{
	// Calculate Harris Corners
	// https://docs.opencv.org/3.0-beta/doc/py_tutorials/py_feature2d/py_features_harris/py_features_harris.html
	cv::Mat imageHarris = cv::Mat::zeros(IMAGE_FIX_SIZE, CV_8U);
	cv::cornerHarris(image, imageHarris, currHarrisBlockSize, currHarrisKSize, currHarrisK);
	cv::imshow("HARRIS", imageHarris);



	cv::Mat imageHarris1;
	//cv::cornerHarris(image, imageHarris1, 3, 9, 0);
	//cv::imshow("harris 1a", imageHarris1);
	imageHarris.convertTo(imageHarris1, CV_8U, 255);
	cv::imshow("harris 1b", imageHarris1);

	cv::Mat imageHarris2;
	//cv::cornerHarris(image, imageHarris2, 3, 9, 0);
	cv::morphologyEx(imageHarris, imageHarris2, cv::MORPH_OPEN, cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5)));
	cv::imshow("harris 2a", imageHarris2);
	cv::threshold(imageHarris2, imageHarris2, 1, 255, cv::THRESH_BINARY);
	imageHarris2.convertTo(imageHarris2, CV_8U, 255);
	cv::imshow("harris 2b", imageHarris2);
	cv::dilate(imageHarris2, imageHarris2, cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(3, 3)));
	cv::imshow("harris 2c", imageHarris2);
	cv::GaussianBlur(imageHarris2, imageHarris2, cv::Size(3, 3), 0);
	cv::imshow("harris 2d", imageHarris2);

	cv::Mat harrisDiff;
	harrisDiff = imageHarris1 - imageHarris2;
	cv::imshow("harris 3", harrisDiff);
}