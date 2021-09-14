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

#include "ot/function.h"

#include "matrix.h"


//
//	OtMatrixClass::rotateX
//

OtObject OtMatrixClass::rotateX(double angle) {
	rotating = glm::rotate(glm::mat4(1.0), (float) angle, glm::vec3(1.0, 0.0, 0.0));
	return shared();
}


//
//	OtMatrixClass::rotateY
//

OtObject OtMatrixClass::rotateY(double angle) {
	rotating = glm::rotate(glm::mat4(1.0), (float) angle, glm::vec3(0.0, 1.0, 0.0));
	return shared();
}


//
//	OtMatrixClass::rotateZ
//

OtObject OtMatrixClass::rotateZ(double angle) {
	rotating = glm::rotate(glm::mat4(1.0), (float) angle, glm::vec3(0.0, 0.0, 1.0));
	return shared();
}


//
//	OtMatrixClass::rotateAroundVector
//

OtObject OtMatrixClass::rotateAroundVector(double angle, double x, double y, double z) {
	rotating = glm::rotate(glm::mat4(1.0), (float) angle, glm::vec3(x, y, z));
	return shared();
}


//
//	OtMatrixClass::yawPitchRoll
//

OtObject OtMatrixClass::yawPitchRoll(double yaw, double pitch, double roll) {
	rotating = glm::yawPitchRoll((float) yaw, (float) pitch, (float) roll);
	return shared();
}


//
//	OtMatrixClass::scale
//

OtObject OtMatrixClass::scale(double x, double y, double z) {
	scaling = glm::scale(glm::mat4(1.0), glm::vec3(x, y, z));
	return shared();
}


//
//	OtMatrixClass::translate
//

OtObject OtMatrixClass::translate(double x, double y, double z) {
	translating = glm::translate(glm::mat4(1.0), glm::vec3(x, y, z));
	return shared();
}


//
//	OtMatrixClass::getComposite
//

glm::mat4 OtMatrixClass::getComposite() {
	return translating * rotating * scaling;
}


//
//	OtMatrixClass::getMeta
//

OtType OtMatrixClass::getMeta() {
	static OtType type = nullptr;

	if (!type) {
		type = OtTypeClass::create<OtMatrixClass>("Matrix", OtGuiClass::getMeta());
		type->set("rotateX", OtFunctionClass::create(&OtMatrixClass::rotateX));
		type->set("rotateY", OtFunctionClass::create(&OtMatrixClass::rotateY));
		type->set("rotateZ", OtFunctionClass::create(&OtMatrixClass::rotateZ));
		type->set("rotateAroundVector", OtFunctionClass::create(&OtMatrixClass::rotateAroundVector));
		type->set("yawPitchRoll", OtFunctionClass::create(&OtMatrixClass::yawPitchRoll));
		type->set("scale", OtFunctionClass::create(&OtMatrixClass::scale));
		type->set("translate", OtFunctionClass::create(&OtMatrixClass::translate));
	}

	return type;
}


//
//	OtMatrixClass::create
//

OtMatrix OtMatrixClass::create() {
	OtMatrix matrix = std::make_shared<OtMatrixClass>();
	matrix->setType(getMeta());
	return matrix;
}
