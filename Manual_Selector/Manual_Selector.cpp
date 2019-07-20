// Manual_Selector.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include "Manual_Selector.h"

int main()
{
	std::cout << "Rozmiar okna:" << std::endl;
	std::cout << "0. Oryginalny" << std::endl;
	std::cout << "1. SVGA (" << IMAGE_SIZE_SVGA.width << "/" << IMAGE_SIZE_SVGA.height << ")" << std::endl;
	std::cout << "2. PAL+ (" << IMAGE_SIZE_PAL.width << "/" << IMAGE_SIZE_PAL.height << ")" << std::endl;
	std::cout << "3. HD720 (" << IMAGE_SIZE_HD720.width << "/" << IMAGE_SIZE_HD720.height << ")" << std::endl;
	std::cout << "4. HD1080 (" << IMAGE_SIZE_HD1080.width << "/" << IMAGE_SIZE_HD1080.height << ")" << std::endl;
	char key;
	do
	{
		std::cin >> key;
	} while (!(key == '0' || key == '1' || key == '2' || key == '3' || key == '4'));
	cv::Size windowSize;
	switch (key)
	{
	case '0':
		std::cout << "Wybrano oryginalny rozmiar" << std::endl << std::endl;
		break;
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
	if (key != 0)
		std::cout << "Wybrano: " << windowSize.width << "/" << windowSize.height << std::endl << std::endl;

	std::string filePath;
	std::string descriptionPath;

	std::cout << "*************************" << std::endl;
	std::cout << "LPM - Zaznacz punkt" << std::endl;
	std::cout << "SPM - Zakoncz linie" << std::endl;
	std::cout << "PPM - Cofnij" << std::endl;
	std::cout << "ESC - Przejdz dalej" << std::endl;
	std::cout << "*************************" << std::endl;

	while (OpenJpgFile(filePath))
	{
		cv::Mat drawing = cv::imread(filePath);
		if (key != 0)
			cv::resize(drawing, drawing, windowSize);
		else
			windowSize = drawing.size();
		std::cout << std::endl << "Wczytano plik: " << filePath << std::endl;

		ManualIntersector intersectorH(filePath, windowSize);
		intersectorH.RunSelector("poziomo",'H');
		if (intersectorH.SaveFileDescription(descriptionPath, "_H"))
		{
			std::cout << "Zapisano definicje pozioma: " << descriptionPath << std::endl;
			cv::imwrite(intersectorH.GetDirFilePathNoExtension() + "_H.jpg", intersectorH.GetLinearDrawing(false));
			std::cout << "Zapisano obraz definicji poziomej: " << intersectorH.GetDirFilePathNoExtension() + "_H.jpg" << std::endl;
		}
		else
			std::cout << "B³¹d zapisu: " << intersectorH.GetDirFilePathNoExtension() + "_H.txt" << std::endl;

			ManualIntersector intersectorV(filePath, windowSize);
			intersectorV.RunSelector("pionowo",'V');
			if (intersectorV.SaveFileDescription(descriptionPath,"_V"))
			{
				std::cout << "Zapisano definicje pionowa: " << descriptionPath << std::endl;
				cv::imwrite(intersectorV.GetDirFilePathNoExtension() + "_V.jpg", intersectorV.GetLinearDrawing(false));
				std::cout << "Zapisano obraz definicji pionowej: " << intersectorV.GetDirFilePathNoExtension() + "_V.jpg" << std::endl;
			}
			else
				std::cout << "B³¹d zapisu: " << intersectorV.GetDirFilePathNoExtension() + "_V.txt" << std::endl;

			ManualIntersector::PointVectorSet resultVectorSet = GetVectorSetsIntersection(intersectorH.GetPointVectorSet(), intersectorV.GetPointVectorSet());
			ManualIntersector intersectorX(filePath, windowSize, resultVectorSet);
			cv::Mat gridDrawing = intersectorX.GetGridDrawing();
			cv::imshow("Siatka", gridDrawing);
			if (intersectorX.SaveFileDescription(descriptionPath, "_X"))
			{
				std::cout << "Zapisano definicje siatki: " << descriptionPath << std::endl;
				cv::imwrite(intersectorX.GetDirFilePathNoExtension() + "_X.jpg", gridDrawing);
				std::cout << "Zapisano obraz siatki: " << intersectorX.GetDirFilePathNoExtension() + "_X.jpg" << std::endl;
			}
			else
				std::cout << "B³¹d zapisu: " << intersectorX.GetDirFilePathNoExtension() + "_X.txt" << std::endl;

			while (cv::waitKey(0) != 27);
			cv::destroyAllWindows();
		}
		std::cout << std::endl << "Dzieki za pomoc <3" << std::endl;
		system("pause");
	}
