#pragma once
#define LEVELS 256

class OtsuN
{
private:
	int nOfLevels;
	cv::Mat image;
	std::vector<cv::Mat> imageLevels;
	cv::Mat imageLeveled;
	double pi[LEVELS] = { 0 };
	double meanT = 0;
	double maxBCV = 0;
	std::vector<int> optimals;
	std::vector<int> currents;
	void NextLevel(int levelNo, double sumP, double sumW, double sumV);
public:
	OtsuN(cv::Mat image, int nOfLevels);
	~OtsuN();
	cv::Mat ReturnLeveledImage();
	cv::Mat ReturnLevelImage(int level);
	std::vector<cv::Mat> ReturnImageLevels();
	std::vector<int> ReturnThresholds();
};

