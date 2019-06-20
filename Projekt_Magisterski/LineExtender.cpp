#include "pch.h"
using namespace cv;
using namespace std;

LineExtender::LineExtender(cv::Vec4f linesVector)
{
	cv::Point2f point1 = cv::Point2f(linesVector[0], linesVector[1]);
	cv::Point2f point2 = cv::Point2f(linesVector[2], linesVector[3]);
	a = (point2.y - point1.y) / (point2.x - point1.x);
	b = point1.y - a * point1.x;
}

LineExtender::LineExtender(cv::Point2f point1, cv::Point2f point2)
{
	a = (point2.y - point1.y) / (point2.x - point1.x);
	b = point1.y - a * point1.x;
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
		if (CalcY(0) <= imageSize.height-1)
			extendX1 = 0;
		else
			extendX1 = CalcX(imageSize.height);
		if (CalcY(imageSize.width) >= 0)
			extendX2 = imageSize.width-1;
		else
			extendX2 = CalcX(0);
	}
	else // malej¹ca
	{
		if (CalcY(0) >= 0)
			extendX2 = 0;
		else
			extendX2 = CalcX(0);
		if (CalcY(imageSize.width) <= imageSize.height)
			extendX1 = imageSize.width-1;
		else
			extendX1 = CalcX(imageSize.height-1);
	}
	extend1 = cv::Point2f(extendX1, CalcY(extendX1));
	extend2 = cv::Point2f(extendX2, CalcY(extendX2));
}

cv::Vec4f LineExtender::GetLineExtends(cv::Size imageSize)
{
	CalcBounds(imageSize);
	return cv::Vec4f(extend1.x, extend1.y, extend2.x, extend2.y);
}

float LineExtender::GetIntersectionAngle(LineExtender line)
{
	float tgA = abs((a - line.a) / (1 + a * line.a));
	float radianA = atan(tgA);
	return radianA * rad2deg;
}

cv::Point2f LineExtender::GetIntersectionPoint(LineExtender line)
{
	float x = (b - line.b) / (line.a - a);
	return cv::Point2f(x, CalcY(x));
}

bool LineExtender::FitsInImage(cv::Point2f point, cv::Size imageSize)
{
	if (point.x < 0 || point.x > imageSize.width)
		return false;
	if (point.y < 0 || point.y > imageSize.height)
		return false;
	return true;
}

float LineExtender::GetOxAngle()
{
	return atan(a) * rad2deg;
}