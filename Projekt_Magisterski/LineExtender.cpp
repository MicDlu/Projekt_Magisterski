#include "pch.h"
using namespace cv;
using namespace std;

float a, b;
cv::Point2f extend1, extend2;
cv::Mat image;

LineExtender::LineExtender(cv::Point2f point1, cv::Point2f point2, cv::Size imageSize)
{
	a = (point2.y - point1.y) / (point2.x - point1.x);
	b = point1.y - a * point1.x;
	CalcBounds(imageSize);
}

LineExtender::~LineExtender()
{
}

float LineExtender::CalcY(float x)
{
	return a * x + b;
}

float LineExtender::CalcX(float y)
{
	return (y - b) / a;
}

void LineExtender::CalcBounds(cv::Size &imageSize)
{
	float extendX1, extendX2;
	if (a < 0) // rosn¹ca
	{
		if (CalcY(0) <= imageSize.height)
			extendX1 = 0;
		else
			extendX1 = CalcX(imageSize.height);
		if (CalcY(imageSize.width) >= 0)
			extendX2 = imageSize.width;
		else
			extendX2 = CalcX(0);
	}
	else // malej¹ca
	{
		if (CalcY(0) >= 0)
			extendX1 = 0;
		else
			extendX1 = CalcX(0);
		if (CalcY(imageSize.width) <= imageSize.height)
			extendX2 = imageSize.width;
		else
			extendX2 = CalcX(imageSize.height);
	}
	extend1 = cv::Point2f(extendX1, CalcY(extendX1));
	extend2 = cv::Point2f(extendX2, CalcY(extendX2));
}

cv::Vec4f LineExtender::GetLineExtends()
{
	return cv::Vec4f(extend1.x, extend1.y, extend2.x, extend2.y);
}