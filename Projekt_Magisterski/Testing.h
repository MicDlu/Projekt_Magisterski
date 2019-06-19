#pragma once
class Testing
{
public:
	Testing();
	~Testing();
};

void TrackbarIntersectionPoints(cv::Mat imageIn);

void OnHarrisBlockSizeChange(int, void *);

std::vector<cv::Point2f> TestGetGridLevelIntersections(cv::Mat imageGridLevel);
