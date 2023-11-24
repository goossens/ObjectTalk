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

#include "bgfx/bgfx.h"
#include "nlohmann/json_fwd.hpp"

#include "OtGenerator.h"
#include "OtShaderProgram.h"
#include "OtUniformVec4.h"


//
//	OtTileableFbm (Fractional Brownian Motion)
//

class OtTileableFbm : public OtGenerator {
public:
	// GUI to change properties
	bool renderGUI();

	// (de)serialize material
	nlohmann::json serialize(std::filesystem::path* basedir);
	void deserialize(nlohmann::json data, std::filesystem::path* basedir);

private:
	// execute generator
	void execute(OtPass& pass, int w, int h) override;

	// properties
	int frequency = 10;
	int lacunarity = 2;
	float amplitude = 0.5f;
	float persistence = 0.5f;
	int octaves = 5;

	// shader resources
	OtShaderProgram program{"OtGeneratorVS", "OtTileableFbmFS"};
	OtUniformVec4 uniform{"u_fbm", 2};
};
