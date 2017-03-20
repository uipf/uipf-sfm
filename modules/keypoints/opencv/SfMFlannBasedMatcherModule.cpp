#include "data/Image.hpp"
#include "data/ImageGraph.hpp"

#include <opencv2/features2d/features2d.hpp>

#define UIPF_MODULE_ID "uipfsfm.keypoint.flann_matcher"
#define UIPF_MODULE_NAME "OpenCV FlannBased Keypoint Matcher"
#define UIPF_MODULE_CATEGORY "sfm.keypoint"
#define UIPF_MODULE_CLASS SfMOpenCVFlannBasedMatcher

#define UIPF_MODULE_INPUTS \
		{"images", uipf::DataDescription(uipf::data::List::id(), "the images with detected keypoints.")}

#define UIPF_MODULE_OUTPUTS \
		{"imageGraph", uipf::DataDescription(uipfsfm::data::ImageGraph::id(), "the image graph of matches.")}

#define UIPF_MODULE_PARAMS \
		{"minRatio", uipf::ParamDescription("minRatio for the Loewe ratio test. Defaults to 0.8.", true) }

#include <uipf/Module.hpp>


/**
 * OpenCV feature matcher based on
 *
 * http://docs.opencv.org/2.4/doc/tutorials/features2d/feature_flann_matcher/feature_flann_matcher.html
 */
void SfMOpenCVFlannBasedMatcher::run()
{
	using namespace uipf;
	using namespace uipf::data;
	using namespace uipfsfm::data;
	using namespace std;

	double minRatio = getParam<double>("minRatio", 0.8);

	List::ptr images = getInputData<List>("images");
	vector<Data::ptr> list = images->getContent();
	int icount = (int) list.size();
	int nummatches = icount * icount / 2 - icount / 2;
	int cmatches = 0;


	// prepare descriptor matrices
	vector<cv::Mat> descriptors;
	descriptors.resize(list.size());
	updateProgress(0, icount);
	for(int i = 0; i < icount; ++i) {
		Image::ptr image = static_pointer_cast<Image>(list[i]);
		UIPF_ASSERT(image->hasKeyPoints && image->keypoints->descriptors.size() > 0);
		int descrSize = image->keypoints->descriptors[0]->cols;
		int type = image->keypoints->descriptors[0]->type();

		descriptors[i] = cv::Mat((int)image->keypoints->descriptors.size(), descrSize, type);
		int k = 0;
		for(cv::Mat* dm: image->keypoints->descriptors) {
			UIPF_ASSERT(dm->type() == type && dm->rows == 1 && dm->cols == descrSize);
			dm->copyTo(descriptors[i].row(k++));
		}
		// convert to floating point to allow distance computation via L2 (eucledian distance)
		descriptors[i].convertTo(descriptors[i], CV_32F);

		updateProgress(i, icount);

		UIPF_LOG_DEBUG(image->getName());
		for(int di = 0; di < descriptors[i].rows && di < 12; ++di) {
			for(int dj = 0; dj < descriptors[i].cols; ++dj) {
				std::cout << ((float)descriptors[i].at<float>(di,dj)) << " ";
			}
			std::cout << std::endl;
		}

	}




	// setup ImageGraph for output
	ImageGraph::ptr imageGraph(new ImageGraph(std::vector<ImagePair::ptr>()));

	// do the matching
	updateProgress(cmatches, nummatches);
	for(int i = 0; i < icount; ++i) {
		Image::ptr imageA = static_pointer_cast<Image>(list[i]);
		imageGraph->images.insert(pair<int, Image::ptr>(i, imageA));

		for(int j = i + 1; j < icount; ++j) {
			Image::ptr imageB = static_pointer_cast<Image>(list[j]);

			std::vector< cv::DMatch > matches;

			bool useBruteforce = false;
			if (useBruteforce) {
				cv::BFMatcher matcher;
				matcher.match( descriptors[j], descriptors[i], matches );
			} else {
				// http://stackoverflow.com/questions/23634730/opencv-flann-matcher-crashes#23639463
				cv::Ptr<cv::flann::IndexParams> indexParams;
				cv::Ptr<cv::flann::SearchParams> searchParams = new cv::flann::SearchParams(50);
				switch (descriptors[i].type()) {
					default:
					case CV_32F:
						indexParams = new cv::flann::KDTreeIndexParams(5);
						UIPF_LOG_DEBUG("using KD Tree Index");
						break;
					case CV_8U:
						indexParams = new cv::flann::HierarchicalClusteringIndexParams();
						UIPF_LOG_DEBUG("using HierarchicalClustering Index");
						break;
				}
				// based on
				// http://docs.opencv.org/master/da/de9/tutorial_py_epipolar_geometry.html
				// http://docs.opencv.org/2.4/doc/tutorials/features2d/feature_flann_matcher/feature_flann_matcher.html

				cv::FlannBasedMatcher matcher(indexParams, searchParams);
//				matcher.match( descriptors[j], descriptors[i], matches );
				std::vector< std::vector< cv::DMatch > > knnmatches;

				matcher.knnMatch( descriptors[j], descriptors[i], knnmatches, 2 );
				// ratio test by Loewe
				for (std::vector< cv::DMatch > mm: knnmatches) {
					const cv::DMatch& bestMatch = mm[0];
					const cv::DMatch& betterMatch = mm[1];
					float distanceRatio = bestMatch.distance / betterMatch.distance;
					if (distanceRatio < minRatio) {
						matches.push_back(bestMatch);
					}
				}
			}


//			UIPF_LOG_INFO(imageA->getName(), " - ", imageB->getName(), ": matches: ", matches.size());

			// calculate min/max distance
			double dist_max = 0;
			double dist_min = INT_MAX;
			for(cv::DMatch match: matches) {
				double dist = match.distance;
				if( dist < dist_min ) dist_min = dist;
				if( dist > dist_max ) dist_max = dist;
			}
			UIPF_LOG_INFO(imageA->getName(), " - ", imageB->getName(), ": matches: ", matches.size(), " dist_min:", dist_min, " dist_max:", dist_max);

			// TODO only use "good" matches

			ImagePair::ptr imagePair(new ImagePair(pair<Image::ptr, Image::ptr>(imageA, imageB)));
			for(cv::DMatch match: matches) {
				// only keep matches that are within 10% of the overall range
				// TODO improve, max should not be relevant at all.
				if ( match.distance > dist_min + 0.35 * (dist_max - dist_min) ) { //|| match.distance > 0.5 * dist_max) {
					continue;
				}
				imagePair->keyPointMatches.push_back(pair<int, int>(match.trainIdx, match.queryIdx));
			}
			UIPF_LOG_INFO(imageA->getName(), " - ", imageB->getName(), ": good matches: ", imagePair->keyPointMatches.size());
			imagePair->hasKeyPointMatches = true;
			if (imagePair->keyPointMatches.size() >= 8) {
				imageGraph->getContent().push_back(imagePair);
				// set output data for early inspection
				setOutputData<ImageGraph>("imageGraph", imageGraph);
			}

			updateProgress(++cmatches, nummatches);
		}
	}

	/*

	//-- Draw only "good" matches (i.e. whose distance is less than 2*min_dist,
	//-- or a small arbitary value ( 0.02 ) in the event that min_dist is very
	//-- small)
	//-- PS.- radiusMatch can also be used here.
	std::vector< DMatch > good_matches;

	for( int i = 0; i < descriptors_1.rows; i++ )
	{ if( matches[i].distance <= max(2*min_dist, 0.02) )
		{ good_matches.push_back( matches[i]); }
	}

	//-- Draw only "good" matches
	Mat img_matches;
	drawMatches( img_1, keypoints_1, img_2, keypoints_2,
	             good_matches, img_matches, Scalar::all(-1), Scalar::all(-1),
	             vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS );

	//-- Show detected matches
	imshow( "Good Matches", img_matches );

	for( int i = 0; i < (int)good_matches.size(); i++ )
	{ printf( "-- Good Match [%d] Keypoint 1: %d  -- Keypoint 2: %d  \n", i, good_matches[i].queryIdx, good_matches[i].trainIdx ); }
*/

	setOutputData<ImageGraph>("imageGraph", imageGraph);
}
