#include "pch.h"
#include "PointLines.h"


PointLines::PointLines(std::vector<cv::Point2f> points, std::vector<cv::Point> quads)
{
	this->points = points;
	this->avgDist = GetAvgDist();
	cv::Point2f quadTopLeft = cv::Point2f(quads[0].x, quads[0].y);
	cv::Point2f quadBotLeft = cv::Point2f(quads[1].x, quads[1].y);
	this->leftBoundAngle = GetLineAngle(quadTopLeft, quadBotLeft);
}

std::vector<std::vector<cv::Point2f>> PointLines::GetVerticalLines()
{
	std::vector<cv::Point2f> points = this->points;
	std::vector<std::vector<cv::Point2f>> verticalLines;
	cv::Point2f lineInitPnt(0, 0);
	std::vector<float> angleHist;
	cv::RNG rng(12345);

	// Visualization main matrice
	cv::Mat test = cv::Mat::zeros(IMAGE_FIX_SIZE, CV_8UC3);
	for (int i = 0; i < points.size(); i++)
		circle(test, points[i], 2, cv::Scalar(0, 0, 255), -1);
	
	// Unitil there are points...
	while (!points.empty() && FitsInImage(lineInitPnt))
	{
		// init line vector and visualization secondary matrice
		cv::Mat test2 = test.clone();
		cv::Scalar colorRng = cv::Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
		std::vector<cv::Point2f> verticalLine;

		// Calc init line point
		int currPntIdx = GetNearestPointIdx(points,lineInitPnt);

		// Break condition - no more lines prediction
		if (!verticalLines.empty())
			if (points[currPntIdx].x < verticalLines.back().front().x)
			{
				lineInitPnt.x += avgDist;
				continue;
			}

		// Transfer point to line vector
		verticalLine.push_back(points[currPntIdx]);
		points.erase(points.begin() + currPntIdx);
		// Angle history init management
		if (angleHist.empty())
			angleHist.push_back(leftBoundAngle);
		else if (angleHist.size() > 10)		
			angleHist = std::vector<float>(angleHist.begin() + 5, angleHist.begin() + 10);
		else
			angleHist.resize(5);

		// Init searching point and shift
		cv::Point2f searchPnt = verticalLine.back();
		cv::Point2f shift = shift = PredictShift(avgDist, angleHist);

		// Visualization
		cv::line(test2, lineInitPnt, verticalLine.back(), cv::Scalar(255, 0, 0));
		circle(test, verticalLine.back(), 2, colorRng, 2);
		circle(test2, verticalLine.back(), 2, colorRng, 2);
		//cv::imshow("test", test2);
		//cv::waitKey(0);

		// Calc rest of line
		while (FitsInImage(searchPnt) && !points.empty())
		{
			searchPnt += shift;
			currPntIdx = GetNearestPointIdx(points,searchPnt);

			// Visualization
			cv::line(test2, searchPnt, searchPnt - shift, colorRng,2);
			circle(test2, points[currPntIdx], 1, cv::Scalar(0, 255, 255), 2);
			//cv::imshow("test", test2);
			//cv::waitKey(0);

			// Skip if no point near searching point
			if (GetPointsDistance(points[currPntIdx], searchPnt) > (avgDist / 2))			
				continue;

			// Transfer point to line vector
			verticalLine.push_back(points[currPntIdx]);
			points.erase(points.begin() + currPntIdx);

			// Visualization
			circle(test, verticalLine.back(), 2, colorRng, 2);
			circle(test2, verticalLine.back(), 2, colorRng, 2);
			//cv::imshow("test", test2);
			//cv::waitKey(0);

			// Calc searching point and shift
			searchPnt = verticalLine.back();
			float currAngle = abs(GetLineAngle(verticalLine[verticalLine.size() - 2], verticalLine[verticalLine.size() - 1]));
			float avgAngle = std::accumulate(angleHist.begin(), angleHist.end(), 0.0) / angleHist.size();
			if (abs(currAngle - avgAngle) < 10)
				angleHist.push_back(currAngle);
			shift = PredictShift(avgDist, angleHist);
		}

		// Calc next line init point
		lineInitPnt = cv::Point2f(verticalLine.front().x + avgDist, verticalLine.front().y - avgDist);
		verticalLines.push_back(verticalLine);
	}
	cv::imshow("test", test);
	cv::waitKey(0);

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

int PointLines::GetNearestPointIdx(std::vector<cv::Point2f> &remainPts,cv::Point2f &pnt)
{
	// https://stackoverflow.com/questions/9825959/find-closest-neighbors-opencv
	cv::flann::KDTreeIndexParams indexParams;
	cv::flann::Index kdtree(cv::Mat(remainPts).reshape(1), indexParams);
	std::vector<float> query;
	query.push_back(pnt.x);
	query.push_back(pnt.y);
	std::vector<int> indices;
	std::vector<float> dists;
	kdtree.knnSearch(query, indices, dists, 1);
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

float PointLines::GetLineAngle(cv::Point2f &pnt1, cv::Point2f &pnt2)
{
	float a = (pnt2.y - pnt1.y) / (pnt2.x - pnt1.x);
	return atan(-a) * 180.0 / CV_PI;
}

cv::Point2f PointLines::PredictShift(float &dist, std::vector<float> &prevAngles)
{
	float avgAngle = GetAvgShiftAngle(prevAngles);
	float xShift = -dist * cos(avgAngle /180.0 * CV_PI);
	float yShift = dist * sin(avgAngle /180.0 * CV_PI);
	return cv::Point2f(xShift, yShift);
}

float PointLines::GetAvgShiftAngle(std::vector<float> &prevShiftAngles)
{
	return std::accumulate(prevShiftAngles.begin(), prevShiftAngles.end(), 0.0) / prevShiftAngles.size();
}

bool PointLines::FitsInImage(cv::Point2f &point)
{
	if (point.x < 0 || point.x > IMAGE_FIX_SIZE.width)
		return false;
	if (point.y < 0 || point.y > IMAGE_FIX_SIZE.height)
		return false;
	return true;
}