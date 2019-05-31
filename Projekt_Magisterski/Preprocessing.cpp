#include "pch.h"

cv::Mat FixImageSize(cv::Mat input)
{
	cv::Mat output;
	cv::Size resizeSize = cv::Size(800, 600);
	cv::resize(input, output, resizeSize);
	return output;
}

cv::Mat ThresholdImage(cv::Mat input)
{
	cv::Mat output;
	cv::Mat inputGray;
	cvtColor(input, inputGray, cv::COLOR_RGB2GRAY);
	cv::adaptiveThreshold(inputGray, output, 255,cv::AdaptiveThresholdTypes::ADAPTIVE_THRESH_GAUSSIAN_C,cv::ThresholdTypes::THRESH_BINARY,11,2);
	return output;
}

std::vector<cv::String> GetFiles()
{
	std::vector<cv::String> files;
	cv::String directory = "C:/Users/micha/Pictures/Images/";
	//files.push_back(directory + "IMG_20190525_115849.jpg");
	//files.push_back(directory + "IMG_20190228_195957.jpg");
	//files.push_back(directory + "IMG_20190228_200009.jpg");
	//files.push_back(directory + "IMG_20190228_200028.jpg");
	//files.push_back(directory + "IMG_20190228_200050.jpg");
	files.push_back(directory + "IMG_20190228_200058.jpg");
	return files;
}

struct otsuThresh {
	double weight;
	double mean;
	double variance;
	double p;
};

// http://www.labbookpages.co.uk/software/imgProc/otsuThreshold.html
// https://stackoverflow.com/questions/35056760/multi-level-4-otsu-thresholding
cv::Vec2d Otsu3(cv::Mat image) {
	const int histLvls = 256;

		std::vector<cv::Mat> bgr;
	if (image.channels() > 1)
	{
		cvtColor(image, image, cv::COLOR_BGR2HSV);
		cv::split(image, bgr);
		image = bgr.at(2);
	}

	imshow("1", bgr.at(2));
	cv::waitKey(1);
	imshow("2", bgr.at(2));
	cv::waitKey(1);
	imshow("3", bgr.at(2));
	cv::waitKey(0);

	int histogram[histLvls] = { 0 };
	int nOfPixels = image.rows * image.cols;
	for (int iR = 0; iR < image.rows; iR++)
	{
		for (int iC = 0; iC < image.cols; iC++)
		{
			uchar value = image.at<uchar>(iR, iC);
			histogram[image.at<uchar>(iR, iC)] += 1;
		}
	}

	double p[histLvls] = { 0 };
	double meanT = 0;
	for (int i = 0; i < histLvls; i++)
	{
		p[i] = (double)histogram[i] / nOfPixels;
		meanT += i * p[i];
	}

	double betweenClassVariance = 0;
	double maxBetweenClassVariance = 0;
	std::vector<double> max;
	otsuThresh thr1;
	otsuThresh thr2;
	otsuThresh thr3;
	cv::Vec2d optimal;

	thr1 = { 0, 0, 0, 0 };
	for (int i1 = 0; i1 < histLvls; i1++)
	{
		thr1.p += p[i1];
		thr1.weight += p[i1] * i1;
		if (thr1.p != 0)
			thr1.mean = thr1.weight / thr1.p;
		thr1.variance = thr1.p * pow(thr1.mean - meanT,2);
		
		thr2 = { 0, 0, 0, 0 };
		for (int i2 = i1 + 1; i2 < histLvls; i2++)
		{
			thr2.p += p[i2];
			thr2.weight += p[i2] * i2;
			if (thr2.p != 0)
				thr2.mean = thr2.weight / thr2.p;
			thr2.variance = thr2.p * pow(thr2.mean - meanT, 2);

			thr3.p = 1 - (thr1.p + thr2.p);
			thr3.weight = meanT - (thr1.weight + thr2.weight);
			thr3.mean = thr3.weight / thr3.p;
			thr3.variance = thr3.p * pow(thr3.mean - meanT, 2);

			betweenClassVariance = thr1.variance + thr2.variance + thr3.variance;
			if (betweenClassVariance > maxBetweenClassVariance)
			{
				maxBetweenClassVariance = betweenClassVariance;
				max.push_back(maxBetweenClassVariance);
				optimal[0] = i1;
				optimal[1] = i2;
			}
		}
	}

	cv::Mat image3lvl = cv::Mat::zeros(image.size(), CV_8UC3);
	for (int iR = 0; iR < image.rows; iR++)
	{
		for (int iC = 0; iC < image.cols; iC++)
		{
			int pixel = image.at<uchar>(iR, iC);
			if (pixel < optimal[0])
				image3lvl.at<cv::Vec3b>(iR, iC) = cv::Vec3b(100,0,0);
			else if (pixel < optimal[1])
				image3lvl.at<cv::Vec3b>(iR, iC) = cv::Vec3b(0, 100, 0);
			else
				image3lvl.at<cv::Vec3b>(iR, iC) = cv::Vec3b(100, 0, 100);
		}
	}

	imshow("a", image3lvl);
	cv::waitKey(0);

	return optimal;
}






class OtsuLevel
{
	//int no;
	double weight;
	double mean;
	double variance;
	double p;
	//double sumP;
	//double sumW;
	//double sumV;
	//double maxBCV;
	//std::vector<double> optimals;
	//std::vector<double> iters;
	//void NextLevel(int pNo, int pi[256], double meanT, double sumP, double sumW, double sumV, double &maxBCV, std::vector<double> &optimals, std::vector<double> &iters);
public:
	OtsuLevel(double *pi, double &meanT, int cno, double csumP, double csumW, double csumV, double &cmaxBCV, std::vector<double> &coptimals, std::vector<double> &citers);
};

OtsuLevel::OtsuLevel(double *pi, double &meanT, int cno, double csumP, double csumW, double csumV, double &cmaxBCV, std::vector<double> &coptimals, std::vector<double> &citers)
{
	//no = cno;
	weight = 0;
	mean = 0;
	variance = 0;
	p = 0;
	//sumP = csumP;
	//sumW = csumW;
	//sumV = csumV;
	//maxBCV = cmaxBCV;
	//optimals = coptimals;
	//iters = citers;

	int iForBeg = 0;
	if (citers.size() > 0)
		iForBeg = citers.back() + 1;
	for (int i = iForBeg; i < 256; i++)
	{
		citers.push_back(i);
		p += pi[i];
		weight += pi[i] * i;
		if (p != 0)
			mean = weight / p;
		variance = p * pow(mean - meanT, 2);

		if (cno > 1)
			OtsuLevel next(pi, meanT, cno-1, csumP+p, csumW+weight, csumV+variance, cmaxBCV, coptimals, citers);
		else
		{
			double lastP = 1 - (csumP+p);
			double lastW = meanT - (csumW + weight);
			double lastM = lastW / lastP;
			double lastV = lastP * pow(lastM - meanT, 2);

			double betweenClassVariance = csumV + variance + lastV;
			if (betweenClassVariance > cmaxBCV)
			{
				cmaxBCV = betweenClassVariance;
				coptimals.clear();
				for (int o = 0; o < citers.size(); o++)
				{
					coptimals.push_back(citers[o]);
				}
			}
		}
		citers.pop_back();
	}
}

//void OtsuLevel::NextLevel(int pNo, int pi[256], double meanT, double sumP, double sumW, double sumV, double &maxBCV, std::vector<double> &optimals, std::vector<double> &iters)
//{
//	OtsuLevel ol(pi,meanT,pNo,sumP,sumW,sumV,maxBCV,optimals,iters);
//	for (int i = iters.back + 1; i < 256; i++)
//	{
//		iters.push_back(i);
//		ol.p += pi[i];
//		ol.weight += pi[i] * i;
//		if (ol.p != 0)
//			ol.mean = ol.weight / ol.p;
//		ol.variance = ol.p * pow(ol.mean - meanT, 2);
//
//		if (pNo>0)
//			ol.NextLevel(pNo - 1, pi, meanT,sumP+p,sumW+weight,sumV+variance,maxBCV,optimals,iters);
//		else
//		{
//			double lastP = 1 - sumP;
//			double lastW = meanT - sumW;
//			double lastM = lastW / lastP;
//			double lastV = lastP * pow(lastM - meanT, 2);
//
//			double betweenClassVariance = sumV + lastV;
//			if (betweenClassVariance > maxBCV)
//			{
//				maxBCV = betweenClassVariance;
//				for (int o = 0; o < optimals.size(); o++)
//				{
//					optimals[o] = iters[o];
//				}				
//			}
//		}
//		iters.pop_back();
//	}
//}

std::vector<double> OtsuN(cv::Mat image, int nOfLevels) {
	const int histLvls = 256;
	if (image.channels() > 1)
		cvtColor(image, image, cv::COLOR_RGB2GRAY);

	int histogram[histLvls] = { 0 };
	int nOfPixels = image.rows * image.cols;
	for (int iR = 0; iR < image.rows; iR++)
	{
		for (int iC = 0; iC < image.cols; iC++)
		{
			uchar value = image.at<uchar>(iR, iC);
			histogram[image.at<uchar>(iR, iC)] += 1;
		}
	}

	double p[histLvls] = { 0 };
	double meanT = 0;
	for (int i = 0; i < histLvls; i++)
	{
		p[i] = (double)histogram[i] / nOfPixels;
		meanT += i * p[i];
	}

	double betweenClassVariance = 0;
	double maxBetweenClassVariance = 0;
	std::vector<double> optimals;
	std::vector<double> iters;

	OtsuLevel start(p, meanT, nOfLevels, 0, 0, 0, maxBetweenClassVariance, optimals, iters);
	///////////////////
	

	cv::Mat image3lvl = cv::Mat::zeros(image.size(), CV_8UC3);
	for (int iR = 0; iR < image.rows; iR++)
	{
		for (int iC = 0; iC < image.cols; iC++)
		{
			int pixel = image.at<uchar>(iR, iC);
			for (int iL = 0; iL < nOfLevels; iL++)
			{
				if (pixel < optimals[iL])
				{
					image3lvl.at<cv::Vec3b>(iR, iC) = cv::Vec3b(50*iL, 250-(50*iL), 0);
					break;
				}
			}
		}
	}

	imshow("a", image3lvl);
	cv::waitKey(0);

	return optimals;
}
