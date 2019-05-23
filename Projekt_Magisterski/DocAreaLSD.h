#pragma once
class DocAreaLSD
{
public:
	DocAreaLSD(cv::Mat src);
	~DocAreaLSD();
private:
	cv::Mat imageOriginal;
	cv::Ptr<cv::LineSegmentDetector> lsd;
	std::vector<cv::Vec4f> GetLSD(cv::Mat imageLSD);
};

	void LineFeaturesTutorial(cv::String file);
