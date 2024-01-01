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

#include "nlohmann/json_fwd.hpp"

#include "OtFrameBuffer.h"
#include "OtNormalMapper.h"
#include "OtTileableFbm.h"


//
//	OtTerrainHeights
//

class OtTerrainHeights {
public:
	// GUI to change properties
	bool renderGUI();

	// (de)serialize material
	nlohmann::json serialize(std::filesystem::path* basedir);
	void deserialize(nlohmann::json data, std::filesystem::path* basedir);

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
