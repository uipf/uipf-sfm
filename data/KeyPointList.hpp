#ifndef SFM_MODULES_KEYPOINTLIST_HPP
#define SFM_MODULES_KEYPOINTLIST_HPP

#include <uipf/data.hpp>
#include <uipf/logging.hpp>
#include <uipf/util.hpp>
#include <opencv2/opencv.hpp>

#include <ostream>

namespace uipfsfm {
	namespace data {

		UIPF_DATA_TYPE_BEGIN (KeyPointList, "cebe.sfm.data.keypointlist", std::vector<cv::KeyPoint>)

		public:
			// TODO free descriptor memory in destructor
			std::vector<cv::Mat*> descriptors;

			std::vector<cv::Point2f> getSimplePoints();

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

			virtual bool isSerializable() const { return true; };

			// Reads a file in the format generated by loewe sift binary
			KeyPointList(std::istream& s)
			{
				// std::stof() is locale aware, meaning params are not portable between platforms
				// the following is a locale independend stof():
				s.imbue(std::locale("C"));

				int point_count = 0;
				s >> point_count;
				int vector_length = 0;
				s >> vector_length;

				for(int i = point_count; i > 0; --i) {
					float row, col, scale, rot;
					s >> col;
					s >> row;
					s >> scale;
					s >> rot;
					// convert value from range [-PI,PI[ (sift) to [0,360[ (openCV)
					rot = (int)(rot * 180/M_PI) + 180;

					getContent().push_back(cv::KeyPoint(row, col, scale, rot));
					cv::Mat* descriptor = new cv::Mat(1, vector_length, CV_8U);
					for(int v = 0; v < vector_length; ++v) {
						int no;
						s >> no;
						descriptor->at<uint8_t>(v) = (uint8_t) no;
					}
					descriptors.push_back(descriptor);
				}
			}

			virtual void serialize(std::ostream& s) const {

				assert(getContent().size() == descriptors.size());

//				The file format starts with 2 integers giving the total number of
//				keypoints and the length of the descriptor vector for each keypoint (128).
				s << getContent().size();
				s << " 128\n";

//				Then the location of each keypoint in the image is specified by
//				4 floating point numbers giving subpixel row and column location,
//				scale, and orientation (in radians from -PI to PI).  Obviously, these
//				numbers are not invariant to viewpoint, but can be used in later
//				stages of processing to check for geometric consistency among matches.
				int i = 0;
				for(cv::KeyPoint k: getContent()) {
					float col = k.pt.x;
					float row = k.pt.y;
					float scale = k.size;
					// convert rot value from range [0,360[ (openCV) to [-PI,PI[ (sift)
					float rot = (float) M_PI * (k.angle - 180) / 180;

					s << col << " " << row << " " << scale << " " << rot << "\n";

//					Finally, the invariant descriptor vector for the keypoint is given as
//			    	a list of 128 integers in range [0,255].
					cv::Mat* d = descriptors[i++];
					assert(d->rows == 1 && d->cols == 128);
					for(int v = 0; v < 128; ++v) {
						s << " " << ((int) d->at<uint8_t>(0, v));
						if (v % 20 == 19) {
							s << "\n";
						}
					}
					s << "\n";
				}

			};


		UIPF_DATA_TYPE_END
	}
}


#endif //SFM_MODULES_KEYPOINTLIST_HPP