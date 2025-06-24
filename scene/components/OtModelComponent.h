//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//


#include <string>

#include "nlohmann/json_fwd.hpp"

#include "OtModelAsset.h"


//
//	OtModelComponent
//

class OtModelComponent {
public:
	// UI to change component properties
	bool renderUI();

	// update model (used for animations)
	void update();

	// (de)serialize component
	nlohmann::json serialize(std::string* basedir);
	void deserialize(nlohmann::json data, std::string* basedir);

	// component name
	static constexpr char const* name = "Model";

	// stored properties
	OtAsset<OtModelAsset> asset;
	bool castShadow = true;
};
