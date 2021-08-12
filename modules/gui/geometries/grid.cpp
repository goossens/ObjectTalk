//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "ot/function.h"

#include "grid.h"


//
//	OtGridClass::OtGridClass()
//

OtGridClass::OtGridClass() {
	createTriangles();
}


//
//	OtGridClass::init
//

OtObject OtGridClass::init(size_t count, OtObject* parameters) {
	// set attributes
	if (count) {
		switch (count) {
			case 3:
				size = parameters[1]->operator double();

			case 2:
				columns = parameters[1]->operator int();

			case 1:
				rows = parameters[0]->operator int();
				break;

			default:
				OtExcept("Too many parameters [%ld] for [Grid] contructor (max 3)", count);
		}

		createTriangles();
	}

	return nullptr;
}


//
//	OtGridClass::createTriangles
//

void OtGridClass::createTriangles() {
	// clear vertex/triangle lists
	clear();

	// calculate starting points
	auto x0 = -size * columns / 2.0;
	auto z0 = -size * rows / 2.0;
	int index= 0;

	// process all rows
	for (auto r = 0; r <= rows; r++) {
		addVertex(OtVertex(
			glm::vec3(x0, 0.0, z0 + r * size),
			glm::vec3(0.0, 1.0, 0.0),
			glm::vec2(0.0, (float) r / (float) rows)));

		addVertex(OtVertex(
			glm::vec3(-x0, 0.0, z0 + r * size),
			glm::vec3(0.0, 1.0, 0.0),
			glm::vec2(1.0, (float) r / (float) rows)));

		addTriangle(index, index + 1,  index + 1);
		index += 2;
	}

	// process all columns
	for (auto c = 0; c <= columns; c++) {
		addVertex(OtVertex(
			glm::vec3(x0 + c * size, 0.0, z0),
			glm::vec3(0.0, 1.0, 0.0),
			glm::vec2((float) c / (float) columns, 0.0)));

		addVertex(OtVertex(
			glm::vec3(x0 + c * size, 0.0, -z0),
			glm::vec3(0.0, 1.0, 0.0),
			glm::vec2((float) c / (float) columns, 1.0)));

		addTriangle(index, index + 1,  index + 1);
		index += 2;
	}
}


//
//	OtGridClass::getMeta
//

OtType OtGridClass::getMeta() {
	static OtType type = nullptr;

	if (!type) {
		type = OtTypeClass::create<OtGridClass>("Grid", OtGeometryClass::getMeta());
		type->set("__init__", OtFunctionClass::create(&OtGridClass::init));
	}

	return type;
}


//
//	OtGridClass::create
//

OtGrid OtGridClass::create() {
	OtGrid grid = std::make_shared<OtGridClass>();
	grid->setType(getMeta());
	return grid;
}
