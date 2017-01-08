#include <iostream>
#include <uipf/logging.hpp>
#include <uipf/data/opencv.hpp>

#include "data/Image.hpp"

// TODO rename this file

#define UIPF_MODULE_NAME "OpenCVMat2SfMImage"
#define UIPF_MODULE_ID "cebe.sfm.converters.OpenCVMat2SfMImage"
#define UIPF_MODULE_CLASS OpenCVMat2SfMImage
#define UIPF_MODULE_CATEGORY "sfm.converters"

#define UIPF_MODULE_INPUTS \
		{"image", uipf::DataDescription(uipf::data::OpenCVMat::id(), "the input image.")}

#define UIPF_MODULE_OUTPUTS \
		{"image", uipf::DataDescription(uipfsfm::data::Image::id(), "the image with annotated keypoints.")}

#include <uipf/Module.hpp>

using namespace uipf;
using namespace uipf::data;
using namespace uipfsfm::data;

void OpenCVMat2SfMImage::run() {

	OpenCVMat::ptr image = getInputData<OpenCVMat>("image");
	Image::ptr sfmImage(new Image(image->filename));
	setOutputData<Image>("image", sfmImage);

}
