/**
 * UIPF SfM
 * Copyright (C) 2017 Carsten Brandt <mail@cebe.cc>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <uipf/data/list.hpp>
#include <uipf/data/opencv.hpp>

#include "data/Image.hpp"

#define UIPF_MODULE_ID "uipfsfm.io.load_images"
#define UIPF_MODULE_NAME "SfM Load Images"
#define UIPF_MODULE_CATEGORY "sfm.io"
#define UIPF_MODULE_CLASS SfMLoadImages

#define UIPF_MODULE_OUTPUTS \
		{"images", uipf::DataDescription(uipf::data::List::id(/*TODO typed list*/), "the loaded images.")}

#define UIPF_MODULE_PARAMS \
		{"path", uipf::ParamDescription("file name of the path to load from.") }, \
		{"focalLength", uipf::ParamDescription("optional annotate the image with focal length, if known.", true) }

#include <uipf/Module.hpp>
#include <uipf/logging.hpp>

void SfMLoadImages::run() {

	using namespace std;
	using namespace uipf::data;
	using namespace uipfsfm::data;

	List::ptr list(new List());

	string path = getParam<string>("path","");
	float focalLength = getParam<float>("focalLength", -1);

	vector<string> images = uipf::data::load_image_names(path);
	updateProgress(0, (int) images.size());
	int i = 0;
	for(string img: images) {
		UIPF_LOG_DEBUG("loading image: ", img);
		Image::ptr image(new Image(img));
		image->camera.f = focalLength;

		// TODO this could be massively speed up by reading file headers
		// http://stackoverflow.com/questions/111345/getting-image-dimensions-without-reading-the-entire-file
		cv::Mat m = cv::imread(img);
		if (m.data) {
			image->height = m.rows;
			image->width = m.cols;
			image->loadExif();

			list->getContent().push_back(image);
		}
		updateProgress(++i, (int) images.size());
	}
	setOutputData<List>("images", list);
}
