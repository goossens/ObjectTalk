//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/glm.hpp"
#include "glm/ext.hpp"

#include "OtFunction.h"

#include "OtMatrix.h"


//
//	OtMatrixClass::rotateX
//

OtObject OtMatrixClass::rotateX(float angle) {
	rotating = glm::rotate(glm::mat4(1.0), angle, glm::vec3(1.0, 0.0, 0.0));
	return shared();
}


//
//	OtMatrixClass::rotateY
//

OtObject OtMatrixClass::rotateY(float angle) {
	rotating = glm::rotate(glm::mat4(1.0), angle, glm::vec3(0.0, 1.0, 0.0));
	return shared();
}


//
//	OtMatrixClass::rotateZ
//

OtObject OtMatrixClass::rotateZ(float angle) {
	rotating = glm::rotate(glm::mat4(1.0), angle, glm::vec3(0.0, 0.0, 1.0));
	return shared();
}


//
//	OtMatrixClass::rotateAroundVector
//

OtObject OtMatrixClass::rotateAroundVector(float angle, float x, float y, float z) {
	rotating = glm::rotate(glm::mat4(1.0), angle, glm::vec3(x, y, z));
	return shared();
}


//
//	OtMatrixClass::yawPitchRoll
//

OtObject OtMatrixClass::yawPitchRoll(float yaw, float pitch, float roll) {
	rotating = glm::yawPitchRoll(yaw, pitch, roll);
	return shared();
}


//
//	OtMatrixClass::scale
//

OtObject OtMatrixClass::scale(float x, float y, float z) {
	scaling = glm::scale(glm::mat4(1.0), glm::vec3(x, y, z));
	return shared();
}


//
//	OtMatrixClass::translate
//

OtObject OtMatrixClass::translate(float x, float y, float z) {
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
	static OtType type;

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
