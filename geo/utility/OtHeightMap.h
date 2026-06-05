//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

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

	// clear heightmap
	void clear();

	// see if heightmap is valid
	inline bool isValid() const { return heightmap != nullptr; }

	// set elevation at specified location
	void setElevation(int x, int y, float value) const;

	// get elevation at specified location
	float getElevation(int x, int y) const;

	// sample elevation at relative location (coordinates are 0.0 to 1.0 on both axis)
	float sampleElevation(float x, float y) const;

	// get normal at relative location (coordinates are 0.0 to 1.0 on both axis)
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

private:
	// properties
	std::shared_ptr<float[]> heightmap;
	int version = 0;
	int width = 0;
	int height = 0;
	std::string errorMessage;
};
