#pragma once

#include <fstream>
#include <atlstr.h>
#include <locale>
#include <codecvt>
#include <math.h>

#define IMAGE_SIZE_SVGA cv::Size(800,600)
#define IMAGE_SIZE_PAL cv::Size(1024,576)
#define IMAGE_SIZE_HD720 cv::Size(1280,720)
#define IMAGE_SIZE_HD1080 cv::Size(1920,1080)
#define IMAGE_SIZE IMAGE_SIZE_HD1080
#define ZOOM_SIZE 40
#define ZOOM_FACTOR 4
#define ZOOM_WINDOW_SIZE ZOOM_SIZE * ZOOM_FACTOR

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
	bool horizontal = false;
	bool vertical = false;
	cv::Mat imageZoom;
	cv::Mat imageDrawing;
//Methods
private:
	void AddPoint(int x, int y);
	void AddPoint(cv::Point point);
	void UndoPoint();
	void InitNewLine();
	static void OnMouseEvent(int event, int x, int y, int flags, void* userdata);
	void TranslateToOriginal(cv::Point &point);
	void TranslateToProjection(cv::Point & point);
	void CorrectVectorDirection(std::vector<cv::Point> &pointVector);
	void UpdateZoomWindow(int x, int y);
public:
	ManualIntersector(cv::String imagePath, cv::Size interfaceSize);
	ManualIntersector(cv::String imagePath, cv::Size interfaceSize, PointVectorSet parsedVectorSet);
	~ManualIntersector();
	void RunSelector(std::string title, char orientation);
	PointVectorSet GetPointVectorSet();
	cv::Mat GetLinearDrawing(bool highlightLast);
	cv::Mat GetGridDrawing();
	bool SaveFileDescription(std::string &filePathRef, std::string fileNameSuffix = "");
	bool LoadImageDescription(std::string fileNameSuffix, char orientation);
	std::string GetFilePathNoExtension();
	PointVectorSet GetScaledVectorSet(float scale = 1);
};

std::vector<std::vector<cv::Point>> GetVectorSetsIntersection(std::vector<std::vector<cv::Point>> horizontalSet, std::vector<std::vector<cv::Point>> verticalSet);

cv::Point GetVectorSetsIntersection(std::vector<cv::Point> horizontalSet, std::vector<cv::Point> verticalSet);