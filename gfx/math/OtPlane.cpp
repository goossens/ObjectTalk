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
	distanceToOrigin /= l;
}


//
//	OtPlane::negate
//

void OtPlane::negate() {
	normal *= -1;
	distanceToOrigin *= -1;
}


//
//	OtPlane::transform
//

OtPlane OtPlane::transform(const glm::mat4& transform) {
	// see https://stackoverflow.com/questions/7685495/transforming-a-3d-plane-using-a-4x4-matrix
	auto O = transform * glm::vec4(normal * distanceToOrigin, 1.0f);
	auto N = glm::transpose(glm::inverse(transform)) * glm::vec4(normal, 0.0f);
	return OtPlane(glm::vec3(N), glm::dot(glm::vec3(O), glm::vec3(N)));
}


//
//	OtPlane::debug
//

void OtPlane::debug() {
	OtGlmDebug("Plane: ", glm::vec4(normal, distanceToOrigin));
}
