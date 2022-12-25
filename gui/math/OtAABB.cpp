//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <algorithm>
#include <limits>

#include "OtAABB.h"


//
//	OtAABB::OtAABB
//

OtAABB::OtAABB(){
	clear();
}


//
//	OtAABB::clear
//

void OtAABB::clear() {
	float minv = std::numeric_limits<float>::lowest();
	float maxv = std::numeric_limits<float>::max();

	minp = glm::vec3(maxv, maxv, maxv);
	maxp = glm::vec3(minv, minv, minv);
}


//
//	OtAABB::addPoint
//

void OtAABB::addPoint(const glm::vec3& point) {
	minp.x = std::min(minp.x, point.x);
	minp.y = std::min(minp.y, point.y);
	minp.z = std::min(minp.z, point.z);
	maxp.x = std::max(maxp.x, point.x);
	maxp.y = std::max(maxp.y, point.y);
	maxp.z = std::max(maxp.z, point.z);
}


//
//	OtAABB::transform
//

OtAABB OtAABB::transform(const glm::mat4& matrix) {
	glm::vec3 points[8] = {
		{ minp.x, minp.y, minp.z },
		{ minp.x, minp.y, maxp.z },
		{ minp.x, maxp.y, maxp.z },
		{ minp.x, maxp.y, minp.z },

		{ maxp.x, minp.y, minp.z },
		{ maxp.x, minp.y, maxp.z },
		{ maxp.x, maxp.y, maxp.z },
		{ maxp.x, maxp.y, minp.z }
	};

	OtAABB aabb;

	for (auto i = 0; i < 6; i++) {
		aabb.addPoint(matrix * glm::vec4(points[i], 1.0));
	}

	return aabb;
}
