#include <uipf/data/list.hpp>
#include <uipf/data/opencv.hpp>

#include "data/Image.hpp"

#define UIPF_MODULE_ID "uipfsfm.io.load_images"
#define UIPF_MODULE_NAME "SfM Load Images"
#define UIPF_MODULE_CATEGORY "sfm.io"
#define UIPF_MODULE_CLASS SfMLoadImages

#define UIPF_MODULE_OUTPUTS \
		{"images", uipf::DataDescription(uipf::data::List::id(/*TODO typed list*/), "the loaded images.")}

#define UIPF_MODULE_PARAMS \
		{"path", uipf::ParamDescription("file name of the path to load from.") }, \
		{"focalLength", uipf::ParamDescription("optional annotate the image with focal length, if known.", true) }

#include <uipf/Module.hpp>
#include <uipf/logging.hpp>

void SfMLoadImages::run() {

	using namespace std;
	using namespace uipf::data;
	using namespace uipfsfm::data;

	List::ptr list(new List());

	string path = getParam<string>("path","");
	float focalLength = getParam<float>("focalLength", -1);

	vector<string> images = uipf::data::load_image_names(path);
	updateProgress(0, (int) images.size());
	int i = 0;
	for(string img: images) {
		UIPF_LOG_DEBUG("loading image: ", img);
		Image::ptr image(new Image(img));
		image->camera.f = focalLength;

		cv::Mat m = cv::imread(img);
		if (m.data) {
			image->height = m.rows;
			image->width = m.cols;
			image->loadExif();

			list->getContent().push_back(image);
		}
		updateProgress(++i, (int) images.size());
	}
	setOutputData<List>("images", list);
}