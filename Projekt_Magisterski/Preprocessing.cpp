#include "pch.h"

cv::Mat FixImageSize(cv::Mat input)
{
	cv::Mat output;
	cv::resize(input, output, IMAGE_FIX_SIZE);
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
	//files.push_back(directory + "IMG_20190525_115849.jpg");
	files.push_back(directory + "IMG_20190228_195957.jpg");
	//files.push_back(directory + "IMG_20190228_200009.jpg"); // finger
	files.push_back(directory + "IMG_20190228_200028.jpg");
	files.push_back(directory + "IMG_20190228_200050.jpg");
	files.push_back(directory + "IMG_20190228_200058.jpg");
	return files;
}

//https://stackoverflow.com/questions/44047819/increase-image-brightness-without-overflow/44054699#44054699
cv::Mat RemoveBackground(cv::Mat image)
{
	cv::Mat hsv;
	cvtColor(image, hsv, cv::COLOR_BGR2HSV);
	std::vector<cv::Mat> hsvChannels;
	split(hsv, hsvChannels);

	cv::Mat imageMask, imageOutput;
	cv::erode(hsvChannels[1], imageMask, cv::Mat::ones(cv::Size(7, 7), CV_8U));
	//cv::imshow("1.dilate", imageMask);
	cv::medianBlur(imageMask, imageMask, 21);
	//cv::imshow("2.median blur", imageMask);
	cv::threshold(imageMask, imageMask, 0, 255, cv::THRESH_OTSU);
	cv::bitwise_not(imageMask, imageMask);
	//cv::imshow("3.threshold", imageMask);
	image.copyTo(imageOutput, imageMask);
	//cv::imshow("RemovedBackground", imageOutput);
	//cv::waitKey(0);

	return imageOutput;
}

cv::Mat OtsuPreReduceVariety(cv::Mat image)
{
	cv::Mat hsv;
	cvtColor(image, hsv, cv::COLOR_BGR2HSV);
	std::vector<cv::Mat> hsvChannels;
	split(hsv, hsvChannels);

	cv::Mat imageMask, imageOutput;;
	cv::dilate(hsvChannels[2], imageMask, cv::Mat::ones(cv::Size(7, 7), CV_8U));
	//cv::imshow("1.dilate", imageMask);
	cv::medianBlur(imageMask, imageMask, 21);
	//cv::imshow("2.median blur", imageMask);
	cv::absdiff(hsvChannels[2], imageMask, imageOutput);
	//cv::imshow("3.difference", imageOutput);

	//cv::waitKey(0);
	return imageOutput;
}

cv::Mat ReduceVariety(cv::Mat image)
{
	cv::Mat hsv;
	cvtColor(image, hsv, cv::COLOR_BGR2HSV);
	std::vector<cv::Mat> hsvChannels;
	split(hsv, hsvChannels);

	//cv::imshow("0.input", hsvChannels[2]);
	cv::Mat imageMask, imageOutput;;
	cv::dilate(hsvChannels[2], imageMask, cv::Mat::ones(cv::Size(7, 7), CV_8U));
	//cv::imshow("1.dilate", imageMask);
	cv::medianBlur(imageMask, imageMask, 21);
	//cv::imshow("2.median blur", imageMask);
	cv::absdiff(hsvChannels[2], imageMask, imageOutput);
	cv::normalize(imageOutput, imageOutput, 0, 255, cv::NORM_MINMAX, CV_8U);
	cv::Mat imageBackground = cv::Mat::zeros(image.size(), CV_8U);
	cv::threshold(hsvChannels[2], imageBackground, 0, 255, cv::ThresholdTypes::THRESH_BINARY_INV);
	//cv::imshow("backgr", imageBackground);
	cv::absdiff(imageOutput, imageBackground, imageOutput);
	cv::bitwise_not(imageOutput, imageOutput);
	//cv::imshow("3.difference", imageOutput);

	//cv::waitKey(0);
	return imageOutput;
}