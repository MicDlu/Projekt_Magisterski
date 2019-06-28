#ifndef PCH_H
#define PCH_H

#include <iostream>
#include <fstream>
#include <atlstr.h>
#include <locale>
#include <codecvt>
#include <numeric>
#include <math.h>
#include <windows.h>

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>

#include "../Projekt_Magisterski/ManualIntersector.h"

#define IMAGE_SIZE_SVGA cv::Size(800,600)
#define IMAGE_SIZE_PAL cv::Size(1024,576)
#define IMAGE_SIZE_HD720 cv::Size(1280,720)
#define IMAGE_SIZE_HD1080 cv::Size(1920,1080)

#endif //PCH_H
