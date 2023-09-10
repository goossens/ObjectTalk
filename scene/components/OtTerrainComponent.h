//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
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

	// GUI to change component properties
	bool renderGUI();

	// (de)serialize component
	nlohmann::json serialize(std::filesystem::path* basedir);
	void deserialize(nlohmann::json data, std::filesystem::path* basedir);

	// component name
	static constexpr char const* name = "Terrain";

	// stored properties
	std::unique_ptr<OtTerrain> terrain;
};