//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <algorithm>
#include <memory>
#include <vector>

#include "glm/glm.hpp"


//
//	OtCurveClass
//

class OtCurveClass;
typedef std::shared_ptr<OtCurveClass> OtCurve;

class OtCurveClass {
public:
	// get a point on the curve at t [0, 1]
	virtual inline glm::vec2 getPoint(float t) {
		return glm::vec2(0.0f);
	}

	// get a point on the curve at distance n [0, 1]
	virtual inline glm::vec2 getPointAt(float u) {
		return getPoint(u2t(u));
	}

	// get sequence of points on curve
	inline void getPoints(std::vector<glm::vec2>& result, size_t division = 5) {
		result.clear();

		for (auto i = 0; i <= division; i++) {
			result.push_back(getPoint((float) i / (float) division));
		}
	}

	// get sequence of equally spaced points on curve
	inline void getSpacedPoints(std::vector<glm::vec2>& result, size_t division = 5) {
		result.clear();

		for (auto i = 0; i <= division; i++) {
			result.push_back(getPointAt((float) i / (float) division));
		}
	}

	// get a unit vector tangent at t [0, 1]
	virtual inline glm::vec2 getTangent(float t) {
		float delta = 0.0001f;

		return glm::normalize(
			getPoint(std::clamp(t - delta, 0.0f, 1.0f)) -
			getPoint(std::clamp(t - delta, 0.0f, 1.0f)));
	}

	virtual inline glm::vec2 getTangentAt(float u) {
		return getTangent(u2t(u));
	}

	// get the length of the curve
	virtual inline float getLength() {
		if (!lengths.size()) {
			getLengths();
		}

		return lengths.back();
	}

protected:
	// calculate list of cumulative segment lengths
	std::vector<float> lengths;

	inline void getLengths(size_t division=200) {
		glm::vec2 current = getPoint(0.0f);
		glm::vec2 last = current;
		float sum = 0.0f;
		lengths.push_back(0.0f);

		for (auto i = 1; i <= division; i++) {
			current = getPoint((float) i / (float) division);
			sum += glm::distance(current, last);
			lengths.push_back(sum);
			last = current;
		}
	}

	// convert distance u to equation parameter t
	inline float u2t(float u) {
		if (!lengths.size()) {
			getLengths();
		}

		float target = lengths.back() * u;
		size_t low = 0;
		size_t high = lengths.size() - 1;
		size_t i;

		while (low <= high) {
			i = (low + high) / 2;

			if (target > lengths[i]) {
				low = i + 1;

			} else if (target < lengths[i]) {
				high = i - 1;

			} else {
				return (float) i / (float) (lengths.size() - 1);
			}
		}

		float fraction = (target - lengths[high]) / (lengths[high + 1] - lengths[high]);
		return ((float) high + fraction) / (float) (lengths.size() - 1);
	}
};
