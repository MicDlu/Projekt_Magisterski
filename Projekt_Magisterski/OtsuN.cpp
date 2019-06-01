#include "pch.h"
#include "OtsuN.h"

OtsuN::OtsuN(cv::Mat image, int nOfLevels)
{
	if (nOfLevels <= 1)
		throw std::invalid_argument("nOfLevels must be higher than 1");
	if (image.channels() > 1)
		cvtColor(image, image, cv::COLOR_RGB2GRAY);

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
	NextLevel(nOfLevels-1, 0, 0, 0);
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

void OtsuN::ShowLevels(cv::Mat image)
{
	if (image.channels() > 1)
		cvtColor(image, image, cv::COLOR_RGB2GRAY);
	cv::Mat image3lvl = cv::Mat::zeros(image.size(), CV_8UC3);

	for (int iR = 0; iR < image.rows; iR++)
	{
		for (int iC = 0; iC < image.cols; iC++)
		{
			int pixel = image.at<uchar>(iR, iC);
			for (int iL = 0; iL < optimals.size(); iL++)
			{
				if (pixel < optimals[iL])
				{
					image3lvl.at<cv::Vec3b>(iR, iC) = cv::Vec3b(50 * iL, 250 - (50 * iL), 0);
					break;
				}
			}
		}
	}
	imshow("Otsu", image3lvl);
	cv::waitKey(0);
}

std::vector<int> OtsuN::ReturnThresholds()
{
	return optimals;
}

