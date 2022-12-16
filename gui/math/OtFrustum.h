//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <array>

#include "glm/glm.hpp"

#include "OtAABB.h"
#include "OtMath.h"
#include "OtPlane.h"
#include "OtRenderer.h"


//
//	OtFrustum
//

class OtFrustumClass;
typedef std::shared_ptr<OtFrustumClass> OtFrustum;

class OtFrustumClass : public OtMathClass {
public:
	enum {
		left = 0,
		right,
		bottom,
		top,
		near,
		far,
		planeCount
	};

	enum {
		nearBottomLeft = 0,
		nearTopLeft,
		nearTopRight,
		nearBottomRight,
		farBottomLeft,
		farTopLeft,
		farTopRight,
		farBottomRight,
		pointCount
	};

	// update the frustum to match the provided matrix
	void update(const glm::mat4& matrix);

	// see if a point is visible
	bool isVisiblePoint(const glm::vec3& point);

	// see if an AABB box is visible
	bool isVisibleAABB(OtAABB aabb);

	// see if a sphere is visible
	bool isVisibleSphere(const glm::vec3& center, float radius);

	// get center of frustum
	glm::vec3 getCenter();

	// get specified corner
	glm::vec3 getCorner(int point) { return points[point]; }

	// get specified plane
	OtPlane getPlane(int plane) { return planes[plane]; }

	// render the frustum for debugging purposes
	void render(OtRenderer& renderer);

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtFrustum create();

private:
	std::array<OtPlane, planeCount> planes;
	std::array<glm::vec3, pointCount> points;
};
