#include "Image.hpp"

#include <uipf/geomview.hpp>
#include <uipf/util.hpp>
#include <tuple>


using namespace std;
using namespace uipfsfm::data;
using namespace uipf::geomview;


std::vector<std::string> Image::visualizations() const
{
	std::vector<std::string> v;
	v.push_back("image");
	if (hasKeyPoints) {
		v.push_back("keypoints");
		v.push_back("keypoints.txt");
	}
//	if (hasProjectionMatrix || hasCameraParameters) {
		v.push_back("params");
//	}
	if (hasCameraParameters) {
		v.push_back("camera 3D");
	}
	if (!exif.empty()) {
		v.push_back("EXIF");
	}
	return v;
}


// calculate camera field of view
inline std::tuple<Vector_3, Vector_3, Vector_3, Vector_3> uipfsfm_data_image_calculate_fov(
		const Vector_3& direction,
        double focalLength,
        double ccd_x,
        double ccd_y
) {
	// create vectors in camera direction and camera plane with length 1
	Vector_3 dirNormal = direction;
	dirNormal = dirNormal / sqrt(dirNormal.squared_length());
	Vector_3 rlNormal = CGAL::cross_product(dirNormal, Vector_3(0, 1, 0));
	if (rlNormal == CGAL::NULL_VECTOR) {
		// singular case, where camera looks directly along y (up or down)
		// default viewing direction is along x, so x is now "up"
		rlNormal = CGAL::cross_product(dirNormal, Vector_3(1, 0, 0));
	}
	rlNormal = rlNormal / sqrt(rlNormal.squared_length());
	Vector_3 tbNormal = CGAL::cross_product(rlNormal, dirNormal);
	tbNormal = tbNormal / sqrt(tbNormal.squared_length());


	// o = i/f if d=1
	double ox = ccd_x;
	double oy = ccd_y;

	dirNormal = dirNormal * focalLength;

	Vector_3 spanA = dirNormal + rlNormal * ox/2 - tbNormal * oy/2;
	Vector_3 spanB = dirNormal + rlNormal * ox/2 + tbNormal * oy/2;
	Vector_3 spanC = dirNormal - rlNormal * ox/2 + tbNormal * oy/2;
	Vector_3 spanD = dirNormal - rlNormal * ox/2 - tbNormal * oy/2;

	return std::make_tuple(spanA, spanB, spanC, spanD);
}


// TODO move this into a cpp file
// draw camera for visualization
inline void uipfsfm_data_image_visualize_camera(uipf::GeomView& gv, const std::string& name, const Image::CameraParameters& c, int w, int h, const std::string& texture) {

	Point_3 position(-c.t(0), -c.t(1), -c.t(2));
	Vector_3 direction(c.direction[0], c.direction[1], c.direction[2]);
	Transformation_3 R(
		c.R(0,0), c.R(0,1), c.R(0,2),
		c.R(1,0), c.R(1,1), c.R(1,2),
		c.R(2,0), c.R(2,1), c.R(2,2)
	);
	position = R.inverse()(position);
	direction = R.inverse()(direction);

//	gv.gv().set_trace(true);

	// calculate FoV
	double scale = 10;
	double f = c.f_mm * scale;
	double ccd = c.ccd_width_mm * scale;
	if (f <= 0 || ccd <= 0) {
		// assume something to still show a camera
		f = 0.035;//m = 35mm
		ccd = f;
		UIPF_LOG_TRACE("No camera parameters, assuming some to show a camera");
	}

	// TODO find some automatic scale for nicer visualisation
	UIPF_LOG_TRACE("Camera viz: f=",f," ccd=",ccd," pos=",position, " w=", w, " h=", h);


	Vector_3 spanA, spanB, spanC, spanD;
	std::tie(spanA, spanB, spanC, spanD) = uipfsfm_data_image_calculate_fov(direction, f, ccd, ccd / w * h);

	int oldw = gv.gv().set_line_width(1);
	gv.gv() << CGAL::RED;


	Polyhedron Pcam;
	std::stringstream scam;
	scam << "OFF\n"
		<< "5 5 0\n"
		<< position << "\n"
		<< (position + spanA) << "\n"
		<< (position + spanB) << "\n"
		<< (position + spanC) << "\n"
		<< (position + spanD) << "\n"
		<< "3 0 1 2\n"
		<< "3 0 2 3\n"
		<< "3 0 3 4\n"
		<< "3 0 4 1\n"
		<< "4 4 3 2 1\n";
	scam >> Pcam;
	gv.print_polyhedron(Pcam, false, string("Camera_") + name /*, texture TODO texture does not work currently */);
//		gv << Segment_3(position, position + (Vector_3(position, spanA) * 100));
//		gv << Segment_3(position, position + (Vector_3(position, spanB) * 100));
//		gv << Segment_3(position, position + (Vector_3(position, spanC) * 100));
//		gv << Segment_3(position, position + (Vector_3(position, spanD) * 100));


	// print Direction only
#if VISUALIZE_CAM_OPTIMAL_DISTANCE
	int oldw2 = gv.set_line_width(4);
					if (positionValid) {
						gv << CGAL::GREEN;
					} else {
						gv << CGAL::RED;
					}
					gv << position;
					gv.set_line_width(oldw2);
					gv << CGAL::RED;
					gv << Segment_3(position, position + dist);
#endif

	gv.gv().set_line_width(oldw);

}


void Image::visualize(std::string option, uipf::VisualizationContext &context) const
{
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
	if (option.compare("keypoints.txt") == 0) {
		std::ostringstream s;
		keypoints->serialize(s);
		context.displayText(s.str());
		return;
	}
	if (option.compare("params") == 0) {
		std::ostringstream s;
		serialize(s);
		context.displayText(s.str());
		return;
	}
	if (option.compare("EXIF") == 0) {
		std::ostringstream s;
		for(auto exifItem: exif) {
			s << exifItem.first << " : " << exifItem.second << "\n";
		}
		context.displayText(s.str());
		return;
	}
	if (option.compare("camera 3D") == 0) {
		// TODO use basename for name
		uipfsfm_data_image_visualize_camera(*uipf::GeomView::instance(), getContent(), camera, width, height, getContent());
		return;
	}
}

void Image::serialize(std::ostream &o) const {

	o << "filename: " << getContent() << "\n";
	o << "width: " << width << "\n";
	o << "height: " << height << "\n";
	o << "hasKeyPoints: " << (hasKeyPoints ? "1" : "0") << "\n";
	// TODO serialize keypoints
	o << "hasProjectionMatrix: " << (hasProjectionMatrix ? "1" : "0") << "\n";
	if (hasProjectionMatrix) {
		o << "P:\n" << P(0, 0) << " " << P(0, 1) << " " << P(0, 2) << " " << P(0, 3) << "\n";
		o <<           P(1, 0) << " " << P(1, 1) << " " << P(1, 2) << " " << P(1, 3) << "\n";
		o <<           P(2, 0) << " " << P(2, 1) << " " << P(2, 2) << " " << P(2, 3) << "\n";
	}
	o << "hasCameraParameters: " << (hasCameraParameters ? "1" : "0") << "\n";
	if (hasCameraParameters) {
		o << "camera.R:\n" << camera.R(0, 0) << " " << camera.R(0, 1) << " " << camera.R(0, 2) << "\n";
		o <<                  camera.R(1, 0) << " " << camera.R(1, 1) << " " << camera.R(1, 2) << "\n";
		o <<                  camera.R(2, 0) << " " << camera.R(2, 1) << " " << camera.R(2, 2) << "\n";
		o << "camera.t: "  << camera.t(0) << " " << camera.t(1) << " " << camera.t(2) << "\n";
		o << "camera.K:\n" << camera.K(0, 0) << " " << camera.K(0, 1) << " " << camera.K(0, 2) << "\n";
		o <<                  camera.K(1, 0) << " " << camera.K(1, 1) << " " << camera.K(1, 2) << "\n";
		o <<                  camera.K(2, 0) << " " << camera.K(2, 1) << " " << camera.K(2, 2) << "\n";
	}
	if (camera.f > 0) {
		o << "camera.f: "  << camera.f << "\n";
	}
	if (camera.f_mm > 0) {
		o << "camera.f_mm: "  << camera.f_mm << "\n";
	}
	if (camera.ccd_width_mm > 0) {
		o << "camera.ccd_width_mm: "  << camera.ccd_width_mm << "\n";
	}

}


inline void uipf_sfm_image_estimate_focal_length(const map<string, string>& exif, Image& image)
{
	UIPF_LOG_TRACE("estimating focal length for image ", image.getContent());
	// TODO simplify this with regex?

	// find Focal length in EXIF data
	auto focal_mm_s = exif.find("Focal length");
	if (focal_mm_s == exif.end()) {
		UIPF_LOG_TRACE("f-estimate: no focal lenghth in EXIF");
		return;
	}
	// value should be something like  5.4mm  (35mm equivalent: 37mm)
	vector<string> focal_parts = split_string(focal_mm_s->second, 'm');
	if (focal_parts[0].empty()) {
		UIPF_LOG_WARNING("f-estimate: badly formatted focal lenghth in EXIF");
		return;
	}
	double focal_mm;
	stringstream sf(focal_parts[0]);
	sf.imbue(std::locale("C"));
	sf >> focal_mm;

	if (focal_mm > 0) {
		focal_mm *= 0.001;
		image.camera.f_mm = focal_mm;
	}

	// find CCD width in EXIF data
	auto ccd_mm_s = exif.find("CCD width");
	if (ccd_mm_s == exif.end()) {
		UIPF_LOG_TRACE("f-estimate: no CCD width in EXIF");
		return;
	}
	// value should be something like 5.23mm
	vector<string> ccd_parts = split_string(ccd_mm_s->second, 'm');
	if (ccd_parts[0].empty()) {
		UIPF_LOG_WARNING("f-estimate: badly formatted CCD width in EXIF");
		return;
	}
	double ccd_width_mm;
	stringstream sc(ccd_parts[0]);
	sc.imbue(std::locale("C"));
	sc >> ccd_width_mm;
	ccd_width_mm *= 0.001;

	// find Resolution in EXIF data
	int res_w = 0;
	int res_h = 0;
	auto res_s = exif.find("Resolution");
	if (res_s == exif.end()) {
		res_w = image.width;
		res_h = image.height;
	} else {
		// value should be something like 5.23mm
		vector<string> res_parts = split_string(res_s->second, 'x');
		if (res_parts[0].size() < 2) {
			res_w = image.width;
			res_h = image.height;
		} else {
			stringstream sw(trim(res_parts[0]));
			sw.imbue(std::locale("C"));
			sw >> res_w;
			stringstream sh(trim(res_parts[1]));
			sh.imbue(std::locale("C"));
			sh >> res_h;
			if (res_h != image.height || res_w != image.width) {
				UIPF_LOG_WARNING("EXIF resolution (",res_w,"x",res_w,") is different from image resolution (",image.width,"x",image.height,") estimated focal length may be inaccurate!");
			}
		}
	}

	if (focal_mm > 0 && ccd_width_mm > 0 && res_w > 0 && res_h > 0) {
		image.camera.f = res_w * (focal_mm / ccd_width_mm);
		image.camera.f_mm = focal_mm;
		image.camera.ccd_width_mm = ccd_width_mm;
	} else {
		UIPF_LOG_WARNING("Failed to estimate focal length (invalid EXIF data, f=", focal_mm, " ccd=", ccd_width_mm, " w=", res_w,
		                 " h=", res_h, ") for image ", image.getContent());
	}
}

void Image::loadExif()
{
	string output;
	try {
		output = uipf_exec_stdout((string("jhead ") + getContent()).c_str());
	} catch(uipf::ErrorException& e) {
		UIPF_LOG_WARNING("Failed to load EXIF information for image ", getContent(), ": ", e.what());
		return;
	}
	// output should contain something like:
//	File name    : kermit000.jpg
//	File size    : 165140 bytes
//	File date    : 2017:01:14 07:39:31
//	Camera make  : Canon
//	Camera model : Canon PowerShot A10
//	Date/Time    : 2004:11:03 21:10:01
//	Resolution   : 640 x 480
//	Flash used   : No
//	Focal length :  5.4mm  (35mm equivalent: 37mm)
//	CCD width    : 5.23mm
//	Exposure time: 0.050 s  (1/20)
//	Aperture     : f/2.8
//	Focus dist.  : 0.62m
//	Light Source : Fluorescent
//	Metering Mode: pattern
//	JPEG Quality : 97

	exif.clear();
	for(string line: split_string(output, '\n')) {
		if (line.empty()) {
			continue;
		}
		vector<string> parts = split_string(line, ':');
		string name = parts[0];
		trim(name);
		if (name.empty()) {
			continue;
		}
		string value;
		for(size_t i = 1; i < parts.size(); ++i) {
			value += parts[i];
		}
		exif.insert(pair<string, string>(name, trim(value)));
	}

	uipf_sfm_image_estimate_focal_length(exif, *this);
}


