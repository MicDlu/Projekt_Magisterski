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
		std::cout << std::endl << "Wczytano plik: " << filePath << std::endl;
		ManualIntersector intersector(filePath, windowSize);
		intersector.RunSelector("poziomo");
		intersector.SaveFileDescription(descriptionPath,"_H");
		std::cout << "Zapisano definicje: " << descriptionPath << std::endl;
		intersector.RunSelector("pionowo");
		intersector.SaveFileDescription(descriptionPath,"_V");
		std::cout << "Zapisano definicje: " << descriptionPath << std::endl;
		system("pause");
	}
	std::cout << std::endl << "Dzieki za pomoc <3" << std::endl;
	system("pause");
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
