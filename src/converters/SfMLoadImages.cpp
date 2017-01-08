#include <uipf/data/list.hpp>
#include <uipf/data/opencv.hpp>

#include "data/Image.hpp"

#define UIPF_MODULE_ID "cebe.sfm.load_images"
#define UIPF_MODULE_NAME "SfM Load Images"
#define UIPF_MODULE_CATEGORY "sfm"
#define UIPF_MODULE_CLASS SfMLoadImages

#define UIPF_MODULE_OUTPUTS \
		{"images", uipf::DataDescription(uipf::data::List::id(/*TODO typed list*/), "the loaded images.")}

#define UIPF_MODULE_PARAMS \
		{"path", uipf::ParamDescription("file name of the path to load from.") }

#include <uipf/Module.hpp>

void SfMLoadImages::run() {

	using namespace std;
	using namespace uipf::data;
	using namespace uipfsfm::data;

	List::ptr list;

	string path = getParam<string>("path","");

	vector<string> images = uipf::data::load_image_names(path);
	for(string img: images) {
		list->getContent().push_back(Image::ptr(new Image(img)));
	}
	setOutputData<List>("images", list);
}
