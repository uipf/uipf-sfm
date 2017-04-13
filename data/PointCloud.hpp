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

#ifndef UIPFSFM_POINTCLOUD_HPP
#define UIPFSFM_POINTCLOUD_HPP

#include <uipf/data.hpp>
#include <opencv2/opencv.hpp>
#include <vector>

namespace uipfsfm {
	namespace data {

		UIPF_DATA_TYPE_BEGIN (PointCloud, "cebe.sfm.data.image", std::vector<cv::Point3d>)
		public:
			// color order: RGB
			std::vector<cv::Scalar> colors;
			// TODO implement
			std::vector<cv::Vec3d> normals;

			/*
			 * @return a list of visualization options.
			 */
			virtual std::vector<std::string> visualizations() const;

			virtual void visualize(std::string option, uipf::VisualizationContext& context) const;


			virtual bool isSerializable() const;

			// serialization, allow reading from files and writing to files
			// or other string transport
			virtual void serialize(std::ostream&, const std::string& color_prefix = std::string("diffuse_")) const;
			// unserialize constructor
			PointCloud(std::istream&);


		UIPF_DATA_TYPE_END
	}
}

#endif //UIPFSFM_POINTCLOUD_HPP
