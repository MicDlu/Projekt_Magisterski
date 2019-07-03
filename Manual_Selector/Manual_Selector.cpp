// Manual_Selector.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include "Manual_Selector.h"

int main()
{
	std::cout << "Rozmiar okna:" << std::endl;
	std::cout << "1. SVGA (" << IMAGE_SIZE_SVGA.width << "/" << IMAGE_SIZE_SVGA.height << ")" << std::endl;
	std::cout << "2. PAL+ (" << IMAGE_SIZE_PAL.width << "/" << IMAGE_SIZE_PAL.height << ")" << std::endl;
	std::cout << "3. HD720 (" << IMAGE_SIZE_HD720.width << "/" << IMAGE_SIZE_HD720.height << ")" << std::endl;
	std::cout << "4. HD1080 (" << IMAGE_SIZE_HD1080.width << "/" << IMAGE_SIZE_HD1080.height << ")" << std::endl;
	char key;
	do
	{
		std::cin >> key;
	} while (!(key == '1' || key == '2' || key == '3' || key == '4'));
	cv::Size windowSize;
	switch (key)
	{
	case '1':
		windowSize = IMAGE_SIZE_SVGA;
		break;
	case '2':
		windowSize = IMAGE_SIZE_PAL;
		break;
	case '3':
		windowSize = IMAGE_SIZE_HD720;
		break;
	case '4':
		windowSize = IMAGE_SIZE_HD1080;
		break;
	default:
		break;
	}
	std::cout << "Wybrano: " << windowSize.width << "/" << windowSize.height << std::endl;

	std::string filePath;
	std::string descriptionPath;

	while (OpenJpgFile(filePath))
	{
		cv::Mat drawing = cv::imread(filePath);
		cv::resize(drawing, drawing, windowSize);
		std::cout << std::endl << "Wczytano plik: " << filePath << std::endl;

		ManualIntersector intersectorH(filePath, windowSize);
		intersectorH.RunSelector("poziomo");
		intersectorH.SaveFileDescription(descriptionPath,"_H");
		std::cout << "Zapisano definicje poziomo: " << descriptionPath << std::endl;

		ManualIntersector intersectorV(filePath, windowSize);
		intersectorV.RunSelector("pionowo");
		intersectorV.SaveFileDescription(descriptionPath,"_V");
		std::cout << "Zapisano definicje pionowa: " << descriptionPath << std::endl;

		ManualIntersector::PointVectorSet resultVectorSet = GetVectorSetsIntersection(intersectorH.GetPointVectorSet(), intersectorV.GetPointVectorSet());
		ManualIntersector intersectorX(filePath, windowSize, resultVectorSet);
		cv::Mat gridDrawing = intersectorX.GetGridDrawing();
		cv::imshow("Drawing X", gridDrawing);
		intersectorX.SaveFileDescription(descriptionPath, "_X");
		std::cout << "Zapisano definicje polaczona: " << descriptionPath << std::endl;
		cv::imwrite(intersectorX.GetFilePathNoExtension() + "_X.jpg",gridDrawing);
		std::cout << "Zapisano obraz siatki: " << descriptionPath << std::endl;
		while (cv::waitKey(0) != 27);
		cv::destroyAllWindows();
	}
	std::cout << std::endl << "Dzieki za pomoc <3" << std::endl;
	system("pause");
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

			if (hMaxX < vMaxX || hMinX > vMaxX || hMaxY < vMinY || hMinY > vMaxY)
				continue;

			float aV = (float)(verticalVec[v + 1].y - verticalVec[v].y) / (verticalVec[v + 1].x - verticalVec[v].x);
			float bV = (float)verticalVec[v].y - aV * verticalVec[v].x;
			float aH = (float)(horizontalVec[h + 1].y - horizontalVec[h].y) / (horizontalVec[h + 1].x - horizontalVec[h].x);
			float bH = (float)horizontalVec[h].y - aH * horizontalVec[h].x;

			float x = (bV - bH) / (aH - aV);
			return cv::Point(x, aH * x + bH);
		}
	}

	return cv::Point();
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
