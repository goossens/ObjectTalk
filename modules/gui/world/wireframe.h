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
#include "geometry.h"
#include "material.h"


//
//	OtWireframeClass
//

class OtWireframeClass;
typedef std::shared_ptr<OtWireframeClass> OtWireframe;

class OtWireframeClass : public OtObject3dClass {
public:
	// constructor/destructor
	OtWireframeClass();
	~OtWireframeClass();

	// initialize
	OtObject init(size_t count, OtObject* parameters);

	// update geometry or material
	OtObject setGeometry(OtObject geometry);
	OtObject setMaterial(OtObject material);

	// render in BGFX
    void render(int view, glm::mat4 parentTransform);

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtWireframe create();

protected:
	// render with culling
	void render(int view, glm::mat4 parentTransform, long flag);

	// geometry and material
	OtGeometry geometry;
	OtMaterial material;

	// BGFX shader
	bgfx::ProgramHandle program = BGFX_INVALID_HANDLE;
};