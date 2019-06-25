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

std::vector<cv::String> GetFiles2()
{
	std::vector<cv::String> files;
	cv::String directory = "C:/Users/micha/Pictures/Images 2/";
	files.push_back(directory + "IMG_20190625_140750.jpg");
	files.push_back(directory + "IMG_20190625_140759.jpg");
	files.push_back(directory + "IMG_20190625_140804.jpg");
	files.push_back(directory + "IMG_20190625_140816.jpg");
	files.push_back(directory + "IMG_20190625_140821.jpg");
	files.push_back(directory + "IMG_20190625_140834.jpg");
	files.push_back(directory + "IMG_20190625_140842.jpg");
	files.push_back(directory + "IMG_20190625_140926.jpg");
	files.push_back(directory + "IMG_20190625_140931.jpg");
	files.push_back(directory + "IMG_20190625_140938.jpg");
	files.push_back(directory + "IMG_20190625_140951.jpg");
	//files.push_back(directory + "IMG_20190625_141012.jpg");
	//files.push_back(directory + "IMG_20190625_141018.jpg");
	//files.push_back(directory + "IMG_20190625_141036.jpg");
	//files.push_back(directory + "IMG_20190625_141133.jpg");
	files.push_back(directory + "IMG_20190625_141205.jpg");
	files.push_back(directory + "IMG_20190625_141211.jpg");
	files.push_back(directory + "IMG_20190625_141216.jpg");
	files.push_back(directory + "IMG_20190625_141221.jpg");
	files.push_back(directory + "IMG_20190625_141443.jpg");
	files.push_back(directory + "IMG_20190625_141450.jpg");
	files.push_back(directory + "IMG_20190625_141458.jpg");
	files.push_back(directory + "IMG_20190625_141503.jpg");
	files.push_back(directory + "IMG_20190625_141541.jpg");
	files.push_back(directory + "IMG_20190625_141545.jpg");
	files.push_back(directory + "IMG_20190625_141549.jpg");
	files.push_back(directory + "IMG_20190625_141553.jpg");
	return files;
}

//https://stackoverflow.com/questions/44047819/increase-imageBGR-brightness-without-overflow/44054699#44054699
cv::Mat CropPage(cv::Mat imageBGR, char pageSide, std::vector<cv::Point> quads)
{
	cv::Mat imageOutput;
	imageBGR.copyTo(imageOutput, GetBackgroundMask(imageBGR));
	//cv::imshow("RemovedBackground", imageOutput);
	imageOutput = RemoveOtherPage(imageOutput, pageSide, quads);
	//cv::imshow("Cropped", imageOutput);
	//cv::waitKey(0);

	return imageOutput;
}

cv::Mat GetBackgroundMask(cv::Mat imageBGR)
{
	cv::Mat hsv;
	cvtColor(imageBGR, hsv, cv::COLOR_BGR2HSV);
	std::vector<cv::Mat> hsvChannels;
	split(hsv, hsvChannels);

	cv::Mat imageMask, imageOutput;
	cv::erode(hsvChannels[1], imageMask, cv::Mat::ones(cv::Size(7, 7), CV_8U));
	cv::medianBlur(imageMask, imageMask, 21);
	cv::threshold(imageMask, imageMask, 0, 255, cv::THRESH_OTSU);
	cv::bitwise_not(imageMask, imageMask);
	cv::erode(imageMask, imageOutput, cv::Mat::ones(cv::Size(7, 7), CV_8U));

	return imageOutput;
}

cv::Mat OtsuPreReduceVariety(cv::Mat imageBGR, cv::Mat bkgdMask)
{
	cv::Mat hsv;
	cvtColor(imageBGR, hsv, cv::COLOR_BGR2HSV);
	std::vector<cv::Mat> hsvChannels;
	split(hsv, hsvChannels);

	cv::Mat imageMask, imageOutput;
	cv::dilate(hsvChannels[2], imageMask, cv::Mat::ones(cv::Size(7, 7), CV_8U));
	//cv::imshow("1.dilate", imageMask);
	//cv::medianBlur(imageMask, imageMask, 21);
	cv::GaussianBlur(imageMask, imageMask, cv::Size(25,25),0);
	//cv::imshow("2.blur", imageMask);
	cv::erode(imageMask, imageMask, cv::Mat::ones(cv::Size(7, 7), CV_8U));
	cv::absdiff(hsvChannels[2], imageMask, imageOutput);
	//cv::imshow("3.difference", imageOutput);

	cv::waitKey(0);
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