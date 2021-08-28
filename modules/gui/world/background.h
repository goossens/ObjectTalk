//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "bgfx/bgfx.h"

#include "object3d.h"
#include "plane.h"
#include "material.h"


//
//	OtBackgroundClass
//

class OtBackgroundClass;
typedef std::shared_ptr<OtBackgroundClass> OtBackground;

class OtBackgroundClass : public OtObject3dClass {
public:
	// constructor/destructor
	OtBackgroundClass();
	~OtBackgroundClass();

	// initialize
	OtObject init(size_t count, OtObject* parameters);

	// update material
	OtObject setMaterial(OtObject material);

	// render in BGFX
    void render(int view, glm::mat4 parentTransform);

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtBackground create();

protected:
	// geometry and material
	OtPlane plane;
	OtMaterial material;

	// BGFX shader
	bgfx::UniformHandle transformUniform = BGFX_INVALID_HANDLE;
	bgfx::ProgramHandle program = BGFX_INVALID_HANDLE;
};
