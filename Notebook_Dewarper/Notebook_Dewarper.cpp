// Notebook_Dewarper.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include "Notebook_Dewarper.h"

int main()
{
	std::string jpgFilePath;
	while (OpenJpgFile(jpgFilePath))
	{
		// HORIZONTAL LINES IMAGE
		ManualIntersector intersectorH(jpgFilePath, IMAGE_SIZE_SVGA);
		if (intersectorH.LoadImageDescription("_H",'H'))
			cv::imshow("Definicja pozioma (H): " + jpgFilePath, intersectorH.GetLinearDrawing(false));

		// VERTICAL LINES IMAGE
		ManualIntersector intersectorV(jpgFilePath, IMAGE_SIZE_SVGA);
		if (intersectorV.LoadImageDescription("_V",'V'))
			cv::imshow("Definicja pionowa (V): " + jpgFilePath, intersectorV.GetLinearDrawing(false));

		// CREATE GRID IF NOT EXIST
		ManualIntersector intersectorX(jpgFilePath, IMAGE_SIZE);
		if (!intersectorX.LoadImageDescription("_X",'X'))
			ParseHVtoX(jpgFilePath);

		// GRID IMAGE
		ManualIntersector intersectorXimg(jpgFilePath, IMAGE_SIZE_SVGA);
		if (intersectorXimg.LoadImageDescription("_X", 'X'))
			cv::imshow("Siatka (X): " + jpgFilePath, intersectorXimg.GetGridDrawing());

		cv::waitKey(1);

		// LOAD DESCRIPTION
		if (intersectorX.LoadImageDescription("_X",'X'))
		{						
			ManualIntersector::PointVectorSet pointVectorSet = intersectorX.GetPointVectorSet();

			// SRC GRID CALCULATIONS
			//cv::Size dstGridSize(25, 25);
			//cv::Size dstGridCount(pointVectorSet.front().size() - 1, pointVectorSet.size() - 1);
			cv::Size dstGridSize(25, 25);
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

				// DST GRID POSITION
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

				cv::namedWindow("Przetwarzanie... : " + jpgFilePath, cv::WINDOW_NORMAL);

				for (int i = 0; i < srcQuads.size(); i++)
				{
					//std::cout << "Przetwarzanie " << i << " / " << srcQuads.size() << std::endl;
					//cv::Mat srcCopy = src;

					// CALC GRID TRANSFORMATION
					cv::Mat H = cv::getPerspectiveTransform(srcQuads[i], dstQuad);

					// PERFORM TRANSFORMATION
					cv::Mat dstPart;
					cv::warpPerspective(src, dstPart, H, IMAGE_SIZE);

					// CROP GRID AREA					
					dstPart(gridRect).copyTo(dst(dstRects[i]));

					// UPDATE VIEW
					cv::imshow("Przetwarzanie...: " + jpgFilePath, dst);
					cv::waitKey(1);
				}

				cv::imshow("Obraz przetworzony: " + jpgFilePath, dst);
				cv::waitKey(0);

				cv::imwrite(intersectorX.GetDirFilePathNoExtension() + "_P.jpg", dst);
				std::cout << "Zapisano przetworzony obraz (P): " << intersectorX.GetDirFilePathNoExtension() + "_P.jpg" << std::endl;
				cv::destroyAllWindows();
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
	if (intersectorH.LoadImageDescription("_H",'H'))
	{
		ManualIntersector::PointVectorSet pointVectorSetH = intersectorH.GetPointVectorSet();

		ManualIntersector intersectorV(jpgFilePath, IMAGE_SIZE_SVGA);
		if (intersectorV.LoadImageDescription("_V",'V'))
		{
			ManualIntersector::PointVectorSet pointVectorSetV = intersectorV.GetPointVectorSet();
			ManualIntersector::PointVectorSet pointVectorSetX = GetVectorSetsIntersection(pointVectorSetH, pointVectorSetV);

			ManualIntersector intersectorX(jpgFilePath, IMAGE_SIZE_SVGA, pointVectorSetX);
			cv::Mat gridDrawing = intersectorX.GetGridDrawing();
			//cv::imshow("Utworzona siatka", gridDrawing);
			std::string descriptionPath;
			intersectorX.SaveFileDescription(descriptionPath, "_X");
			std::cout << "Zapisano definicje polaczona: " << descriptionPath << std::endl;
			cv::imwrite(intersectorX.GetDirFilePathNoExtension() + "_X.jpg", gridDrawing);
			std::cout << "Zapisano obraz siatki: " << intersectorX.GetDirFilePathNoExtension() + "_X.jpg" << std::endl;
			return true;
		}
	}
	return false;
}