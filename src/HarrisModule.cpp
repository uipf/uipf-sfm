#include <iostream>
#include <uipf/logging.hpp>
#include <uipf/data/opencv.hpp>

#include "data/Image.hpp"

#define UIPF_MODULE_NAME "Harris Point Detector"
#define UIPF_MODULE_ID "cebe.sfm.points.harris"
#define UIPF_MODULE_CLASS Harris
#define UIPF_MODULE_CATEGORY "sfm"

#define UIPF_MODULE_OUTPUTS \
/*		{"image", uipf::DataDescription(uipfsfm::data::SfMImage::id(), "the result image.")}*/ \
		{"image", uipf::DataDescription(uipf::data::OpenCVMat::id(), "the result image.")}

//#define UIPF_MODULE_PARAMS \
//		{"string", uipf::ParamDescription("a string.", true)}, \
//		{"int", uipf::ParamDescription("an int.", true)}


#include <uipf/Module.hpp>

using namespace uipfsfm;
using namespace uipfsfm::data;


void Harris::run() {

	std::cout << "harris" << std::endl;
/*
	Image i("hey ho");
	SfMImage* image = new SfMImage(i);

	setOutputData<SfMImage>("image", image);*/
	UIPF_LOG_INFO("created image: hey ho");

}
