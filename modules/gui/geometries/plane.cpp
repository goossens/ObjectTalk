//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "ot/function.h"

#include "plane.h"


//
//	OtPlaneClass::OtPlaneClass()
//

OtPlaneClass::OtPlaneClass() {
	createTriangles();
}

OtPlaneClass::OtPlaneClass(double w, double h) {
	width = w;
	height = h;
	createTriangles();
}


//
//	OtPlaneClass::init
//

OtObject OtPlaneClass::init(size_t count, OtObject* parameters) {
	// set attributes
	if (count) {
		switch (count) {
			case 2:
				height = parameters[1]->operator double();

			case 1:
				width = parameters[0]->operator double();
				break;

			default:
				OtExcept("Too many parameters [%ld] for [Plane] contructor (max 2)", count);
		}

		createTriangles();
	}

	return nullptr;
}


//
//	OtPlaneClass::createTriangles
//

void OtPlaneClass::createTriangles() {
	// clear vertex/triangle lists
	clear();

	// add vertices
	double w = width / 2.0;
	double h = height / 2.0;
	glm::vec3 normal = glm::vec3(0.0, 0.0, 1.0);

	addVertex(OtVertex(glm::vec3(-w, h, 0.0), normal, glm::vec2(0.0, 0.0)));
	addVertex(OtVertex(glm::vec3(w, h, 0.0), normal, glm::vec2(1.0, 0.0)));
	addVertex(OtVertex(glm::vec3(w, -h, 0.0), normal, glm::vec2(1.0, 1.0)));
	addVertex(OtVertex(glm::vec3(-w, -h, 0.0), normal, glm::vec2(0.0, 1.0)));

	// add triangles
	addTriangle(3, 1, 0);
	addTriangle(3, 2, 1);
}


//
//	OtPlaneClass::getMeta
//

OtType OtPlaneClass::getMeta() {
	static OtType type = nullptr;

	if (!type) {
		type = OtTypeClass::create<OtPlaneClass>("Plane", OtGeometryClass::getMeta());
		type->set("__init__", OtFunctionClass::create(&OtPlaneClass::init));
	}

	return type;
}


//
//	OtPlaneClass::create
//

OtPlane OtPlaneClass::create() {
	OtPlane plane = std::make_shared<OtPlaneClass>();
	plane->setType(getMeta());
	return plane;
}

OtPlane OtPlaneClass::create(double width, double height) {
	OtPlane plane = std::make_shared<OtPlaneClass>(width, height);
	plane->setType(getMeta());
	return plane;
}
