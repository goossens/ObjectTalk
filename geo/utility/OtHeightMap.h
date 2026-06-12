//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <algorithm>
#include <cmath>
#include <memory>
#include <string>

#include "glm/glm.hpp"


//
//	OtHeightMap
//

class OtHeightMap {
public:
	// create/update a heightmap
	void update(int w, int h, bool clear=false);

	// load heightmap from file
	void load(const std::string& path);

	// save heightmap to file
	void save(const std::string& path);

	// clone this heightmap
	OtHeightMap clone();

	// clear heightmap
	void clear();

	// see if heightmap is valid
	inline bool isValid() const { return heightmap != nullptr; }

	// set elevation at specified location
	void setElevation(int x, int y, float value) const;

	// adjust elevation t specified location
	void adjustElevation(int x, int y, float value) const;

	// get elevation at specified location
	inline float getElevation(int x, int y) const {
		x = std::clamp(x, 0, width - 1);
		y = std::clamp(y, 0, height - 1);
		return heightmap[y * width + x];
	}

	// get normal at specified location
	glm::vec3 getNormal(int x, int y) const;

	// sample interpolated elevation at relative location (coordinates are 0.0 to 1.0 on both axis)
	float sampleElevation(float x, float y) const;

	// sample interpolated normal at relative location (coordinates are 0.0 to 1.0 on both axis)
	glm::vec3 sampleNormal(float x, float y) const;

	// get minimum and maximum elevations
	float getMinElevation() const;
	float getMaxElevation() const;

	// get height map dimensions
	inline int getWidth() { return width; }
	inline int getHeight() { return height; }

	// access the error message
	inline void setErrorMessage(const std::string& message) { errorMessage = message; }
	inline std::string getErrorMessage() const { return errorMessage; }

	// version management
	inline void setVersion(int v) { version = v; }
	inline int getVersion() { return version; }
	inline void incrementVersion() { version++; }

	// see if heightmaps are identical
	inline bool operator==(OtHeightMap& rhs) {
		return heightmap == rhs.heightmap && version == rhs.version;
	}

	inline bool operator!=(OtHeightMap& rhs) {
		return !operator==(rhs);
	}

	// access erosion properties
	inline void setSeed(int value) { seed = value; }
	inline int getSeed() const { return seed; }

	inline void setDT(float value) { dt = value; }
	inline float getDT() const { return dt; }

	inline void setDensity(float value) { density = value; }
	inline float getDensity() const { return density; }

	inline void setEvaporationRate(float value) { evaporationRate = value; }
	inline float getEvaporationRate() const { return evaporationRate; }

	inline void setDepositionRate(float value) { depositionRate = value; }
	inline float getDepositionRate() const { return depositionRate; }

	inline void setMinimumVolume(float value) { minimumVolume = value; }
	inline float getMinimumVolume() const { return minimumVolume; }

	inline void setFriction(float value) { friction = value; }
	inline float getFriction() const { return friction; }

	// erode heightmap
	void erode(int run, int drops);

private:
	// heightmap properties
	std::shared_ptr<float[]> heightmap;
	int version = 0;
	int width = 0;
	int height = 0;
	std::string errorMessage;

	// erosion properties
	int seed = 1337;
	float dt = 1.2f;
	float density = 1.0f;
	float evaporationRate = 0.001f;
	float depositionRate = 0.1f;
	float minimumVolume = 0.01f;
	float friction = 0.05f;

	// single particle that started as a drop of water that carries sediment
	struct Particle {
		Particle(glm::vec2 pos) : pos(pos) {}

		glm::vec2 pos;
		glm::vec2 speed = glm::vec2(0.0f);

		float volume = 1.0f;
		float sediment = 0.0f;
	};
};
