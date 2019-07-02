// Notebook_Dewarper.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include "Notebook_Dewarper.h"

int main()
{
    std::cout << "Hello World!\n";
	std::string jpgFilePath;
	OpenJpgFile(jpgFilePath);
	ManualIntersector intersectorH(jpgFilePath, IMAGE_SIZE_SVGA);
	if (intersectorH.LoadImageDescription("_H"))
	{
		ManualIntersector::PointVectorSet pointVectorSetH = intersectorH.GetPointVectorSet();
		cv::imshow("Drawing H", intersectorH.GetLinearDrawing(false));

		ManualIntersector::PointVectorSet resultVectorSet;
		if (true)
		{
			ManualIntersector intersectorV(jpgFilePath, IMAGE_SIZE_SVGA);
			if (intersectorV.LoadImageDescription("_V"))
			{
				ManualIntersector::PointVectorSet pointVectorSetV = intersectorV.GetPointVectorSet();
				cv::imshow("Drawing V", intersectorV.GetLinearDrawing(false));
				resultVectorSet = GetVectorSetsIntersection(pointVectorSetH, pointVectorSetV);

				cv::Mat drawing = cv::imread(jpgFilePath);
				cv::resize(drawing, drawing, IMAGE_SIZE_SVGA);
				for (std::vector<cv::Point> line : resultVectorSet)
				{
					for (cv::Point point : line)
					{
						cv::circle(drawing, point, 2, cv::Scalar(0, 0, 255),3);
					}
				}
				cv::imshow("drawing", drawing);
				cv::waitKey(0);
			}
		}
		///////////////////////////////////////////////////

		ManualIntersector::PointVectorSet pointVectorSet = resultVectorSet;
		//ManualIntersector::PointVectorSet pointVectorSet = intersectorH.GetPointVectorSet();

		cv::Size2f dstGridSize(50, 50);
		cv::Size dstGridCount(pointVectorSet.front().size(), pointVectorSet.size());

		// PERSPECTIVE
		if (true)
		{
			std::vector<std::vector<cv::Point2f>> srcQuads;
			std::vector<cv::Rect> dstRects;
			for (int iL = 0; iL <= pointVectorSet.size() - 2; iL++)
			{
				for (int iP = 0; iP <= pointVectorSet[iL].size() - 2 && iP <= pointVectorSet[iL+1].size() - 2; iP++)
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

				int minX = min(srcQuads[i][0].x, srcQuads[i][3].x);
				int maxX = max(srcQuads[i][2].x, srcQuads[i][1].x);
				int minY = min(srcQuads[i][0].y, srcQuads[i][1].y);
				int maxY = max(srcQuads[i][3].y, srcQuads[i][2].y);
				int margin = 20 % min(src.cols- maxX, src.rows - maxY);
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

			cv::imwrite(intersectorH.GetFilePathNoExtension() + "_Persp.jpg", dst);
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
