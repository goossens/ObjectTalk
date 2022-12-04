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
#include "OtSceneObject.h"


//
//	OtLightClass
//

class OtLightClass;
typedef std::shared_ptr<OtLightClass> OtLight;

class OtLightClass : public OtSceneObjectClass {
public:
	// destructor
	~OtLightClass();

	// enable shadow casting
	OtObject castShadow();
	void toggleShadow(bool flag) { shadow = flag; }

	// enable/disable frustum rendering (debugging)
	OtObject renderFrustum(bool flag);

	// update state
	void update(OtRenderingContext context) override;

	// submit to GPU
	void render(OtRenderingContext context) override;

	// render GUI for controllers
	void renderShadowCameraGUI();

	// get type definition
	static OtType getMeta();

protected:
	// shadowing
	bool shadow = false;
	OtCamera shadowCamera;
	bgfx::ViewId shadowView;
	bgfx::TextureHandle shadowmapTexture = BGFX_INVALID_HANDLE;
	bgfx::FrameBufferHandle shadowmapFrameBuffer = BGFX_INVALID_HANDLE;
};
