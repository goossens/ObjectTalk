//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "bgfx/bgfx.h"

#include "ot/function.h"

#include "group.h"


//
//	OtGroupClass::validateChild
//

void OtGroupClass::validateChild(OtComponent child) {
	if (!child->isKindOf("Object3D")) {
		OtExcept("A [Group] can only have [Object3D] subclasses as children, not [%s]", child->getType()->getName().c_str());
	}
}


//
//	OtGroupClass::render
//

void OtGroupClass::render(int view, OtCamera camera, glm::mat4 parentTransform) {
	// calculate object transformation
	glm::mat4 t = parentTransform * translating * rotating * scaling;

	// render all children
	for (auto& child : children) {
		child->cast<OtObject3dClass>()->render(view, camera, t);
	}
}


//
//	OtGroupClass::getMeta
//

OtType OtGroupClass::getMeta() {
	static OtType type = nullptr;

	if (!type) {
		type = OtTypeClass::create<OtGroupClass>("Group", OtObject3dClass::getMeta());
	}

	return type;
}


//
//	OtGroupClass::create
//

OtGroup OtGroupClass::create() {
	OtGroup group = std::make_shared<OtGroupClass>();
	group->setType(getMeta());
	return group;
}
