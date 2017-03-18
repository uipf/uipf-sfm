#include <iostream>
#include <uipf/logging.hpp>
#include <uipf/data/opencv.hpp>

#include "data/Image.hpp"

#define UIPF_MODULE_NAME "SfMImage2OpenCVMat"
#define UIPF_MODULE_ID "cebe.sfm.converters.SfMImage2OpenCVMat"
#define UIPF_MODULE_CLASS SfMImage2OpenCVMat
#define UIPF_MODULE_CATEGORY "sfm.converters"

#define UIPF_MODULE_INPUTS \
		{"image", uipf::DataDescription(uipf::data::OpenCVMat::id(), "the input image.")}

#define UIPF_MODULE_OUTPUTS \
		{"image", uipf::DataDescription(uipfsfm::data::Image::id(), "the image with annotated keypoints.")}

#define UIPF_MODULE_PARAMS \
		{"mode", uipf::ParamDescription("can be either 'color' or 'grayscale' for whether to load the image in color or grayscale mode. Defaults to 'color'.", true) }

#include <uipf/Module.hpp>

using namespace uipf;
using namespace uipf::data;
using namespace uipfsfm::data;

void SfMImage2OpenCVMat::run() {

	Image::ptr image = getInputData<Image>("image");

	OpenCVMat::ptr mat;

	// check whether to load the image in grayscale mode, defaults to color
	if (getParam<std::string>("mode", "color").compare("grayscale") == 0) {
		mat = uipf::data::load_image_greyscale(image->getContent());
	} else {
		mat = uipf::data::load_image_color(image->getContent());
	}
	mat->exif = image->exif;

	setOutputData<OpenCVMat>("image", mat);

}
