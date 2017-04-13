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

#define UIPF_MODULE_ID "uipfsfm.keypoint.fast"
#define UIPF_MODULE_NAME "OpenCV FAST Keypoint Detector."
#define UIPF_MODULE_CATEGORY "sfm.keypoint"
#define UIPF_MODULE_CLASS SfMOpenCVFAST

#define UIPF_MODULE_INPUTS \
		{"image", uipf::DataDescription(uipfsfm::data::Image::id(), "the image.")}

#define UIPF_MODULE_OUTPUTS \
		{"image", uipf::DataDescription(uipfsfm::data::Image::id(), "the image with added keypoints.")}

#define UIPF_MODULE_PARAMS \
		{"threshold", uipf::ParamDescription("See OpenCV docs for details.", false) }, \
		{"nonmaxSuppression", uipf::ParamDescription("See OpenCV docs for details. Defaults to true.", true) }

#include <uipf/Module.hpp>


/**
 * OpenCV FAST Keypoint Detector and Descriptor
 *
 *
 * http://docs.opencv.org/2.4.13.2/modules/features2d/doc/feature_detection_and_description.html#fast
 */
void SfMOpenCVFAST::run() {

	using namespace uipf;
	using namespace uipfsfm::data;


	Image::ptr image = getInputData<Image>("image");



	cv::Mat m = cv::imread(image->getContent(), CV_LOAD_IMAGE_GRAYSCALE);

	image->keypoints = KeyPointList::ptr(new KeyPointList(std::vector<cv::KeyPoint>()));
	image->hasKeyPoints = true;

	// initialize FAST detector class
	int threshold = getParam<int>("threshold", 1);
	bool nonmaxSuppression = getParam<bool>("nonmaxSuppression", true); // default values from OpenCV docs.
	cv::FAST(m, image->keypoints->getContent(), threshold, nonmaxSuppression);

	setOutputData<Image>("image", image);
}
