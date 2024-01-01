//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <filesystem>

#include "glm/glm.hpp"
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
	OtMaterialComponent(const std::string& type);

	// GUI to change geometry properties
	bool renderGUI();

	// (de)serialize geometry
	nlohmann::json serialize(std::filesystem::path* basedir);
	void deserialize(nlohmann::json data, std::filesystem::path* basedir);

	// component name
	static constexpr char const* name = "Material";

	// stored properties
	OtMaterial material;

private:
	// create a named material instance
	void createMaterial(const std::string& type);
};
