#include "data/Image.hpp"

#include <opencv2/features2d/features2d.hpp>

#define UIPF_MODULE_ID "uipfsfm.keypoint.fast"
#define UIPF_MODULE_NAME "OpenCV FAST Keypoint Detector."
#define UIPF_MODULE_CATEGORY "sfm.keypoint"
#define UIPF_MODULE_CLASS SfMOpenCVFAST

#define UIPF_MODULE_INPUTS \
		{"image", uipf::DataDescription(uipfsfm::data::Image::id(), "the image.")}

#define UIPF_MODULE_OUTPUTS \
		{"image", uipf::DataDescription(uipfsfm::data::Image::id(), "the image with added keypoints.")}

#define UIPF_MODULE_PARAMS \
		{"threshold", uipf::ParamDescription("See OpenCV docs for details.", false) }, \
		{"nonmaxSuppression", uipf::ParamDescription("See OpenCV docs for details. Defaults to true.", true) }

#include <uipf/Module.hpp>


/**
 * OpenCV FAST Keypoint Detector and Descriptor
 *
 *
 * http://docs.opencv.org/2.4.13.2/modules/features2d/doc/feature_detection_and_description.html#fast
 */
void SfMOpenCVFAST::run() {

	using namespace uipf;
	using namespace uipfsfm::data;


	Image::ptr image = getInputData<Image>("image");



	cv::Mat m = cv::imread(image->getContent(), CV_LOAD_IMAGE_GRAYSCALE);

	image->keypoints = KeyPointList::ptr(new KeyPointList(std::vector<cv::KeyPoint>()));
	image->hasKeyPoints = true;

	// initialize FAST detector class
	int threshold = getParam<int>("threshold", 1);
	bool nonmaxSuppression = getParam<bool>("nonmaxSuppression", true); // default values from OpenCV docs.
	cv::FAST(m, image->keypoints->getContent(), threshold, nonmaxSuppression);

	setOutputData<Image>("image", image);
}
