#include "PointCloud.hpp"

#include <uipf/geomview.hpp>

#include <tr1/functional>

#include <libply/ply/ply.hpp>
#include <libply/ply/byte_order.hpp>
#include <libply/ply/io_operators.hpp>
#include <libply/ply/ply_parser.hpp>

#include <functional>

using namespace uipfsfm::data;

std::vector<std::string> PointCloud::visualizations() const {
	std::vector<std::string> v;
	v.push_back("points 3D");
	return v;
}

void PointCloud::visualize(std::string option, uipf::VisualizationContext &context) const {

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

bool PointCloud::isSerializable() const
{
	return true;
}

void PointCloud::serialize(std::ostream& s) const
{
	throw uipf::ErrorException("serialize() is not yet implemented.");
}


using namespace std::tr1::placeholders;

class ply_to_pointcloud
{
public:
	ply_to_pointcloud() {};
	bool convert(std::istream& istream) {
		ply::ply_parser::flags_type ply_parser_flags = 0;
		ply::ply_parser ply_parser(ply_parser_flags);

		ply_parser.info_callback(std::tr1::bind(&ply_to_pointcloud::info_callback, this, std::tr1::_Placeholder<1>(), std::tr1::_Placeholder<2>()));
		ply_parser.warning_callback(std::tr1::bind(&ply_to_pointcloud::warning_callback, this, std::tr1::_Placeholder<1>(), std::tr1::_Placeholder<2>()));
		ply_parser.error_callback(std::tr1::bind(&ply_to_pointcloud::error_callback, this, std::tr1::_Placeholder<1>(), std::tr1::_Placeholder<2>()));

		ply_parser.element_definition_callback(std::tr1::bind(&ply_to_pointcloud::element_definition_callback, this, std::tr1::_Placeholder<1>(), std::tr1::_Placeholder<2>()));

		ply::ply_parser::scalar_property_definition_callbacks_type scalar_property_definition_callbacks;
		ply::at<ply::float32>(scalar_property_definition_callbacks) = std::tr1::bind(&ply_to_pointcloud::scalar_property_definition_callback<ply::float32>, this, std::tr1::_Placeholder<1>(), std::tr1::_Placeholder<2>());
		ply_parser.scalar_property_definition_callbacks(scalar_property_definition_callbacks);

		return ply_parser.parse(istream);
	};
	std::vector<cv::Point3d> getVertices() { return vertices_; }
private:
	void info_callback(std::size_t line_number, const std::string& message) {
		std::cerr << line_number << ": " << "info: " << message << std::endl;
	};
	void warning_callback( std::size_t line_number, const std::string& message) {
		std::cerr << line_number << ": " << "warning: " << message << std::endl;
	};
	void error_callback( std::size_t line_number, const std::string& message) {
		std::cerr << line_number << ": " << "error: " << message << std::endl;
	};
	std::tr1::tuple<std::tr1::function<void()>, std::tr1::function<void()> > element_definition_callback(const std::string& element_name, std::size_t count) {
		if (element_name == "vertex") {
			return std::tr1::tuple<std::tr1::function<void()>, std::tr1::function<void()> >(
					std::tr1::bind(&ply_to_pointcloud::vertex_begin, this),
					std::tr1::bind(&ply_to_pointcloud::vertex_end, this)
			);
		} else {
			return std::tr1::tuple<std::tr1::function<void()>, std::tr1::function<void()> >(0, 0);
		}
	};

	template <typename ScalarType> std::tr1::function<void (ScalarType)> scalar_property_definition_callback(const std::string& element_name, const std::string& property_name) {
		if (element_name == "vertex") {
			if (property_name == "x") {
				return std::tr1::bind(&ply_to_pointcloud::vertex_x, this, std::tr1::_Placeholder<1>());
			} else if (property_name == "y") {
				return std::tr1::bind(&ply_to_pointcloud::vertex_y, this, std::tr1::_Placeholder<1>());
			} else if (property_name == "z") {
				return std::tr1::bind(&ply_to_pointcloud::vertex_z, this, std::tr1::_Placeholder<1>());
			}
		}
		return 0;
	}
	void vertex_begin() {};
	void vertex_x(ply::float32 x) { vertex_x_ = x; };
	void vertex_y(ply::float32 y) { vertex_y_ = y; };
	void vertex_z(ply::float32 z) { vertex_z_ = z; };
	void vertex_end() {
		vertices_.push_back(cv::Point3d(vertex_x_, vertex_y_, vertex_z_));
	};
	ply::float32 vertex_x_, vertex_y_, vertex_z_;
	std::vector<cv::Point3d> vertices_;
};



PointCloud::PointCloud(std::istream& s)
{
	// TODO read normals and color

	ply_to_pointcloud p2p;
	if (!p2p.convert(s)) {
		throw uipf::ErrorException("Failed to read point cloud from PLY file.");
	}
	setContent(p2p.getVertices());

}

// https://en.wikipedia.org/wiki/PLY_%28file_format%29


