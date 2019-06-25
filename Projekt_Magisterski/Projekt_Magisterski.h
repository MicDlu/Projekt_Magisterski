#pragma once

int main();

void OldCode_DrawingAndPerspectiveTransform();

cv::Point2f GetNearestPointIdx(std::vector<cv::Point2f> &intersectionPoints, cv::Point2f &pnt);

cv::Mat RemoveOtherPage(cv::Mat image, char pageSide, std::vector<cv::Point> quads);

char CalcPageSide(cv::Mat imageBackground);

std::vector<cv::Point2f> GetGridLevelIntersections(cv::Mat imageGridLevel);

