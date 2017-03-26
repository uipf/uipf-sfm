#include "data/PointCloud.hpp"

#include <fstream>

#define UIPF_MODULE_ID "uipfsfm.io.store_pointcloud"
#define UIPF_MODULE_NAME "SfM Store PointCloud"
#define UIPF_MODULE_CATEGORY "sfm.io"
#define UIPF_MODULE_CLASS SfMStorePointCloud

#define UIPF_MODULE_INPUTS \
		{"points", uipf::DataDescription(uipfsfm::data::PointCloud::id(), "the point cloud.")}

#define UIPF_MODULE_PARAMS \
		{"filename", uipf::ParamDescription("file name of the file to store to (PLY format).") }

#include <uipf/Module.hpp>

void SfMStorePointCloud::run() {

	using namespace uipfsfm::data;

	std::string filename = getParam<std::string>("filename", "");

	PointCloud::ptr points = getInputData<PointCloud>("points");

	// TODO check for error opening
	std::ofstream s(filename);
	points->serialize(s);
	s.close();
}
