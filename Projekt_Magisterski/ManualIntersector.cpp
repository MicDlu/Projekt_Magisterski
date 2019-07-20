#include "pch.h"
#include "ManualIntersector.h"

ManualIntersector::ManualIntersector(cv::String imagePath, cv::Size interfaceSize)
{
	this->imagePath = imagePath;
	this->inputImage = cv::imread(imagePath);	
	cv::resize(inputImage, this->fixedImage, interfaceSize);
	this->sizeFactor.width = (float)this->inputImage.size().width / interfaceSize.width;
	this->sizeFactor.height = (float)this->inputImage.size().height / interfaceSize.height;
	this->pointVectorSet.clear();
	this->imageDrawing = this->fixedImage;
	// PREPARE DIRECTORY
	_mkdir(GetFilePathNoExtension().c_str());
	struct stat buffer;
	if (stat(GetFilePathNoExtension().c_str(), &buffer))
	{
		std::cout << "Blad tworzenia folderu" << std::endl;
		system("pause");
		throw "Blad tworzenia folderu";
	}		
}

ManualIntersector::ManualIntersector(cv::String imagePath, cv::Size interfaceSize, PointVectorSet parsedVectorSet)
{
	this->imagePath = imagePath;
	this->inputImage = cv::imread(imagePath);
	cv::resize(inputImage, this->fixedImage, interfaceSize);
	this->sizeFactor.width = (float)this->inputImage.size().width / interfaceSize.width;
	this->sizeFactor.height = (float)this->inputImage.size().height / interfaceSize.height;
	this->pointVectorSet = parsedVectorSet;
	this->imageDrawing = this->fixedImage;
	// PREPARE DIRECTORY
	_mkdir(GetFilePathNoExtension().c_str());
	struct stat buffer;
	if (stat(GetFilePathNoExtension().c_str(), &buffer))
	{
		std::cout << "Blad tworzenia folderu" << std::endl;
		system("pause");
		throw "Blad tworzenia folderu";
	}
}

ManualIntersector::~ManualIntersector()
{
}

void ManualIntersector::RunSelector(std::string titlePrefix, char orientation)
{
	this->windowName = titlePrefix + (titlePrefix.empty()?"":" - ") + this->imagePath;
	cv::namedWindow(windowName, 1);
	cv::imshow(windowName, this->fixedImage);
	cv::waitKey(1);
	cv::setMouseCallback(windowName,OnMouseEvent,this);
	this->pointVectorSet.clear();
	InitNewLine();
	while (cv::waitKey(0) != 27);
	cv::destroyWindow(windowName);
	cv::destroyWindow("Lupa");
	if (pointVectorSet.back().size() == 0)
		pointVectorSet.pop_back();

	// SORTING
	if (orientation == 'H')
		this->horizontal = true;
	if (orientation == 'V')
		this->vertical = true;
	for (int i = 0; i < pointVectorSet.size(); i++)
		CorrectVectorDirection(pointVectorSet[i]);
}

void ManualIntersector::CorrectVectorDirection(std::vector<cv::Point> &pointVector)
{
	if (this->vertical && pointVector.front().y > pointVector.back().y)
			std::reverse(pointVector.begin(), pointVector.end());
	if (this->horizontal && pointVector.front().x > pointVector.back().x)
		std::reverse(pointVector.begin(), pointVector.end());
}

void ManualIntersector::UpdateZoomWindow(int x, int y)
{
	// RECT LIMITED TO INPUT IMAGE
	cv::Rect zoomRect(x - ZOOM_SIZE / 2, y - ZOOM_SIZE / 2, ZOOM_SIZE, ZOOM_SIZE);
	if (zoomRect.x < 0)
		zoomRect.x = 0;
	if (zoomRect.x + zoomRect.width >= imageDrawing.cols)
		zoomRect.x = imageDrawing.cols - zoomRect.width;
	if (zoomRect.y < 0)
		zoomRect.y = 0;
	if (zoomRect.y + zoomRect.height >= imageDrawing.rows)
		zoomRect.y = imageDrawing.rows - zoomRect.height;

	// CREATE ZOOM IMAGE
	cv::resize(imageDrawing(zoomRect), this->imageZoom, cv::Size(ZOOM_WINDOW_SIZE, ZOOM_WINDOW_SIZE));

	// DRAW CROSS
	cv::Point zoomPoint(ZOOM_WINDOW_SIZE / 2, ZOOM_WINDOW_SIZE / 2);
	zoomPoint.x = (x - zoomRect.x) * ZOOM_FACTOR;
	zoomPoint.y = (y - zoomRect.y) * ZOOM_FACTOR;
	cv::line(this->imageZoom, cv::Point(zoomPoint.x, 0), cv::Point(zoomPoint.x, ZOOM_WINDOW_SIZE - 1), cv::Scalar(255, 0, 0));
	cv::line(this->imageZoom, cv::Point(0, zoomPoint.y), cv::Point(ZOOM_WINDOW_SIZE - 1, zoomPoint.y), cv::Scalar(255, 0, 0));

	cv::imshow("Lupa", this->imageZoom);
	
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
	case cv::MouseEventTypes::EVENT_MOUSEMOVE:
		manualIntersector->UpdateZoomWindow(x,y);
		break;
	default:
		break;
	}
	cv::imshow(manualIntersector->windowName, manualIntersector->GetLinearDrawing(true));
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
	else if (pointVectorSet.size() > 1)
		pointVectorSet.pop_back();
}

void ManualIntersector::InitNewLine()
{
	if (pointVectorSet.empty() || !pointVectorSet.back().empty())
		pointVectorSet.push_back(std::vector<cv::Point>());
}

cv::Mat ManualIntersector::GetLinearDrawing(bool highlightLast)
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
	this->imageDrawing = drawing;
	return drawing;
}

cv::Mat ManualIntersector::GetGridDrawing()
{
	cv::Mat drawing = fixedImage.clone();
	cv::Scalar color(0, 0, 255);
	for (int iL = 0; iL < pointVectorSet.size(); iL++)
	{
		for (int iP = 0; iP < pointVectorSet[iL].size(); iP++)
		{
			if (iL && iP < pointVectorSet[iL-1].size())
				cv::line(drawing, pointVectorSet[iL][iP], pointVectorSet[iL - 1][iP], color);
			if (iP)
				cv::line(drawing, pointVectorSet[iL][iP], pointVectorSet[iL][iP - 1], color);
			cv::circle(drawing, pointVectorSet[iL][iP], 0, color, 3);

		}
	}
	return drawing;
}

bool ManualIntersector::SaveFileDescription(std::string &filePathRef, std::string fileNameSuffix)
{
	this->imageDescrptionPath = GetDirFilePathNoExtension() + fileNameSuffix + ".txt";
	for (int i = 2; std::ifstream(this->imageDescrptionPath.c_str()).good(); i++)
	{
		this->imageDescrptionPath = GetDirFilePathNoExtension() + fileNameSuffix + "(" + std::to_string(i) + ").txt";
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
	return std::ifstream(this->imageDescrptionPath.c_str()).good();
}

bool ManualIntersector::LoadImageDescription(std::string fileNameSuffix, char orientation)
{
	if (orientation == 'H')
		this->horizontal = true;
	if (orientation == 'V')
		this->vertical = true;
	std::string txtFilePath = GetDirFilePathNoExtension() + fileNameSuffix + ".txt";
	std::ifstream imageDescriptionFile(txtFilePath);
	if (imageDescriptionFile.is_open())
	{
		this->imageDescrptionPath = txtFilePath;
		cv::String line;
		this->pointVectorSet.clear();
		while (std::getline(imageDescriptionFile,line))
		{
			if (line == "")
				continue;
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
			CorrectVectorDirection(pointVectorSet.back());
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

std::string ManualIntersector::GetFilePathNoExtension()
{
	int fileExtDotPos = imagePath.rfind('.');
	return imagePath.substr(0, fileExtDotPos);
}

std::string ManualIntersector::GetFileName()
{
	int fileNamePos = GetFilePathNoExtension().rfind('\\');
	return GetFilePathNoExtension().substr(fileNamePos + 1, GetFilePathNoExtension().size());
}

std::string ManualIntersector::GetDirFilePathNoExtension()
{
	return GetFilePathNoExtension() + "\\" + GetFileName();
}

ManualIntersector::PointVectorSet ManualIntersector::GetScaledVectorSet(float scale)
{
	PointVectorSet scaledVectorSet;
	for (std::vector<cv::Point> line : this->pointVectorSet)
	{
		std::vector<cv::Point> scaledLine;
		for (cv::Point point : line)
		{
			scaledLine.push_back(point*scale);
		}
		scaledVectorSet.push_back(scaledLine);
	}
	return scaledVectorSet;
}

std::vector<std::vector<cv::Point>> GetVectorSetsIntersection(std::vector<std::vector<cv::Point>> horizontalSet, std::vector<std::vector<cv::Point>> verticalSet)
{
	ManualIntersector::PointVectorSet resultSet;
	for (int h = 0; h < horizontalSet.size(); h++)
	{
		resultSet.push_back(std::vector<cv::Point>());
		for (int v = 0; v < verticalSet.size(); v++)
		{
			cv::Point point = GetVectorSetsIntersection(horizontalSet[h], verticalSet[v]);
			if (point.x == 0)
				break;
			resultSet[h].push_back(point);
		}
	}

	return resultSet;
}

cv::Point GetVectorSetsIntersection(std::vector<cv::Point> horizontalVec, std::vector<cv::Point> verticalVec)
{
	for (int v = 0; v < verticalVec.size() - 1; v++)
	{
		int vMinX = std::min(verticalVec[v].x, verticalVec[v + 1].x);
		int vMaxX = std::max(verticalVec[v].x, verticalVec[v + 1].x);
		int vMinY = std::min(verticalVec[v].y, verticalVec[v + 1].y);
		int vMaxY = std::max(verticalVec[v].y, verticalVec[v + 1].y);
		for (int h = 0; h < horizontalVec.size() - 1; h++)
		{
			int hMinX = std::min(horizontalVec[h].x, horizontalVec[h + 1].x);
			int hMaxX = std::max(horizontalVec[h].x, horizontalVec[h + 1].x);
			int hMinY = std::min(horizontalVec[h].y, horizontalVec[h + 1].y);
			int hMaxY = std::max(horizontalVec[h].y, horizontalVec[h + 1].y);

			// SKIP IF NOT INTERSECTING
			if (hMaxX < vMinX || hMinX > vMaxX || hMaxY < vMinY || hMinY > vMaxY)
				continue;

			if (verticalVec[v + 1].x == verticalVec[v].x)
			{
				float aH = (float)(horizontalVec[h + 1].y - horizontalVec[h].y) / (horizontalVec[h + 1].x - horizontalVec[h].x);
				float bH = (float)horizontalVec[h].y - aH * horizontalVec[h].x;
				float x = verticalVec[v].x;
				float y = aH * x + bH;
				return cv::Point(x, y);
			}
			else
			{
				// CALC LINE SEGMENTS PARAMS
				float aV = (float)(verticalVec[v + 1].y - verticalVec[v].y) / (verticalVec[v + 1].x - verticalVec[v].x);
				float bV = (float)verticalVec[v].y - aV * verticalVec[v].x;
				float aH = (float)(horizontalVec[h + 1].y - horizontalVec[h].y) / (horizontalVec[h + 1].x - horizontalVec[h].x);
				float bH = (float)horizontalVec[h].y - aH * horizontalVec[h].x;
				// CALC INTERSECTION POINT
				float x = (bV - bH) / (aH - aV);
				float y = aH * x + bH;
				return cv::Point(x, y);
			}			
		}
	}
	return cv::Point();
}