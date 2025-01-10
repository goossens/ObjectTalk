//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
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
//	OtShapeSegment
//

class OtShapeSegment {
public:
	// get length of segment
	virtual float getLength() = 0;

	// get points on segment
	virtual void getPoints(std::vector<glm::vec2>& result) = 0;

	// convert segment to string representation
	virtual std::string toString() = 0;
};
