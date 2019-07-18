#pragma once

bool ParseHVtoX(std::string & jpgFilePath);

cv::Point GetVectorSetsIntersection(std::vector<cv::Point> horizontalSet, std::vector<cv::Point> verticalSet);

std::vector<std::vector<cv::Point>> GetVectorSetsIntersection(std::vector<std::vector<cv::Point>> horizontalSet, std::vector<std::vector<cv::Point>> verticalSet);
