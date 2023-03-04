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
		OtObject3d(child)->castShadow(flag);
	}

	OtObject3dClass::castShadow(flag);
	return OtObject(this);
}


//
//	OtGroupClass::receiveShadow
//

OtObject OtGroupClass::receiveShadow(bool flag) {
	for (auto& child : children) {
		OtObject3d(child)->receiveShadow(flag);
	}

	OtObject3dClass::receiveShadow(flag);
	return OtObject(this);
}


//
//	OtGroupClass::render
//

void OtGroupClass::render(OtRenderer& renderer) {
	// render all children
	for (auto& child : children) {
		OtObject3d(child)->render(renderer);
	}
}


//
//	OtGroupClass::getMeta
//

OtType OtGroupClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtType::create<OtGroupClass>("Group", OtObject3dClass::getMeta());
	}

	return type;
}

