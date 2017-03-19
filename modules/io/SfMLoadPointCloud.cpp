#include "data/PointCloud.hpp"

#include <fstream>

#define UIPF_MODULE_ID "uipfsfm.io.load_pointcloud"
#define UIPF_MODULE_NAME "SfM Load PointCloud"
#define UIPF_MODULE_CATEGORY "sfm.io"
#define UIPF_MODULE_CLASS SfMLoadPointCloud

#define UIPF_MODULE_OUTPUTS \
		{"points", uipf::DataDescription(uipfsfm::data::PointCloud::id(), "the loaded point cloud.")}

#define UIPF_MODULE_PARAMS \
		{"filename", uipf::ParamDescription("file name of the file to load from (PLY format).") }

#include <uipf/Module.hpp>

void SfMLoadPointCloud::run() {

	using namespace uipfsfm::data;

	std::string filename = getParam<std::string>("filename", "");

	std::ifstream s(filename);
	// TODO check for error opening
	PointCloud::ptr points = PointCloud::ptr(new PointCloud(s));

	setOutputData<PointCloud>("points", points);
}
