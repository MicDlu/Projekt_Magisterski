#pragma once
class ManualIntersector
{
//Type
public:
	typedef std::vector<std::vector<cv::Point>> PointVectorSet;
//Variables
private:
	cv::String imagePath;
	cv::String imageDescrptionPath;
	cv::Mat inputImage;
	cv::Mat fixedImage;
	cv::Size2f sizeFactor;
	PointVectorSet pointVectorSet;
	cv::String windowName;
//Methods
private:
	void AddPoint(int x, int y);
	void UndoPoint();
	void InitNewLine();
	static void OnMouseEvent(int event, int x, int y, int flags, void* userdata);
public:
	ManualIntersector(cv::String filePath, cv::Size interfaceSize);
	~ManualIntersector();
	void RunSelector(std::string title);
	PointVectorSet GetPointVectorSet();
	cv::Mat GetDrawing(bool highlightLast);
	bool SaveFileDescription(std::string &filePathRef, std::string fileNameSuffix = "");
	bool LoadImageDescription(std::string fileNameSuffix = "");
};

