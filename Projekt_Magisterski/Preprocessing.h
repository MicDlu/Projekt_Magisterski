#pragma once

cv::Mat FixImageSize(cv::Mat input);

cv::Mat ThresholdImage(cv::Mat input);

std::vector<cv::String> GetFiles();

cv::Mat RemoveBackground(cv::Mat image);

cv::Mat ReduceVariety(cv::Mat image);

cv::Mat OtsuPreReduceVariety(cv::Mat image);
