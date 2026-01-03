//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <memory>

#include "nlohmann/json_fwd.hpp"

#include "OtGrass.h"


//
//	OtGrassComponent
//

class OtGrassComponent {
public:
	// constructor
	OtGrassComponent();

	// UI to change component properties
	bool renderUI();

	// (de)serialize component
	nlohmann::json serialize(std::string* basedir);
	void deserialize(nlohmann::json data, std::string* basedir);

	// component name
	static constexpr char const* name = "Grass";

	// stored properties
	std::shared_ptr<OtGrass> grass;
};
