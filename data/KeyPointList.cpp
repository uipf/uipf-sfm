#include "KeyPointList.hpp"

std::vector<cv::Point2f> uipfsfm::data::KeyPointList::getSimplePoints() {
	std::vector<cv::Point2f> points;
	for(cv::KeyPoint kp: getContent()) {
		points.push_back(kp.pt);
	}
	return points;
}
