//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtFunction.h"

#include "OtGroup.h"


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

void OtGroupClass::render(OtRenderingContext* context) {
	// update parent transformation
	glm::mat4 oldTransform = context->transform;
	context->transform = context->transform * translating * rotating * scaling;

	// render all children
	for (auto& child : children) {
		child->cast<OtObject3dClass>()->render(context);
	}

	// restore old transform
	context->transform = oldTransform;
}


//
//	OtGroupClass::getMeta
//

OtType OtGroupClass::getMeta() {
	static OtType type;

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
