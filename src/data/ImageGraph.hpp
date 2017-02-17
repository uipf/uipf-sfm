#ifndef UIPFSFM_DATA_SFMIMAGEGRAPH_HPP
#define UIPFSFM_DATA_SFMIMAGEGRAPH_HPP

#include <sstream>

#include <uipf/data.hpp>
#include <uipf/data/list.hpp>
#include "Image.hpp"
#include "ImagePair.hpp"

// TODO check if external 'C' is needed here

namespace uipfsfm {
	namespace data {

		UIPF_DATA_TYPE_BEGIN (ImageGraph, "cebe.sfm.data.image_graph", std::vector<ImagePair::ptr>) // TODO internal data type is not really needed

		public:
			virtual bool isList() const { return true; };
			virtual std::vector<uipf::Data::ptr> getListContent() const {
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


			/**
			 * @return a list of visualization options.
			 */
			virtual std::vector<std::string> visualizations() const {
				std::vector<std::string> v;
				// TODO do not offer this option if graphviz is not installed
				v.push_back("graph");
				for(auto img: images) {
					if (img.second->hasCameraParameters) {
						v.push_back("cameras 3D");
						break;
					}
				}
				return v;
			};

			virtual void visualize(std::string option, uipf::VisualizationContext& context) const {
				if (option.compare("graph") == 0) {
					// TODO choose temp path by uipf or directly pipe to commansd
					std::ofstream s("/tmp/uipfsfm-graph.dot");
					s << "strict graph {\n";
//					s << "  rankdir=\"LR\";\n";

					for(ImagePair::ptr pair: getContent()) {

						Image::ptr imageA = pair->getContent().first;
						Image::ptr imageB = pair->getContent().second;
						std::string fileA = boost::filesystem::path(imageA->getContent()).filename().string();
						std::string fileB = boost::filesystem::path(imageB->getContent()).filename().string();

						s << "\"" << fileA << "\" -- \"" << fileB << "\";";
					}
					s << "}\n";
					s.close();
//					uipf_exec_stdout("dot -Tpng /tmp/uipfsfm-graph.dot -o /tmp/uipfsfm-graph.png");
					uipf_exec_stdout("neato /tmp/uipfsfm-graph.dot | dot -Tpng -o /tmp/uipfsfm-graph.png");
					// TODO proper error handling, uipf::ErrorException
					uipf::data::OpenCVMat::ptr m = uipf::data::load_image_color("/tmp/uipfsfm-graph.png");
//					boost::filesystem::remove(boost::filesystem::path("/tmp/uipfsfm-graph.dot"));
					boost::filesystem::remove(boost::filesystem::path("/tmp/uipfsfm-graph.png"));
					context.displayImage(m->getContent());
					return;

				}
				if (option.compare("cameras 3D") == 0) {
					for(auto img: images) {
						if (img.second->hasCameraParameters) {
							img.second->visualize("camera 3D", context);
						}
					}
				}
			}

		UIPF_DATA_TYPE_END
	}
}

#endif // UIPFSFM_DATA_SFMIMAGEGRAPH_HPP
