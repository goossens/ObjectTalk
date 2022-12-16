//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <limits>

#include "OtAABB.h"


//
//	OtAABBClass::OtAABBClass
//

OtAABBClass::OtAABBClass(){
	clear();
}


//
//	OtAABBClass::clear
//

void OtAABBClass::clear() {
	float minv = std::numeric_limits<float>::lowest();
	float maxv = std::numeric_limits<float>::max();

	minp = glm::vec3(maxv, maxv, maxv);
	maxp = glm::vec3(minv, minv, minv);
}


//
//	OtAABBClass::addPoint
//

void OtAABBClass::addPoint(const glm::vec3& point) {
	minp.x = std::min(minp.x, point.x);
	minp.y = std::min(minp.y, point.y);
	minp.z = std::min(minp.z, point.z);
	maxp.x = std::max(maxp.x, point.x);
	maxp.y = std::max(maxp.y, point.y);
	maxp.z = std::max(maxp.z, point.z);
}


//
//	OtAABBClass::transform
//

OtAABB OtAABBClass::transform(const glm::mat4& matrix) {
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

	OtAABB aabb = create();

	for (auto i = 0; i < 6; i++) {
		aabb->addPoint(points[i]);
	}

	return aabb;
}


//
//	OtAABBClass::render
//

void OtAABBClass::render(OtRenderer& renderer) {
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

	renderer.debugSetColor(0xff00ff00);
	renderer.debugMoveTo(points[0]);
	renderer.debugLineTo(points[1]);
	renderer.debugLineTo(points[2]);
	renderer.debugLineTo(points[3]);
	renderer.debugLineTo(points[0]);

	renderer.debugMoveTo(points[4]);
	renderer.debugLineTo(points[5]);
	renderer.debugLineTo(points[6]);
	renderer.debugLineTo(points[7]);
	renderer.debugLineTo(points[4]);

	renderer.debugMoveTo(points[0]);
	renderer.debugLineTo(points[4]);

	renderer.debugMoveTo(points[1]);
	renderer.debugLineTo(points[5]);

	renderer.debugMoveTo(points[2]);
	renderer.debugLineTo(points[6]);

	renderer.debugMoveTo(points[3]);
	renderer.debugLineTo(points[7]);
}


//
//	OtAABBClass::getMeta
//

OtType OtAABBClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtTypeClass::create<OtAABBClass>("AABB", OtMathClass::getMeta());
	}

	return type;
}


//
//	OtAABBClass::create
//

OtAABB OtAABBClass::create() {
	OtAABB aabb = std::make_shared<OtAABBClass>();
	aabb->setType(getMeta());
	return aabb;
}
