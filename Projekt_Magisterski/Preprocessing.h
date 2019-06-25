#pragma once

cv::Mat FixImageSize(cv::Mat input);

cv::Mat ThresholdImage(cv::Mat input);

std::vector<cv::String> GetFiles();

std::vector<cv::String> GetFiles2();

cv::Mat ReduceVariety(cv::Mat image);

cv::Mat OtsuPreReduceVariety(cv::Mat image);

cv::Mat CropPage(cv::Mat imageBGR, char pageSide, std::vector<cv::Point> quads);

cv::Mat GetBackgroundMask(cv::Mat image);

cv::Mat OtsuPreReduceVariety(cv::Mat imageBGR, cv::Mat bkgdMask);
