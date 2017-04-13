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

#ifndef UIPFSFM_DATA_SFMIMAGEGRAPH_HPP
#define UIPFSFM_DATA_SFMIMAGEGRAPH_HPP

#include <sstream>
#include <fstream>

#include <uipf/data.hpp>
#include <uipf/data/list.hpp>
#include "Image.hpp"
#include "ImagePair.hpp"
#include "PointCloud.hpp"

namespace uipfsfm {
	namespace data {

		UIPF_DATA_TYPE_BEGIN (ImageGraph, "cebe.sfm.data.image_graph", std::vector<ImagePair::ptr>) // TODO internal data type is not really needed

		public:
			/**
			 * All images indexed by ID to define an ordering
			 */
			std::map<int, Image::ptr> images;

			PointCloud::ptr points3D;

			class PointRef {
			public:
				unsigned long p3;
				unsigned long imageId;
				unsigned long keyPointId;
			};
			std::vector<PointRef> pointRef;

			virtual bool isList() const { return true; };
			virtual std::vector<uipf::Data::ptr> getListContent() const;

			/**
			 *
			 */
			std::map<std::string, std::string> tags;
			std::map<std::string, uipf::Data::ptr> data;


			/**
			 * @return a list of visualization options.
			 */
			virtual std::vector<std::string> visualizations() const;
			virtual void visualize(std::string option, uipf::VisualizationContext& context) const;

			virtual bool isSerializable() const { return true; };
			virtual void serialize(std::ostream&) const;
			// unserialize constructor
			ImageGraph(std::istream&, const std::string& filename = std::string());


		UIPF_DATA_TYPE_END
	}
}

#endif // UIPFSFM_DATA_SFMIMAGEGRAPH_HPP
