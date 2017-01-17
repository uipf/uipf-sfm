#include <iostream>
#include <uipf/logging.hpp>
#include <uipf/data/opencv.hpp>
#include <algorithm>

#include "data/KeyPointList.hpp"
#include "data/Image.hpp"
#include "data/ImageGraph.hpp"

// TODO move this out of keypoint section WIP
#define UIPF_MODULE_NAME "View Image Graph"
#define UIPF_MODULE_ID "cebe.sfm.view_image_graph"
#define UIPF_MODULE_CLASS ViewImageGraphModule
#define UIPF_MODULE_CATEGORY "sfm"

#define UIPF_MODULE_INPUTS \
		{"imageGraph", uipf::DataDescription(uipfsfm::data::ImageGraph::id(), "the image graph to visualize.")}

#define UIPF_MODULE_PARAMS \
		{"store", uipf::ParamDescription("Whether to store the visualization instead of showing them. Defaults to show.", true)}


#include <uipf/Module.hpp>

using namespace uipf;
using namespace uipf::data;
using namespace uipf::util;
using namespace uipfsfm::data;


void showImage(cv::Mat& img, bool show, bool save = false, const std::string& filename = std::string() ) {

	cv::Mat aux = img.clone();

	// scale and convert
	if (img.channels() == 1) {
		normalize(aux, aux, 0, 255, CV_MINMAX);
	}
	aux.convertTo(aux, CV_8UC1);
	// show
	if (show){
		cv::namedWindow("keypoint_matches", CV_WINDOW_NORMAL);
		imshow("keypoint_matches", aux);
		cv::waitKey(0);
	}
	// save
	if (save) {
		imwrite(filename.c_str(), aux);
	}
}


void ViewImageGraphModule::run() {

	ImageGraph::ptr imageGraph = getInputData<ImageGraph>("imageGraph");

	uipf_foreach(imagePair, imageGraph->getContent()) {

		Image::ptr imageA = (*imagePair)->getContent().first;
		Image::ptr imageB = (*imagePair)->getContent().second;
		cv::Mat mA = cv::imread(imageA->getContent(), CV_LOAD_IMAGE_COLOR);
		if (!mA.data) {
			throw ErrorException(std::string("failed to load file: ") + imageA->getContent());
		}
		cv::Mat mB = cv::imread(imageB->getContent(), CV_LOAD_IMAGE_COLOR);
		if (!mB.data) {
			throw ErrorException(std::string("failed to load file: ") + imageB->getContent());
		}

		// image must be the same type otherwise we can not be sure to display them
		// TODO maybe add some conversion here? unlikely to happen though
		assert(mA.type() == mB.type());

		cv::Mat viz(std::max(mA.rows, mB.rows), mA.cols + mB.cols, mA.type());
		mA.copyTo(viz(cv::Rect(0, 0, mA.cols, mA.rows)));
		mB.copyTo(viz(cv::Rect(mA.cols, 0, mB.cols, mB.rows)));

		// draw matches
		uipf_cforeach(match, (*imagePair)->keyPointMatches) {

			cv::KeyPoint kA = imageA->keypoints->getContent()[match->first];
			cv::KeyPoint kB = imageB->keypoints->getContent()[match->second];
			cv::Point pB = kB.pt;
			pB.x += mA.cols;
			cv::line(viz, kA.pt, pB, cv::Scalar(0,255,0));

		}



		// TODO store viz
//		if (getParam<bool>("store", false)) {
//			showImage(img, false, true, rename_postfix(image->getContent(), "_keypoints"));
//		} else {
//			showImage(img, true);
//		}

		// TODO optional cv::drawKeypoints(img, points->getContent(), img, cv::Scalar(0,0,255), cv::DrawMatchesFlags::DRAW_OVER_OUTIMG + cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);


		showImage(viz, true);
	}


	// TODO graphviz output

}
