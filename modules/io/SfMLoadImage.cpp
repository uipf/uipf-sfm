#include "data/Image.hpp"

#define UIPF_MODULE_ID "uipfsfm.io.load_image"
#define UIPF_MODULE_NAME "SfM Load Image"
#define UIPF_MODULE_CATEGORY "sfm.io"
#define UIPF_MODULE_CLASS SfMLoadImage

#define UIPF_MODULE_OUTPUTS \
		{"image", uipf::DataDescription(uipfsfm::data::Image::id(), "the loaded image.")}

#define UIPF_MODULE_PARAMS \
		{"filename", uipf::ParamDescription("file name of the file to load from.") }, \
		{"focalLength", uipf::ParamDescription("optional annotate the image with focal length, if known.", true) }

#include <uipf/Module.hpp>

void SfMLoadImage::run() {

	using namespace uipfsfm::data;

	std::string filename = getParam<std::string>("filename", "");
	float focalLength = getParam<float>("focalLength", -1);

	Image::ptr image = Image::ptr(new Image(filename));
	image->camera.f = focalLength;

	cv::Mat m = cv::imread(filename);
	image->height = m.rows;
	image->width = m.cols;
	image->loadExif();

	setOutputData<Image>("image", image);
}
