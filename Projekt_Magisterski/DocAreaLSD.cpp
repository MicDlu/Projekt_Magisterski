#include "pch.h"
#include "DocAreaLSD.h"

DocAreaLSD::DocAreaLSD(cv::Mat src)
{
	lsd = cv::createLineSegmentDetector();

	imageOriginal = src;

	// LSD resized image
	cv::Mat imageResized = imageOriginal;
	//cv::Size resizeSize = cv::Size(100, 100);
	//cv::resize(src, imageResized, resizeSize);
	std::vector<cv::Vec4f> lines_ext = GetLSD(imageResized);
	//lsd->drawSegments(imageResized, lines_ext);

	// scale extends
	std::vector<cv::Vec4f> lines_original;
	float factor[2];
	factor[0] = imageOriginal.size().width / imageResized.size().width;
	factor[1] = imageOriginal.size().height / imageResized.size().height;
	for (int i = 0; i < lines_ext.size(); i++)
	{
		cv::Vec4f vecIn = cv::Vec4f(lines_ext[i].val);
		cv::Vec4f vecOut;
		vecOut[0] = vecIn[0] * factor[0];
		vecOut[1] = vecIn[1] * factor[1];
		vecOut[2] = vecIn[2] * factor[0];
		vecOut[3] = vecIn[3] * factor[1];
		lines_original.push_back(vecOut);
	}

	lsd->drawSegments(src, lines_original);
	cv::imshow("test", src);
	cv::waitKey(0);
	//lsd
	//bound
}


DocAreaLSD::~DocAreaLSD()
{
}


void LineFeaturesTutorial(cv::String file)
{
	cv::Mat src = cv::imread(file, cv::IMREAD_GRAYSCALE);
	cv::resize(src, src, cv::Size(60, 80));
	std::cout << "--------------------------------\n";

	cv::Mat blank = cv::Mat::zeros(src.size(), CV_8UC1);

	/* create a pointer to an LSDDetector object */
	cv::Ptr<cv::LineSegmentDetector> lsd = cv::createLineSegmentDetector();
	std::vector<cv::Vec4f> lines_std;

	// Detect the lines
	lsd->detect(src, lines_std);

	std::vector<cv::Vec4f> lines_ext;
	cv::Mat drawnLines(src);
	for (int i = 0; i < lines_std.size(); i++)
	{
		// Show found lines
		cv::Vec4f vec = cv::Vec4f(lines_std[i].val);
		cv::Point2f p1 = cv::Point2f(vec[0], vec[1]);
		cv::Point2f p2 = cv::Point2f(vec[2], vec[3]);
		LineExtender lineExtend(p1, p2, drawnLines.size());
		cv::Vec4f ext = lineExtend.GetLineExtends();
		lines_ext.push_back(ext);
	}

	lsd->drawSegments(drawnLines, lines_ext);
	cv::imshow("LineExtended", drawnLines);
	cv::waitKey(0);
}

std::vector<cv::Vec4f> DocAreaLSD::GetLSD(cv::Mat imageLSD)
{
	/* create a pointer to an LSDDetector object */
	std::vector<cv::Vec4f> lines_std;

	// Detect the lines
	cv::Mat imageGray;
	//cvtColor(imageLSD, imageGray, cv::COLOR_RGB2GRAY);
	lsd->detect(imageLSD, lines_std);

	// Extend lines
	std::vector<cv::Vec4f> lines_ext;
	for (int i = 0; i < lines_std.size(); i++)
	{
		cv::Vec4f vec = cv::Vec4f(lines_std[i].val);
		cv::Point2f p1 = cv::Point2f(vec[0], vec[1]);
		cv::Point2f p2 = cv::Point2f(vec[2], vec[3]);
		LineExtender lineExtend(p1, p2, imageLSD.size());
		cv::Vec4f ext = lineExtend.GetLineExtends();
		lines_ext.push_back(ext);
	}

	return lines_ext;
}