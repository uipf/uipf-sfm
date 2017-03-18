#include "data/ImageGraph.hpp"

#include <fstream>

#define UIPF_MODULE_NAME "Read Image Graph from NVM file"
#define UIPF_MODULE_ID "cebe.sfm.read_image_graph_nvm"
#define UIPF_MODULE_CLASS ReadNVMImageGraphModule
#define UIPF_MODULE_CATEGORY "sfm.io"

#define UIPF_MODULE_OUTPUTS \
		{"imageGraph", uipf::DataDescription(uipfsfm::data::ImageGraph::id(), "the image graph.")}, \
		{"images", uipf::DataDescription(uipf::data::List::id(/*of images*/), "all images.")}, \
		{"pointcloud", uipf::DataDescription(uipfsfm::data::PointCloud::id(), "3D point cloud.")}

#define UIPF_MODULE_PARAMS \
		{"filename", uipf::ParamDescription("NVM file name.", false)}

#include <uipf/Module.hpp>

using namespace uipf;
using namespace uipf::data;
using namespace uipfsfm::data;


void ReadNVMImageGraphModule::run() {

	using namespace std;

	string filename = getParam<string>("filename", "");

	ifstream s(filename);
	ImageGraph::ptr imageGraph(new ImageGraph(s, filename));
	s.close();
	setOutputData<ImageGraph>("imageGraph", imageGraph);

	List::ptr images(new List);
	for(pair<int, Image::ptr> ip: imageGraph->images) {
		images->getContent().push_back(ip.second);
	}
	setOutputData<List>("images", images);
	setOutputData<PointCloud>("pointcloud", imageGraph->points3D);
}
