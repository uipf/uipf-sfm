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

#include <opencv2/features2d/features2d.hpp>

#define UIPF_MODULE_ID "uipfsfm.keypoint.freak"
#define UIPF_MODULE_NAME "OpenCV FREAK Keypoint Descriptor."
#define UIPF_MODULE_CATEGORY "sfm.keypoint"
#define UIPF_MODULE_CLASS SfMOpenCVFREAK

#define UIPF_MODULE_INPUTS \
		{"image", uipf::DataDescription(uipfsfm::data::Image::id(), "the image with detected keypoints.")}

#define UIPF_MODULE_OUTPUTS \
		{"image", uipf::DataDescription(uipfsfm::data::Image::id(), "the image with descriptors added for the keypoints.")}

#define UIPF_MODULE_PARAMS \
		{"patternScale", uipf::ParamDescription("See OpenCV docs for details. Defaults to 22.0", true) }, \
		{"nOctaves", uipf::ParamDescription("See OpenCV docs for details. Defaults to 4.", true) }

#include <uipf/Module.hpp>


/**
 * OpenCV FREAK Keypoint Detector and Descriptor
 *
 *
 * http://docs.opencv.org/2.4.13.2/modules/features2d/doc/feature_detection_and_description.html#freak
 */
void SfMOpenCVFREAK::run() {

	using namespace uipf;
	using namespace uipfsfm::data;


	Image::ptr image = getInputData<Image>("image");

	if (!image->hasKeyPoints) {
		throw ErrorException("Image has no keypoints attached.");
	}

	cv::Mat m = cv::imread(image->getContent(), CV_LOAD_IMAGE_GRAYSCALE);

	// initialize FREAK detector class, always normalize for scale and orientation,
	// as not doing so does not make sense in SfM approach
	float patternScale = getParam<float>("patternScale", 22.0f); // default values from OpenCV docs.
	int nOctaves = getParam<int>("nOctaves", 4); // default values from OpenCV docs.
	cv::FREAK freak(true, true, patternScale, nOctaves);

	// http://docs.opencv.org/2.4.13.2/modules/features2d/doc/common_interfaces_of_descriptor_extractors.html#descriptorextractor-compute
	cv::Mat descriptors;
	freak.compute(m, image->keypoints->getContent(), descriptors);
	image->hasKeyPoints = true;
	image->keypoints->descriptors.resize((unsigned) descriptors.rows);
	for(int j = 0; j < descriptors.rows; ++j) {
		cv::Mat *d = new cv::Mat(descriptors.row(j).clone());
		image->keypoints->descriptors[j] = d;
	}


	setOutputData<Image>("image", image);
}
