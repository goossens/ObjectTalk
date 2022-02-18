//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "OtCamera.h"
#include "OtComponent.h"
#include "OtRenderingContext.h"


//
//	OtSceneClass
//

class OtSceneClass;
typedef std::shared_ptr<OtSceneClass> OtScene;

class OtSceneClass : public OtComponentClass {
public:
	// destructor
	~OtSceneClass();

	// ensure specified component is allowed as a child
	void validateChild(OtComponent child);

	// set shadow flag
	OtObject setShadow(bool flag);

	// update state
	void update(OtCamera camera, float x, float y, float w, float h);

	// render contents
	void render();

	// just render all of our children in the context provided
	void renderChildren(OtRenderingContext context);

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtScene create();

private:
	// create a shadowmap
	void createShadowmap();

	// do we render a shadow
	bool shadow = false;

	// our rendering context
	OtRenderingContextClass context;

	// shadow shader
	bgfx::ProgramHandle shader = BGFX_INVALID_HANDLE;

	// shadowmap
	float shadowmapAspectRation = -1;
	bgfx::TextureHandle shadowmapTexture = BGFX_INVALID_HANDLE;
	bgfx::FrameBufferHandle shadowmapFrameBuffer = BGFX_INVALID_HANDLE;
	OtCamera shadowCamera;
	bgfx::ViewId shadowView;
};
