#include "pch.h"
#include "OtsuN.h"

OtsuN::OtsuN(cv::Mat image, int nOfLevels = 3)
{
	if (nOfLevels <= 1)
		throw std::invalid_argument("nOfLevels must be higher than 1");

	this->image = image;
	this->nOfLevels = nOfLevels;
	if (image.channels() > 1)
		cvtColor(image, image, cv::COLOR_RGB2GRAY);

	// init histogram values
	int histogram[LEVELS] = { 0 };
	int nOfPixels = image.rows * image.cols;
	for (int iR = 0; iR < image.rows; iR++)
	{
		for (int iC = 0; iC < image.cols; iC++)
		{
			uchar value = image.at<uchar>(iR, iC);
			histogram[image.at<uchar>(iR, iC)] += 1;
		}
	}
	for (int i = 0; i < LEVELS; i++)
	{
		pi[i] = (double)histogram[i] / nOfPixels;
		meanT += i * pi[i];
	}
	maxBCV = 0;

	// start recurences
	NextLevel(nOfLevels-1, 0, 0, 0);

	// create images
	int colorStep = 256 / nOfLevels;
	imageLeveled = cv::Mat::zeros(image.size(), CV_8U);
	for (int iI = 0; iI < nOfLevels; iI++)
	{
		cv::Mat imageCurrLevel = cv::Mat::zeros(image.size(), CV_8U);
		cv::Vec2d bounds;
		bounds[1] = (iI < optimals.size()) ? (optimals[iI]) : (256);
		bounds[0] = (iI) ? (optimals[iI - 1]) : (0);
		for (int iR = 0; iR < image.rows; iR++)
		{
			for (int iC = 0; iC < image.cols; iC++)
			{
				int pixel = image.at<uchar>(iR, iC);
				if (pixel < bounds[1] && pixel >= bounds[0])
				{
					imageCurrLevel.at<uchar>(iR, iC) = 255;
					imageLeveled.at<uchar>(iR, iC) = (iI + 1) * colorStep;
				}
			}
		}
		imageLevels.push_back(imageCurrLevel);
	}

	// fix levels
	if (OTSU_LEVELFIX)
	{
		//imshow("level1 before fix", imageLevels[1]);

		//variant #1
		cv::Mat reducer;
		cv::dilate(imageLevels[2], reducer, cv::Mat::ones(cv::Size(3, 3), CV_8U));
		cv::Mat v1 = imageLevels[1] - reducer;
		//imshow("level1 after fix", v1);

		//// variant #2
		//cv::morphologyEx(imageLevels[1], reducer, cv::MORPH_OPEN, cv::Mat::ones(cv::Size(2, 2), CV_8U));
		//cv::Mat v2 = reducer;
		//imshow("v2", v2);

		imageLevels[1] = v1;
		//cv::waitKey(0);
	}
}


OtsuN::~OtsuN()
{
}

void OtsuN::NextLevel(int levelNo, double sumP, double sumW, double sumV)
{
	double weight = 0;
	double mean = 0;
	double variance = 0;
	double p = 0;

	int iForBeg = 0;
	if (currents.size() > 0)
		iForBeg = currents.back() + 1;
	for (int i = iForBeg; i < 256; i++)
	{
		currents.push_back(i);
		p += pi[i];
		weight += pi[i] * i;
		if (p != 0)
			mean = weight / p;
		variance = p * pow(mean - meanT, 2);

		if (levelNo > 1)
			NextLevel(levelNo - 1, sumP + p, sumW + weight, sumV + variance);
		else
		{
			double lastP = 1 - (sumP + p);
			double lastW = meanT - (sumW + weight);
			double lastM = lastW / lastP;
			double lastV = lastP * pow(lastM - meanT, 2);

			double betweenClassVariance = sumV + variance + lastV;
			if (betweenClassVariance > maxBCV)
			{
				maxBCV = betweenClassVariance;
				optimals.clear();
				for (int o = 0; o < currents.size(); o++)
				{
					optimals.push_back(currents[o]);
				}
			}
		}
		currents.pop_back();
	}
}

cv::Mat OtsuN::ReturnLeveledImage()
{
	/*imshow("All levels", imageLeveled);
	cv::waitKey(0);*/
	return imageLeveled;
}

cv::Mat OtsuN::ReturnLevelImage(int level)
{
	if (level < 0 || level > optimals.size())
		throw std::invalid_argument("level not in range");	
	//imshow("Level" + std::to_string(level), imageLevels[level]);
	//cv::waitKey(0);
	return imageLevels[level];
}

std::vector<int> OtsuN::ReturnThresholds()
{
	return optimals;
}

