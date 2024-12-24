//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
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
#include "OtPlane.h"


//
//	OtFrustum
//

class OtFrustum {
public:
	enum class Plane {
		leftPlane,
		rightPlane,
		bottomPlane,
		topPlane,
		nearPlane,
		farPlane,
		count
	};

	enum class Corner{
		nearBottomLeft = 0,
		nearTopLeft,
		nearTopRight,
		nearBottomRight,
		farBottomLeft,
		farTopLeft,
		farTopRight,
		farBottomRight,
		count
	};

	// constructors
	OtFrustum();
	OtFrustum(const glm::mat4& matrix);

	// see if a point is visible
	bool isVisiblePoint(const glm::vec3& point);

	// see if an AABB box is visible
	bool isVisibleAABB(const OtAABB& aabb);

	// see if a sphere is visible
	bool isVisibleSphere(const glm::vec3& center, float radius);

	// get center of frustum
	glm::vec3 getCenter();

	// get specified corner
	inline glm::vec3 getCorner(Corner corner) const { return corners[static_cast<size_t>(corner)]; }

	// get specified plane
	inline OtPlane getPlane(Plane plane) const { return planes[static_cast<size_t>(plane)]; }

	// get AABB (Axis-Alligned bounding box)
	OtAABB getAABB();

	// debugging support
	void debugPlanes();
	void debugPoints();

private:
	std::array<OtPlane, static_cast<size_t>(Plane::count)> planes;
	std::array<glm::vec3, static_cast<size_t>(Corner::count)> corners;
};
