#pragma once
class PointLines
{
public:
	PointLines(std::vector<cv::Point2f> points, std::vector<cv::Point> quads);
	std::vector<std::vector<cv::Point2f>> GetVerticalLines();
	~PointLines();
private:
	std::vector<cv::Point2f> points;
	float leftBoundAngle;
	float avgDist;
	float GetAvgDist();
	int GetNearestPointIdx(std::vector<cv::Point2f> &remainPts, cv::Point2f & pnt);
	float GetPointsDistance(cv::Point2f & pnt1, cv::Point2f & pnt2);
	float GetLineAngle(cv::Point2f & pnt1, cv::Point2f & pnt2);
	cv::Point2f PredictShift(float & dist, std::vector<float> &prevAngles);
	float GetAvgShiftAngle(std::vector<float> prevShiftAngles);
	bool FitsInImage(cv::Point2f & point);
};

