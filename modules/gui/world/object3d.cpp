//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/ext.hpp"
#include "bgfx/bgfx.h"

#include "ot/function.h"

#include "object3d.h"


//
//	OtObject3dClass::rotate
//

OtObject OtObject3dClass::rotate(double angle, double x, double y, double z) {
	rotating = glm::rotate(glm::mat4(1.0), (float) angle, glm::vec3(x, y, z));
	return shared();
}


//
//	OtObject3dClass::rotateX
//

OtObject OtObject3dClass::rotateX(double angle) {
	rotating = glm::rotate(glm::mat4(1.0), (float) angle, glm::vec3(1.0, 0.0, 0.0));
	return shared();
}


//
//	OtObject3dClass::rotateY
//

OtObject OtObject3dClass::rotateY(double angle) {
	rotating = glm::rotate(glm::mat4(1.0), (float) angle, glm::vec3(0.0, 1.0, 0.0));
	return shared();
}


//
//	OtObject3dClass::rotateZ
//

OtObject OtObject3dClass::rotateZ(double angle) {
	rotating = glm::rotate(glm::mat4(1.0), (float) angle, glm::vec3(0.0, 0.0, 1.0));
	return shared();
}


//
//	OtObject3dClass::scale
//

OtObject OtObject3dClass::scale(double x, double y, double z) {
	scaling = glm::scale(glm::mat4(1.0), glm::vec3(x, y, z));
	return shared();
}


//
//	OtObject3dClass::translate
//

OtObject OtObject3dClass::translate(double x, double y, double z) {
	translating = glm::translate(glm::mat4(1.0), glm::vec3(x, y, z));
	return shared();
}


//
//	OtObject3dClass::render
//

void OtObject3dClass::render(int view, glm::mat4 parentTransform) {
	// calculate object transformation
	glm::mat4 t = parentTransform * translating * rotating * scaling;
	bgfx::setTransform(glm::value_ptr(t));
}


//
//	OtObject3dClass::getMeta
//

OtType OtObject3dClass::getMeta() {
	static OtType type = nullptr;

	if (!type) {
		type = OtTypeClass::create<OtObject3dClass>("Object3D", OtSceneObjectClass::getMeta());
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
