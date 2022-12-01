//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "bgfx/bgfx.h"

#include "OtSceneObject.h"
#include "OtPlaneGeometry.h"


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
	void init(size_t count, OtObject* parameters);

	// update color
	OtObject setColor(const std::string& color);

	// update texture
	OtObject setTexture(const std::string& texture);

	// render in BGFX
	void render(OtRenderingContext context) override;

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtBackground create();

protected:
	// properties
	OtPlaneGeometry plane;

	glm::vec3 color = glm::vec3(0.0);
	bgfx::TextureHandle texture = BGFX_INVALID_HANDLE;
	bool hasTexture = false;

	// BGFX shader
	bgfx::UniformHandle transformUniform = BGFX_INVALID_HANDLE;
	bgfx::UniformHandle backgroundUniform = BGFX_INVALID_HANDLE;
	bgfx::UniformHandle textureUniform = BGFX_INVALID_HANDLE;
	bgfx::ProgramHandle shader = BGFX_INVALID_HANDLE;
};
