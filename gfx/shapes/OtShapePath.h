//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <memory>
#include <vector>

#include "fmt/core.h"
#include "glm/glm.hpp"

#include "OtAssert.h"

#include "OtCubicBezierSegment.h"
#include "OtLineSegment.h"
#include "OtQuadraticBezierSegment.h"
#include "OtShapeSegment.h"


//
//	OtShapePath
//

class OtShapePath {
public:
	// clear the path
	inline void clear() {
		segments.clear();
		startPoint = glm::vec2(0.0);
		currentPoint = glm::vec2(0.0);
		state = State::undefined;
	}

	// create a path out of segments
	inline void moveTo(const glm::vec2& point) {
		OtAssert(state == State::undefined);
		state = State::started;
		startPoint = point;
		currentPoint = point;
	}

	inline void lineTo(const glm::vec2& point){
		OtAssert(state == State::started || state == State::building);
		state = State::building;
		add(std::make_shared<OtLineSegment>(currentPoint, point));
		currentPoint = point;
	}

	inline void quadraticCurveTo(const glm::vec2& control, const glm::vec2& point) {
		OtAssert(state == State::started || state == State::building);
		state = State::building;
		add(std::make_shared<OtQuadraticBezierSegment>(currentPoint, control, point));
		currentPoint = point;
	}

	inline void bezierCurveTo(const glm::vec2& control1, const glm::vec2& control2, const glm::vec2& point) {
		OtAssert(state == State::started || state == State::building);
		state = State::building;
		add(std::make_shared<OtCubicBezierSegment>(currentPoint, control1, control2, point));
		currentPoint = point;
	}

	inline void close() {
		OtAssert(state == State::building);

		if (startPoint != currentPoint) {
			moveTo(startPoint);
		}

		state = State::closed;
	}

	// get the length of the segment
	inline float getLength() {
		return currentLength;
	}

	// see if we have completed segments
	inline bool hasSegments() {
		return segments.size();
	}

	// get points on entire shape
	void getPoints(std::vector<glm::vec2>& result) {
		OtAssert(state == State::closed);
		result.clear();
		result.push_back(startPoint);

		for (auto& segment : segments) {
			segment->getPoints(result);
		}
	}

	std::string toString() {
		std::string result = fmt::format("m {} {}\n", startPoint.x, startPoint.y);

		for (auto& segment : segments) {
			result += segment->toString();
		}

		result += "z\n";
		return result;
	}

private:
	// add a segment
	inline void add(std::shared_ptr<OtShapeSegment> segment) {
		currentLength += segment->getLength();
		segments.push_back(segment);
	}

	// properties
	enum class State {
		undefined,
		started,
		building,
		closed
	};

	State state = State::undefined;

	std::vector<std::shared_ptr<OtShapeSegment>> segments;
	glm::vec2 startPoint = glm::vec2(0.0);
	glm::vec2 currentPoint = glm::vec2(0.0);
	float currentLength = 0.0;
};
