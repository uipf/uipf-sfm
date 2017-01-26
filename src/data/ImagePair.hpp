#ifndef UIPFSFM_DATA_SFMIMAGEPAIR_HPP
#define UIPFSFM_DATA_SFMIMAGEPAIR_HPP

#include <uipf/data.hpp>
#include <uipf/data/list.hpp>
#include "Image.hpp"

// TODO check if external 'C' is needed here

namespace uipfsfm {
	namespace data {

		#define IMAGE_PAIR_T std::pair<uipfsfm::data::Image::ptr, uipfsfm::data::Image::ptr> // avoid issue with macro arguments
		UIPF_DATA_TYPE_BEGIN (ImagePair, "cebe.sfm.data.image_pair", IMAGE_PAIR_T) // TODO internal data type is not really needed

		public:

			bool hasKeyPointMatches = false;
			std::vector< std::pair<int, int> > keyPointMatches;

			// TODO move this down to data?
			std::map<std::string, std::string> tags;
			std::map<std::string, uipf::Data::ptr> data;

		UIPF_DATA_TYPE_END
	}
}

#endif // UIPFSFM_DATA_SFMIMAGEPAIR_HPP
