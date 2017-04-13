/**
 * UIPF SfM
 * Copyright (C) 2017 Carsten Brandt <mail@cebe.cc>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "data/ImageGraph.hpp"

#include <fstream>

#define UIPF_MODULE_NAME "Read Image Graph from NVM file"
#define UIPF_MODULE_ID "uipfsfm.io.read_image_graph_nvm"
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
