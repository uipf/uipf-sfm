#include "data/ImageGraph.hpp"

#define UIPF_MODULE_NAME "Save Image Graph to NVM file"
#define UIPF_MODULE_ID "cebe.sfm.save_image_graph_nvm"
#define UIPF_MODULE_CLASS SaveNVMImageGraphModule
#define UIPF_MODULE_CATEGORY "sfm.io"

#define UIPF_MODULE_INPUTS \
		{"imageGraph", uipf::DataDescription(uipfsfm::data::ImageGraph::id(), "the image graph.")}

#define UIPF_MODULE_PARAMS \
		{"filename", uipf::ParamDescription("NVM file name.", false)}

#include <uipf/Module.hpp>

using namespace uipf;
using namespace uipf::data;
using namespace uipfsfm::data;


void SaveNVMImageGraphModule::run() {

	using namespace std;

	ImageGraph::ptr imageGraph = getInputData<ImageGraph>("imageGraph");

	string filename = getParam<string>("filename", "");

	ofstream s(filename);
	imageGraph->serialize(s);
	s.close();
}
