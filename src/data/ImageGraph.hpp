#ifndef UIPFSFM_DATA_SFMIMAGEGRAPH_HPP
#define UIPFSFM_DATA_SFMIMAGEGRAPH_HPP

#include <uipf/data.hpp>
#include <uipf/data/list.hpp>
#include "Image.hpp"
#include "ImagePair.hpp"

// TODO check if external 'C' is needed here

namespace uipfsfm {
	namespace data {

		UIPF_DATA_TYPE_BEGIN (ImageGraph, "cebe.sfm.data.image_graph", std::vector<ImagePair::ptr>) // TODO internal data type is not really needed

		public:
//			/**
//			 * All images indexed by ID
//			 */
//			std::map<int, Image::ptr> images;
//
//			/**
//			 * edges
//			 */
//			std::vector<std::pair<int, int>> edges;
//
//
//			KeyPointList::ptr keypoints;
			/**
			 *
			 */
			std::map<std::string, std::string> tags;
			std::map<std::string, Data::ptr> data;

		UIPF_DATA_TYPE_END
	}
}

#endif // UIPFSFM_DATA_SFMIMAGEGRAPH_HPP
