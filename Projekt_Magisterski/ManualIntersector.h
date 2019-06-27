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
	cv::String const windowName = "Manual intersection selection";
//Methods
private:
	void AddPoint(int x, int y);
	void UndoPoint();
	void InitNewLine();
	static void OnMouseEvent(int event, int x, int y, int flags, void* userdata);
public:
	ManualIntersector(cv::String filePath);
	~ManualIntersector();
	void RunSelector();
	PointVectorSet GetPointVectorSet();
	cv::Mat GetDrawing();
	void SaveToFile();
};

