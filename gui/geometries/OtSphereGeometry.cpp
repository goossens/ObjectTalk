//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <cmath>

#include "OtFunction.h"

#include "OtSphereGeometry.h"


//
//	OtSphereGeometryClass::~OtSphereGeometryClass
//

OtSphereGeometryClass::~OtSphereGeometryClass() {
	if (heightmap) {
		heightmap->detach(heightmapID);
	}
}


//
//	OtSphereGeometryClass::init
//

void OtSphereGeometryClass::init(size_t count, OtObject* parameters) {
	// set attributes
	if (count) {
		switch (count) {
			case 7:
				setThetaLength(parameters[6]->operator float());

			case 6:
				setThetaStart(parameters[5]->operator float());

			case 5:
				setPhiLength(parameters[4]->operator float());

			case 4:
				setPhiStart(parameters[3]->operator float());

			case 3:
				setHeightSegments(parameters[2]->operator int());

			case 2:
				setWidthSegments(parameters[1]->operator int());

			case 1:
				setRadius(parameters[0]->operator float());
				break;

			default:
				OtExcept("Too many parameters [%ld] for [SphereGeometry] constructor (max 7)", count);
		}

		refreshGeometry = true;
	}
}


//
//	OtSphereGeometryClass::setRadius
//

OtObject OtSphereGeometryClass::setRadius(float r) {
	radius = r;
	refreshGeometry = true;
	return shared();
}


//
//	OtSphereGeometryClass::setWidthSegments
//

OtObject OtSphereGeometryClass::setWidthSegments(int segments) {
	widthSegments = segments;
	refreshGeometry = true;
	return shared();
}


//
//	OtSphereGeometryClass::setHeightSegments
//

OtObject OtSphereGeometryClass::setHeightSegments(int segments) {
	heightSegments = segments;
	refreshGeometry = true;
	return shared();
}


//
//	OtSphereGeometryClass::setPhiStart
//

OtObject OtSphereGeometryClass::setPhiStart(float ps) {
	phiStart = ps;
	refreshGeometry = true;
	return shared();
}


//
//	OtSphereGeometryClass::setPhiLength
//

OtObject OtSphereGeometryClass::setPhiLength(float pl) {
	phiLength = pl;
	refreshGeometry = true;
	return shared();
}


//
//	OtSphereGeometryClass::setThetaStart
//

OtObject OtSphereGeometryClass::setThetaStart(float ts) {
	thetaStart = ts;
	refreshGeometry = true;
	return shared();
}


//
//	OtSphereGeometryClass::setThetaLength
//

OtObject OtSphereGeometryClass::setThetaLength(float tl) {
	thetaLength = tl;
	refreshGeometry = true;
	return shared();
}


//
//	OtSphereGeometryClass::setHeightMap
//

OtObject OtSphereGeometryClass::setHeightMap(OtObject object) {
	// sanity check
	object->expectKindOf("HeightMap");

	// cleanup if required
	if (heightmap) {
		heightmap->detach(heightmapID);
		heightmap = nullptr;
	}

	// set new heightmap
	heightmap = object->cast<OtHeightMapClass>();

	heightmapID = heightmap->attach([this]() {
		refreshGeometry = true;
	});

	refreshGeometry = true;
	return shared();
}


//
//	OtSphereGeometryClass::setTextureAbsolute
//

OtObject OtSphereGeometryClass::setTextureAbsolute(bool flag) {
	textureAbsolute = flag;
	refreshGeometry = true;
	return shared();
}


//
//	OtSphereGeometryClass::fillGeometry
//

void OtSphereGeometryClass::fillGeometry() {
	// get increments
	float ringDelta = thetaLength / heightSegments;
	float segDelta = phiLength / widthSegments;

	// address each ring
	for (auto ring = 0; ring <= heightSegments; ring++) {
		auto theta = thetaStart + ring * ringDelta;

		// address each segment
		for (auto seg = 0; seg <= widthSegments; seg++) {
			auto phi = phiStart + seg * segDelta;

			glm::vec2 uv = textureAbsolute
				? glm::vec2(phi / (std::numbers::pi * 2.0), theta / std::numbers::pi)
				: glm::vec2((float) seg / (float) widthSegments, (float) ring / (float) heightSegments);

			auto r = heightmap ? radius + heightmap->getHeight(uv.x, uv.y) : radius;
			auto r0 = r * std::sin(theta);
			auto y0 = r * std::cos(theta);
			auto x0 = r0 * -std::sin(phi);
			auto z0 = r0 * -std::cos(phi);

			// add vertex
			addVertex(OtVertex(
				glm::vec3(x0, y0, z0),
				glm::normalize(glm::vec3(x0, y0, z0)),
				uv));
		}
	}

	// add triangles and lines
	for (auto ring = 0; ring < heightSegments; ring++) {
		for (auto seg = 0; seg < widthSegments; seg++) {
			auto a = ring * (widthSegments + 1) + seg;
			auto b = a + (widthSegments + 1);
			auto c = b + 1;
			auto d = a + 1;

			addTriangle(a, b, d);
			addTriangle(b, c, d);

			if (ring == 0) {
				addLine(a, d);
			}

			addLine(a, b);
			addLine(b, c);
		}
	}
}


//
//	OtSphereGeometryClass::getMeta
//

OtType OtSphereGeometryClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtTypeClass::create<OtSphereGeometryClass>("SphereGeometry", OtGeometryClass::getMeta());
		type->set("__init__", OtFunctionClass::create(&OtSphereGeometryClass::init));
		type->set("setRadius", OtFunctionClass::create(&OtSphereGeometryClass::setRadius));
		type->set("setWidthSegments", OtFunctionClass::create(&OtSphereGeometryClass::setWidthSegments));
		type->set("setHeightSegments", OtFunctionClass::create(&OtSphereGeometryClass::setHeightSegments));
		type->set("setPhiStart", OtFunctionClass::create(&OtSphereGeometryClass::setPhiStart));
		type->set("setPhiLength", OtFunctionClass::create(&OtSphereGeometryClass::setPhiLength));
		type->set("setThetaStart", OtFunctionClass::create(&OtSphereGeometryClass::setThetaStart));
		type->set("setThetaLength", OtFunctionClass::create(&OtSphereGeometryClass::setThetaLength));
		type->set("setHeightMap", OtFunctionClass::create(&OtSphereGeometryClass::setHeightMap));
		type->set("setTextureAbsolute", OtFunctionClass::create(&OtSphereGeometryClass::setTextureAbsolute));
	}

	return type;
}


//
//	OtSphereGeometryClass::create
//

OtSphereGeometry OtSphereGeometryClass::create() {
	OtSphereGeometry spheregeometry = std::make_shared<OtSphereGeometryClass>();
	spheregeometry->setType(getMeta());
	return spheregeometry;
}
