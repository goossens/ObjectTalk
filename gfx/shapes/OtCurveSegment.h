//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "glm/glm.hpp"

#include "OtShapeSegment.h"


//
//	OtCurveSegment
//

class OtCurveSegment : public OtShapeSegment {
	// get length of segment
	float getLength() override {return length; };

	// get points on segment
	void getPoints(std::vector<glm::vec2>& result) override {
		// determine point count based on size
		int points = int(length / 0.1f);

		for (auto i = 1; i <= points; i++) {
			result.push_back(getPoint((float) i / (float) points));
		}
	}

protected:
	// get a point in the segment at curve parameter t [0, 1]
	virtual glm::vec2 getPoint(float t) = 0;

	// calculate length of segment
	inline void calculateLength(size_t division=100) {
		glm::vec2 last = getPoint(0.0f);

		for (auto i = 1; i <= division; i++) {
			glm::vec2 current = getPoint((float) i / (float) division);
			length += glm::distance(current, last);
			last = current;
		}
	}

	// total segment length
	float length = 0.0f;
};
