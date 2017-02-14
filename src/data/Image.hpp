#ifndef UIPFSFM_DATA_SFMIMAGE_HPP
#define UIPFSFM_DATA_SFMIMAGE_HPP

#include <uipf/data.hpp>
#include <uipf/data/list.hpp>
#include <uipf/data/opencv.hpp>
#include "KeyPointList.hpp"

#include <opencv2/opencv.hpp>

// TODO check if external 'C' is needed here

namespace uipfsfm {
	namespace data {

		UIPF_DATA_TYPE_BEGIN (Image, "cebe.sfm.data.image", std::string)

		public:

			int width = -1;
			int height = -1;

			// TODO width height, focal length


			bool hasKeyPoints = false;

			KeyPointList::ptr keypoints;

			bool hasProjectionMatrix = false;
			/**
			 * Estimated 3x4 projection matrix for this image.
			 */
			cv::Matx34d P;


			typedef struct {
				// external parameters
				cv::Matx33d R;     /* Rotation */
				cv::Vec3d t;     /* Translation */

				// internal parameters
				cv::Matx33d K;     /* Internal */

				double f;        /* Focal length */
			} CameraParameters;
			bool hasCameraParameters = false;
			/**
			 * Estimated 3x4 projection matrix for this image.
			 */
			CameraParameters camera;

			/**
			 * @return a list of visualization options.
			 */
			virtual std::vector<std::string> visualizations() {
				std::vector<std::string> v;
				v.push_back("image");
				if (hasKeyPoints) {
					v.push_back("keypoints");
				}
				return v;
			};

			virtual void visualize(std::string option, uipf::VisualizationContext& context) {
				if (option.compare("image") == 0) {
					uipf::data::OpenCVMat::ptr m = uipf::data::load_image_color(getContent());
					context.displayImage(m->getContent());
					return;
				}
				if (option.compare("keypoints") == 0) {
					cv::Mat m = uipf::data::load_image_color(getContent())->getContent();

					cv::drawKeypoints(m, keypoints->getContent(), m, cv::Scalar(0,0,255), cv::DrawMatchesFlags::DRAW_OVER_OUTIMG + cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);

					context.displayImage(m);
					return;
				}
			};


			virtual bool isSerializable() { return true; };
			virtual void serialize(std::ostream& o) const {

				o << "filename: " << getContent() << "\n";
				o << "hasKeyPoints: " << (hasKeyPoints ? "1" : "0") << "\n";
				// TODO serialize keypoints
				o << "hasProjectionMatrix: " << (hasProjectionMatrix ? "1" : "0") << "\n";
				o << "P: " << P(0,0) << " TODO" << "\n"; // TODO
				o << "hasCameraParameters: " << (hasCameraParameters ? "1" : "0") << "\n";
				o << "camera.R: " << camera.R(0,0) << " TODO" << "\n";
				o << "camera.t: " << camera.t(0) << " " << camera.t(1) << " " << camera.t(2) << "\n";
				o << "camera.K: " << camera.K(0,0) << " TODO" << "\n";
				o << "camera.f: " << camera.f << "\n";

			};
			// unserialize constructor
			Image(std::istream&) {
				// TODO implement
				throw uipf::ErrorException("Data Type is not serializable.");
			};


			// TODO PMVS supports image masks

			/**
			 *
			 */
			std::map<std::string, std::string> tags;
			std::map<std::string, uipf::Data::ptr> data;

		UIPF_DATA_TYPE_END
	}
}

#endif // UIPFSFM_DATA_SFMIMAGE_HPP
