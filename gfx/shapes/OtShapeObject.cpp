//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtFunction.h"

#include "OtShapeObject.h"


//
//	OtShapeClass::getMeta
//

OtType OtShapeClass::getMeta() {
	static OtType type = nullptr;

	if (!type) {
		type = OtType::create<OtShapeClass>("Shape", OtObjectClass::getMeta());

		type->set("moveTo", OtFunction::create(&OtShapeClass::moveTo));
		type->set("lineTo", OtFunction::create(&OtShapeClass::lineTo));
		type->set("bezierCurveTo", OtFunction::create(&OtShapeClass::bezierCurveTo));
		type->set("quadraticCurveTo", OtFunction::create(&OtShapeClass::quadraticCurveTo));
		type->set("close", OtFunction::create(&OtShapeClass::close));
	}

	return type;
}