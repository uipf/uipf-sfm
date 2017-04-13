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

#include "data/Image.hpp"

#define UIPF_MODULE_ID "uipfsfm.io.load_image"
#define UIPF_MODULE_NAME "SfM Load Image"
#define UIPF_MODULE_CATEGORY "sfm.io"
#define UIPF_MODULE_CLASS SfMLoadImage

#define UIPF_MODULE_OUTPUTS \
		{"image", uipf::DataDescription(uipfsfm::data::Image::id(), "the loaded image.")}

#define UIPF_MODULE_PARAMS \
		{"filename", uipf::ParamDescription("file name of the file to load from.") }, \
		{"focalLength", uipf::ParamDescription("optional annotate the image with focal length, if known.", true) }

#include <uipf/Module.hpp>

void SfMLoadImage::run() {

	using namespace uipfsfm::data;

	std::string filename = getParam<std::string>("filename", "");
	float focalLength = getParam<float>("focalLength", -1);

	Image::ptr image = Image::ptr(new Image(filename));
	image->camera.f = focalLength;

	cv::Mat m = cv::imread(filename);
	image->height = m.rows;
	image->width = m.cols;
	image->loadExif();

	setOutputData<Image>("image", image);
}
