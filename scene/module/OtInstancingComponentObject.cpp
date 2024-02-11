//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtException.h"
#include "OtFunction.h"

#include "OtVec3.h"

#include "OtInstancingComponentObject.h"
#include "OtTransformComponentObject.h"


//
//	OtInstancingComponentObjectClass::appendInstance
//

void OtInstancingComponentObjectClass::appendInstance(OtObject translation, OtObject rotation, OtObject scale) {
	// sanity checks
	if (!translation.isKindOf<OtVec3Class>()) {
		OtError("Expected a [Vec3] for translation, not a [{}]", translation->getType()->getName());
	}

	if (!rotation.isKindOf<OtVec3Class>()) {
		OtError("Expected a [Vec3] for rotation, not a [{}]", rotation->getType()->getName());
	}

	if (!scale.isKindOf<OtVec3Class>()) {
		OtError("Expected a [Vec3] for scaling, not a [{}]", scale->getType()->getName());
	}

	// add new instance
	instancing->appendInstance(*OtVec3(translation), *OtVec3(rotation), *OtVec3(scale));
}


//
//	OtInstancingComponentObjectClass::getInstance
//

OtObject OtInstancingComponentObjectClass::getInstance(size_t index) {
	// sanity checks
	if (index >= instancing->instanceCount()) {
		OtError("Invalid index [{}] for instancing component with size [{}]", index, instancing->instanceCount());
	}

	return OtTransformComponentObject::create(&instancing->getInstance(index));
}


//
//	OtInstancingComponentObjectClass::setInstance
//

void OtInstancingComponentObjectClass::setInstance(size_t index, OtObject translation, OtObject rotation, OtObject scale) {
	// sanity checks
	if (index >= instancing->instanceCount()) {
		OtError("Invalid index [{}] for instancing component with size [{}]", index, instancing->instanceCount());
	}

	if (!translation.isKindOf<OtVec3Class>()) {
		OtError("Expected a [Vec3] for translation, not a [{}]", translation->getType()->getName());
	}

	if (!rotation.isKindOf<OtVec3Class>()) {
		OtError("Expected a [Vec3] for rotation, not a [{}]", rotation->getType()->getName());
	}

	if (!scale.isKindOf<OtVec3Class>()) {
		OtError("Expected a [Vec3] for scaling, not a [{}]", scale->getType()->getName());
	}

	// add new instance
	instancing->setInstance(index, *OtVec3(translation), *OtVec3(rotation), *OtVec3(scale));
}


//
//	OtInstancingComponentObjectClass::eraseInstance
//

void OtInstancingComponentObjectClass::eraseInstance(size_t index) {
	// sanity checks
	if (index >= instancing->instanceCount()) {
		OtError("Invalid index [{}] for instancing component with size [{}]", index, instancing->instanceCount());
	}

	// delete instance
 	instancing->eraseInstance(index);
}


//
//	OtInstancingComponentObjectClass::getMeta
//

OtType OtInstancingComponentObjectClass::getMeta() {
	static OtType type = nullptr;

	if (!type) {
		type = OtType::create<OtInstancingComponentObjectClass>("InstancingComponent", OtObjectClass::getMeta());

		type->set("clear", OtFunction::create(&OtInstancingComponentObjectClass::clearInstances));
		type->set("append", OtFunction::create(&OtInstancingComponentObjectClass::appendInstance));
		type->set("getInstance", OtFunction::create(&OtInstancingComponentObjectClass::getInstance));
		type->set("setInstance", OtFunction::create(&OtInstancingComponentObjectClass::setInstance));
		type->set("erase", OtFunction::create(&OtInstancingComponentObjectClass::eraseInstance));
		type->set("size", OtFunction::create(&OtInstancingComponentObjectClass::instanceCount));
	}

	return type;
}
