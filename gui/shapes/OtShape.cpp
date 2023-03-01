//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtException.h"
#include "OtFunction.h"

#include "OtShape.h"
#include "OtFont.h"


//
//	OtShapeClass::moveTo
//

OtObject OtShapeClass::moveTo(float x, float y) {
	if (currentPath && currentPath->hasSegments()) {
		currentPath->close();
	}

	if (!currentPath || currentPath->hasSegments()) {
		currentPath = std::make_shared<OtCurvePathClass>();
		paths.push_back(currentPath);
	}

	currentPath->moveTo(glm::vec2(x, y));
	return OtObject(this);
}


//
//	OtShapeClass::lineTo
//

OtObject OtShapeClass::lineTo(float x, float y) {
	if (!currentPath) {
		currentPath = std::make_shared<OtCurvePathClass>();
		paths.push_back(currentPath);
	}

	currentPath->lineTo(glm::vec2(x, y));
	return OtObject(this);
}


//
//	OtShapeClass::quadraticCurveTo
//

OtObject OtShapeClass::quadraticCurveTo(float cx, float cy, float x, float y) {
	if (!currentPath) {
		currentPath = std::make_shared<OtCurvePathClass>();
		paths.push_back(currentPath);
	}

	currentPath->quadraticCurveTo(glm::vec2(cx, cy), glm::vec2(x, y));
	return OtObject(this);
}


//
//	OtShapeClass::quadBezierTo
//

OtObject OtShapeClass::bezierCurveTo(float cx1, float cy1, float cx2, float cy2, float x, float y) {
	if (!currentPath) {
		currentPath = std::make_shared<OtCurvePathClass>();
		paths.push_back(currentPath);
	}

	currentPath->bezierCurveTo(glm::vec2(cx1, cy1), glm::vec2(cx2, cy2), glm::vec2(x, y));
	return OtObject(this);
}


//
//	OtShapeClass::close
//

OtObject OtShapeClass::close() {
	if (!currentPath || currentPath->hasSegments()) {
		if (currentPath) {
			currentPath->close();
		}

		currentPath = std::make_shared<OtCurvePathClass>();
		paths.push_back(currentPath);
	}

	return OtObject(this);
}


//
//	OtShapeClass::circle
//

OtObject OtShapeClass::circle(float x, float y, float radius, bool clockwise) {
	close();
	currentPath->circle(x, y, radius, clockwise);
	return OtObject(this);
}


//
//	OtShapeClass::text
//

OtObject OtShapeClass::text(OtObject object, const std::string& text) {
	object->expectKindOf("Font");
	OtFont(object)->createShape(OtShape(this), text);
	return OtObject(this);
}


//
//	OtShapeClass::getMeta
//

OtType OtShapeClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtTypeClass::create<OtShapeClass>("Shape", OtGuiClass::getMeta());
		type->set("moveTo", OtFunction::create(&OtShapeClass::moveTo));
		type->set("lineTo", OtFunction::create(&OtShapeClass::lineTo));
		type->set("quadraticCurveTo", OtFunction::create(&OtShapeClass::quadraticCurveTo));
		type->set("bezierCurveTo", OtFunction::create(&OtShapeClass::bezierCurveTo));
		type->set("close", OtFunction::create(&OtShapeClass::close));

		type->set("circle", OtFunction::create(&OtShapeClass::circle));
		type->set("text", OtFunction::create(&OtShapeClass::text));
	}

	return type;
}
