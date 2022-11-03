//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <vector>

#include "glm/glm.hpp"


//
//	Polygon helper functions
//


// return area of polygon
inline float OtPolygonArea(const std::vector<glm::vec2>& points) {
	auto n = points.size();
	float area = 0.0;

	for (size_t p = n - 1, q = 0; q < n; p = q++) {
		area += points[p].x * points[q].y - points[q].x * points[p].y;
	}

	return area * 0.5;
}


// see if polygon winding is clockwise
inline bool OtPolygonIsClockwise(const std::vector<glm::vec2>& points) {
	return OtPolygonArea(points) < 0;
}
