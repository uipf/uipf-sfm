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

#ifndef UIPFSFM_DATA_SFMIMAGE_HPP
#define UIPFSFM_DATA_SFMIMAGE_HPP

#include <uipf/data.hpp>
#include <uipf/data/list.hpp>
#include <uipf/data/opencv.hpp>
#include "KeyPointList.hpp"

#include <opencv2/opencv.hpp>
#include <sstream>

namespace uipfsfm {
	namespace data {

		UIPF_DATA_TYPE_BEGIN (Image, "cebe.sfm.data.image", std::string)

		public:

			int width = -1;
			int height = -1;

			// TODO width height, focal length

			// TODO exif

			bool hasKeyPoints = false;

			KeyPointList::ptr keypoints;

			bool hasProjectionMatrix = false;
			/**
			 * Estimated 3x4 projection matrix for this image.
			 */
			cv::Matx34d P;


			typedef struct {
				// external parameters
				// in  P = K[R T]
				cv::Matx33d R;   /* Rotation  R */
				cv::Vec3d t;     /* Translation  t = - RC  ;  C = -R⁻¹t */

				// internal parameters
				cv::Matx33d K;     /* Internal */

				double f = -1;            /* Focal length (in pixel) */
				double f_mm = -1;         /* Focal length (in mm) */
				double ccd_width_mm = -1; /* CCD width (in mm) */

				// viewing direction
				// defaults to negative z axis
				cv::Vec3d direction = {0, 0, -1};

			} CameraParameters;
			bool hasCameraParameters = false;
			/**
			 * Estimated 3x4 projection matrix for this image.
			 */
			CameraParameters camera;


			virtual std::string getName() const;

			/**
			 * @return a list of visualization options.
			 */
			virtual std::vector<std::string> visualizations() const;

			virtual void visualize(std::string option, uipf::VisualizationContext& context) const;


			virtual bool isSerializable() const { return true; };
			virtual void serialize(std::ostream& o) const;

			// unserialize constructor
			Image(std::istream&) {
				// TODO implement
				throw uipf::ErrorException("Data Type is not serializable.");
			};

			std::map<std::string, std::string> exif;
			void loadExif();
			void estimateFocalLengthPrior();


			// TODO PMVS supports image masks

			/**
			 *
			 */
			std::map<std::string, std::string> tags;
			std::map<std::string, uipf::Data::ptr> data;

		UIPF_DATA_TYPE_END
	}
}

#endif // UIPFSFM_DATA_SFMIMAGE_HPP
