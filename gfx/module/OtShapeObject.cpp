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
		type->set("arcTo", OtFunction::create(&OtShapeClass::arcTo));
		type->set("close", OtFunction::create(&OtShapeClass::close));

		type->set("rect", OtFunction::create(&OtShapeClass::rect));
		type->set("roundedRect", OtFunction::create(&OtShapeClass::roundedRect));
		type->set("circle", OtFunction::create(&OtShapeClass::circle));
		type->set("ellipse", OtFunction::create(&OtShapeClass::ellipse));
		type->set("arc", OtFunction::create(&OtShapeClass::arc));

		type->set("transform", OtFunction::create(&OtShapeClass::transform));
		type->set("translate", OtFunction::create(&OtShapeClass::translate));
		type->set("rotate", OtFunction::create(&OtShapeClass::rotate));
		type->set("scale", OtFunction::create(&OtShapeClass::scale));
		type->set("shear", OtFunction::create(&OtShapeClass::shear));
	}

	return type;
}