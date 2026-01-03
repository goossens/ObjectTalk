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
#include <string>

#include "nlohmann/json_fwd.hpp"

#include "OtMaterial.h"
#include "OtTexture.h"


//
//	OtMaterialComponent
//

class OtMaterialComponent {
public:
	// constructors
	OtMaterialComponent();

	// UI to change primitive's properties
	bool renderUI();

	// (de)serialize primitive
	nlohmann::json serialize(std::string* basedir);
	void deserialize(nlohmann::json data, std::string* basedir);

	// component name
	static constexpr char const* name = "Material";

	// stored properties
	std::shared_ptr<OtMaterial> material;
};
