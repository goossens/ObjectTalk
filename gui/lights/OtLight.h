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
#include "OtFrameBuffer.h"
#include "OtSceneObject.h"


//
//	OtLightClass
//

class OtLightClass;
typedef std::shared_ptr<OtLightClass> OtLight;

class OtLightClass : public OtSceneObjectClass {
public:
	// enable shadow casting
	OtObject castShadow();
	bool castsShadow() { return castShadowFlag; }
	OtObject toggleShadow(bool flag);

	// update state
	void update(OtRenderer& renderer) override;

	// submit to GPU
	void render(OtRenderer& renderer) override;

	// get type definition
	static OtType getMeta();

protected:
	// shadowing properties
	bool castShadowFlag = false;
	OtCamera shadowCamera;
	OtPass shadowPass;

	OtFrameBuffer framebuffer = OtFrameBuffer(
		OtFrameBuffer::noTexture,
		OtFrameBuffer::dFloatTexture);
};
