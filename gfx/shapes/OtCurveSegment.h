//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
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
	// get points on segment
	void getPoints(std::vector<glm::vec2>& result) override {
		// determine point count based on size
		int points = int(getLength() / 0.1f);

		for (auto i = 1; i <= points; i++) {
			result.push_back(getPoint((float) i / (float) points));
		}
	}

protected:
	// get a point in the segment at curve parameter t [0, 1]
	virtual glm::vec2 getPoint(float t) = 0;

	// get approximate length of curve
	virtual float getLength() = 0;
};
