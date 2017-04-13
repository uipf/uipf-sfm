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
#include <algorithm>
#include <boost/filesystem.hpp>

#include "data/KeyPointList.hpp"
#include "data/Image.hpp"
#include "data/ImageGraph.hpp"

// TODO move this out of keypoint section WIP
#define UIPF_MODULE_NAME "Filter Matches in Image Graph"
#define UIPF_MODULE_ID "uipfsfm.filter.filter_matches"
#define UIPF_MODULE_CLASS FilterImageGraphModule
#define UIPF_MODULE_CATEGORY "sfm.filters"

#define UIPF_MODULE_INPUTS \
		{"imageGraph", uipf::DataDescription(uipfsfm::data::ImageGraph::id(), "the image graph.")}

#define UIPF_MODULE_OUTPUTS \
		{"imageGraph", uipf::DataDescription(uipfsfm::data::ImageGraph::id(), "the image graph.")}

#define UIPF_MODULE_PARAMS \
		{"matchThreshold", uipf::ParamDescription("Remove image pairs that have less than this number of matching point paris.", true)}, \
		{"filterRegion", uipf::ParamDescription("Remove matches that have similar pixel value to skip artefacts added by the camera or lens.", true)}


#include <uipf/Module.hpp>

using namespace uipf;
using namespace uipf::data;
using namespace uipfsfm::data;


void FilterImageGraphModule::run() {

	using namespace std;

	ImageGraph::ptr imageGraph = getInputData<ImageGraph>("imageGraph");

	int filterRegion = getParam<int>("filterRegion", -1);
	if (filterRegion > 0) {
		vector<ImagePair::ptr>& v = imageGraph->getContent();
		for(auto it = v.begin(); it != v.end(); ++it) {

			Image::ptr imageA = (*it)->getContent().first;
			Image::ptr imageB = (*it)->getContent().second;
			for(auto matchIt = (*it)->keyPointMatches.begin(); matchIt != (*it)->keyPointMatches.end(); /* ++ is done below */) {

				cv::KeyPoint kA = imageA->keypoints->getContent()[matchIt->first];
				cv::KeyPoint kB = imageB->keypoints->getContent()[matchIt->second];

				if (abs(kA.pt.x - kB.pt.x) < filterRegion && abs(kA.pt.y - kB.pt.y) < filterRegion) {
					matchIt = (*it)->keyPointMatches.erase(matchIt);
				} else {
					matchIt++;
				}
			}

		}
	}

	int matchThreshold = getParam<int>("matchThreshold", -1);
	if (matchThreshold > 0) {
		vector<ImagePair::ptr>& v = imageGraph->getContent();
		for(auto it = v.begin(); it != v.end(); /* ++ is done below*/) {
			if (!(*it)->hasKeyPointMatches || (*it)->keyPointMatches.size() < (unsigned)matchThreshold) {
				UIPF_LOG_INFO("removed image pair ", (*it)->getContent().first->getContent(), (*it)->getContent().second->getContent());
				it = v.erase(it);
			} else {
				it++;
			}
		}
	}

	setOutputData<ImageGraph>("imageGraph", imageGraph);
}
