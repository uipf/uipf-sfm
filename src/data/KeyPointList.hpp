#ifndef SFM_MODULES_KEYPOINTLIST_HPP
#define SFM_MODULES_KEYPOINTLIST_HPP

#include <uipf/data.hpp>
#include <opencv2/opencv.hpp>

// TODO check if external 'C' is needed here

namespace uipfsfm {
	namespace data {

		UIPF_DATA_TYPE_BEGIN (KeyPointList, "cebe.sfm.data.keypointlist", std::vector<cv::KeyPoint>)
		UIPF_DATA_TYPE_END
	}
}


#endif //SFM_MODULES_KEYPOINTLIST_HPP
