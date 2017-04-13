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
#include "data/ImageGraph.hpp"

#include <opencv2/calib3d/calib3d.hpp>

#define UIPF_MODULE_ID "uipfsfm.keypoint.filter_ransac"
#define UIPF_MODULE_NAME "OpenCV RANSAC Match Filter"
#define UIPF_MODULE_CATEGORY "sfm.filters"
#define UIPF_MODULE_CLASS SfMOpenCVRANSACFilter

#define UIPF_MODULE_INPUTS \
		{"imageGraph", uipf::DataDescription(uipfsfm::data::ImageGraph::id(), "the image graph of matches.")}

#define UIPF_MODULE_OUTPUTS \
		{"imageGraph", uipf::DataDescription(uipfsfm::data::ImageGraph::id(), "the image graph of matches.")}, \
		{"images", uipf::DataDescription(uipf::data::List::id(), "the images of the image graph.")}

#define UIPF_MODULE_PARAMS \
		{"maxEpiDistance", uipf::ParamDescription("The maximum distance from a point to an epipolar line in pixels, beyond which the point is considered an outlier. Defaults to 3.0", true) }, \
		{"confidence", uipf::ParamDescription("Desirable level of confidence (probability) that the estimated matrix is correct. Defaults to 0.99", true) }

#include <uipf/Module.hpp>


void SfMOpenCVRANSACFilter::run()
{
	using namespace uipf;
	using namespace uipfsfm::data;

	ImageGraph::ptr imageGraph = getInputData<ImageGraph>("imageGraph");

	double maxEpiDistance = getParam<double>("maxEpiDistance", 3.0d);
	double confidence = getParam<double>("confidence", 0.99d);

	auto pairs = imageGraph->getContent();
	int pairscount = (int) pairs.size();
	int i = 0;
	updateProgress(i, pairscount);
	std::vector<ImagePair::ptr> newPairs;
	for(ImagePair::ptr imagePair: pairs) {

		UIPF_ASSERT(imagePair->getContent().first->hasKeyPoints);
		UIPF_ASSERT(imagePair->getContent().second->hasKeyPoints);

		std::vector<cv::Point2f> allPointsA = imagePair->getContent().first->keypoints->getSimplePoints();
		std::vector<cv::Point2f> allPointsB = imagePair->getContent().second->keypoints->getSimplePoints();

		std::vector<cv::Point2f> pointsA;
		std::vector<cv::Point2f> pointsB;
		for(std::pair<int,int> match: imagePair->keyPointMatches) {
			pointsA.push_back(allPointsA[match.first]);
			pointsB.push_back(allPointsB[match.second]);
		}

		cv::Mat outliers;
		cv::Mat F = cv::findFundamentalMat(pointsA, pointsB, CV_FM_RANSAC, maxEpiDistance, confidence, outliers);
		int outliercount = 0;
		int o = 0;
		std::vector< std::pair<int,int> > newMatches;
		for(std::pair<int,int> match: imagePair->keyPointMatches) {
			if (outliers.at<uint8_t>(o++) > 0) {
				newMatches.push_back(match);
			} else {
				outliercount++;
			}
		}
		imagePair->keyPointMatches = newMatches;
		UIPF_LOG_INFO("Estimated F, removed ", outliercount, " outliers, ", F.type());
		UIPF_LOG_INFO(F.at<float>(0,0), " ", F.at<float>(0,1), " ", F.at<float>(0,2), "\n",
				F.at<float>(1,0), " ", F.at<float>(1,1), " ", F.at<float>(1,2), "\n",
				F.at<float>(2,0), " ", F.at<float>(2,1), " ", F.at<float>(2,2), "\n"
		);
		F.copyTo(imagePair->F);
		imagePair->hasF = true;

		// remove pair if count of matching points pairs < 8
		if (imagePair->keyPointMatches.size() >= 8) {

			// C++: void computeCorrespondEpilines(InputArray points, int whichImage, InputArray F, OutputArray lines)¶
			// remove if error to epipolar lines is too big

			newPairs.push_back(imagePair);
		} else {
			UIPF_LOG_INFO("Removing image pair with < 8 matches: ", imagePair->getName());
		}

		updateProgress(++i, pairscount);
	}

	imageGraph->setContent(newPairs);

	setOutputData<ImageGraph>("imageGraph", imageGraph);

}

