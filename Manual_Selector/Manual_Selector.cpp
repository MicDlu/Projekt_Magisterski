// Manual_Selector.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include "Manual_Selector.h"

int main()
{
	std::string filePath;
	std::string descriptionPath;
	while (OpenJpgFile(filePath))
	{
		std::cout << std::endl << "Wczytano plik: " << filePath;
		ManualIntersector intersector(filePath);
		intersector.RunSelector("poziomo");
		intersector.SaveFileDescription(descriptionPath,"_H");
		std::cout << "Zapisano definicje: " << descriptionPath << std::endl;
		intersector.RunSelector("pionowo");
		intersector.SaveFileDescription(descriptionPath,"_V");
		std::cout << "Zapisano definicje: " << descriptionPath << std::endl;
	}
	std::cout << std::endl << "Dziêki za pomoc <3";
	std::cin.ignore();
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
		filePath = converter.to_bytes(filename);    //<----------Save filepath in global variable
		return true;
	}
	return false;
}
