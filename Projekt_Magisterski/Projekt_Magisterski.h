#pragma once

int main();

cv::Point2f GetNearestPointIdx(std::vector<cv::Point2f> &intersectionPoints, cv::Point2f &pnt);

void RemoveOtherPage(cv::Mat &imageRemBkgd, const cv::Mat &imageFixSize);

char CalcPageSide(cv::Mat imageBackground);

std::vector<cv::Point2f> GetGridLevelIntersections(cv::Mat imageGridLevel);

