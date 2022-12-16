//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "glm/glm.hpp"

#include "OtRenderer.h"
#include "OtMath.h"


//
//	OtAABB (Axis-Alligned bounding box)
//

class OtAABBClass;
typedef std::shared_ptr<OtAABBClass> OtAABB;

class OtAABBClass : public OtMathClass {
public:
	// constructor
	OtAABBClass();

	// clear the bounding box
	void clear();

	// add a point to the bounding box
	void addPoint(const glm::vec3& point);

	// get min/max values
	glm::vec3 getMin() { return minp; }
	glm::vec3 getMax() { return maxp; }

	// transform bounding box
	OtAABB transform(const glm::mat4& matrix);

	// render the bounding box for debugging purposes
	void render(OtRenderer& renderer);

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtAABB create();

private:
	glm::vec3 minp = glm::vec3(0.0);
	glm::vec3 maxp = glm::vec3(0.0);
};
