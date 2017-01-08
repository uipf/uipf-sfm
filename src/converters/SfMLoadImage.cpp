#include "data/Image.hpp"

#define UIPF_MODULE_ID "cebe.sfm.load_image"
#define UIPF_MODULE_NAME "SfM Load Image"
#define UIPF_MODULE_CATEGORY "sfm"
#define UIPF_MODULE_CLASS SfMLoadImage

#define UIPF_MODULE_OUTPUTS \
		{"image", uipf::DataDescription(uipfsfm::data::Image::id(), "the loaded image.")}

#define UIPF_MODULE_PARAMS \
		{"filename", uipf::ParamDescription("file name of the file to load from.") }

#include <uipf/Module.hpp>

void SfMLoadImage::run() {

	using namespace uipfsfm::data;

	std::string filename = getParam<std::string>("filename","");
	setOutputData<Image>("image", new Image(filename));
}
