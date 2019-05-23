#pragma once
class DocAreaLSD
{
public:
	DocAreaLSD(cv::Mat src);
	~DocAreaLSD();
private:
	cv::Mat imageOriginal;
	cv::Ptr<cv::LineSegmentDetector> lsd;
	const int intersectionDegTolerance = 20;
	void ScaleExtends(std::vector<cv::Vec4f> &lines_ext);
	std::vector<cv::Vec4f> GetExtendedLSD(cv::Mat imageLSD);
	cv::Mat PreprocessLSD();
	std::vector<cv::Point2f> GetIntersectionPoints(std::vector<cv::Vec4f> &lines_ext);
};

	void LineFeaturesTutorial(cv::String file);
