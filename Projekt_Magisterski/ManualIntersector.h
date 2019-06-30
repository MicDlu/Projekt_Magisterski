#pragma once

#include <fstream>
#include <atlstr.h>
#include <locale>
#include <codecvt>

#define IMAGE_SIZE_SVGA cv::Size(800,600)
#define IMAGE_SIZE_PAL cv::Size(1024,576)
#define IMAGE_SIZE_HD720 cv::Size(1280,720)
#define IMAGE_SIZE_HD1080 cv::Size(1920,1080)

bool OpenJpgFile(std::string &filePath);

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
	void AddPoint(cv::Point point);
	void UndoPoint();
	void InitNewLine();
	static void OnMouseEvent(int event, int x, int y, int flags, void* userdata);
	void TranslateToOriginal(cv::Point &point);
	void TranslateToProjection(cv::Point & point);
public:
	ManualIntersector(cv::String filePath, cv::Size interfaceSize);
	~ManualIntersector();
	void RunSelector(std::string title);
	PointVectorSet GetPointVectorSet();
	cv::Mat GetDrawing(bool highlightLast);
	bool SaveFileDescription(std::string &filePathRef, std::string fileNameSuffix = "");
	bool LoadImageDescription(std::string fileNameSuffix = "");
	std::string GetFilePathNoExtension();
};

