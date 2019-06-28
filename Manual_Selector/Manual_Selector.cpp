// Manual_Selector.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include "Manual_Selector.h"

int main()
{
	std::string filePath;
	while (OpenJpgFile(filePath))
	{
		std::cout << filePath << std::endl;
		//CREATE
		ManualIntersector intersector(filePath);
		intersector.RunSelector();
		ManualIntersector::PointVectorSet pointVectorSet = intersector.GetPointVectorSet();
		intersector.SaveFileDescription();
	}
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
