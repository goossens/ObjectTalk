//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "glm/glm.hpp"

#include "OtTexture.h"

#include "OtTerrainConstants.h"


//
//	OtTerrainHeights
//

class OtTerrainHeights {
public:
	// constructor
	OtTerrainHeights();

	// access height and normal
	inline float getHeight(int x, int y) {
		return points[(wrap(y) * OtTerrainSize) + wrap(x)].height;
	}

	inline void setHeight(int x, int y, float height) {
		points[(wrap(y) * OtTerrainSize) + wrap(x)].height = height;
	}

private:
	// wrap around function for coordinates
	inline int wrap(int i) {
		return (i % OtTerrainSize + OtTerrainSize) %OtTerrainSize;
	}

	// generate a new heightmap with normals
	void generate();
	void generateNoise();
	void generateRepeat();
	void generateNormals();

private:
	// heights and normals
	struct Point {
		glm::vec3 normal;
		float height;
	};

	Point points[OtTerrainSize * OtTerrainSize];

	// noise displacement properties
	int seed = 1337;
	int octaves = 3;
	float frequency = 0.01f;
	float lacunarity = 1.0f;
	float persistance = 0.5f;
	float factor = 1.0f;
	float overlap = 0.25f;

	// texture containing heights and normals
	OtTexture heightmap;
};
