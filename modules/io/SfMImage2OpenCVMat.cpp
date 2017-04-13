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

#include <iostream>
#include <uipf/logging.hpp>
#include <uipf/data/opencv.hpp>

#include "data/Image.hpp"

#define UIPF_MODULE_NAME "SfMImage to OpenCVMat"
#define UIPF_MODULE_ID "uipfsfm.io.sfm_image2opencv_mat"
#define UIPF_MODULE_CLASS SfMImage2OpenCVMat
#define UIPF_MODULE_CATEGORY "sfm.io"

#define UIPF_MODULE_INPUTS \
		{"image", uipf::DataDescription(uipf::data::OpenCVMat::id(), "the input image.")}

#define UIPF_MODULE_OUTPUTS \
		{"image", uipf::DataDescription(uipfsfm::data::Image::id(), "the image with annotated keypoints.")}

#define UIPF_MODULE_PARAMS \
		{"mode", uipf::ParamDescription("can be either 'color' or 'grayscale' for whether to load the image in color or grayscale mode. Defaults to 'color'.", true) }

#include <uipf/Module.hpp>

using namespace uipf;
using namespace uipf::data;
using namespace uipfsfm::data;

void SfMImage2OpenCVMat::run() {

	Image::ptr image = getInputData<Image>("image");

	OpenCVMat::ptr mat;

	// check whether to load the image in grayscale mode, defaults to color
	if (getParam<std::string>("mode", "color").compare("grayscale") == 0) {
		mat = uipf::data::load_image_greyscale(image->getContent());
	} else {
		mat = uipf::data::load_image_color(image->getContent());
	}
	mat->exif = image->exif;

	setOutputData<OpenCVMat>("image", mat);

}
