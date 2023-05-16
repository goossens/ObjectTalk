//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <cstring>

#include "FastNoiseLite.h"

#include "OtNumbers.h"

#include "OtTerrainHeights.h"


//
//	OtTerrainHeights::OtTerrainHeights
//

OtTerrainHeights::OtTerrainHeights() {
	generate();
}


//
//	OtTerrainHeights::generate
//

void OtTerrainHeights::generate() {
	std::memset(points, 0, sizeof(points));
	generateNoise();
	generateNormals();
	heightmap.loadFromMemory(OtTerrainSize, OtTerrainSize, (float*) points);
}


//
//	OtTerrainHeights::generateNoise
//

void OtTerrainHeights::generateNoise() {
	FastNoiseLite noise;
	noise.SetFractalType(FastNoiseLite::FractalType::FractalType_FBm);
	noise.SetSeed(seed);
	noise.SetFrequency(frequency);
	noise.SetFractalOctaves(octaves);
	noise.SetFractalLacunarity(lacunarity);
	noise.SetFractalGain(persistance);

	auto p = points;

	for (auto y = 0; y < OtTerrainSize; y++) {
		for (auto x = 0; x < OtTerrainSize; x++) {
			(p++)->height = noise.GetNoise(factor * x / OtTerrainSize, factor * y / OtTerrainSize);
		}
	}
}


//
//	OtTerrainHeights::generateRepeat
//

void OtTerrainHeights::generateRepeat() {
	auto band = (int) (OtTerrainSize * overlap);
	auto step = 1.0f / band;

	for (auto y = 0; y < OtTerrainSize; y++) {
		for (auto x = 0; x < band; x++) {
			setHeight(x, y, std::lerp(getHeight(OtTerrainSize - x - 1, y), getHeight(x, y), x * step));
		}
	}

	for (auto y = 0; y < band; y++) {
		for (auto x = 0; x < OtTerrainSize; x++) {
			setHeight(x, y, std::lerp(getHeight(x, OtTerrainSize - y - 1), getHeight(x, y), y * step));
		}
	}
}


//
//	OtTerrainHeights::generateNormals
//

void OtTerrainHeights::generateNormals() {
	float sq2 = std::sqrt(2.0f);

	for (auto x = 0; x < OtTerrainSize; x++) {
		for (auto y = 0; y < OtTerrainSize; y++) {
			float tl = getHeight(x - 1, y - 1);
			float t = getHeight(x, y - 1);
			float tr = getHeight(x + 1, y - 1);
			float l = getHeight(x - 1, y);
			float c = getHeight(x, y);
			float r = getHeight(x + 1, y);
			float bl = getHeight(x - 1, y + 1);
			float b = getHeight(x, y + 1);
			float br = getHeight(x + 1, y + 1);

			glm::vec3 n(0.0f);
			n += glm::vec3(0.15f) * glm::normalize(glm::vec3(c - r, 1.0, 0.0));
			n += glm::vec3(0.15f) * glm::normalize(glm::vec3((l - c), 1.0, 0.0));
			n += glm::vec3(0.15f) * glm::normalize(glm::vec3(0.0, 1.0, (c - b)));
			n += glm::vec3(0.15f) * glm::normalize(glm::vec3(0.0, 1.0, (t - c)));

			n += glm::vec3(0.1f) * glm::normalize(glm::vec3((c - br) / sq2, sq2, (c - br) / sq2));
			n += glm::vec3(0.1f) * glm::normalize(glm::vec3((c - tr) / sq2, sq2, (c - tr) / sq2));
			n += glm::vec3(0.1f) * glm::normalize(glm::vec3((c - bl) / sq2, sq2, (c - bl) / sq2));
			n += glm::vec3(0.1f) * glm::normalize(glm::vec3((c - tl) / sq2, sq2, (c - tl) / sq2));
		}
	}
}
