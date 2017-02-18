#include "PointCloud.hpp"

#include <uipf/geomview.hpp>


std::vector<std::string> uipfsfm::data::PointCloud::visualizations() const {
	std::vector<std::string> v;
	v.push_back("points 3D");
	return v;
}

void uipfsfm::data::PointCloud::visualize(std::string option, uipf::VisualizationContext &context) const {

	using namespace uipf::geomview;

	if (option.compare("points 3D") == 0) {

		const std::vector<cv::Point3d>& cvPoints = getContent();
		if (colors.size() == 0) {

			std::vector<Point_3> points;
			for(size_t i = 0; i < cvPoints.size(); ++i) {
				points.push_back(Point_3(cvPoints[i].x, cvPoints[i].y, cvPoints[i].z));
			}
			uipf::GeomView::instance()->print_pointcloud(points);

		} else {

			if (cvPoints.size() != colors.size()) {
				throw uipf::ErrorException("Point and color are of different size.");
			}

			std::vector< std::tuple<Point_3, Color> > points;
			for(size_t i = 0; i < cvPoints.size(); ++i) {
				points.push_back(std::tuple<Point_3, Color>(
					Point_3(cvPoints[i].x, cvPoints[i].y, cvPoints[i].z),
					Color(colors[i](0), colors[i](1), colors[i](2))
                ));
			}
			uipf::GeomView::instance()->print_colored_pointcloud(points);

		}

		return;
	}
}
