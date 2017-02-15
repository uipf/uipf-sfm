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
			virtual bool isList() { return true; };
			virtual std::vector<uipf::Data::ptr> getListContent() {
				std::vector<uipf::Data::ptr> list;
				for(ImagePair::ptr ip: data_) {
					list.push_back(std::static_pointer_cast<uipf::Data>(ip));
				}
				return list;
			};


			/**
			 * All images indexed by ID to define an ordering
			 */
			std::map<int, Image::ptr> images;
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
			std::map<std::string, uipf::Data::ptr> data;

		UIPF_DATA_TYPE_END
	}
}

#endif // UIPFSFM_DATA_SFMIMAGEGRAPH_HPP
