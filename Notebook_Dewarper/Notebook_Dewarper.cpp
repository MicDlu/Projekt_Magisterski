// Notebook_Dewarper.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"

int main()
{
    std::cout << "Hello World!\n";
	std::string jpgFilePath;
	OpenJpgFile(jpgFilePath);
	ManualIntersector intersector(jpgFilePath, IMAGE_SIZE_SVGA);
	if (intersector.LoadImageDescription("_H"))
	{
		cv::imshow("test", intersector.GetDrawing(false));
		//cv::waitKey(0);

		ManualIntersector::PointVectorSet pointVectorSet = intersector.GetPointVectorSet();

		cv::Size2f dstGridSize(50, 50);
		cv::Size dstGridCount(pointVectorSet.front().size(), pointVectorSet.size());

		std::vector<std::vector<cv::Point2f>> srcQuads;
		std::vector<cv::Rect> dstRects;
		for (int iL = 0; iL <= pointVectorSet.size() - 2; iL++)
		{
			for (int iP = 0; iP <= pointVectorSet[iL].size() - 2; iP++)
			{
				std::vector<cv::Point2f> srcQuad;
				srcQuad.push_back(pointVectorSet[iL][iP]);
				srcQuad.push_back(pointVectorSet[iL][iP+1]);
				srcQuad.push_back(pointVectorSet[iL+1][iP+1]);
				srcQuad.push_back(pointVectorSet[iL+1][iP]);
				srcQuads.push_back(srcQuad);

				dstRects.push_back(cv::Rect(iP*dstGridSize.width, iL*dstGridSize.height, dstGridSize.width, dstGridSize.height));
			}
		}
		
		cv::Mat src = cv::imread(jpgFilePath);
		cv::Mat dst = cv::Mat::zeros(cv::Size(dstGridSize.width*dstGridCount.width-1, dstGridSize.height*dstGridCount.height-2), src.type());

		std::vector<cv::Point2f> dstQuad;
		dstQuad.push_back(cv::Point(0, 0));
		dstQuad.push_back(cv::Point(dstGridSize.width, 0));
		dstQuad.push_back(cv::Point(dstGridSize.width, dstGridSize.height));
		dstQuad.push_back(cv::Point(0, dstGridSize.height));

		for (int i = 0; i < srcQuads.size(); i++)
		{
			src = cv::imread(jpgFilePath);
			cv::resize(src, src, IMAGE_SIZE_SVGA);
			//cv::line(src, srcQuads[i][0], srcQuads[i][1], cv::Scalar(255, 0, 0));
			//cv::line(src, srcQuads[i][1], srcQuads[i][2], cv::Scalar(255, 0, 0));
			//cv::line(src, srcQuads[i][2], srcQuads[i][3], cv::Scalar(255, 0, 0));
			//cv::line(src, srcQuads[i][3], srcQuads[i][0], cv::Scalar(255, 0, 0));
			//cv::imshow("src", src);
			//cv::waitKey(0);

			int margin = 30;
			int minX = min(srcQuads[i][0].x, srcQuads[i][3].x);
			int maxX = max(srcQuads[i][2].x, srcQuads[i][1].x);
			int minY = min(srcQuads[i][0].y, srcQuads[i][1].y);
			int maxY = max(srcQuads[i][3].y, srcQuads[i][2].y);
			cv::Rect roiRect(cv::Point(minX - margin, minY - margin), cv::Point(maxX + margin, maxY + margin));
			cv::Mat roi = src(roiRect);
			//cv::imshow("roi", roi);
			//cv::waitKey(0);

			std::vector<cv::Point2f> roiSrcQuad = srcQuads[i];
			for (int r = 0; r < roiSrcQuad.size(); r++)
			{
				roiSrcQuad[r].x -= minX - margin;
				roiSrcQuad[r].y -= minY - margin;
			}


			cv::Mat dstPart;
			cv::Mat transform = cv::getPerspectiveTransform(roiSrcQuad, dstQuad);
			cv::warpPerspective(roi, dstPart, transform, IMAGE_SIZE_SVGA);

			cv::Rect rect(dstQuad[0], dstQuad[2]);
			dstPart = dstPart(rect);

			dstPart.copyTo(dst(dstRects[i]));

			//cv::imshow("part", dst);
			//cv::waitKey(0);
		}

		cv::imshow("part", dst);
		cv::waitKey(0);

		cv::imwrite(intersector.GetFilePathNoExtension + "_Persp.jpg", dst);

		//cv::Mat transform = cv::getPerspectiveTransform(srcQuads.front(), dstQuads.front());
		//cv::Mat imageWarped;
		//cv::warpPerspective(src, dst, transform, IMAGE_SIZE_HD720);
		//cv::imshow("dst", dst);
		//cv::waitKey(0);
	}

}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
