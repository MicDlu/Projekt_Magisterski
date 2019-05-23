#pragma once

cv::Mat LoadImage();

cv::Mat FixImageSize(cv::Mat input);

cv::Mat ThresholdImage(cv::Mat input);

std::vector<cv::String> GetFiles();
