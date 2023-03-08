//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "glm/glm.hpp"

#include "OtObject.h"


//
//	OtMatrixClass
//

class OtMatrixClass;
using OtMatrix = OtObjectPointer<OtMatrixClass>;

class OtMatrixClass : public OtObjectClass {
public:
	// change geometry
	OtObject rotateX(float angle);
	OtObject rotateY(float angle);
	OtObject rotateZ(float angle);
	OtObject rotateAroundVector(float angle, float x, float y, float z);
	OtObject yawPitchRoll(float yaw, float pitch, float roll);
	OtObject scale(float x, float y, float z);
	OtObject translate(float x, float y, float z);

	// return composite matrix
	glm::mat4 getComposite();

	// get type definition
	static OtType getMeta();

protected:
	// object geometry
	glm::mat4 translating = glm::mat4(1.0f);
	glm::mat4 rotating = glm::mat4(1.0f);
	glm::mat4 scaling = glm::mat4(1.0f);
};
