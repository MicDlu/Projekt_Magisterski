#pragma once

class LineExtender
{
public:
	LineExtender(cv::Point2f p1, cv::Point2f p2, cv::Size imgSize);
	~LineExtender();
	cv::Vec4f GetLineExtends();
private:
	float CalcY(float x);
	float CalcX(float y);
	void CalcBounds(cv::Size & imageSize);
};




