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

#include "ImagePair.hpp"

using namespace uipfsfm::data;


std::string ImagePair::getName() const {
	std::string nameA = getContent().first->getName();
	std::string nameB = getContent().second->getName();

	if (nameA.empty()) {
		return nameB;
	} else if(nameB.empty()) {
		return nameA;
	} else {
		return nameA + "-" + nameB;
	}
}

std::vector<std::string> ImagePair::visualizations() const {
	std::vector<std::string> v;
	v.push_back("image pair");
	if (hasKeyPointMatches) {
		v.push_back("point matches");
	}
	if (hasF) {
		v.push_back("epipolar lines");
	}
	return v;
}

void ImagePair::visualize(std::string option, uipf::VisualizationContext &context) const {
	if (option.compare("image pair") == 0 || option.compare("point matches") == 0 || option.compare("epipolar lines") == 0) {

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

		if (option.compare("point matches") == 0 && hasKeyPointMatches) {

			std::vector<cv::DMatch> matches;
			uipf_cforeach(match, keyPointMatches) {
				matches.push_back(cv::DMatch(match->first, match->second, 0));
			}

			cv::Mat viz;
			cv::drawMatches(
					mA, imageA->keypoints->getContent(),
					mB, imageB->keypoints->getContent(),
					matches, viz, cv::Scalar::all(-1), cv::Scalar::all(-1),
					std::vector<char>(), cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS
			);

			context.displayImage(viz);
		} else {
			// image must be the same type otherwise we can not be sure to display them
			// TODO maybe add some conversion here? unlikely to happen though
			UIPF_ASSERT(mA.type() == mB.type());

			cv::Mat viz(std::max(mA.rows, mB.rows), mA.cols + mB.cols, mA.type());
			mA.copyTo(viz(cv::Rect(0, 0, mA.cols, mA.rows)));
			mB.copyTo(viz(cv::Rect(mA.cols, 0, mB.cols, mB.rows)));


			if (option.compare("epipolar lines") == 0 && hasF) {

				double data[] = {
						F(0,0), F(0,1), F(0,2),
						F(1,0), F(1,1), F(1,2),
						F(2,0), F(2,1), F(2,2)
				};
				cv::Mat mF(3, 3, CV_64F, data);

				std::vector<cv::Point2f> kpointsA = imageA->keypoints->getSimplePoints();
				std::vector<cv::Point2f> kpointsB = imageB->keypoints->getSimplePoints();
				std::vector<cv::Point2f> pointsA;
				std::vector<cv::Point2f> pointsB;

				for(std::pair<int,int> match: keyPointMatches) {
					pointsA.push_back(kpointsA[match.first]);
					pointsB.push_back(kpointsB[match.second]);
				}

				cv::RNG rng;

				// first image
				cv::Mat linesA;
				cv::computeCorrespondEpilines(pointsB, 2, mF, linesA);
				std::vector<cv::Scalar> colors((unsigned) linesA.rows);
				for(int i = 0; i < std::min(linesA.rows, 50); ++i) {
					colors[i] = cv::Scalar(rng.uniform(0,255), rng.uniform(0, 255), rng.uniform(0, 255));
					UIPF_LOG_DEBUG("line: ", linesA.at<float>(i, 0), linesA.at<float>(i, 1), linesA.at<float>(i, 2));
					cv::Point2f a(0, -linesA.at<float>(i, 2) / linesA.at<float>(i, 1));
					cv::Point2f b(mA.cols, -(linesA.at<float>(i, 2) + linesA.at<float>(i, 0) * (float)mA.cols) / linesA.at<float>(i, 1));
					UIPF_LOG_DEBUG("draw at: ", a, " , ", b);
					cv::line(viz, a, b, colors[i], 2);
					cv::Point2f pb = pointsB[i];
					pb.x = pb.x + mA.cols;
					cv::circle(viz, pb, 6, colors[i], 6);
				}
				// sceond image
				cv::Mat linesB;
				cv::computeCorrespondEpilines(pointsA, 1, mF, linesB);
				for(int i = 0; i < std::min(linesB.rows, 50); ++i) {
					UIPF_LOG_DEBUG("line: ", linesB.at<float>(i, 0), linesB.at<float>(i, 1), linesB.at<float>(i, 2));
					cv::Point2f a(mA.cols, -linesB.at<float>(i, 2) / linesB.at<float>(i, 1));
					cv::Point2f b(mA.cols + mB.cols, -(linesB.at<float>(i, 2) + linesB.at<float>(i, 0) * (float)mB.cols) / linesB.at<float>(i, 1));
					UIPF_LOG_DEBUG("draw at: ", a, " , ", b);
					cv::line(viz, a, b, colors[i], 2);
					cv::circle(viz, pointsA[i], 6, colors[i], 6);
				}

//				for r,pt1,pt2 in zip(lines,pts1,pts2):
//				color = tuple(np.random.randint(0,255,3).tolist())
//				x0,y0 = map(int, [0, -r[2]/r[1] ])
//				x1,y1 = map(int, [c, -(r[2]+r[0]*c)/r[1] ])
//				img1 = cv2.line(img1, (x0,y0), (x1,y1), color,1)
//				img1 = cv2.circle(img1,tuple(pt1),5,color,-1)
//				img2 = cv2.circle(img2,tuple(pt2),5,color,-1)
//				return img1,img2

			}

			context.displayImage(viz);
		}

		// TODO optional cv::drawKeypoints(img, points->getContent(), img, cv::Scalar(0,0,255), cv::DrawMatchesFlags::DRAW_OVER_OUTIMG + cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);

	}

}
