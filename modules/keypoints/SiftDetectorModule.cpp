#include <iostream>
#include <uipf/logging.hpp>
#include <uipf/data/opencv.hpp>

// uipf imports
#include "data/Image.hpp"

#define UIPF_MODULE_NAME "Sift Point Detector"
#define UIPF_MODULE_ID "cebe.sfm.points.sift"
#define UIPF_MODULE_CLASS SiftDetect
#define UIPF_MODULE_CATEGORY "sfm"

#define UIPF_MODULE_INPUTS \
		{"sfmimage", uipf::DataDescription(uipfsfm::data::Image::id(), "the input image.", true)}, \
		{"image", uipf::DataDescription(uipf::data::OpenCVMat::id(), "the input image.", true)}

#define UIPF_MODULE_OUTPUTS \
		{"image", uipf::DataDescription(uipfsfm::data::Image::id(), "the image with annotated keypoints.")}

#define UIPF_MODULE_PARAMS \
		{"use_gpu", uipf::ParamDescription("whether to use GPU for computation. Defaults to false.", true)}

#include <uipf/Module.hpp>

using namespace uipfsfm;
using namespace uipfsfm::data;


void SiftDetect::run() {

	std::cout << "sift" << std::endl;

	Image::ptr image = getInputData<Image>("image");

	// TODO implement openCV sift
}
