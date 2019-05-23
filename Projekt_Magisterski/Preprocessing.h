#pragma once

cv::Mat LoadImage();

cv::Mat ResizeImage(cv::Mat input);

cv::Mat ThresholdImage(cv::Mat input);

std::vector<cv::String> GetFiles();
