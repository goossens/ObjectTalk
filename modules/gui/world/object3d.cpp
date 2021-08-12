//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "ot/function.h"

#include "object3d.h"


//
//	OtObject3dClass::validateChild
//

void OtObject3dClass::validateChild(OtComponent child) {
	if (!child->isKindOf("Object3D")) {
		OtExcept("An [Object3D] can only have [Object3D]s as children, not [%s]", child->getType()->getName().c_str());
	}
}


//
//	OtObject3dClass::render
//

void OtObject3dClass::render(int view, glm::mat4 parentTransform) {
	// calculate transformation for object
	glm::mat4 t = parentTransform * translating * rotating * scaling;
	bgfx::setTransform(glm::value_ptr(t));
}


//
//	OtObject3dClass::getMeta
//

OtType OtObject3dClass::getMeta() {
	static OtType type = nullptr;

	if (!type) {
		type = OtTypeClass::create<OtObject3dClass>("Object3D", OtCompositeClass::getMeta());
		type->set("rotate", OtFunctionClass::create(&OtObject3dClass::rotate));
		type->set("rotateX", OtFunctionClass::create(&OtObject3dClass::rotateX));
		type->set("rotateY", OtFunctionClass::create(&OtObject3dClass::rotateY));
		type->set("rotateZ", OtFunctionClass::create(&OtObject3dClass::rotateZ));
		type->set("scale", OtFunctionClass::create(&OtObject3dClass::scale));
		type->set("translate", OtFunctionClass::create(&OtObject3dClass::translate));
	}

	return type;
}


//
//	OtObject3dClass::create
//

OtObject3d OtObject3dClass::create() {
	OtObject3d shape = std::make_shared<OtObject3dClass>();
	shape->setType(getMeta());
	return shape;
}
