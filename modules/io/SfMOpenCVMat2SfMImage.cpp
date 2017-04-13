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

// TODO rename this file

#define UIPF_MODULE_NAME "OpenCVMat to SfMImage"
#define UIPF_MODULE_ID "uipfsfm.io.opencv_mat2sfm_image"
#define UIPF_MODULE_CLASS SfMOpenCVMat2SfMImage
#define UIPF_MODULE_CATEGORY "sfm.io"

#define UIPF_MODULE_INPUTS \
		{"image", uipf::DataDescription(uipf::data::OpenCVMat::id(), "the input image.")}

#define UIPF_MODULE_OUTPUTS \
		{"image", uipf::DataDescription(uipfsfm::data::Image::id(), "the image with annotated keypoints.")}

#define UIPF_MODULE_PARAMS \
		{"focalLength", uipf::ParamDescription("optional annotate the image with focal length, if known.", true) }


#include <uipf/Module.hpp>

using namespace uipf;
using namespace uipf::data;
using namespace uipfsfm::data;

void SfMOpenCVMat2SfMImage::run() {

	float focalLength = getParam<float>("focalLength", -1);

	OpenCVMat::ptr image = getInputData<OpenCVMat>("image");
	Image::ptr sfmImage(new Image(image->filename));
	sfmImage->camera.f = focalLength;

	sfmImage->height = image->getContent().rows;
	sfmImage->width = image->getContent().cols;
	if (image->exif.empty()) {
		sfmImage->loadExif();
	} else {
		sfmImage->exif = image->exif;
		sfmImage->estimateFocalLengthPrior();
	}

	setOutputData<Image>("image", sfmImage);

}
