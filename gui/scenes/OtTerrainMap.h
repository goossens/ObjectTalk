//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <cstdint>

#include "glm/glm.hpp"

#include "OtCache.h"

#include "OtController.h"
#include "OtGui.h"
#include "OtEasing.h"
#include "OtPerlin.h"


//
//	OtTerrainMapClass
//

class OtTerrainMapClass;
typedef std::shared_ptr<OtTerrainMapClass> OtTerrainMap;

class OtTerrainMapClass : public OtGuiClass {
public:
	// constructor
	OtTerrainMapClass();

	// initialize
	OtObject init(size_t count, OtObject* parameters);

	// provide a new seed for the perlin noise generator
	OtObject setSeed(int seed);

	// set number of perlin octaves
	OtObject setOctaves(int octaves);

	// set perlin persistence
	OtObject setPersistence(float persistence);

	// set the noisemap scaling factors
	OtObject setScaleXY(float xy);

	// set the noisemap scaling factors
	OtObject setScaleZ(float z);

	// set vertical noisemap offset (after scaling)
	OtObject setOffsetZ(float z);

	// set the noisemap easing function
	OtObject setEasing(int easing);

	// get minimum height
	float getMinHeight();

	// get maximum height
	float getMaxHeight();

	// get height at location
	float getHeight(int32_t x, int32_t y);

	// get normal at location
	glm::vec3 getNormal(int32_t x, int32_t y);

	// GUI to change properties
	void renderGUI();

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtTerrainMap create();

private:
	// generate noisemap from perlin noise
	OtPerlin perlin;
	int currentSeed = 237;
	int octaves = 8;
	float persistence = 0.5;

	// noisemap properties
	float scaleXY = 200.0;
	float scaleZ = 1.0;
	float offsetZ = 0.0;

	int easing = 0;
	OtEasingFunction easingFunction = OtEasingGetFunction(0);

	// function to handle property changes
	void propertiesChanged();

	// height cache
	OtCache<uint64_t, float, 1000000> cache;
	bool cacheDirty = false;

	// create cache hash from coordinates
	size_t hash(int32_t x, int32_t y) {
		union {
			struct {
				int32_t x;
				int32_t y;
			};

			uint64_t hash;
		} h;

		h.x = x;
		h.y = y;
		return h.hash;
	}
};


//
//	Controller widget
//

OT_CONTROLLER(TerrainMap)
