//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <vector>

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/glm.hpp"
#include "glm/ext.hpp"
#include "bgfx/bgfx.h"

#include "vertex.h"
#include "composite.h"
#include "material.h"


//
//	OtObject3dClass
//

class OtObject3dClass;
typedef std::shared_ptr<OtObject3dClass> OtObject3d;

class OtObject3dClass : public OtCompositeClass {
public:
	// ensure specified component is allowed as a child
	void validateChild(OtComponent child);

	// change geometry
	void rotate(double angle, double x, double y, double z) { rotating = glm::rotate(glm::mat4(1.0), (float) angle, glm::vec3(x, y, z)); }
	void rotateX(double angle) { rotating = glm::rotate(glm::mat4(1.0), (float) angle, glm::vec3(1.0, 0.0, 0.0)); }
	void rotateY(double angle) { rotating = glm::rotate(glm::mat4(1.0), (float) angle, glm::vec3(0.0, 1.0, 0.0)); }
	void rotateZ(double angle) { rotating = glm::rotate(glm::mat4(1.0), (float) angle, glm::vec3(0.0, 0.0, 1.0)); }
	void scale(double x, double y, double z) { scaling = glm::scale(glm::mat4(1.0), glm::vec3(x, y, z)); }
	void translate(double x, double y, double z) { translating = glm::translate(glm::mat4(1.0), glm::vec3(x, y, z)); }

	// render in BGFX
    virtual void render(int view, glm::mat4 parentTransform);

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtObject3d create();

protected:
	// object geometry
	glm::mat4 translating = glm::mat4(1.0);
	glm::mat4 rotating = glm::mat4(1.0);
	glm::mat4 scaling = glm::mat4(1.0);
};
