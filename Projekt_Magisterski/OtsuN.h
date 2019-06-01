#pragma once
#define LEVELS 256

class OtsuN
{
private:
	double pi[LEVELS] = { 0 };
	double meanT = 0;
	double maxBCV = 0;
	std::vector<int> optimals;
	std::vector<int> currents;
	void NextLevel(int levelNo, double sumP, double sumW, double sumV);
public:
	OtsuN(cv::Mat image, int nOfLevels);
	~OtsuN();
	void ShowLevels(cv::Mat image);
	std::vector<int> ReturnThresholds();
};

