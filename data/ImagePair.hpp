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

#ifndef UIPFSFM_DATA_SFMIMAGEPAIR_HPP
#define UIPFSFM_DATA_SFMIMAGEPAIR_HPP

#include <uipf/data.hpp>
#include <uipf/data/list.hpp>
#include "Image.hpp"

namespace uipfsfm {
	namespace data {

		#define IMAGE_PAIR_T std::pair<uipfsfm::data::Image::ptr, uipfsfm::data::Image::ptr> // avoid issue with macro arguments
		UIPF_DATA_TYPE_BEGIN (ImagePair, "cebe.sfm.data.image_pair", IMAGE_PAIR_T) // TODO internal data type is not really needed

		public:

			bool hasKeyPointMatches = false;
			std::vector< std::pair<int, int> > keyPointMatches;

			cv::Matx33d F;
			bool hasF = false;

			// TODO move this down to data?
			std::map<std::string, std::string> tags;
			std::map<std::string, uipf::Data::ptr> data;


			virtual std::string getName() const;

			/**
			 * @return a list of visualization options.
			 */
			virtual std::vector<std::string> visualizations() const;

			virtual void visualize(std::string option, uipf::VisualizationContext& context) const;

		UIPF_DATA_TYPE_END
	}
}

#endif // UIPFSFM_DATA_SFMIMAGEPAIR_HPP
