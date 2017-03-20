#include "data/Image.hpp"

#include <opencv2/features2d/features2d.hpp>

#define UIPF_MODULE_ID "uipfsfm.keypoint.orb"
#define UIPF_MODULE_NAME "OpenCV ORB Keypoint Detector and Descriptor"
#define UIPF_MODULE_CATEGORY "sfm.keypoint"
#define UIPF_MODULE_CLASS SfMOpenCVORB

#define UIPF_MODULE_INPUTS \
		{"image", uipf::DataDescription(uipfsfm::data::Image::id(), "the image, optionally with keypoints detected.")}

#define UIPF_MODULE_OUTPUTS \
		{"image", uipf::DataDescription(uipfsfm::data::Image::id(), "the image with added keypoints and optionally descriptors.")}

#define UIPF_MODULE_PARAMS \
		{"detectOnly", uipf::ParamDescription("Do not compute Descriptors. This has no effect if image has already keypoints. Defaults to false.", true) }, \
		{"nfeatures", uipf::ParamDescription("See OpenCV docs for details. Defaults to 500.", true) }, \
		{"scaleFactor", uipf::ParamDescription("See OpenCV docs for details. Defaults to 1.2f.", true) }, \
		{"nlevels", uipf::ParamDescription("See OpenCV docs for details. Defaults to 8.", true) }, \
		{"edgeThreshold", uipf::ParamDescription("See OpenCV docs for details. Defaults to 31.", true) }, \
		{"firstLevel", uipf::ParamDescription("See OpenCV docs for details. Defaults to 0.", true) }, \
		{"WTA_K", uipf::ParamDescription("See OpenCV docs for details. Defaults to 2.", true) }, \
		{"scoreType", uipf::ParamDescription("HARRIS_SCORE or FAST_SCORE. See OpenCV docs for details. Defaults to HARRIS.", true) }, \
		{"patchSize", uipf::ParamDescription("See OpenCV docs for details. Defaults to 31.", true) }, \

#include <uipf/Module.hpp>


/**
 * OpenCV ORB Keypoint Detector and Descriptor
 *
 * If the image has keypoints, it computes the descriptors only.
 * If you only want to compute the detection, set the parameter.
 *
 *
 * http://docs.opencv.org/2.4.13.2/modules/features2d/doc/feature_detection_and_description.html#orb
 */
void SfMOpenCVORB::run() {

	using namespace uipf;
	using namespace uipfsfm::data;

	// read parameters
	int nfeatures     = getParam<int>("nfeatures", 500);
	float scaleFactor = getParam<float>("scaleFactor", 1.2f);
	int nlevels       = getParam<int>("nlevels", 8);
	int edgeThreshold = getParam<int>("scaleFactor", 31);
	int firstLevel = getParam<int>("scaleFactor", 0);
	int WTA_K = getParam<int>("scaleFactor", 2);
	int scoreType = 0;
	std::string scoreTypeStr = getParam<std::string>("scoreType", "");
	if (scoreTypeStr.empty() || scoreTypeStr.compare("HARRIS_SCORE") == 0) {
		scoreType = cv::ORB::HARRIS_SCORE;
	} else if (scoreTypeStr.compare("FAST_SCORE") == 0) {
		scoreType = cv::ORB::FAST_SCORE;
	} else {
		throw InvalidConfigException("scoreType must be one of HARRIS_SCORE or FAST_SCORE.");
	}
	int patchSize = getParam<int>("scaleFactor", 31);

	bool detectOnly = getParam<bool>("detectOnly", false);

	// read input image
	Image::ptr image = getInputData<Image>("image");
	cv::Mat m = cv::imread(image->getContent(), CV_LOAD_IMAGE_GRAYSCALE);


	cv::ORB orb(nfeatures, scaleFactor, nlevels, edgeThreshold, firstLevel, WTA_K, scoreType, patchSize);

	cv::Mat descriptors;

	if (image->hasKeyPoints) {
		orb(m, cv::Mat(), image->keypoints->getContent(), descriptors, true);
	} else {
		image->keypoints = KeyPointList::ptr(new KeyPointList(std::vector<cv::KeyPoint>()));

		if (detectOnly) {
			orb(m, cv::Mat(), image->keypoints->getContent(), cv::noArray(), false);
		} else {
			orb(m, cv::Mat(), image->keypoints->getContent(), descriptors, false);
		}
	}

	if (!detectOnly || image->hasKeyPoints) {
		image->keypoints->descriptors.resize((unsigned) descriptors.rows);
		for(int j = 0; j < descriptors.rows; ++j) {
			cv::Mat *d = new cv::Mat(descriptors.row(j).clone());
			image->keypoints->descriptors[j] = d;
		}
	}

	image->hasKeyPoints = true;

	setOutputData<Image>("image", image);
}
