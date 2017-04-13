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

#ifndef SFM_MODULES_KEYPOINTLIST_HPP
#define SFM_MODULES_KEYPOINTLIST_HPP

#include <uipf/data.hpp>
#include <uipf/exceptions.hpp>
#include <uipf/logging.hpp>
#include <uipf/util.hpp>
#include <opencv2/opencv.hpp>

#include <istream>
#include <ostream>

namespace uipfsfm {
	namespace data {

		UIPF_DATA_TYPE_BEGIN (KeyPointList, "cebe.sfm.data.keypointlist", std::vector<cv::KeyPoint>)

		public:
			// TODO free descriptor memory in destructor
			std::vector<cv::Mat*> descriptors;

			std::vector<cv::Point2f> getSimplePoints();

			void print(bool verbose = false);

			virtual bool isSerializable() const { return true; };

			// Reads a file in the format generated by loewe sift binary
			KeyPointList(std::istream& s);

			virtual void serialize(std::ostream& s) const;;


		UIPF_DATA_TYPE_END
	}
}


#endif //SFM_MODULES_KEYPOINTLIST_HPP
