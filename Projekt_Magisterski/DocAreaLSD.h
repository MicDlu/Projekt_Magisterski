#pragma once
class DocAreaLSD
{
public:
	DocAreaLSD(cv::Mat src);
	~DocAreaLSD();
	std::vector<cv::Point2f> GetQuadPoints();
private:
	cv::Mat imageOriginal;
	cv::Ptr<cv::LineSegmentDetector> lsd;
	const int intersectionDegTolerance = 15;
	const int lineDegTolerance = 15;
	const cv::Size lsdImageSize = cv::Size(300, 300);
	std::vector<cv::Point2f> quadliterals;
	//
	std::vector<cv::Vec4f> ScaleExtends(std::vector<cv::Vec4f> &lines_ext);
	cv::Mat PreprocessLSD(cv::Mat inputImage);
	
	std::vector<cv::Vec4f> GetExtendedLSD(cv::Mat imageLSD);
	std::vector<cv::Point2f>* GetQuadliteralPoints(std::vector<cv::Point2f> &intersectionPoints);
	std::vector<cv::Point2f> GetIntersectionPoints(std::vector<cv::Vec4f> &lines_ext);
};

	void LineFeaturesTutorial(cv::String file);

	cv::Point2f GetMedianPoint(std::vector<cv::Point2f> scores);
