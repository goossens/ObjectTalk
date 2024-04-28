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

	// get the length of the segment
	inline float getLength() override {
		return glm::distance(p1, p2);
	}

	// get points on segment
	virtual void getPoints(std::vector<glm::vec2>& result) override {
		result.push_back(p2);
	}

	// convert segment to string representation
	inline std::string toString() override {
		return fmt::format("l {} {}\n", p2.x, p2.y);
	}

private:
	// control points
	glm::vec2 p1, p2;
};
