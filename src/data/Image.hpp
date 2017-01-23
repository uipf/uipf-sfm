#ifndef UIPFSFM_DATA_SFMIMAGE_HPP
#define UIPFSFM_DATA_SFMIMAGE_HPP

#include <uipf/data.hpp>
#include <uipf/data/list.hpp>
#include "KeyPointList.hpp"

#include <opencv2/opencv.hpp>

// TODO check if external 'C' is needed here

namespace uipfsfm {
	namespace data {

		UIPF_DATA_TYPE_BEGIN (Image, "cebe.sfm.data.image", std::string)

		public:

			float focalLength = -1;

			// TODO width height, focal length


			bool hasKeyPoints = false;

			KeyPointList::ptr keypoints;

			bool hasProjectionMatrix = false;
			/**
			 * Estimated 3x4 projection matrix for this image.
			 */
			cv::Matx34d P;


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
