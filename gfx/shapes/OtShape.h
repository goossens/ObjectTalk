//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
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

	// see if path is valid
	inline bool isValid() { return getPolygonCount() != 0; }

	// shape I/O
	void load(const std::string& path);
	void save(const std::string& path);

	// construct a segment
	OtShape* moveTo(float x, float y);
	OtShape* lineTo(float x, float y);
	OtShape* quadraticCurveTo(float cx, float cy, float x, float y);
	OtShape* bezierCurveTo(float cx1, float cy1, float cx2, float cy2, float x, float y);
	OtShape* close();

	// add simple geometries
	OtShape* circle(float x, float y, float radius);
	OtShape* text(OtFont& font, const std::string& text, float size);

	// get number of polygons in shape
	inline size_t getPolygonCount() {
		return paths ? paths->size() : 0;
	}

	// get Nth polygon
	inline void getPolygon(std::vector<glm::vec2>& points, size_t n, size_t division) {
		return (*paths)[n].getSpacedPoints(points, division);
	}

	// return area of polygon
	static inline float getPolygonArea(const std::vector<glm::vec2>& points) {
		auto n = points.size();
		float area = 0.0f;

		for (size_t p = n - 1, q = 0; q < n; p = q++) {
			area += points[p].x * points[q].y - points[q].x * points[p].y;
		}

		return area * 0.5f;
	}

	// see if polygon winding is clockwise
	static inline bool isPolygonClockwise(const std::vector<glm::vec2>& points) {
		return getPolygonArea(points) < 0;
	}

	// version management
	inline void setVersion(int v) { version = v; }
	inline int getVersion() { return version; }
	inline void incrementVersion() { version++; }

	// see if shapes are identical
	inline bool operator==(OtShape& rhs) {
		return paths == rhs.paths && version == rhs.version;
	}

	inline bool operator!=(OtShape& rhs) {
		return !operator==(rhs);
	}

private:
	std::shared_ptr<std::vector<OtShapePath>> paths;
	OtShapePath currentPath;
	int version = 0;
};
