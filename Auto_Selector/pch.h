// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file

#ifndef PCH_H
#define PCH_H

#define IMAGE_FIX_SIZE cv::Size(800,600)
#define RAD2DEG 180.0 / CV_PI
#define GRIDS_VERTIC 42
#define GRIDS_HORIZ 30

// TODO: add headers that you want to pre-compile here

#include <iostream>
#include <fstream>
#include <numeric>
#include <math.h>
#include <windows.h>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>

//#include "Projekt_Magisterski.h"
//#include "Preprocessing.h"
//#include "LineExtender.h"
//#include "DocAreaLSD.h"
//#include "OtsuN.h"
//#include "Testing.h"
//#include "PointLines.h"
#include "../Projekt_Magisterski/ManualIntersector.h"

#endif //PCH_H
