#include "pch.h"
#include "ManualIntersector.h"


ManualIntersector::ManualIntersector(cv::String imagePath)
{
	this->imagePath = imagePath;
	this->inputImage = cv::imread(imagePath);
	cv::resize(inputImage, this->fixedImage, IMAGE_SIZE_HD);
	this->sizeFactor.width = this->inputImage.size().width / this->inputImage.size().width;
	this->sizeFactor.height = this->inputImage.size().height / this->inputImage.size().height;
	pointVectorSet.clear();
}


ManualIntersector::~ManualIntersector()
{
}

void ManualIntersector::RunSelector()
{
	InitNewLine();
	cv::namedWindow(windowName, 1);
	cv::setMouseCallback(windowName,OnMouseEvent,this);
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

void ManualIntersector::SaveFileDescription()
{
	int fileExtDotPos = imagePath.rfind('.');
	if (std::string::npos != fileExtDotPos)
	{
		this->imageDescrptionPath = imagePath.substr(0, fileExtDotPos) + ".txt";
		for (int i = 2; std::ifstream(this->imageDescrptionPath.c_str()).good(); i++)
		{
			this->imageDescrptionPath = imagePath.substr(0, fileExtDotPos) + "(" + std::to_string(i) + ").txt";
		}
		std::ofstream imageDescriptionFile;
		imageDescriptionFile.open(this->imageDescrptionPath);
		for (std::vector<cv::Point> line : this->pointVectorSet)
		{
			for (cv::Point point : line)
			{
				imageDescriptionFile << cv::format("(%d,%d);", point.x, point.y);
			}
			imageDescriptionFile << std::endl;
		}
		imageDescriptionFile.close();
	}
}

bool ManualIntersector::LoadImageDescription(cv::String txtFilePath)
{
	if (txtFilePath == "")
	{
		int fileExtDotPos = imagePath.rfind('.');
		txtFilePath = imagePath.substr(0, fileExtDotPos) + ".txt";
	}
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
				int x = std::stoi(element.substr(1, separatorPos));
				int y = std::stoi(element.substr(separatorPos + 1, element.size() - separatorPos - 2));
				AddPoint(x, y);
			}
		}
		return true;
	}
	return false;
}
