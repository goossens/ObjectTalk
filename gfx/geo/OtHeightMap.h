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

	// void load(const std::string& path, float minValue, float maxValue);

	// clear heightmap
	inline void clear() {
		heightmap = nullptr;
		width = 0;
		height = 0;
	}

	// see if heightmap is valid
	inline bool isValid() const { return heightmap != nullptr; }

	// set height at specified location
	void setHeight(int x, int y, float value) const;

	// get height at absolute location
	float getHeight(int x, int y) const;

	// get height at specified location (in relative coordinates, 0.0 to 1.0)
	float getHeight(float x, float y) const;

	// get normal (as normalized vector) at specified location (in relative coordinates, 0.0 to 1.0)
	glm::vec3 getNormal(float x, float y) const;

	// get minimum and maximum heights
	void getMinMaxHeights(float& minHeight, float& maxHeight) const;

	// access the error message
	inline void setErrorMessage(const std::string& message) { errorMessage = message; }
	inline std::string getErrorMessage() const { return errorMessage; }

private:
	// properties
	std::unique_ptr<float[]> heightmap = nullptr;
	int width = 0;
	int height = 0;
	std::string errorMessage;
};
