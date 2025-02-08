//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <memory>
#include <string>
#include <vector>

#include "glm/glm.hpp"

#include "OtFont.h"
#include "OtShapePath.h"


//
//	OtShape
//

class OtShape {
public:
	// clear the shape
	inline void clear() {
		if (paths) {
			paths.reset();
			currentPath.clear();
			incrementVersion();
		}
	}

	// see if shape is valid
	inline bool isValid() { return getPathCount() != 0; }

	// shape I/O
	void load(const std::string& path);
	void save(const std::string& path);

	// construct a path
	void moveTo(float x, float y);
	void lineTo(float x, float y);
	void quadraticCurveTo(float cx, float cy, float x, float y);
	void bezierCurveTo(float cx1, float cy1, float cx2, float cy2, float x, float y);
	void close();

	// add simple geometries
	void circle(float x, float y, float radius);
	void text(OtFont& font, const std::string& text, float size, bool center);

	// get number of paths in shape
	inline size_t getPathCount() {
		return paths ? paths->size() : 0;
	}

	// get Nth path
	inline void getPath(std::vector<glm::vec2>& points, size_t n) {
		return (*paths)[n].getPoints(points);
	}

	// return area of path
	static inline float getPathArea(const std::vector<glm::vec2>& points) {
		auto n = points.size();
		float area = 0.0f;

		for (size_t p = n - 1, q = 0; q < n; p = q++) {
			area += points[p].x * points[q].y - points[q].x * points[p].y;
		}

		return area * 0.5f;
	}

	// see if path winding is clockwise
	static inline bool isPathClockwise(const std::vector<glm::vec2>& points) {
		return getPathArea(points) < 0.0f;
	}

	// version management
	inline void setVersion(int v) { version = v; }
	inline int getVersion() { return version; }
	inline void incrementVersion() { version++; }

	// see if shapes are identical
	inline bool operator==(OtShape& rhs) {
		return version == rhs.version;
	}

	inline bool operator!=(OtShape& rhs) {
		return !operator==(rhs);
	}

private:
	std::shared_ptr<std::vector<OtShapePath>> paths;
	OtShapePath currentPath;
	int version = 0;
};
