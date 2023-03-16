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
#include <memory>

#include "glm/glm.hpp"


//
//	OtHeightMap
//

class OtHeightMap {
public:
	// load heightmap from file
	void load(const std::filesystem::path& path);

	// clear heightmap
	void clear() { heightmap = nullptr; }

	// see if heightmap is valid
	bool isValid() { return heightmap != nullptr; }

	// get height (0.0 to 1.0) at specified location (in relative coordinates, 0.0 to 1.0)
	float getHeight(float x, float y);

	// get normal (as normalized vector) at specified location (in relative coordinates, 0.0 to 1.0)
	glm::vec3 getNormal(float x, float y);

private:
	// get height at pixel location
	float getHeightAbs(int x, int y);

	// properties
	int width;
	int height;
	std::unique_ptr<float[]> heightmap = nullptr;
};
