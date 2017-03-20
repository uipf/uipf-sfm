#include "data/Image.hpp"

#include <opencv2/features2d/features2d.hpp>
#include <boost/filesystem.hpp>

#define UIPF_MODULE_ID "uipfsfm.keypoint.sift"
#define UIPF_MODULE_NAME "OpenCV SIFT Keypoint Detector and Descriptor"
#define UIPF_MODULE_CATEGORY "sfm.keypoint"
#define UIPF_MODULE_CLASS SfMOpenCVSIFT

#define UIPF_MODULE_INPUTS \
		{"image", uipf::DataDescription(uipfsfm::data::Image::id(), "the image, optionally with keypoints detected.")}

#define UIPF_MODULE_OUTPUTS \
		{"image", uipf::DataDescription(uipfsfm::data::Image::id(), "the image with added keypoints and optionally descriptors.")}

#define UIPF_MODULE_PARAMS \
		{"detectOnly", uipf::ParamDescription("Do not compute Descriptors. This has no effect if image has already keypoints. Defaults to false.", true) }, \
		{"nfeatures", uipf::ParamDescription("See OpenCV docs for details. Defaults to 0.", true) }, \
		{"nOctaveLayers", uipf::ParamDescription("See OpenCV docs for details. Defaults to 3.", true) }, \
		{"contrastThreshold", uipf::ParamDescription("See OpenCV docs for details. Defaults to 0.04.", true) }, \
		{"edgeThreshold", uipf::ParamDescription("See OpenCV docs for details. Defaults to 10.", true) }, \
		{"sigma", uipf::ParamDescription("See OpenCV docs for details. Defaults to 1.6.", true) }, \
		{"cache", uipf::ParamDescription("Whether to cache calculated key points. Defaults to false.", true) }

#include <uipf/Module.hpp>

#include "nonfree/features2d.hpp"
#include "nonfree/nonfree_init.cpp"
#include "nonfree/sift.cpp"
#include "nonfree/surf.cpp"




/**
 * OpenCV SIFT Keypoint Detector and Descriptor
 *
 * If the image has keypoints, it computes the descriptors only.
 * If you only want to compute the detection, set the parameter.
 *
 *
 * http://docs.opencv.org/2.4.13.2/modules/nonfree/doc/feature_detection.html#sift-sift
 */
void SfMOpenCVSIFT::run() {

	using namespace uipf;
	using namespace uipfsfm::data;
	namespace fs = boost::filesystem;

	cv::initModule_nonfree();

	// read parameters
	int nfeatures     = getParam<int>("nfeatures", 0);
	int nOctaveLayers = getParam<int>("nOctaveLayers", 3);
	double contrastThreshold = getParam<double>("contrastThreshold", 0.04);
	double edgeThreshold = getParam<double>("edgeThreshold", 10);
	double sigma = getParam<double>("sigma", 1.6);

	bool detectOnly = getParam<bool>("detectOnly", false);

	Image::ptr image = getInputData<Image>("image");

	bool cache = getParam<bool>("cache", false);
	fs::path cachePath(image->getContent());
	cachePath.replace_extension(".key");
	if (cache && fs::exists(cachePath)) {
		std::ifstream cacheFile(cachePath.string());
		image->keypoints = KeyPointList::ptr(new KeyPointList(cacheFile));
		image->hasKeyPoints = true;
		setOutputData<Image>("image", image);
		return;
	}

	// read input image
	cv::Mat m = cv::imread(image->getContent(), CV_LOAD_IMAGE_GRAYSCALE);


	cv::SIFT sift(nfeatures, nOctaveLayers, contrastThreshold, edgeThreshold, sigma);

	cv::Mat descriptors;

	if (image->hasKeyPoints) {
		sift(m, cv::Mat(), image->keypoints->getContent(), descriptors, true);
	} else {
		image->keypoints = KeyPointList::ptr(new KeyPointList(std::vector<cv::KeyPoint>()));

		if (detectOnly) {
			sift(m, cv::Mat(), image->keypoints->getContent(), cv::noArray(), false);
		} else {
			sift(m, cv::Mat(), image->keypoints->getContent(), descriptors, false);
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

	if (cache) {
		std::ofstream cacheFile(cachePath.string());
		image->keypoints->serialize(cacheFile);
		cacheFile.close();
	}

	setOutputData<Image>("image", image);
}
