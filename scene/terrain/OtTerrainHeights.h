//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <string>

#include "nlohmann/json_fwd.hpp"

#include "OtFrameBuffer.h"
#include "OtNormalMapper.h"
#include "OtTileableFbm.h"


//
//	OtTerrainHeights
//

class OtTerrainHeights {
public:
	// UI to change properties
	bool renderUI();

	// (de)serialize material
	nlohmann::json serialize(std::string* basedir);
	void deserialize(nlohmann::json data, std::string* basedir);

	// properties
	int heightmapSize = 256;
	float normalStrength = 10.0f;
	int frequency = 10;
	int lacunarity = 2;
	float amplitude = 0.5f;
	float persistence = 0.5f;
	int octaves = 5;

	// rendering tools
	bool dirty = true;

	// the maps on the GPU
	OtFrameBuffer heightmap{OtFrameBuffer::rFloat32Texture};
	OtFrameBuffer normalmap{OtFrameBuffer::rgbaFloat32Texture};

	// update the heightmap
	void update(OtTileableFbm& noise, OtNormalMapper& normals);
};
