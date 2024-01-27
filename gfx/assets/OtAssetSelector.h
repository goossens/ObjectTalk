//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <functional>
#include <string>


//
//	OtAssetSelector
//

class OtAssetSelector {
public:
	struct Info {
		const char* label;
		std::string path;
		const char* supportedFileTypes = nullptr;
		bool hasEditor = false;
		bool virtualOnly = false;
		bool* virtualMode = nullptr;
		std::function<void(const std::string&)> creator = nullptr;
	};

	static bool renderUI(Info& info);
};
