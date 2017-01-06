#ifndef UIPFSFM_DATA_SFMIMAGE_HPP
#define UIPFSFM_DATA_SFMIMAGE_HPP

#include <uipf/data.hpp>
#include "KeyPointList.hpp"

// TODO check if external 'C' is needed here

namespace uipfsfm {
	namespace data {

		UIPF_DATA_TYPE_BEGIN (Image, "cebe.sfm.data.image", std::string)

		public:
			KeyPointList::ptr keypoints;

		UIPF_DATA_TYPE_END

	}
}

#endif // UIPFSFM_DATA_SFMIMAGE_HPP
