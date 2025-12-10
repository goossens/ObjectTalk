//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "glm/glm.hpp"

#include "OtArray.h"
#include "OtClass.h"
#include "OtFunction.h"
#include "OtLog.h"
#include "OtModule.h"
#include "OtObject.h"
#include "OtValue.h"

#include "OtManifoldModule.h"
#include "OtShapeModule.h"


//
//	OtManifoldClass::decompose
//

OtObject OtManifoldClass::decompose() {
	OtArray result = OtArray::create();

	for (auto& m : manifold.decompose()) {
		result->add(OtManifoldObject::create(m));
	}

	return result;
}

//
//	OtManifoldClass::compose
//

OtObject OtManifoldClass::compose(OtObject array) {
	// sanity check
	array.expect<OtArrayClass>("Array");

	// create list of manifolds
	auto members = OtArray(array)->raw();
	std::vector<OtManifold> manifolds;

	for (auto& object: members) {
		object.expect<OtManifoldClass>("Manifold");
		manifolds.emplace_back(OtManifoldObject(object)->manifold);
	}

	// combine manifolds into a single one
	return OtManifoldObject::create(OtManifold::compose(manifolds));
}


//
//	OtManifoldClass::slice
//

OtObject OtManifoldClass::slice(float height) {
	return OtShapeObject::create(manifold.slice(height));
}


//
//	OtManifoldClass::project
//

OtObject OtManifoldClass::project() {
	return OtShapeObject::create(manifold.project());
}


//
//	OtManifoldClass::split
//

OtObject OtManifoldClass::split(float x, float y, float z, float d) {
	auto manifolds = manifold.split(glm::vec3(x, y, z), d);
	OtArray result = OtArray::create();
	result->add(OtManifoldObject::create(manifolds.first));
	result->add(OtManifoldObject::create(manifolds.second));
	return result;
}


//
//	OtManifoldClass::getMeta
//

OtType OtManifoldClass::getMeta() {
	static OtType type = nullptr;

	if (!type) {
		type = OtType::create<OtManifoldClass>("Manifold", OtObjectClass::getMeta());
		type->set("clear", OtFunction::create(&OtManifoldClass::clear));

		type->set("load", OtFunction::create(&OtManifoldClass::load));
		type->set("save", OtFunction::create(&OtManifoldClass::save));

		type->set("__add__", OtFunction::create(&OtManifoldClass::unionManifolds));
		type->set("__sub__", OtFunction::create(&OtManifoldClass::differenceManifolds));
		type->set("__pow__", OtFunction::create(&OtManifoldClass::intersectManifolds));

		type->set("decompose", OtFunction::create(&OtManifoldClass::decompose));
		type->set("slice", OtFunction::create(&OtManifoldClass::slice));
		type->set("project", OtFunction::create(&OtManifoldClass::project));

		type->set("simplify", OtFunction::create(&OtManifoldClass::simplify));
		type->set("refine", OtFunction::create(&OtManifoldClass::refine));

		type->set("translate", OtFunction::create(&OtManifoldClass::translate));
		type->set("rotate", OtFunction::create(&OtManifoldClass::rotate));
		type->set("scale", OtFunction::create(&OtManifoldClass::scale));
		type->set("mirror", OtFunction::create(&OtManifoldClass::mirror));

		type->set("split", OtFunction::create(&OtManifoldClass::split));
		type->set("hull", OtFunction::create(&OtManifoldClass::hull));

		type->set("getSurfaceArea", OtFunction::create(&OtManifoldClass::getSurfaceArea));
		type->set("getVolume", OtFunction::create(&OtManifoldClass::getVolume));

		type->set("getVertexCount", OtFunction::create(&OtManifoldClass::getVertexCount));
		type->set("getTriangleCount", OtFunction::create(&OtManifoldClass::getTriangleCount));
	}

	return type;
}


//
//	Module registration
//

static OtModuleRegistration registration{"manifold", [](OtModule module) {
	// register classes
	module->set("Manifold", OtClass::create(OtManifoldClass::getMeta()));

	// register functions
	module->set("cube", OtFunction::create(OtManifoldClass::cube));
	module->set("cylinder", OtFunction::create(OtManifoldClass::cylinder));
	module->set("sphere", OtFunction::create(OtManifoldClass::sphere));

	module->set("compose", OtFunction::create(OtManifoldClass::compose));
	module->set("extrude", OtFunction::create(OtManifoldClass::extrude));
	module->set("revolve", OtFunction::create(OtManifoldClass::revolve));
}};
