#include <iostream>
#include <uipf/logging.hpp>
#include <uipf/data/opencv.hpp>

#include "data/KeyPointList.hpp"
#include "data/Image.hpp"

#define UIPF_MODULE_NAME "View Keypoints"
#define UIPF_MODULE_ID "cebe.sfm.points.view"
#define UIPF_MODULE_CLASS ViewKeyPointsModule
#define UIPF_MODULE_CATEGORY "sfm"

#define UIPF_MODULE_INPUTS \
		{"image", uipf::DataDescription(uipfsfm::data::Image::id(), "the input image annotated with keypoints.")}

#define UIPF_MODULE_PARAMS \
		{"store", uipf::ParamDescription("Whether to store the visualization instead of showing them. Defaults to show.", true)}


#include <uipf/Module.hpp>

using namespace uipf;
using namespace uipf::data;
using namespace uipf::util;
using namespace uipfsfm::data;

// Function displays image (after proper normalization)
/*
win   :  Window name
img   :  Image that shall be displayed
cut   :  whether to cut or scale values outside of [0,255] range
*/
void showImage(cv::Mat& img, bool show, bool save = false, const std::string& filename = std::string() ) {

	cv::Mat aux = img.clone();

	// scale and convert
	if (img.channels() == 1) {
		normalize(aux, aux, 0, 255, CV_MINMAX);
	}
	aux.convertTo(aux, CV_8UC1);
	// show
	if (show){
		cv::namedWindow("keypoints", CV_WINDOW_NORMAL);
		imshow("keypoints", aux);
		cv::waitKey(0);
	}
	// save
	if (save) {
		imwrite(filename.c_str(), aux);
	}
}


void ViewKeyPointsModule::run() {

	Image::ptr image = getInputData<Image>("image");
	assert(image->hasKeyPoints);
	KeyPointList::ptr points = image->keypoints;

	UIPF_LOG_INFO("Number of detected interest points: ", image->getContent(), " : ", points->getContent().size());

	// plot result
	cv::Mat img = cv::imread(image->getContent(), CV_LOAD_IMAGE_COLOR);
	if (!img.data) {
		throw ErrorException(std::string("failed to load file: ") + image->getContent());
	}
//	points->print(true);
	cv::drawKeypoints(img, points->getContent(), img, cv::Scalar(0,0,255), cv::DrawMatchesFlags::DRAW_OVER_OUTIMG + cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);

	if (getParam<bool>("store", false)) {
		showImage(img, false, true, rename_postfix(image->getContent(), "_keypoints"));
	} else {
		showImage(img, true);
	}

}

