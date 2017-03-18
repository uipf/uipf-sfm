#include "ImageGraph.hpp"
#include <boost/filesystem.hpp>
#include "lib/pba/util.h"
#include "lib/pba/DataInterface.h"
#include "Image.hpp"

#include <algorithm>
#include <functional>

using namespace uipfsfm::data;

std::vector<uipf::Data::ptr> ImageGraph::getListContent() const {
	std::vector<uipf::Data::ptr> list;
	for(ImagePair::ptr ip: data_) {
		list.push_back(std::static_pointer_cast<uipf::Data>(ip));
	}
	return list;
}

std::vector<std::string> ImageGraph::visualizations() const {
	std::vector<std::string> v;
	// TODO do not offer this option if graphviz is not installed
	v.push_back("graph");
	for(auto img: images) {
		if (img.second->hasCameraParameters) {
			v.push_back("cameras 3D");
			break;
		}
	}
	return v;
}

void ImageGraph::visualize(std::string option, uipf::VisualizationContext &context) const {
	if (option.compare("graph") == 0) {
		// TODO choose temp path by uipf or directly pipe to commansd
		std::ofstream s("/tmp/uipfsfm-graph.dot");
		s << "strict graph {\n";
//					s << "  rankdir=\"LR\";\n";

		for(ImagePair::ptr pair: getContent()) {

			Image::ptr imageA = pair->getContent().first;
			Image::ptr imageB = pair->getContent().second;
			std::string fileA = boost::filesystem::path(imageA->getContent()).filename().string();
			std::string fileB = boost::filesystem::path(imageB->getContent()).filename().string();

			s << "\"" << fileA << "\" -- \"" << fileB << "\";";
		}
		s << "}\n";
		s.close();
//					uipf_exec_stdout("dot -Tpng /tmp/uipfsfm-graph.dot -o /tmp/uipfsfm-graph.png");
		uipf_exec_stdout("neato /tmp/uipfsfm-graph.dot | dot -Tpng -o /tmp/uipfsfm-graph.png");
		// TODO proper error handling, uipf::ErrorException
		uipf::data::OpenCVMat::ptr m = uipf::data::load_image_color("/tmp/uipfsfm-graph.png");
//					boost::filesystem::remove(boost::filesystem::path("/tmp/uipfsfm-graph.dot"));
		boost::filesystem::remove(boost::filesystem::path("/tmp/uipfsfm-graph.png"));
		context.displayImage(m->getContent());
		return;

	}
	if (option.compare("cameras 3D") == 0) {
		for(auto img: images) {
			if (img.second->hasCameraParameters) {
				img.second->visualize("camera 3D", context);
			}
		}
	}
}

void ImageGraph::serialize(std::ostream &s) const {

	vector<CameraT> camera_data;
	vector<string> names;

	vector<Point3D> point_data;
	vector<Point2D> measurements;
	vector<int> ptidx;
	vector<int> camidx;
	vector<int> ptc;

	UIPF_LOG_TRACE("Prepare saving NVM file...");

	// sort pointRef by imageId to make access liner in the next step
	std::vector<PointRef> sortedPointRef(pointRef.size());
	if (pointRef.size() > 0) {
		std::copy(pointRef.begin(), pointRef.end(), sortedPointRef.begin());
		std::sort(sortedPointRef.begin(), sortedPointRef.end(), [](const PointRef &a, const PointRef &b) {
			return a.imageId < b.imageId;
		});
	}

	// whether the world needs to be transformed to match the assumptions
	// met by the NVM format (camera looks in +z direction)
	cv::Matx33d cameraTransform({1, 0, 0, 0, 1, 0, 0, 0, 1});

	unsigned long camid = 0;
	size_t pr = 0;
	camera_data.resize(images.size());
	names.resize(images.size());
	for(pair<int, Image::ptr> image: images) {
		if (image.first != camid) {
			UIPF_LOG_WARNING("camid mismatch! ", image.first, " - ", camid);
		}

		Image::CameraParameters ic = image.second->camera;

		if (ic.direction != cv::Vec3d(0, 0, 1)) {
			if (ic.direction == cv::Vec3d(0, 0, -1)) {
				UIPF_LOG_WARNING("NVM coordinate system assumptions are different from current camera system, converting!")
				cameraTransform = cv::Matx33d({-1, 0, 0, 0, -1, 0, 0, 0, -1});
				ic.R = cameraTransform * ic.R;
				ic.t = cameraTransform * ic.t;
			} else {
				throw uipf::ErrorException("No conversion for camera direction implemented.");
			}
		}


		CameraT camera;
		double t[3];
		t[0] = ic.t[0];
		t[1] = ic.t[1];
		t[2] = ic.t[2];
		camera.SetTranslation(t);
		float r[9];
		r[0] = (float) ic.R(0,0);
		r[1] = (float) ic.R(0,1);
		r[2] = (float) ic.R(0,2);
		r[3] = (float) ic.R(1,0);
		r[4] = (float) ic.R(1,1);
		r[5] = (float) ic.R(1,2);
		r[6] = (float) ic.R(2,0);
		r[7] = (float) ic.R(2,1);
		r[8] = (float) ic.R(2,2);
		camera.SetMatrixRotation(r);
		camera.SetFocalLength(ic.f);

		for(; pr < sortedPointRef.size(); ++pr) {
			if (sortedPointRef[pr].imageId < camid) {
				continue;
			}
			if (sortedPointRef[pr].imageId != camid) {
				break;
			}
			// TODO test whether keypoint exists: sortedPointRef[pr].keyPointId
			// TODO also check why this can happen :-/
			cv::KeyPoint keyPoint = image.second->keypoints->getContent()[sortedPointRef[pr].keyPointId];
			measurements.push_back(Point2D(keyPoint.pt.x, keyPoint.pt.y));
			camidx.push_back(camid);
			ptidx.push_back(sortedPointRef[pr].p3);
		}

		camera_data[camid] = camera;
		names[camid] = image.second->getContent();

		camid++;
	}

	if (points3D) {
		UIPF_LOG_TRACE("Preparing Point Data... ", points3D->getContent().size(), " points");
		int c = 0;
		for (cv::Point3d p: points3D->getContent()) {

			Point3D pp;
			pp.SetPoint(p.x, p.y, p.z);
			point_data.push_back(pp);
			ptc.push_back(points3D->colors[c](0));
			ptc.push_back(points3D->colors[c](1));
			ptc.push_back(points3D->colors[c](2));

			c++;
		}
	}

	UIPF_LOG_TRACE("Saving NVM File... ", camera_data.size(), " cameras, ", point_data.size(), " points, ", measurements.size(), " measurements");
	SaveNVMs(s, camera_data, point_data, measurements, ptidx, camidx, names, ptc);
	UIPF_LOG_TRACE("Done Saving NVM File...");
}

ImageGraph::ImageGraph(std::istream &s, const std::string& filename)
{
	namespace fs = boost::filesystem;

	vector<CameraT> camera_data;
	vector<Point3D> point_data;
	vector<Point2D> measurements;
	vector<int> ptidx;
	vector<int> camidx;
	vector<string> names;
	vector<int> ptc;

	UIPF_LOG_TRACE("Reading NVM file...");
	if (!LoadNVM(s, camera_data, point_data, measurements, ptidx, camidx, names, ptc)) {
		throw uipf::ErrorException("Failed to read NVM file.");
	}

	fs::path basePath = fs::path(filename).remove_filename();

	UIPF_LOG_TRACE("Populating Camera Data...");
	int i = 0;
	images.get_allocator().allocate(camera_data.size());
	for(CameraT camera: camera_data) {

		fs::path imgPath(names[i]);
		if (imgPath.is_relative()) {
			imgPath = basePath / imgPath;
		}
		Image::ptr image(new Image(imgPath.string()));
		cv::Mat m = cv::imread(imgPath.string());
		if (m.data) {
			image->height = m.rows;
			image->width = m.cols;
		}
		image->loadExif();

		image->hasCameraParameters = true;
		image->camera.f = camera.f;
		// NVM assumes camera direction (0, 0, 1)
		// for (0, 0, -1) the following conversion needs to be applied:
		cv::Matx33d cameraTransform({-1, 0, 0, 0, -1, 0, 0, 0, -1});

		image->camera.direction = cv::Vec3d(0, 0, -1);
		image->camera.R = cameraTransform * cv::Matx33d(
				camera.m[0][0], camera.m[0][1], camera.m[0][2],
				camera.m[1][0], camera.m[1][1], camera.m[1][2],
				camera.m[2][0], camera.m[2][1], camera.m[2][2]
		);
		image->camera.t = cameraTransform * cv::Vec3d(camera.t[0], camera.t[1], camera.t[2]);
		image->camera.K = {
			camera.f, 0, 0,
			0, camera.f, 0,
			0       , 0, 1,
		};
		// P = K[R T]
		image->hasProjectionMatrix = true;
		image->P = image->camera.K * cv::Matx34d(
			image->camera.R(0,0), image->camera.R(0,1), image->camera.R(0,2), image->camera.t[0],
			image->camera.R(1,0), image->camera.R(1,1), image->camera.R(1,2), image->camera.t[1],
			image->camera.R(2,0), image->camera.R(2,1), image->camera.R(2,2), image->camera.t[2]
		);
		image->keypoints = KeyPointList::ptr(new KeyPointList(std::vector<cv::KeyPoint>()));


		// TODO reconstruct image pairs from data

		images.insert(pair<int, Image::ptr>(i, image));

		i++;
	}

	UIPF_LOG_TRACE("Populating Point Data...");
	int p3i = 0;
	points3D = PointCloud::ptr(new PointCloud(std::vector<cv::Point3d>(point_data.size())));
	if (ptc.size() > 0) {
		points3D->colors.resize(point_data.size());
	}
	for(Point3D p3: point_data) {

		points3D->getContent()[p3i] = cv::Point3d(p3.xyz[0], p3.xyz[1], p3.xyz[2]);
		if (ptc.size() > 0) {
			points3D->colors[p3i] = cv::Scalar(ptc[p3i * 3], ptc[p3i * 3 + 1], ptc[p3i * 3 + 2]);
		}
		p3i++;
	}
	UIPF_LOG_TRACE("Done. points: ", points3D->getContent().size() , " colors: ", points3D->colors.size());

	UIPF_LOG_TRACE("Populating Point References...");
	int pi = 0;
	for(Point2D p: measurements) {

		Image::ptr img = images[camidx[pi]];
		PointRef ref;
		ref.p3 = (unsigned long) ptidx[pi];
		ref.imageId = (unsigned long) camidx[pi];
		ref.keyPointId = img->keypoints->getContent().size();
		img->keypoints->getContent().push_back(cv::KeyPoint(p.x, p.y, 1));
		pointRef.push_back(ref);

		pi++;
	}

	UIPF_LOG_TRACE("Reconstructing Image Pairs from ", pointRef.size(), " references...");
	vector<pair <int, int> > pairCandidates;
	size_t jc = pointRef.size();
	for(size_t jj = 0; jj < jc; ++jj) {
		for(size_t jk = jj + 1; jk < jc; ++jk) {
			if (pointRef[jj].p3 == pointRef[jk].p3) {
				if (pointRef[jj].imageId != pointRef[jk].imageId) {
					if (pointRef[jj].imageId < pointRef[jk].imageId) {
						if (std::find(pairCandidates.begin(), pairCandidates.end(),
						              pair<int, int>(pointRef[jj].imageId, pointRef[jk].imageId)) == pairCandidates.end()) {
							pairCandidates.push_back(pair<int, int>(pointRef[jj].imageId, pointRef[jk].imageId));
						}
					} else {
						if (std::find(pairCandidates.begin(), pairCandidates.end(),
						              pair<int, int>(pointRef[jk].imageId, pointRef[jj].imageId)) == pairCandidates.end()) {
							pairCandidates.push_back(pair<int, int>(pointRef[jk].imageId, pointRef[jj].imageId));
						}
					}
				}
			} else {
				break;
			}
		}
	}
	UIPF_LOG_TRACE("Reconstructing ", pairCandidates.size(), " Image Pairs...");
	for(pair<int,int> r: pairCandidates) {
		ImagePair::ptr pair(new ImagePair(std::pair<Image::ptr, Image::ptr>(images[r.first], images[r.second])));
		pair->hasKeyPointMatches = false;
		// TODO recover keypoint matches
		getContent().push_back(pair);
	}
	UIPF_LOG_TRACE("Done Importing Image Graph.");


}

