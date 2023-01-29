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

#include "OtPass.h"
#include "OtSampler.h"
#include "OtShader.h"
#include "OtSphereGeometry.h"
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

	// stored properties
	std::filesystem::path image;

	// render the sky sphere
	void render(OtPass& pass);

	// runtime properties
	bool update = false;
	OtSphereGeometry geometry;
	OtTexture texture;
	OtSampler sampler;
	OtShader shader;
};
