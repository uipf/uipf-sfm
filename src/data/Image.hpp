#ifndef UIPFSFM_DATA_SFMIMAGE_HPP
#define UIPFSFM_DATA_SFMIMAGE_HPP

#include <uipf/data.hpp>
#include <uipf/data/list.hpp>
#include "KeyPointList.hpp"

// TODO check if external 'C' is needed here

namespace uipfsfm {
	namespace data {

		UIPF_DATA_TYPE_BEGIN (Image, "cebe.sfm.data.image", std::string)

		public:

			float focalLength = -1;


			KeyPointList::ptr keypoints;


			// TODO width height, focal length

			/**
			 *
			 */
			std::map<std::string, std::string> tags;
			std::map<std::string, uipf::Data::ptr> data;

		UIPF_DATA_TYPE_END
	}
}

#endif // UIPFSFM_DATA_SFMIMAGE_HPP
