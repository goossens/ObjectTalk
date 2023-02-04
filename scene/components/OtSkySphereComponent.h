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

#include "nlohmann/json_fwd.hpp"

#include "OtTexture.h"


//
//	OtSkySphereComponent
//

class OtSkySphereComponent {
public:
	// GUI to change component properties
	bool renderGUI();

	// (de)serialize component
	nlohmann::json serialize(std::filesystem::path* basedir);
	void deserialize(nlohmann::json data, std::filesystem::path* basedir);

	// component name
	static constexpr char const* name = "Sky Sphere";

	// see if texture is valid
	bool isValid();

	// stored properties
	std::filesystem::path image;
	float brightness = 1.0f;
	float gamma = 1.0f;

	// runtime properties
	bool update = false;
	OtTexture texture;
};
