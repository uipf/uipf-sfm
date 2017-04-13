/**
 * UIPF SfM
 * Copyright (C) 2017 Carsten Brandt <mail@cebe.cc>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "KeyPointList.hpp"

std::vector<cv::Point2f> uipfsfm::data::KeyPointList::getSimplePoints() {
	std::vector<cv::Point2f> points;
	for(cv::KeyPoint kp: getContent()) {
		points.push_back(kp.pt);
	}
	return points;
}

void uipfsfm::data::KeyPointList::serialize(std::ostream &s) const {

	UIPF_ASSERT(getContent().size() == descriptors.size());

//				The file format starts with 2 integers giving the total number of
//				keypoints and the length of the descriptor vector for each keypoint (128).
	s << getContent().size();
	s << " 128\n"; // TODO support other descriptor length

//				Then the location of each keypoint in the image is specified by
//				4 floating point numbers giving subpixel row and column location,
//				scale, and orientation (in radians from -PI to PI).  Obviously, these
//				numbers are not invariant to viewpoint, but can be used in later
//				stages of processing to check for geometric consistency among matches.
	int i = 0;
	for(cv::KeyPoint k: getContent()) {
		float row = k.pt.y;
		float col = k.pt.x;
		float scale = k.size;
		// convert rot value from range [0,360[ (openCV) to [-PI,PI[ (sift)
		float rot = (float) M_PI * (k.angle - 180) / 180;

		s << row << " " << col << " " << scale << " " << rot << "\n";

//					Finally, the invariant descriptor vector for the keypoint is given as
//			    	a list of 128 integers in range [0,255].
		cv::Mat* d = descriptors[i++];
		// TODO support storing float and doulbe matrixes
		UIPF_ASSERT(d->rows == 1 && d->cols == 128 && d->type() == CV_8U);
		for(int v = 0; v < 128; ++v) {
			s << " " << ((int) d->at<uint8_t>(0, v));
			if (v % 20 == 19) {
				s << "\n";
			}
		}
		s << "\n";
	}

}

uipfsfm::data::KeyPointList::KeyPointList(std::istream &s) {
	setContent(std::vector<cv::KeyPoint>());

	// std::stof() is locale aware, meaning params are not portable between platforms
	// the following is a locale independend stof():
	s.imbue(std::locale("C"));

	int point_count = 0;
	s >> point_count;
	int vector_length = 0;
	s >> vector_length;

	for(int i = point_count; i > 0; --i) {
		float row, col, scale, rot;
		s >> row;
		s >> col;
		s >> scale;
		s >> rot;
		// convert value from range [-PI,PI[ (sift) to [0,360[ (openCV)
		rot = (int)(rot * 180/M_PI) + 180;

		getContent().push_back(cv::KeyPoint(col, row, scale, rot));
		if (vector_length > 0) {
			cv::Mat *descriptor = new cv::Mat(1, vector_length, CV_8U);
			for (int v = 0; v < vector_length; ++v) {
				int no;
				s >> no;
				descriptor->at<uint8_t>(v) = (uint8_t) no;
			}
			descriptors.push_back(descriptor);
		}
	}
}

void uipfsfm::data::KeyPointList::print(bool verbose) {

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
