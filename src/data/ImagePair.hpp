#ifndef UIPFSFM_DATA_SFMIMAGEPAIR_HPP
#define UIPFSFM_DATA_SFMIMAGEPAIR_HPP

#include <uipf/data.hpp>
#include <uipf/data/list.hpp>
#include "Image.hpp"

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


			/**
			 * @return a list of visualization options.
			 */
			virtual std::vector<std::string> visualizations() {
				std::vector<std::string> v;
				v.push_back("image pair");
				if (hasKeyPointMatches) {
					v.push_back("point matches");
				}
				return v;
			};

			virtual void visualize(std::string option, uipf::VisualizationContext& context) {
				if (option.compare("image pair") == 0 || option.compare("point matches") == 0) {

					Image::ptr imageA = getContent().first;
					Image::ptr imageB = getContent().second;
					cv::Mat mA = cv::imread(imageA->getContent(), CV_LOAD_IMAGE_COLOR);
					if (!mA.data) {
						throw uipf::ErrorException(std::string("failed to load file: ") + imageA->getContent());
					}
					cv::Mat mB = cv::imread(imageB->getContent(), CV_LOAD_IMAGE_COLOR);
					if (!mB.data) {
						throw uipf::ErrorException(std::string("failed to load file: ") + imageB->getContent());
					}

					// image must be the same type otherwise we can not be sure to display them
					// TODO maybe add some conversion here? unlikely to happen though
					assert(mA.type() == mB.type());

					cv::Mat viz(std::max(mA.rows, mB.rows), mA.cols + mB.cols, mA.type());
					mA.copyTo(viz(cv::Rect(0, 0, mA.cols, mA.rows)));
					mB.copyTo(viz(cv::Rect(mA.cols, 0, mB.cols, mB.rows)));

					if (option.compare("point matches") == 0 && hasKeyPointMatches) {
						// draw matches
						uipf_cforeach(match, keyPointMatches) {

							cv::KeyPoint kA = imageA->keypoints->getContent()[match->first];
							cv::KeyPoint kB = imageB->keypoints->getContent()[match->second];
							cv::Point pB = kB.pt;
							pB.x += mA.cols;
							cv::line(viz, kA.pt, pB, cv::Scalar(0, 255, 0));

						}
					}

					// TODO optional cv::drawKeypoints(img, points->getContent(), img, cv::Scalar(0,0,255), cv::DrawMatchesFlags::DRAW_OVER_OUTIMG + cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);

					context.displayImage(viz);
				}

			};
		UIPF_DATA_TYPE_END
	}
}

#endif // UIPFSFM_DATA_SFMIMAGEPAIR_HPP
