//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <algorithm>
#include <vector>

#include "OtAssert.h"
#include "OtText.h"

#include "OtAssetBase.h"
#include "OtAssetManager.h"


//
//	OtAssetBase::~OtAssetBase
//

OtAssetBase::~OtAssetBase() {
	OtAssert(references == 0);
}


//
//	OtAssetBase::getStateName
//

const char* OtAssetBase::getStateName() {
	static const char* names[] = {
		"Null",
		"Loading",
		"Missing",
		"Invalid",
		"Loaded",
		"Ready"
	};

	return names[static_cast<size_t>(state)];
}


//
//	OtAssetBase::supportsFileType
//

bool OtAssetBase::supportsFileType(const std::string& ext) {
	std::vector<std::string> extensions;
	OtText::split(getSupportedFileTypes(), extensions, ',');
	return std::find(extensions.begin(), extensions.end(), ext) != extensions.end();
}
