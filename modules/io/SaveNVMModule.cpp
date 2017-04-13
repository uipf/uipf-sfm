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

#define UIPF_MODULE_NAME "Save Image Graph to NVM file"
#define UIPF_MODULE_ID "uipfsfm.io.save_image_graph_nvm"
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
