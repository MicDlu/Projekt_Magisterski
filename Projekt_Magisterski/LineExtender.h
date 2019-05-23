#pragma once

class LineExtender
{
public:
	LineExtender(cv::Vec4f linesVector);
	LineExtender(cv::Point2f p1, cv::Point2f p2);
	~LineExtender();
	cv::Vec4f GetLineExtends(cv::Size imageSize);
	float GetIntersectionAngle(LineExtender line);
	cv::Point2f GetIntersectionPoint(LineExtender line);
private:
	float a, b;
	cv::Point2f extend1, extend2;
	cv::Mat image;
	float CalcY(float x);
	float CalcX(float y);
	void CalcBounds(cv::Size & imageSize);
};




