//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <vector>

#include "simulation.h"
#include "planegeometry.h"


//
//	OtClothClass
//

class OtClothClass;
typedef std::shared_ptr<OtClothClass> OtCloth;

class OtClothClass : public OtSimulationClass {
public:
	// initialize geometry
	OtObject init(size_t count, OtObject* parameters);

	// update attributes
	OtObject setPlane(OtObject plane);
	OtObject setPins(int pins);

	// run simulation
	void step(int32_t deltaMilliseconds);

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtCloth create();

private:
	// our plane
	OtPlaneGeometry plane;

	// attributes
	int pins = 3;
	float mass = 0.05;
	float dampling = 0.03;
	float drag = 1 - 0.03;

	// private classes
	class OtClothParticle {
	public:
		// constructors
		OtClothParticle() {}
		OtClothParticle(glm::vec3 p) : original(p), oldPos(p) {}

		// attributes
		glm::vec3 oldPos;
		glm::vec3 original;
	};

	class OtClothConstraint {
	public:
		OtClothConstraint() {}
		OtClothConstraint(size_t p1, size_t p2, float d) : particle1(p1), particle2(p2), distance(d) {}

		size_t particle1;
		size_t particle2;
		float distance;
	};

	// the particles that make up our cloth
	std::vector<OtClothParticle> particles;
	std::vector<OtClothConstraint> constraints;
};
