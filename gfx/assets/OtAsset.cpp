//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "nlohmann/json.hpp"

#include "OtPath.h"

#include "OtAsset.h"


//
//	OtAssetSerialize
//

std::string OtAssetSerialize(const std::string& path, std::string* basedir) {
	// make a path absolute based on a provided base directory
	if (path.size()) {
		// don't transform virtual paths
		if (OtText::startsWith(path, "virtual:")) {
			return path;

		} else {
			return OtPath::getRelative(path, basedir);
		}

	} else {
		return "";
	}
}


//
//	OtAssetDeserialize
//

std::string OtAssetDeserialize(nlohmann::json* data, const char* field, std::string* basedir) {
	// make a path absolute based on a provided base directory
	if (data->contains(field)) {
		std::string path = (*data)[field];

		// don't transform virtual paths
		if (OtText::startsWith(path, "virtual:")) {
			return path;

		} else {
			return OtPath::getAbsolute(path, basedir);
		}

	} else {
		return "";
	}
}
