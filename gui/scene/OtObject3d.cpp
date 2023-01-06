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
#include "OtText.h"

#include "OtObject3d.h"


//
//	OtObject3dClass::rotateX
//

OtObject OtObject3dClass::rotateX(float angle) {
	rotating = glm::rotate(glm::mat4(1.0), angle, glm::vec3(1.0, 0.0, 0.0));
	return shared();
}


//
//	OtObject3dClass::rotateY
//

OtObject OtObject3dClass::rotateY(float angle) {
	rotating = glm::rotate(glm::mat4(1.0), angle, glm::vec3(0.0, 1.0, 0.0));
	return shared();
}


//
//	OtObject3dClass::rotateZ
//

OtObject OtObject3dClass::rotateZ(float angle) {
	rotating = glm::rotate(glm::mat4(1.0), angle, glm::vec3(0.0, 0.0, 1.0));
	return shared();
}


//
//	OtObject3dClass::rotateAroundVector
//

OtObject OtObject3dClass::rotateAroundVector(float angle, float x, float y, float z) {
	rotating = glm::rotate(glm::mat4(1.0), angle, glm::vec3(x, y, z));
	return shared();
}


//
//	OtObject3dClass::yawPitchRoll
//

OtObject OtObject3dClass::yawPitchRoll(float yaw, float pitch, float roll) {
	rotating = glm::yawPitchRoll(yaw, pitch, roll);
	return shared();
}


//
//	OtObject3dClass::scale
//

OtObject OtObject3dClass::scale(float x, float y, float z) {
	scaling = glm::scale(glm::mat4(1.0), glm::vec3(x, y, z));
	return shared();
}


//
//	OtObject3dClass::translate
//

OtObject OtObject3dClass::translate(float x, float y, float z) {
	translating = glm::translate(glm::mat4(1.0), glm::vec3(x, y, z));
	return shared();
}


//
//	OtObject3dClass::transformationOrder
//

OtObject OtObject3dClass::transformationOrder(const std::string& orderString) {
	std::string os = OtText::lower(orderString);

	if (os == "srt") {
		order = orderSRT;

	} else if (os == "str") {
		order = orderSTR;

	} else if (os == "rts") {
		order = orderRTS;

	} else if (os == "rst") {
		order = orderRST;

	} else if (os == "tsr") {
		order = orderTSR;

	} else if (os == "trs") {
		order = orderTRS;

	} else {
		OtExcept("Invalid transformation order [%s] for [%s]", orderString.c_str(), getType()->getName().c_str());
	}

	return shared();
}


//
//	OtObject3dClass::getLocalTransform
//

glm::mat4 OtObject3dClass::getLocalTransform() {
	glm::mat4 transform;

	switch (order) {
		case orderSRT:
			transform = translating * rotating * scaling;
			break;

		case orderSTR:
			transform = rotating * translating  * scaling;
			break;

		case orderRTS:
			transform = scaling * translating * rotating;
			break;

		case orderRST:
			transform = translating * scaling * rotating;
			break;

		case orderTSR:
			transform = rotating * scaling * translating;
			break;

		case orderTRS:
			transform = scaling * rotating * translating;
			break;
	}

	return transform;
}


//
//	OtObject3dClass::getGlobalTransform
//

glm::mat4 OtObject3dClass::getGlobalTransform() {
	if (hasParent()) {
		OtObject parent = getParent();
		
		if (parent->isKindOf("Object3D")) {
			return parent->cast<OtObject3dClass>()->getGlobalTransform() * getLocalTransform();

		} else {
			return getLocalTransform();
		}

	} else {
		return getLocalTransform();
	}
}


//
//	OtObject3dClass::getMeta
//

OtType OtObject3dClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtTypeClass::create<OtObject3dClass>("Object3D", OtSceneObjectClass::getMeta());
		type->set("rotateX", OtFunctionClass::create(&OtObject3dClass::rotateX));
		type->set("rotateY", OtFunctionClass::create(&OtObject3dClass::rotateY));
		type->set("rotateZ", OtFunctionClass::create(&OtObject3dClass::rotateZ));
		type->set("rotateAroundVector", OtFunctionClass::create(&OtObject3dClass::rotateAroundVector));
		type->set("yawPitchRoll", OtFunctionClass::create(&OtObject3dClass::yawPitchRoll));
		type->set("scale", OtFunctionClass::create(&OtObject3dClass::scale));
		type->set("translate", OtFunctionClass::create(&OtObject3dClass::translate));
		type->set("transformationOrder", OtFunctionClass::create(&OtObject3dClass::transformationOrder));
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
