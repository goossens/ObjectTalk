//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtException.h"
#include "OtFunction.h"

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
