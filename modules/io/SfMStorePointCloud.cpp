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
