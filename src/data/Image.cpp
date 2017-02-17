#include "Image.hpp"

#include <uipf/geomview.hpp>
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
inline std::tuple<Point_3, Point_3, Point_3, Point_3> uipfsfm_data_image_calculate_fov(
		const Point_3& position,
		const Direction_3 & direction,
        double focalLength,
        double resolution_x,
        double resolution_y
) {
	// create vectors in camera direction and camera plane with length 1
	Vector_3 dirNormal = direction.vector();
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
	double ox = resolution_x / focalLength;
	double oy = resolution_y / focalLength;

	Point_3 spanA = position + dirNormal + rlNormal * ox/2 - tbNormal * oy/2;
	Point_3 spanB = position + dirNormal + rlNormal * ox/2 + tbNormal * oy/2;
	Point_3 spanC = position + dirNormal - rlNormal * ox/2 + tbNormal * oy/2;
	Point_3 spanD = position + dirNormal - rlNormal * ox/2 - tbNormal * oy/2;

	return std::make_tuple(spanA, spanB, spanC, spanD);
}


// TODO move this into a cpp file
// draw camera for visualization
inline void uipfsfm_data_image_visualize_camera(uipf::GeomView& gv, const std::string& name, const Image::CameraParameters& c, int w, int h) {

	Point_3 position(c.t(0), c.t(1), c.t(2));
	// TODO provide proper direction
	Direction_3 direction(1, 0, -1);
	Vector_3 dist = direction.vector();

	int oldw = gv.gv().set_line_width(1);
	gv.gv() << CGAL::RED;

	// print FoV
	Point_3 spanA, spanB, spanC, spanD;
	std::tie(spanA, spanB, spanC, spanD) = uipfsfm_data_image_calculate_fov(position, direction, c.f, w, h);

	Polyhedron Pcam;
	std::stringstream scam;
	scam << "OFF\n"
		<< "5 5 0\n"
		<< position << "\n"
		<< (position + (Vector_3(position, spanA) * 100)) << "\n"
		<< (position + (Vector_3(position, spanB) * 100)) << "\n"
		<< (position + (Vector_3(position, spanC) * 100)) << "\n"
		<< (position + (Vector_3(position, spanD) * 100)) << "\n"
		<< "3 0 1 2\n"
		<< "3 0 2 3\n"
		<< "3 0 3 4\n"
		<< "3 0 4 1\n"
		<< "4 4 3 2 1\n";
	scam >> Pcam;
	gv.print_polyhedron(Pcam, true, string("Camera_") + name);
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
		uipfsfm_data_image_visualize_camera(*uipf::GeomView::instance(), getContent(), camera, width, height);
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

}

// http://stackoverflow.com/a/236803/1106908
template<typename Out>
void split_string(const std::string &s, char delim, Out result) {
	std::stringstream ss;
	ss.str(s);
	std::string item;
	while (std::getline(ss, item, delim)) {
		*(result++) = item;
	}
}
std::vector<std::string> split_string(const std::string &s, char delim) {
	std::vector<std::string> elems;
	split_string(s, delim, std::back_inserter(elems));
	return elems;
}

// http://stackoverflow.com/a/29892589/1106908
std::string & ltrim(std::string & str)
{
	auto it2 =  std::find_if( str.begin() , str.end() , [](char ch){ return !std::isspace<char>(ch , std::locale::classic() ) ; } );
	str.erase( str.begin() , it2);
	return str;
}
std::string & rtrim(std::string & str)
{
	auto it1 =  std::find_if( str.rbegin() , str.rend() , [](char ch){ return !std::isspace<char>(ch , std::locale::classic() ) ; } );
	str.erase( it1.base() , str.end() );
	return str;
}
std::string & trim(std::string & str)
{
	return ltrim(rtrim(str));
}
std::string trim_copy(std::string const & str)
{
	auto s = str;
	return ltrim(rtrim(s));
}

void uipf_sfm_image_extimate_focal_length()
{
	// TODO
//	printf("[Extracting exif tags from image %s]\n", $img);
//
//	$make_line = `$JHEAD_EXE $img | grep "Camera make"`;
//	$make_line =~ s/\r?\n$//;
//	($make) = $make_line =~ /: (.*)$/;
//
//	$model_line = `$JHEAD_EXE $img | grep "Camera model"`;
//	$model_line =~ s/\r?\n$//;
//	($model) = $model_line =~ /: (.*)$/;
//
//# Grab focal length
//	$focal_line = `$JHEAD_EXE $img | grep "Focal length" | awk '{print \$4}'`;
//	($focal_mm) = $focal_line =~ /(.*)mm/;
//	printf("  [Focal length = %0.3fmm]\n", $focal_mm);
//
//# Grab CCD width
//
//	$str = sprintf("%s %s", $make, $model);
//
//# Trim leading, trailing spaces
//	$str =~ s/^\s+|\s+$//g ;
//
//	$ccd_width_mm = $ccd_widths{$str};
//# printf("[Looking up %s, got %s\n", $str, $ccd_width_mm);
//
//	if ($ccd_width_mm == 0) {
//		printf("[Couldn't find CCD width for camera %s]\n", $str);
//		$ccd_width_line =
//		`$JHEAD_EXE $img | grep "CCD width" | awk '{print \$4}'`;
//		($ccd_width_mm) = $ccd_width_line =~ /(.*)mm/;
//
//		if ($ccd_width_mm != 0) {
//			printf("[Found in EXIF tags]\n");
//		}
//	}
//
//	printf("  [CCD width = %0.3fmm]\n", $ccd_width_mm);
//
//# Grab resolution
//	$resolution_line = `$JHEAD_EXE $img | grep "Resolution"`;
//	($res_x, $res_y) = $resolution_line =~ /: ([0-9]*) x ([0-9]*)/;
//	printf("  [Resolution = %d x %d]\n", $res_x, $res_y);
//
//# Check that we got everything we need
//#    if ($focal_mm == 0 || $ccd_width_mm == 0 || $res_x == 0) {
//#	printf("  [** Couldn't extract required fields, stop]\n");
//#
//#	if ($model ne "") {
//	#	    printf("  [** $img: Couldn't find info on model %s %s]\n",
//#		   $make, $model);
//#	}
//#
//#	next;
//#    }
//
//    if ($focal_mm == 0 || $ccd_width_mm == 0 || $res_x == 0) {
//	$has_focal = 0;
//    } else {
//	$has_focal = 1;
//    }
//
//    if ($res_x < $res_y) {
//	# Aspect ratio is wrong
//	$tmp = $res_x;
//	$res_x = $res_y;
//	$res_y = $tmp;
//    }
//
//    # Convert to bmp and pgm
//    $basename = `echo $img | sed 's/.[jJ][pP][gG]//'`;
//    chomp($basename);
//
//    if ($has_focal == 1) {
//	# Compute focal length in pixels
//	$focal_pixels = $res_x * ($focal_mm / $ccd_width_mm);
//	printf("  [Focal length (pixels) = %0.3f\n", $focal_pixels);
//	$line = sprintf("%s.jpg 0 %0.5f", $basename, $SCALE * $focal_pixels);
//    } else {
//	$line = sprintf("%s.jpg", $basename);
//    }
//
//    `echo $line >> $OUT_DIR/list.txt`;
//    $num_output_images++;
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
}


