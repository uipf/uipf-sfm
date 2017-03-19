#ifndef SFM_MODULES_FOERSTNER_HPP
#define SFM_MODULES_FOERSTNER_HPP

#include <iostream>
#include <opencv2/opencv.hpp>

class Foerstner {

public:
	// constructor
	Foerstner() { sigma = 0.5; };
	Foerstner(double s) : sigma(s) {};
	// destructor
	~Foerstner(void) {};

	// processing routines
	// start keypoint detection
	void run(const cv::Mat& in, std::vector<cv::KeyPoint>& points);

private:

	cv::Mat createFstDevKernel(double sigma);

	void getInterestPoints(const cv::Mat& img, double sigma, std::vector<cv::KeyPoint>& points);

	cv::Mat nonMaxSuppression(cv::Mat& img);

	void showImage(cv::Mat& img, const char* win, int wait, bool show, bool save);

	double sigma;
};

#endif //SFM_MODULES_FOERSTNER_HPP
