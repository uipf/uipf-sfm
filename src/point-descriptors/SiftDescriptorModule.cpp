#include <iostream>
#include <uipf/logging.hpp>
#include <uipf/data/opencv.hpp>

#include "data/Image.hpp"

#define UIPF_MODULE_NAME "Sift Descriptor"
#define UIPF_MODULE_ID "cebe.sfm.points.sift"
#define UIPF_MODULE_CLASS Sift
#define UIPF_MODULE_CATEGORY "sfm"

#define UIPF_MODULE_INPUTS \
/*		{"image", uipf::DataDescription(uipfsfm::data::SfMImage::id(), "the result image.")}*/ \
		{"image", uipf::DataDescription(uipf::data::OpenCVMat::id(), "the result image.")} \


//#define UIPF_MODULE_PARAMS \
//		{"string", uipf::ParamDescription("a string.", true)}, \
//		{"int", uipf::ParamDescription("an int.", true)}


#include <uipf/Module.hpp>

using namespace uipfsfm;
using namespace uipfsfm::data;


void Sift::run() {

	// TODO implement OpenCV Sift

	std::cout << "sift" << std::endl;

	Image::ptr image = getInputData<Image>("image");

	UIPF_LOG_WARNING("loaded image:");
//	UIPF_LOG_WARNING(image->getContent().getFilePath());

}
