#include "data/Image.hpp"

#include <opencv2/features2d/features2d.hpp>
#include <boost/filesystem.hpp>

#define UIPF_MODULE_ID "uipfsfm.keypoint.surf"
#define UIPF_MODULE_NAME "OpenCV SURF Keypoint Detector and Descriptor"
#define UIPF_MODULE_CATEGORY "sfm.keypoint"
#define UIPF_MODULE_CLASS SfMOpenCVSURF

#define UIPF_MODULE_INPUTS \
		{"image", uipf::DataDescription(uipfsfm::data::Image::id(), "the image, optionally with keypoints detected.")}

#define UIPF_MODULE_OUTPUTS \
		{"image", uipf::DataDescription(uipfsfm::data::Image::id(), "the image with added keypoints and optionally descriptors.")}

#define UIPF_MODULE_PARAMS \
		{"detectOnly", uipf::ParamDescription("Do not compute Descriptors. This has no effect if image has already keypoints. Defaults to false.", true) }, \
		{"hessianThreshold", uipf::ParamDescription("See OpenCV docs for details.", false) }, \
		{"nOctaves", uipf::ParamDescription("See OpenCV docs for details. Defaults to 4.", true) }, \
		{"nOctaveLayers", uipf::ParamDescription("See OpenCV docs for details. Defaults to 2.", true) }, \
		{"extended", uipf::ParamDescription("See OpenCV docs for details. Defaults to true.", true) }, \
		{"upright", uipf::ParamDescription("See OpenCV docs for details. Defaults to false.", true) }, \
		{"cache", uipf::ParamDescription("Whether to cache calculated key points. Defaults to false.", true) }

#include <uipf/Module.hpp>

#include "nonfree/features2d.hpp"
#include "nonfree/nonfree_init.cpp"
#include "nonfree/sift.cpp"
#include "nonfree/surf.cpp"




/**
 * OpenCV SURF Keypoint Detector and Descriptor
 *
 * If the image has keypoints, it computes the descriptors only.
 * If you only want to compute the detection, set the parameter.
 *
 *
 * http://docs.opencv.org/2.4.13.2/modules/nonfree/doc/feature_detection.html#surf-surf
 */
void SfMOpenCVSURF::run() {

	using namespace uipf;
	using namespace uipfsfm::data;
	namespace fs = boost::filesystem;

	cv::initModule_nonfree();

	// read parameters
	double hessianThreshold = getParam<double>("hessianThreshold", 0.0);
	int nOctaves = getParam<int>("nOctaves", 4);
	int nOctaveLayers = getParam<int>("nOctaveLayers", 2);
	bool extended = getParam<bool>("extended", true);
	bool upright = getParam<bool>("upright", false);

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


	cv::SURF surf(hessianThreshold, nOctaves, nOctaveLayers, extended, upright);

	cv::Mat descriptors;

	if (image->hasKeyPoints) {
		surf(m, cv::Mat(), image->keypoints->getContent(), descriptors, true);
	} else {
		image->keypoints = KeyPointList::ptr(new KeyPointList(std::vector<cv::KeyPoint>()));

		if (detectOnly) {
			surf(m, cv::Mat(), image->keypoints->getContent(), cv::noArray(), false);
		} else {
			surf(m, cv::Mat(), image->keypoints->getContent(), descriptors, false);
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
