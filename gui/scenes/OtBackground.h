//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "OtSceneObject.h"
#include "OtPlaneGeometry.h"
#include "OtTexture.h"


//
//	OtBackgroundClass
//

class OtBackgroundClass;
typedef std::shared_ptr<OtBackgroundClass> OtBackground;

class OtBackgroundClass : public OtSceneObjectClass {
public:
	// constructor/destructor
	OtBackgroundClass();
	~OtBackgroundClass();

	// initialize
	OtObject init(size_t count, OtObject* parameters);

	// update color
	OtObject setColor(const std::string& color);

	// update texture
	OtObject setTexture(OtObject texture);

	// render in BGFX
	void render(OtRenderingContext context) override;

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtBackground create();

protected:
	// geometry and material
	OtPlaneGeometry plane;

	glm::vec3 color = glm::vec3(0.0);
	OtTexture texture;

	// BGFX shader
	bgfx::UniformHandle transformUniform = BGFX_INVALID_HANDLE;
	bgfx::UniformHandle backgroundUniform = BGFX_INVALID_HANDLE;
	bgfx::UniformHandle textureUniform = BGFX_INVALID_HANDLE;
	bgfx::ProgramHandle shader = BGFX_INVALID_HANDLE;
};
