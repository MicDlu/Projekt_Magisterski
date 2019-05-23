#include "pch.h"
#include "DocAreaLSD.h"

DocAreaLSD::DocAreaLSD(cv::Mat src)
{
	// Pass variable
	imageOriginal = src;

	// Init LSD
	lsd = cv::createLineSegmentDetector();

	// Preprocess for LSD
	cv::Mat imagePre = PreprocessLSD();

	// Extended LSD
	std::vector<cv::Vec4f> lines_ext = GetExtendedLSD(imagePre);
	lsd->drawSegments(src, lines_ext);

	// scale extends
	//ScaleExtends(lines_ext);

	// Calc points
	//std::vector<cv::Point2f> intersectionPoints;
	for (int i = 0; i < lines_ext.size(); i++)
	{
		LineExtender line1 = LineExtender(lines_ext[i]);
		for (int j = i+1; j < lines_ext.size(); j++)
		{
			LineExtender line2 = LineExtender(lines_ext[j]);
			cv::Point2f intersection = line1.GetIntersectionPoint(line2);
			float angle = line1.GetIntersectionAngle(line2);
			//if (angle > 70 && angle < 110)
				//intersectionPoints.push_back(intersection);			
				cv::circle(src, intersection, 3, cv::Scalar(0, 0, 0), 3);
		}
	}

	cv::imshow("test", src);
	cv::waitKey(0);
}

void DocAreaLSD::ScaleExtends(std::vector<cv::Vec4f> &lines_ext)
{
	cv::Mat imageResized;
	cv::resize(imageOriginal, imageResized, cv::Size(100, 100));
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
	lsd->drawSegments(imageResized, lines_original);
}

cv::Mat DocAreaLSD::PreprocessLSD()
{
	cv::Mat imageProcess = ThresholdImage(imageOriginal);
	// erode + dilate
	cv::InputArray kernel = cv::Mat::ones(cv::Size(5, 5), CV_8U);
	cv::morphologyEx(imageProcess, imageProcess, cv::MORPH_CLOSE, kernel);
	return imageProcess;
}


DocAreaLSD::~DocAreaLSD()
{
}

std::vector<cv::Vec4f> DocAreaLSD::GetExtendedLSD(cv::Mat imageLSD)
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
		LineExtender lineExtend(vec);
		cv::Vec4f ext = lineExtend.GetLineExtends(imageLSD.size());
		lines_ext.push_back(ext);
	}

	return lines_ext;
}