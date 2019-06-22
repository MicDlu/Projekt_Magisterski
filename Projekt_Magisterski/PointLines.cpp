#include "pch.h"
#include "PointLines.h"


PointLines::PointLines(std::vector<cv::Point2f> points)
{
	this->points = points;
	this->avgDist = GetAvgDist();

	GetVerticalLines(points);
}

std::vector<std::vector<cv::Point2f>> PointLines::GetVerticalLines(std::vector<cv::Point2f> points)
{
	std::vector<std::vector<cv::Point2f>> verticalLines;
	cv::Point2f zeroPnt(0, 0);
	//
	cv::Mat test = cv::Mat::zeros(IMAGE_FIX_SIZE, CV_8UC3);
	for (int i = 0; i < points.size(); i++)
		circle(test, points[i], 2, cv::Scalar(0, 0, 255), -1);
	//
	cv::RNG rng(12345);
	while (!points.empty())
	{
		cv::Mat test2 = test.clone();
		cv::Scalar colorRng = cv::Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
		std::vector<cv::Point2f> verticalLine;
		// Calc init line point
		int currPntIdx = GetNearestPointIdx(points,zeroPnt);
		verticalLine.push_back(points[currPntIdx]);
		points.erase(points.begin() + currPntIdx);

		cv::line(test2, zeroPnt, verticalLine.back(), cv::Scalar(255, 0, 0));
		circle(test, verticalLine.back(), 2, colorRng, 2);
		circle(test2, verticalLine.back(), 2, colorRng, 2);
		cv::Point2f searchPnt = verticalLine.back();
		float angle = -90;
		cv::Point2f shift(0, avgDist);
		// Calc other points
		while (FitsInImage(searchPnt))
		{
			currPntIdx = GetNearestPointIdx(points,searchPnt, shift.x, shift.y);
			circle(test2, points[currPntIdx], 1, cv::Scalar(0, 255, 255), 2);
			cv::line(test2, searchPnt, searchPnt + shift, colorRng,2);
			searchPnt += shift;
			cv::imshow("test", test2);
			cv::waitKey(0);

			if (GetPointsDistance(points[currPntIdx], searchPnt) > (avgDist / 2))
			{
				continue;
			}
			verticalLine.push_back(points[currPntIdx]);
			points.erase(points.begin() + currPntIdx);
			shift = GetShift(verticalLine[verticalLine.size() - 2], verticalLine[verticalLine.size() - 1], avgDist, angle);
			searchPnt = verticalLine.back();

			circle(test, verticalLine.back(), 2, colorRng, 2);
			circle(test2, verticalLine.back(), 2, colorRng, 2);
			//cv::imshow("test", test2);
			//cv::waitKey(0);
		}

		verticalLines.push_back(verticalLine);
	}

	return verticalLines;
}


PointLines::~PointLines()
{
}

float PointLines::GetAvgDist()
{
	int pntCount = 10;
	srand(time(NULL));
	// init flann
	cv::flann::KDTreeIndexParams indexParams;
	cv::flann::Index kdtree(cv::Mat(points).reshape(1), indexParams);
	std::vector<float> query(2);
	std::vector<int> indices;
	std::vector<float> dists;

	//cv::Mat test = cv::Mat::zeros(IMAGE_FIX_SIZE, CV_8UC3);
	//for (int i = 0; i < points.size(); i++)
	//	circle(test, points[i], 2, cv::Scalar(0, 0, 255), -1);

	// calc nearest point dists for random points
	float distSum = 0;
	for (int i = 0; i < pntCount; i++)
	{
		int randIdx = rand() % points.size();
		cv::Point2f randPoint = points[randIdx];
		query[0] = randPoint.x;
		query[1] = randPoint.y;
		kdtree.knnSearch(query, indices, dists, 2);
		distSum += GetPointsDistance(randPoint, points[indices[1]]);

		//cv::line(test, points[indices[1]], points[randIdx], cv::Scalar(255, 0, 0));
		//circle(test, points[indices[1]], 2, cv::Scalar(255, 0, 0), -1);
		//circle(test, points[randIdx], 3, cv::Scalar(0, 255, 0), -1);
		//cv::imshow("test", test);
		//cv::waitKey(0);
	}
	return distSum / pntCount;
}

int PointLines::GetNearestPointIdx(std::vector<cv::Point2f> &remainPts,cv::Point2f &pnt, int xShift, int yShift)
{
	// https://stackoverflow.com/questions/9825959/find-closest-neighbors-opencv
	cv::flann::KDTreeIndexParams indexParams;
	cv::flann::Index kdtree(cv::Mat(remainPts).reshape(1), indexParams);
	std::vector<float> query;
	query.push_back(pnt.x + xShift);
	query.push_back(pnt.y + yShift);
	std::vector<int> indices;
	std::vector<float> dists;
	kdtree.knnSearch(query, indices, dists, 2);
	// Reject entry point
	for (int i = 0; i < 2; i++)
		if (remainPts[indices[i]] != pnt)
			return indices[i];
}

float PointLines::GetPointsDistance(cv::Point2f &pnt1, cv::Point2f &pnt2)
{
	cv::Point2f diff = pnt2 - pnt1;
	return cv::sqrt(diff.x*diff.x + diff.y*diff.y);
}

//float PointLines::GetLineSegmentAngle(cv::Point2f &pnt1, cv::Point2f &pnt2)
//{
//	cv::Point2f diff = pnt2 - pnt1;
//	float a = (diff.y) / (diff.x);
//	return atan(a) * RAD2DEG;
//}

cv::Point2f PointLines::GetShift(cv::Point2f &pnt1, cv::Point2f &pnt2, float &dist, float &prevAngle)
{
	float a = (pnt2.y - pnt1.y) / (pnt2.x - pnt1.x);
	float b = pnt1.y - a * pnt1.x;
	float currAngle = atan(a) * 180.0 / CV_PI;
	float avgAngle = (currAngle + prevAngle) / 2;
	prevAngle = currAngle;
	float xShift = -dist * cos(avgAngle /180.0 * CV_PI);
	float yShift = -dist * sin(avgAngle /180.0 * CV_PI);
	return cv::Point2f(xShift, yShift);
}

bool PointLines::FitsInImage(cv::Point2f &point)
{
	if (point.x < 0 || point.x > IMAGE_FIX_SIZE.width)
		return false;
	if (point.y < 0 || point.y > IMAGE_FIX_SIZE.height)
		return false;
	return true;
}