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

#include "data/Image.hpp"

#include <boost/filesystem.hpp>

#define UIPF_MODULE_ID "uipfsfm.io.copy_image"
#define UIPF_MODULE_NAME "SfM Copy Image"
#define UIPF_MODULE_CATEGORY "sfm.io"
#define UIPF_MODULE_CLASS SfMCopyImage

#define UIPF_MODULE_INPUTS \
		{"image", uipf::DataDescription(uipfsfm::data::Image::id(), "the source image.")}

#define UIPF_MODULE_OUTPUTS \
		{"image", uipf::DataDescription(uipfsfm::data::Image::id(), "the copied image.")}

#define UIPF_MODULE_PARAMS \
		{"path", uipf::ParamDescription("the target path to copy to.") }, \
		{"overwrite", uipf::ParamDescription("whether to overwrite existing files. Defaults to false.", true) }, \
		{"lowercase", uipf::ParamDescription("whether to convert the file name to lower case. Defaults to false.", true) }

#include <uipf/Module.hpp>

void SfMCopyImage::run() {

	using namespace std;
	using namespace uipf;
	using namespace uipfsfm::data;

	namespace fs = boost::filesystem;

	Image::ptr image = getInputData<Image>("image");
	fs::path path = getParam<string>("path", ".");

	fs::path src = image->getContent();

	fs::path target = path;
	if (!fs::exists(target)) {
		fs::create_directories(target);
	}
	if (getParam<bool>("lowercase", false)) {
		target /= uipf_str_to_lower(src.filename().string());
	} else {
		target /= src.filename();
	}

	if (!fs::exists(src)) {
		throw ErrorException(string("Source file does not exist: ") + src.string());
	}
	if (fs::exists(target)) {
		if (getParam<bool>("overwrite", false)) {
			fs::remove(target);
		} else {
			throw ErrorException(string("Target file already exists: ") + src.string());
		}
	}
	// TODO error handling
	fs::copy(src, target);
	// TODO really copy the object and all info
	image->setContent(target.string());

	setOutputData<Image>("image", image);
}
