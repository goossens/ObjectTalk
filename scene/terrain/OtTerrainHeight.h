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

#include "OtFrameBuffer.h"
#include "OtSampler.h"
#include "OtTileableFbm.h"


//
//	OtTerrainHeight
//

class OtTerrainHeight {
public:
	// GUI to change properties
	bool renderGUI();

	// (de)serialize material
	nlohmann::json serialize(std::filesystem::path* basedir);
	void deserialize(nlohmann::json data, std::filesystem::path* basedir);

	// bind heightmap to specified sampler
	void bindHeightmap(OtSampler& sampler, int unit);

	// get heightmap size
	int getSize() { return heightmapSize; }

private:
	// properties
	int heightmapSize = 256;
	OtTileableFbm noise;
	bool dirty = true;

	// the heightmap on the GPU
	OtFrameBuffer heightmap{OtFrameBuffer::rFloat32Texture};

	// update the heightmap
	void update();
};
