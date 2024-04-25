//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "OtShapeSegment.h"


//
//	OtLineSegment
//

class OtLineSegment : public OtShapeSegment {
public:
	// constructor
	inline OtLineSegment(const glm::vec2& v1, const glm::vec2& v2) {
		p1 = v1;
		p2 = v2;
	}

	// get a point in the segment at curve parameter t [0, 1]
	inline glm::vec2 getPoint(float t) override {
		return glm::mix(p1, p2, t);
	}

	// get a point in the segment at arc length u [0, 1]
	inline glm::vec2 getPointAtArcLength(float u) override {
		return getPoint(u);
	}

	// get a unit vector tangent at t [0, 1]
	inline glm::vec2 getTangent(float t) override {
		return glm::normalize(p2 - p1);
	}

	virtual inline glm::vec2 getTangentAt(float u) override {
		return getTangent(u);
	}

	// get the length of the segment
	inline float getLength() override {
		return glm::distance(p1, p2);
	}

	// convert segment to string representation
	inline std::string toString() override {
		return fmt::format("l {} {}\n", p2.x, p2.y);
	}

private:
	// control points
	glm::vec2 p1, p2;
};
