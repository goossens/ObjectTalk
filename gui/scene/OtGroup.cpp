//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
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

void OtGroupClass::validateChild(OtNode child) {
	if (!child->isKindOf("Object3D")) {
		OtExcept("A [Group] can only have [Object3D] subclasses as children, not [%s]", child->getType()->getName().c_str());
	}
}


//
//	OtGroupClass::castShadow
//

OtObject OtGroupClass::castShadow(bool flag) {
	for (auto& child : children) {
		child->cast<OtObject3dClass>()->castShadow(flag);
	}

	OtObject3dClass::castShadow(flag);
	return shared();
}


//
//	OtGroupClass::receiveShadow
//

OtObject OtGroupClass::receiveShadow(bool flag) {
	for (auto& child : children) {
		child->cast<OtObject3dClass>()->receiveShadow(flag);
	}

	OtObject3dClass::receiveShadow(flag);
	return shared();
}


//
//	OtGroupClass::render
//

void OtGroupClass::render(OtRenderer& renderer) {
	// render all children
	for (auto& child : children) {
		child->cast<OtObject3dClass>()->render(renderer);
	}
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
