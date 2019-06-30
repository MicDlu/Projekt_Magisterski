#include "pch.h"
#include "ManualIntersector.h"

ManualIntersector::ManualIntersector(cv::String imagePath, cv::Size interfaceSize)
{
	this->imagePath = imagePath;
	this->inputImage = cv::imread(imagePath);
	cv::resize(inputImage, this->fixedImage, interfaceSize);
	this->sizeFactor.width = (float)this->inputImage.size().width / interfaceSize.width;
	this->sizeFactor.height = (float)this->inputImage.size().height / interfaceSize.height;
	pointVectorSet.clear();
}

ManualIntersector::~ManualIntersector()
{
}

void ManualIntersector::RunSelector(std::string titlePrefix)
{
	this->windowName = titlePrefix + (titlePrefix.empty()?"":" - ") + this->imagePath;
	cv::namedWindow(windowName, 1);
	cv::setMouseCallback(windowName,OnMouseEvent,this);
	this->pointVectorSet.clear();
	InitNewLine();
	cv::waitKey(0);
	cv::destroyWindow(windowName);
	if (pointVectorSet.back().size() == 0)
		pointVectorSet.pop_back();
}

ManualIntersector::PointVectorSet ManualIntersector::GetPointVectorSet()
{
	return pointVectorSet;
}

void ManualIntersector::OnMouseEvent(int event, int x, int y, int flags, void * userdata)
{
	ManualIntersector* manualIntersector = reinterpret_cast<ManualIntersector*>(userdata);
	switch (event)
	{
	case cv::MouseEventTypes::EVENT_LBUTTONUP:
		manualIntersector->AddPoint(x, y);
		break;
	case cv::MouseEventTypes::EVENT_RBUTTONUP:
		manualIntersector->UndoPoint();
		break;
	case cv::MouseEventTypes::EVENT_MBUTTONUP:
		manualIntersector->InitNewLine();
		break;
		default:
		break;
	}
	cv::imshow(manualIntersector->windowName, manualIntersector->GetDrawing(true));
	cv::waitKey(1);
}

void ManualIntersector::AddPoint(int x, int y)
{
	pointVectorSet.back().push_back(cv::Point(x, y));
}

void ManualIntersector::AddPoint(cv::Point point)
{
	pointVectorSet.back().push_back(point);
}

void ManualIntersector::UndoPoint()
{
	if (pointVectorSet.back().size())
		pointVectorSet.back().pop_back();
	else
		pointVectorSet.pop_back();
}

void ManualIntersector::InitNewLine()
{
	pointVectorSet.push_back(std::vector<cv::Point>());
}

cv::Mat ManualIntersector::GetDrawing(bool highlightLast)
{
	cv::Mat drawing = fixedImage.clone();
	cv::Scalar color;
	for (int iL = 0; iL < pointVectorSet.size(); iL++)
	{
		if (highlightLast && (iL == pointVectorSet.size()-1))
			color = cv::Scalar(0, 0, 255);
		else
			color = cv::Scalar(0, 255, 255);
		for (int iP = 0; iP < pointVectorSet[iL].size(); iP++)
		{
			if (iP)
				cv::line(drawing, pointVectorSet[iL][iP], pointVectorSet[iL][iP-1],color);
			cv::circle(drawing, pointVectorSet[iL][iP], 0, color, 3);
		}
	}
	return drawing;
}

bool ManualIntersector::SaveFileDescription(std::string &filePathRef, std::string fileNameSuffix)
{
	int fileExtDotPos = imagePath.rfind('.');
	if (std::string::npos != fileExtDotPos)
	{
		this->imageDescrptionPath = imagePath.substr(0, fileExtDotPos) + fileNameSuffix + ".txt";
		for (int i = 2; std::ifstream(this->imageDescrptionPath.c_str()).good(); i++)
		{
			this->imageDescrptionPath = imagePath.substr(0, fileExtDotPos) + fileNameSuffix + "(" + std::to_string(i) + ").txt";
		}
		filePathRef = this->imageDescrptionPath;
		std::ofstream imageDescriptionFile;
		imageDescriptionFile.open(this->imageDescrptionPath);
		for (std::vector<cv::Point> line : this->pointVectorSet)
		{
			for (cv::Point point : line)
			{
				TranslateToOriginal(point);
				imageDescriptionFile << cv::format("(%d,%d);", point.x, point.y);
			}
			imageDescriptionFile << std::endl;
		}
		imageDescriptionFile.close();
		return true;
	}
	return false;
}

bool ManualIntersector::LoadImageDescription(std::string fileNameSuffix)
{
	int fileExtDotPos = imagePath.rfind('.');
	std::string txtFilePath = imagePath.substr(0, fileExtDotPos) + fileNameSuffix + ".txt";
	std::ifstream imageDescriptionFile(txtFilePath);
	if (imageDescriptionFile.is_open())
	{
		this->imageDescrptionPath = txtFilePath;
		cv::String line;
		this->pointVectorSet.clear();
		while (std::getline(imageDescriptionFile,line))
		{
			std::stringstream lineStream(line);
			cv::String element;
			InitNewLine();
			while (std::getline(lineStream, element, ';'))
			{
				int separatorPos = element.rfind(",");
				cv::Point point;
				point.x = std::stoi(element.substr(1, separatorPos));
				point.y = std::stoi(element.substr(separatorPos + 1, element.size() - separatorPos - 2));
				TranslateToProjection(point);
				AddPoint(point);
			}
		}
		return true;
	}
	return false;
}

void ManualIntersector::TranslateToOriginal(cv::Point &point)
{
	point.x = point.x * this->sizeFactor.width;
	point.y = point.y * this->sizeFactor.height;
}

void ManualIntersector::TranslateToProjection(cv::Point &point)
{
	point.x = point.x / this->sizeFactor.width;
	point.y = point.y / this->sizeFactor.height;
}

bool OpenJpgFile(std::string &filePath)
{
	const std::wstring title = L"Select a File";
	std::wstring filename(MAX_PATH, L'\0');

	OPENFILENAMEW ofn = { };
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = NULL;
	ofn.lpstrFilter = L"Image (.jpg)\0*.jpg\0All\0*.*\0";
	ofn.lpstrFile = &filename[0];  // use the std::wstring buffer directly
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrTitle = title.c_str();
	ofn.Flags = OFN_DONTADDTORECENT | OFN_FILEMUSTEXIST;

	if (GetOpenFileNameW(&ofn))
	{
		using convert_type = std::codecvt_utf8<wchar_t>;
		std::wstring_convert<convert_type, wchar_t> converter;
		filePath = converter.to_bytes(filename).c_str();
		return true;
	}
	return false;
}