//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <memory>

#include "glm/glm.hpp"
#include "nlohmann/json_fwd.hpp"

#include "OtTerrain.h"


//
//	OtTerrainComponent
//

class OtTerrainComponent {
public:
	// constructor
	OtTerrainComponent();

	// UI to change component properties
	bool renderUI();

	// (de)serialize component
	nlohmann::json serialize(std::string* basedir);
	void deserialize(nlohmann::json data, std::string* basedir);

	// component name
	static constexpr char const* name = "Terrain";

	// the actual terrain
	std::shared_ptr<OtTerrain> terrain;
};
