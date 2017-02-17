#ifndef UIPFSFM_DATA_SFMIMAGE_HPP
#define UIPFSFM_DATA_SFMIMAGE_HPP

#include <uipf/data.hpp>
#include <uipf/data/list.hpp>
#include <uipf/data/opencv.hpp>
#include "KeyPointList.hpp"

#include <opencv2/opencv.hpp>
#include <sstream>

// TODO check if external 'C' is needed here

namespace uipfsfm {
	namespace data {

		UIPF_DATA_TYPE_BEGIN (Image, "cebe.sfm.data.image", std::string)

		public:

			int width = -1;
			int height = -1;

			// TODO width height, focal length

			// TODO exif

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

				double f = -1;        /* Focal length */
			} CameraParameters;
			bool hasCameraParameters = false;
			/**
			 * Estimated 3x4 projection matrix for this image.
			 */
			CameraParameters camera;

			/**
			 * @return a list of visualization options.
			 */
			virtual std::vector<std::string> visualizations() const;

			virtual void visualize(std::string option, uipf::VisualizationContext& context) const;


			virtual bool isSerializable() const { return true; };
			virtual void serialize(std::ostream& o) const;

			// unserialize constructor
			Image(std::istream&) {
				// TODO implement
				throw uipf::ErrorException("Data Type is not serializable.");
			};

			std::map<std::string, std::string> exif;
			void loadExif();


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
