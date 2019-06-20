#pragma once

int main();

void RemoveOtherPage(cv::Mat &imageRemBkgd, const cv::Mat &imageFixSize);

char CalcPageSide(cv::Mat imageBackground);

std::vector<cv::Point2f> GetGridLevelIntersections(cv::Mat imageGridLevel);

