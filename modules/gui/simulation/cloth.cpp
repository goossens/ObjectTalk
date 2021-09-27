//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "ot/function.h"

#include "application.h"
#include "cloth.h"


//
//	Helpers
//

#define INDEX(x, y) ((y) * (widthSegments + 1) + (x))

static glm::vec3 calculateNormal(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3) {
	glm::vec3 v1 = p2 - p1;
	glm::vec3 v2 = p3 - p1;
	return glm::cross(v1, v2);
}


//
//	OtClothClass::init
//

OtObject OtClothClass::init(size_t count, OtObject* parameters) {
	// set attributes
	if (count) {
		switch (count) {
			case 2:
				setPins(parameters[1]->operator int());

			case 1:
				setPlane(parameters[0]);
				break;

			default:
				OtExcept("Too many parameters [%ld] for [Cloth] constructor (max 2)", count);
		}
	}

	return nullptr;
}


//
//	OtClothClass::setPlane
//

OtObject OtClothClass::setPlane(OtObject object) {
	// ensure object is a surface
	if (object->isKindOf("Plane")) {
		surface = object->cast<OtSurfaceClass>();

	} else {
		OtExcept("Expected a [Plane] object, not a [%s]", object->getType()->getName().c_str());
	}

	// clear particles and constraints
	particles.clear();
	constraints.clear();
	return shared();
}


//
//	OtClothClass::setPins
//

OtObject OtClothClass::setPins(int p) {
	pins = p < 1 ? 1 : p;
	return shared();
}


//
//	OtClothClass::step
//

void OtClothClass::step(int32_t deltaMilliseconds) {
	// sanity check
	if (!surface) {
		OtExcept("No [Plane] specified for [Cloth]");
	}

	// get information about plane
	int widthSegments = surface->getWidthSegments();
	int heightSegments = surface->getHeightSegments();
	int points = (widthSegments + 1) * (heightSegments + 1);

	// rebuild cloth if required
	if (particles.size() != points) {
		// rebuild particle matrix
		particles.clear();

		for (auto y = 0; y <= heightSegments; y++) {
			for (auto x = 0; x <= widthSegments; x++) {
				particles.push_back(OtClothParticle(surface->getVertex(INDEX(x, y)).position));
			}
		}

		// rebuild constraint list
		constraints.clear();
		auto dx = surface->getWidth() / widthSegments;
		auto dy = surface->getHeight() / heightSegments;

		for (auto y = 0; y < heightSegments; y++) {
			for (auto x = 0; x < widthSegments; x++) {
				constraints.push_back(OtClothConstraint(INDEX(x, y), INDEX(x, y + 1), dy));
				constraints.push_back(OtClothConstraint(INDEX(x, y), INDEX(x + 1, y), dx));
			}
		}

		for (auto x = widthSegments, y = 0; y < heightSegments; y++) {
			constraints.push_back(OtClothConstraint(INDEX(x, y), INDEX(x, y + 1), dy));
		}

		for (auto y = heightSegments, x = 0; x < widthSegments; x++) {
			constraints.push_back(OtClothConstraint(INDEX(x, y), INDEX(x + 1, y), dx));
		}
	}

	// create wind vector
	double now = OtApplicationClass::getTime();
	auto strength = std::cos(now / 7.0) * 5 + 10;
	auto direction = glm::vec3(std::sin(now / 2.0), std::cos(now / 3.0), std::sin(now / 1.0));
	auto wind = glm::normalize(direction) * (float) strength;

	// run simulation
	auto gravity = glm::vec3(0.0, -65, 0.0) * (float) mass;
	auto timesq = std::pow(18.0 / 1000.0, 2.0);

	for (auto y = 0; y <= heightSegments; y++) {
		for (auto x = 0; x <= widthSegments; x++) {
			// get vertex
			OtVertex& v = surface->getVertex(INDEX(x, y));

			// simulate gravity
			glm::vec3 acceleration = gravity;

			// simulate wind effect
			auto normal = v.normal;
			acceleration += (normal * glm::dot(normal, wind)) * (float) mass;

			// perform Verlet integration
			auto delta = v.position - particles[INDEX(x, y)].oldPos;
			particles[INDEX(x, y)].oldPos = v.position;
			v.position += (delta * (float) drag) + (acceleration * (float) timesq);
		}
	}

	// apply constraint vectors
	for (auto& c : constraints) {
		OtVertex& v1 = surface->getVertex(c.particle1);
		OtVertex& v2 = surface->getVertex(c.particle2);

		auto diff = v2.position - v1.position;
		auto dist = glm::length(diff);

		if (dist != 0.0) {
			auto correction = diff * ((1.0f - c.distance / dist) * 0.5f);
			v1.position += correction;
			v2.position -= correction;
		}
	}

	// apply the pins
	for (auto pin = 0; pin < pins; pin++) {
		auto i = pins == 1 ? 0 : INDEX((pin * widthSegments) / (pins - 1), 0);
		surface->getVertex(i).position = particles[i].original;
	}

	// reset the normals
	for (auto y = 0; y <= heightSegments; y++) {
		for (auto x = 0; x <= widthSegments; x++) {
			surface->getVertex(INDEX(x, y)).normal = glm::vec3(0.0);
		}
	}

	// recalculate the normals
	for (auto y = 0; y < heightSegments; y++) {
		for (auto x = 0; x < widthSegments; x++) {
			OtVertex& v1 = surface->getVertex(INDEX(x, y));
			OtVertex& v2 = surface->getVertex(INDEX(x, y + 1));
			OtVertex& v3 = surface->getVertex(INDEX(x + 1, y + 1));
			OtVertex& v4 = surface->getVertex(INDEX(x + 1, y));

			glm::vec3 normal = calculateNormal(v1.position, v2.position, v4.position);
			v1.normal += normal;
			v2.normal += normal;
			v4.normal += normal;

			normal = calculateNormal(v2.position, v3.position, v4.position);
			v2.normal += normal;
			v3.normal += normal;
			v4.normal += normal;
		}
	}

	surface->forceBufferRefresh();
}


//
//	OtClothClass::getMeta
//

OtType OtClothClass::getMeta() {
	static OtType type = nullptr;

	if (!type) {
		type = OtTypeClass::create<OtClothClass>("Cloth", OtSimulationClass::getMeta());
		type->set("__init__", OtFunctionClass::create(&OtClothClass::init));
		type->set("setPlane", OtFunctionClass::create(&OtClothClass::setPlane));
		type->set("setPins", OtFunctionClass::create(&OtClothClass::setPins));
	}

	return type;
}


//
//	OtClothClass::create
//

OtCloth OtClothClass::create() {
	OtCloth cloth = std::make_shared<OtClothClass>();
	cloth->setType(getMeta());
	return cloth;
}
