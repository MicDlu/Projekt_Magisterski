#include "pch.h"
#include "DocAreaLSD.h"

DocAreaLSD::DocAreaLSD(cv::Mat src)
{
	// Pass variable
	imageOriginal = src;

	// Init LSD
	lsd = cv::createLineSegmentDetector();

	// Preprocess for LSD
	//cv::Mat imageResized;
	//cv::resize(imageOriginal, imageResized, lsdImageSize);
	cv::Mat imagePre = PreprocessLSD(imageOriginal);

	// Extended LSD
	std::vector<cv::Vec4f> lines_ext = GetExtendedLSD(imagePre);
	//lsd->drawSegments(src, lines_ext);

	// scale extends
	//std::vector<cv::Vec4f> lines_org = ScaleExtends(lines_ext);
	//lsd->drawSegments(src, lines_org);

	// Get ~90 angles intersections that fit in image
	std::vector<cv::Point2f> intersectionPoints = GetIntersectionPoints(lines_ext);

	// Calc quadliterals
	std::vector<cv::Point2f>* quadPoints = GetQuadliteralPoints(intersectionPoints);

	for (int q = 0; q < 4; q++)
	{
		quadliterals.push_back(GetMedianPoint(quadPoints[q]));

		// Draw intersection points
		//for (int i = 0; i < quadPoints[q].size(); i++)
		//{
		//	cv::circle(src, quadPoints[q][i], 1, cv::Scalar(0, 255, 0), 2);
		//}
		// Draw quad point
		//cv::circle(src, quadliterals[q], 5, cv::Scalar(255, 0, 255), 5);
	}

	//cv::imshow("test", src);
	//cv::waitKey(0);
}

std::vector<cv::Point2f>* DocAreaLSD::GetQuadliteralPoints(std::vector<cv::Point2f> &intersectionPoints)
{
	cv::Size size = imageOriginal.size();
	cv::Rect quadliterals[4];
	quadliterals[0] = cv::Rect(cv::Point(0, 0), cv::Size(size.width / 2, size.height / 2));
	quadliterals[1] = cv::Rect(cv::Point(0, size.height / 2), cv::Size(size.width / 2, size.height / 2));
	quadliterals[2] = cv::Rect(cv::Point(size.width / 2, 0), cv::Size(size.width / 2, size.height / 2));
	quadliterals[3] = cv::Rect(cv::Point(size.width / 2, size.height / 2), cv::Size(size.width / 2, size.height / 2));

	static std::vector<cv::Point2f> quadPoints[4];
	for (int i = 0; i < 4; i++)
		quadPoints[i].clear();

	for (int i = 0; i < intersectionPoints.size(); i++)
	{
		for (int q = 0; q < 4; q++)
		{
			if (quadliterals[q].contains(intersectionPoints[i]))
			{
				quadPoints[q].push_back(intersectionPoints[i]);
			}
		}
	}

	return quadPoints;
}

std::vector<cv::Point2f> DocAreaLSD::GetIntersectionPoints(std::vector<cv::Vec4f> &lines_ext)
{
	std::vector<cv::Point2f> intersectionPoints;
	for (int i = 0; i < lines_ext.size(); i++)
	{
		LineExtender line1 = LineExtender(lines_ext[i]);
		for (int j = i + 1; j < lines_ext.size(); j++)
		{
			LineExtender line2 = LineExtender(lines_ext[j]);
			cv::Point2f intersection = line1.GetIntersectionPoint(line2);

			if (line1.FitsInImage(intersection, imageOriginal.size()))
			{
				float angle = line1.GetIntersectionAngle(line2);
				if (angle >(90 - intersectionDegTolerance) && angle < (90 + intersectionDegTolerance))
				{
					intersectionPoints.push_back(intersection);
				}
			}
		}
	}
	return intersectionPoints;
}

std::vector<cv::Vec4f> DocAreaLSD::ScaleExtends(std::vector<cv::Vec4f> &lines_ext)
{
	std::vector<cv::Vec4f> lines_original;
	float factor[2];
	factor[0] = imageOriginal.size().width / lsdImageSize.width;
	factor[1] = imageOriginal.size().height / lsdImageSize.height;
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
	return lines_original;
}

cv::Mat DocAreaLSD::PreprocessLSD(cv::Mat inputImage)
{
	cv::Mat imageProcess = ThresholdImage(inputImage);
	// erode + dilate
	cv::InputArray kernel = cv::Mat::ones(cv::Size(5, 5), CV_8U);
	cv::morphologyEx(imageProcess, imageProcess, cv::MORPH_CLOSE, kernel);
	return imageProcess;
}


DocAreaLSD::~DocAreaLSD()
{
}

std::vector<cv::Point2f> DocAreaLSD::GetQuadPoints()
{
	return quadliterals;
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
		std::vector<float> angles;
	for (int i = 0; i < lines_std.size(); i++)
	{
		cv::Vec4f vec = cv::Vec4f(lines_std[i].val);
		LineExtender lineExtend(vec);
		cv::Vec4f ext = lineExtend.GetLineExtends(imageLSD.size());
		float angle = lineExtend.GetOxAngle();
		if (abs(angle) < (lineDegTolerance) || abs(angle) > (90 - lineDegTolerance))
		{
			angles.push_back(angle);
			lines_ext.push_back(ext);
		}
	}

	return lines_ext;
}

cv::Point2f GetMedianPoint(std::vector<cv::Point2f> scores)
{
	size_t size = scores.size();
	std::vector<float> xScores;
	std::vector<float> yScores;
	for (int i = 0; i < size; i++)
	{
		xScores.push_back(scores[i].x);
		yScores.push_back(scores[i].y);
	}
	sort(xScores.begin(), xScores.end());
	sort(yScores.begin(), yScores.end());
	if (size % 2 == 0)
	{
		float medianX = (xScores[size / 2 - 1] + xScores[size / 2]) / 2;
		float medianY = (yScores[size / 2 - 1] + yScores[size / 2]) / 2;
		return cv::Point2f(medianX, medianY);
	}
	else
	{
		float medianX = xScores[size / 2];
		float medianY = yScores[size / 2];
		return cv::Point2f(medianX, medianY);
	}
}