#pragma once

cv::Mat FixImageSize(cv::Mat input);

cv::Mat ThresholdImage(cv::Mat input);

std::vector<cv::String> GetFiles();

cv::Vec2d Otsu3(cv::Mat image);

std::vector<double> OtsuN(cv::Mat image, int nOfLevels);
