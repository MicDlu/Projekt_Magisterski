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

		// PERSPECTIVE
		if (false)
		{
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

			cv::imwrite(intersector.GetFilePathNoExtension() + "_Persp.jpg", dst);
		}

		// THIN PLATE SPLINE
		if (false)
		{
			std::vector<cv::Point> srcPts, dstPts;
			for (int iL = 0; iL < pointVectorSet.size(); iL++)
			{
				for (int iP = 0; iP < pointVectorSet[iL].size(); iP++)
				{
					srcPts.push_back(pointVectorSet[iL][iP]);
					dstPts.push_back(cv::Point(iP*dstGridSize.width, iL*dstGridSize.height));
				}
			}
			CThinPlateSpline tps(srcPts, dstPts);

			cv::Mat	src = cv::imread(jpgFilePath);
			cv::Mat dst;
			tps.warpImage(src, dst, 0.05, INTER_CUBIC, BACK_WARP);

			cv::imshow("original", src);
			cv::imshow("distorted", dst);
			cv::waitKey(0);
		}
	}

}