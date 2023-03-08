//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "glm/glm.hpp"
#include "glm/gtx/euler_angles.hpp"

#include "OtFunction.h"

#include "OtMatrix.h"


//
//	OtMatrixClass::rotateX
//

OtObject OtMatrixClass::rotateX(float angle) {
	rotating = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(1.0f, 0.0f, 0.0f));
	return OtObject(this);
}


//
//	OtMatrixClass::rotateY
//

OtObject OtMatrixClass::rotateY(float angle) {
	rotating = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0.0f, 1.0f, 0.0f));
	return OtObject(this);
}


//
//	OtMatrixClass::rotateZ
//

OtObject OtMatrixClass::rotateZ(float angle) {
	rotating = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0.0f, 0.0f, 1.0f));
	return OtObject(this);
}


//
//	OtMatrixClass::rotateAroundVector
//

OtObject OtMatrixClass::rotateAroundVector(float angle, float x, float y, float z) {
	rotating = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(x, y, z));
	return OtObject(this);
}


//
//	OtMatrixClass::yawPitchRoll
//

OtObject OtMatrixClass::yawPitchRoll(float yaw, float pitch, float roll) {
	rotating = glm::yawPitchRoll(yaw, pitch, roll);
	return OtObject(this);
}


//
//	OtMatrixClass::scale
//

OtObject OtMatrixClass::scale(float x, float y, float z) {
	scaling = glm::scale(glm::mat4(1.0f), glm::vec3(x, y, z));
	return OtObject(this);
}


//
//	OtMatrixClass::translate
//

OtObject OtMatrixClass::translate(float x, float y, float z) {
	translating = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, z));
	return OtObject(this);
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
		type = OtType::create<OtMatrixClass>("Matrix", OtObjectClass::getMeta());
		type->set("rotateX", OtFunction::create(&OtMatrixClass::rotateX));
		type->set("rotateY", OtFunction::create(&OtMatrixClass::rotateY));
		type->set("rotateZ", OtFunction::create(&OtMatrixClass::rotateZ));
		type->set("rotateAroundVector", OtFunction::create(&OtMatrixClass::rotateAroundVector));
		type->set("yawPitchRoll", OtFunction::create(&OtMatrixClass::yawPitchRoll));
		type->set("scale", OtFunction::create(&OtMatrixClass::scale));
		type->set("translate", OtFunction::create(&OtMatrixClass::translate));
	}

	return type;
}
