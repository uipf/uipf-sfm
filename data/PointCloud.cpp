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
		if (cvPoints.size() == 0) {
			throw uipf::ErrorException("Point cloud is empty.");
		}

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

void PointCloud::serialize(std::ostream& s, const std::string& color_prefix) const
{
	// std::stof() is locale aware, meaning params are not portable between platforms
	// the following is a locale independend stof():
	s.imbue(std::locale("C"));

	s << "ply\n";
	s << "format ascii 1.0\n";
	s << "element vertex " <<  getContent().size() << "\n";

	s << "property float x\n";
	s << "property float y\n";
	s << "property float z\n";

	bool has_normals = false;
	if (normals.size() > 0) {
		assert(normals.size() == getContent().size());

		has_normals = true;

		s << "property float nx\n";
		s << "property float ny\n";
		s << "property float nz\n";
	}

	bool has_color = false;
	if (colors.size() > 0) {
		assert(colors.size() == getContent().size());

		has_color = true;

		s << "property uchar " << color_prefix << "red\n";
		s << "property uchar " << color_prefix << "green\n";
		s << "property uchar " << color_prefix << "blue\n";
	}

	s << "end_header\n";

	const std::vector<cv::Point3d>& points = getContent();
	size_t ps = points.size();
	for(size_t i = 0; i < ps; ++i) {
		s << points[i].x << " " << points[i].y << " " << points[i].z;
		if (has_normals) {
			s << " " << normals[i](0) << " " << normals[i](1) << " " << normals[i](2);
		}
		if (has_color) {
			s << " " << colors[i](0) << " " << colors[i](1) << " " << colors[i](2);
		}
		s << "\n";
	}

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
		scalar_property_definition_callbacks.get<ply::float32>() = std::tr1::bind(&ply_to_pointcloud::scalar_property_definition_callback<ply::float32>, this, std::tr1::_Placeholder<1>(), std::tr1::_Placeholder<2>());
		scalar_property_definition_callbacks.get<ply::uint8>() = std::tr1::bind(&ply_to_pointcloud::scalar_property_definition_callback_color<ply::uint8>, this, std::tr1::_Placeholder<1>(), std::tr1::_Placeholder<2>());
		ply_parser.scalar_property_definition_callbacks(scalar_property_definition_callbacks);

		return ply_parser.parse(istream);
	};
	std::vector<cv::Point3d> getVertices() { return vertices_; }
	std::vector<cv::Scalar> getColors() { return colors_; }
	std::vector<cv::Point3d> getNormals() { return normals_; }
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
			if (property_name == "nx") {
				return std::tr1::bind(&ply_to_pointcloud::vertex_nx, this, std::tr1::_Placeholder<1>());
			} else if (property_name == "ny") {
				return std::tr1::bind(&ply_to_pointcloud::vertex_ny, this, std::tr1::_Placeholder<1>());
			} else if (property_name == "nz") {
				return std::tr1::bind(&ply_to_pointcloud::vertex_nz, this, std::tr1::_Placeholder<1>());
			}
		}
		return 0;
	}
	template <typename ScalarType> std::tr1::function<void (ScalarType)> scalar_property_definition_callback_color(const std::string& element_name, const std::string& property_name) {
		if (element_name == "vertex") {
			if (property_name == "diffuse_red") {
				hasColor = true;
				return std::tr1::bind(&ply_to_pointcloud::vertex_red, this, std::tr1::_Placeholder<1>());
			} else if (property_name == "diffuse_green") {
				hasColor = true;
				return std::tr1::bind(&ply_to_pointcloud::vertex_green, this, std::tr1::_Placeholder<1>());
			} else if (property_name == "diffuse_blue") {
				hasColor = true;
				return std::tr1::bind(&ply_to_pointcloud::vertex_blue, this, std::tr1::_Placeholder<1>());
			}
		}
		return 0;
	}
	void vertex_begin() {};
	void vertex_x(ply::float32 x) { vertex_x_ = x; };
	void vertex_y(ply::float32 y) { vertex_y_ = y; };
	void vertex_z(ply::float32 z) { vertex_z_ = z; };
	void vertex_nx(ply::float32 nx) { vertex_nx_ = nx; };
	void vertex_ny(ply::float32 ny) { vertex_ny_ = ny; };
	void vertex_nz(ply::float32 nz) { vertex_nz_ = nz; };
	void vertex_red(ply::uint8 r) { vertex_red_ = r; };
	void vertex_green(ply::uint8 g) { vertex_green_ = g; };
	void vertex_blue(ply::uint8 b) { vertex_blue_ = b; };
	void vertex_end() {
		vertices_.push_back(cv::Point3d(vertex_x_, vertex_y_, vertex_z_));
		if (hasNormal) {
			normals_.push_back(cv::Point3d(vertex_nx_, vertex_ny_, vertex_nz_));
		}
		if (hasColor) {
			colors_.push_back(cv::Scalar(vertex_red_, vertex_green_, vertex_blue_));
		}
	};
	ply::float32 vertex_x_, vertex_y_, vertex_z_;
	bool hasNormal = false;
	ply::float32 vertex_nx_, vertex_ny_, vertex_nz_;
	bool hasColor = false;
	ply::uint8 vertex_red_, vertex_green_, vertex_blue_;
	std::vector<cv::Point3d> vertices_;
	std::vector<cv::Point3d> normals_;
	std::vector<cv::Scalar> colors_;
};



PointCloud::PointCloud(std::istream& s)
{
	// TODO read normals

	ply_to_pointcloud p2p;
	if (!p2p.convert(s)) {
		throw uipf::ErrorException("Failed to read point cloud from PLY file.");
	}
	setContent(p2p.getVertices());
	colors = p2p.getColors();

}

// https://en.wikipedia.org/wiki/PLY_%28file_format%29


