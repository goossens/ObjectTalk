//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "OtSceneObject.h"


//
//	OtObject3dClass
//

class OtObject3dClass;
using OtObject3d = OtObjectPointer<OtObject3dClass>;

class OtObject3dClass : public OtSceneObjectClass {
public:
	// change geometry
	OtObject rotateX(float angle);
	OtObject rotateY(float angle);
	OtObject rotateZ(float angle);
	OtObject rotateAroundVector(float angle, float x, float y, float z);
	OtObject yawPitchRoll(float yaw, float pitch, float roll);
	OtObject scale(float x, float y, float z);
	OtObject translate(float x, float y, float z);

	// set transformation order
	OtObject transformationOrder(const std::string& order);

	// get the transformation matrices
	glm::mat4 getLocalTransform();
	glm::mat4 getGlobalTransform();

	// get type definition
	static OtType getMeta();

protected:
	// matrix multiplication order
	enum {
		orderSRT,
		orderSTR,
		orderRTS,
		orderRST,
		orderTSR,
		orderTRS
	} order = orderSRT;

	// object geometry
	glm::mat4 translating = glm::mat4(1.0);
	glm::mat4 rotating = glm::mat4(1.0);
	glm::mat4 scaling = glm::mat4(1.0);
};
