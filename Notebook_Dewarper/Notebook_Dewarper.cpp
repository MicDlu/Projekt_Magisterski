#include "..\Projekt_Magisterski\Projekt_Magisterski.h"
#include "..\Projekt_Magisterski\Projekt_Magisterski.h"
// Notebook_Dewarper.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include "Notebook_Dewarper.h"

int main()
{

	std::string jpgFilePath;
	while (OpenJpgFile(jpgFilePath))
	{
		ManualIntersector intersectorX(jpgFilePath, IMAGE_SIZE);
		if (!intersectorX.LoadImageDescription("_X"))
			ParseHVtoX(jpgFilePath);

		if (intersectorX.LoadImageDescription("_X"))
		{
			ManualIntersector::PointVectorSet pointVectorSet = intersectorX.GetPointVectorSet();
			cv::Mat scaledGridDrawing;
			cv::resize(intersectorX.GetGridDrawing(), scaledGridDrawing, IMAGE_SIZE_SVGA);
			cv::imshow("Obraz siatki: " + jpgFilePath, scaledGridDrawing);
			cv::waitKey(1);

			// SRC GRID CALCULATIONS
			cv::Size2f dstGridSize(25, 25);
			int maxPointVectorWidth = pointVectorSet.front().size();
			for (std::vector<cv::Point> pointVector : pointVectorSet)
			{
				if (pointVector.size() > maxPointVectorWidth)
					maxPointVectorWidth = pointVector.size();
			}
			cv::Size dstGridCount(maxPointVectorWidth - 1, pointVectorSet.size() - 1);

			// PERSPECTIVE
			if (true)
			{
				std::cout << "Przetwarzanie siatki: " << jpgFilePath << std::endl;

				// CALC GRID SRC/DST QUADLITERALS
				std::vector<std::vector<cv::Point2f>> srcQuads;
				std::vector<cv::Rect> dstRects;
				for (int iL = 0; iL <= pointVectorSet.size() - 2; iL++)
				{
					for (int iP = 0; iP <= pointVectorSet[iL].size() - 2 && iP <= pointVectorSet[iL + 1].size() - 2; iP++)
					{
						std::vector<cv::Point2f> srcQuad;
						srcQuad.push_back(pointVectorSet[iL][iP]);
						srcQuad.push_back(pointVectorSet[iL][iP + 1]);
						srcQuad.push_back(pointVectorSet[iL + 1][iP + 1]);
						srcQuad.push_back(pointVectorSet[iL + 1][iP]);
						srcQuads.push_back(srcQuad);

						dstRects.push_back(cv::Rect(iP*dstGridSize.width, iL*dstGridSize.height, dstGridSize.width, dstGridSize.height));
					}
				}

				// DST GRID SIZE
				std::vector<cv::Point2f> dstQuad;
				dstQuad.push_back(cv::Point(0, 0));
				dstQuad.push_back(cv::Point(dstGridSize.width, 0));
				dstQuad.push_back(cv::Point(dstGridSize.width, dstGridSize.height));
				dstQuad.push_back(cv::Point(0, dstGridSize.height));
				cv::Rect gridRect(dstQuad[0], dstQuad[2]);

				// READ SRC / CREATE DST
				cv::Mat src = cv::imread(jpgFilePath);
				cv::resize(src, src, IMAGE_SIZE);
				cv::Mat dst = cv::Mat::zeros(cv::Size(dstGridSize.width*dstGridCount.width + 1, dstGridSize.height*dstGridCount.height), src.type());

				for (int i = 0; i < srcQuads.size(); i++)
				{
					//std::cout << "Przetwarzanie " << i << " / " << srcQuads.size() << std::endl;
					cv::Mat srcCopy = src;

					// CALC GRID TRANSFORM
					cv::Mat dstPart;
					cv::Mat transform = cv::getPerspectiveTransform(srcQuads[i], dstQuad);
					cv::warpPerspective(src, dstPart, transform, IMAGE_SIZE);

					// CROP GRID AREA					
					dstPart(gridRect).copyTo(dst(dstRects[i]));

					cv::imshow("part", dst);
					cv::waitKey(1);
				}

				//cv::imshow("part", dst);
				//cv::waitKey(0);

				cv::imwrite(intersectorX.GetFilePathNoExtension() + "_P.jpg", dst);
				std::cout << "Zapisano przetworzony obraz: " << intersectorX.GetFilePathNoExtension() + "_P.jpg" << std::endl;
				system("pause");
			}

		}
		else
		{
			std::cout << "Nie uda³o siê wczytac definicji siatki (_X / _H+_V): " << jpgFilePath << std::endl;
			system("pause");
		}
	}
	std::cout << std::endl << "Dzieki za pomoc <3" << std::endl;
	system("pause");
}

bool ParseHVtoX(std::string &jpgFilePath)
{
	ManualIntersector intersectorH(jpgFilePath, IMAGE_SIZE_SVGA);
	if (intersectorH.LoadImageDescription("_H"))
	{
		ManualIntersector::PointVectorSet pointVectorSetH = intersectorH.GetPointVectorSet();

		ManualIntersector intersectorV(jpgFilePath, IMAGE_SIZE_SVGA);
		if (intersectorV.LoadImageDescription("_V"))
		{
			ManualIntersector::PointVectorSet pointVectorSetV = intersectorV.GetPointVectorSet();
			ManualIntersector::PointVectorSet pointVectorSetX = GetVectorSetsIntersection(pointVectorSetH, pointVectorSetV);

			ManualIntersector intersectorX(jpgFilePath, IMAGE_SIZE_SVGA, pointVectorSetX);
			cv::Mat gridDrawing = intersectorX.GetGridDrawing();
			cv::imshow("Drawing X", gridDrawing);
			std::string descriptionPath;
			intersectorX.SaveFileDescription(descriptionPath, "_X");
			std::cout << "Zapisano definicje polaczona: " << descriptionPath << std::endl;
			cv::imwrite(intersectorX.GetFilePathNoExtension() + "_X.jpg", gridDrawing);
			std::cout << "Zapisano obraz siatki: " << intersectorX.GetFilePathNoExtension() + "_X.jpg" << std::endl;
			return true;
		}
	}
	return false;
}

cv::Point GetVectorSetsIntersection(std::vector<cv::Point> horizontalVec, std::vector<cv::Point> verticalVec)
{
	for (int v = 0; v < verticalVec.size() - 1; v++)
	{
		int vMinX = min(verticalVec[v].x, verticalVec[v + 1].x);
		int vMaxX = max(verticalVec[v].x, verticalVec[v + 1].x);
		int vMinY = min(verticalVec[v].y, verticalVec[v + 1].y);
		int vMaxY = max(verticalVec[v].y, verticalVec[v + 1].y);
		for (int h = 0; h < horizontalVec.size() - 1; h++)
		{
			int hMinX = min(horizontalVec[h].x, horizontalVec[h + 1].x);
			int hMaxX = max(horizontalVec[h].x, horizontalVec[h + 1].x);
			int hMinY = min(horizontalVec[h].y, horizontalVec[h + 1].y);
			int hMaxY = max(horizontalVec[h].y, horizontalVec[h + 1].y);
			
			if (hMaxX < vMaxX || hMinX > vMaxX || hMaxY < vMinY || hMinY > vMaxY)
				continue;

			float aV = (float)(verticalVec[v + 1].y - verticalVec[v].y) / (verticalVec[v + 1].x - verticalVec[v].x);
			float bV = (float)verticalVec[v].y - aV * verticalVec[v].x;
			float aH = (float)(horizontalVec[h+1].y - horizontalVec[h].y) / (horizontalVec[h+1].x - horizontalVec[h].x);
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
