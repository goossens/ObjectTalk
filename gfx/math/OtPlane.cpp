//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtGlm.h"
#include "OtPlane.h"


//
//	OtPlane::normalize
//

void OtPlane::normalize() {
	auto l = glm::length(normal);
	normal /= l;
	distance /= l;
}


//
//	OtPlane::negate
//

void OtPlane::negate() {
	normal *= -1;
	distance *= -1;
}


//
//	OtPlane::transform
//

OtPlane OtPlane::transform(const glm::mat4& transform) {
	// see https://stackoverflow.com/questions/7685495/transforming-a-3d-plane-using-a-4x4-matrix
	auto O = transform * glm::vec4(normal * distance, 1.0);
	auto N = glm::transpose(glm::inverse(transform)) * glm::vec4(normal, 0.0);
	return OtPlane(glm::vec3(N), glm::dot(glm::vec3(O), glm::vec3(N)));
}


//
//	OtPlane::debug
//

void OtPlane::debug() {
	OtGlmDebug(glm::vec4(normal, distance));
}
