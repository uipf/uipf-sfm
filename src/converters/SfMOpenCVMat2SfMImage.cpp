#include <iostream>
#include <uipf/logging.hpp>
#include <uipf/data/opencv.hpp>

#include "data/Image.hpp"

// TODO rename this file

#define UIPF_MODULE_NAME "OpenCVMat2SfMImage"
#define UIPF_MODULE_ID "cebe.sfm.converters.OpenCVMat2SfMImage"
#define UIPF_MODULE_CLASS SfMOpenCVMat2SfMImage
#define UIPF_MODULE_CATEGORY "sfm.converters"

#define UIPF_MODULE_INPUTS \
		{"image", uipf::DataDescription(uipf::data::OpenCVMat::id(), "the input image.")}

#define UIPF_MODULE_OUTPUTS \
		{"image", uipf::DataDescription(uipfsfm::data::Image::id(), "the image with annotated keypoints.")}

#define UIPF_MODULE_PARAMS \
		{"focalLength", uipf::ParamDescription("optional annotate the image with focal length, if known.", true) }


#include <uipf/Module.hpp>

using namespace uipf;
using namespace uipf::data;
using namespace uipfsfm::data;

void SfMOpenCVMat2SfMImage::run() {

	float focalLength = getParam<float>("focalLength", -1);

	OpenCVMat::ptr image = getInputData<OpenCVMat>("image");
	Image::ptr sfmImage(new Image(image->filename));
	sfmImage->camera.f = focalLength;

	sfmImage->height = image->getContent().rows;
	sfmImage->width = image->getContent().cols;
	sfmImage->loadExif();

	setOutputData<Image>("image", sfmImage);

}
