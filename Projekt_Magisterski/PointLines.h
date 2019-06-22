#pragma once
class PointLines
{
public:
	PointLines(std::vector<cv::Point2f> points);
	std::vector<std::vector<cv::Point2f>> GetVerticalLines(std::vector<cv::Point2f> points);
	~PointLines();
private:
	std::vector<cv::Point2f> points;
	float avgDist;
	float GetAvgDist();
	int GetNearestPointIdx(std::vector<cv::Point2f> &remainPts, cv::Point2f & pnt);
	float GetPointsDistance(cv::Point2f & pnt1, cv::Point2f & pnt2);
	float GetLineSegmentAngle(cv::Point2f & pnt1, cv::Point2f & pnt2);
	cv::Point2f CalcShift(cv::Point2f & pnt1, cv::Point2f & pnt2, float & dist, std::vector<float> &prevAngles);
	bool FitsInImage(cv::Point2f & point);
};

