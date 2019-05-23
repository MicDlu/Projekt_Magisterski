#pragma once
class DocAreaLSD
{
public:
	DocAreaLSD(cv::Mat src);
	void ScaleExtends(std::vector<cv::Vec4f> &lines_ext);
	~DocAreaLSD();
private:
	cv::Mat imageOriginal;
	cv::Ptr<cv::LineSegmentDetector> lsd;
	std::vector<cv::Vec4f> GetExtendedLSD(cv::Mat imageLSD);
	cv::Mat PreprocessLSD();
};

	void LineFeaturesTutorial(cv::String file);
