#ifndef SFM_MODULES_KEYPOINTLIST_HPP
#define SFM_MODULES_KEYPOINTLIST_HPP

#include <uipf/data.hpp>
#include <uipf/util.hpp>
#include <opencv2/opencv.hpp>

// TODO check if external 'C' is needed here

namespace uipfsfm {
	namespace data {

		UIPF_DATA_TYPE_BEGIN (KeyPointList, "cebe.sfm.data.keypointlist", std::vector<cv::KeyPoint>)

		public:
			std::vector<cv::Mat*> descriptors;

			void print(bool verbose = false) {

				const std::vector<cv::KeyPoint>& keyPoints = getContent();
				std::cout << "KeyPoints:" << std::endl;
				size_t i = 0;
				uipf_foreach(k, keyPoints) {
					std::cout << "  " << k->pt << ", s=" << k->size << ", r=" << k->angle;
					if (verbose) {
						std::cout << ", v=[" << std::endl;  ;
						for(int r = 0; r < descriptors[i]->rows; ++r) {
							for(int c = 0; c < descriptors[i]->cols; ++c) {
								std::cout << " " << ((int) descriptors[i]->at<uint8_t>(r, c));
							}
							std::cout << std::endl;
						}
						std::cout << "]";
						i++;
						if (i>10) break;
					}
					std::cout << std::endl;
				}
			}


		UIPF_DATA_TYPE_END
	}
}


#endif //SFM_MODULES_KEYPOINTLIST_HPP
