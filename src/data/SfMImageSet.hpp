#ifndef UIPFSFM_DATA_SFMIMAGESET_HPP
#define UIPFSFM_DATA_SFMIMAGESET_HPP

#include <uipf/data.hpp>
#include "Image.hpp"

// TODO check if external 'C' is needed here

namespace uipfsfm {
	namespace data {

		class ImageSet {
		public:

			ImageSet(std::string path) : path_(path) {

				// TODO read all image files, put them into list

			};

			std::string getPath() { return path_; };
		private:
			std::string path_;
			std::vector< SfMImage::ptr > files_;
		};


		UIPF_DATA_TYPE_BEGIN (SfMImageSet, "cebe.sfm.data.imageset", Image)
		UIPF_DATA_TYPE_END
	}
}

#endif // UIPFSFM_DATA_SFMIMAGE_HPP
