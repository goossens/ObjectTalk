//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <filesystem>

#include "nlohmann/json.hpp"

#include "OtUi.h"


//
//	OtPathGetRelative
//

static inline std::filesystem::path OtPathGetRelative(std::filesystem::path path, std::filesystem::path* basedir) {
	// make a path relative to the provided base directory
	if (basedir) {
		if (path.empty()) {
			return path;

		} else {
			auto relative = std::filesystem::relative(path, *basedir);
			return relative == "." ? std::filesystem::path() : relative;
		}

	} else {
		return path;
	}
}


//
//	OtPathGetAbsolute
//

static inline std::filesystem::path OtPathGetAbsolute(nlohmann::json data, const char* field, std::filesystem::path* basedir) {
	// make a path absolute based on a provided base directory
	if (data.contains(field)) {
		std::string value = data[field];

		if (value.size()) {
			if (basedir) {
				return *basedir / std::filesystem::path(value);

			} else {
				return std::filesystem::path(value);
			}

		} else {
			return std::filesystem::path();
		}

	} else {
		return std::filesystem::path();
	}
}


//
//	OtPathEdit
//

static inline bool OtPathEdit(const char* label, std::filesystem::path& path, bool& flag) {
	bool changed = false;

	if (OtUiFileSelector(label, path)) {
		changed = true;
		flag = true;
	}

	return changed;
}
