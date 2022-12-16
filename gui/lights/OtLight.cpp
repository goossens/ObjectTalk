//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "glm/glm.hpp"
#include "imgui.h"

#include "OtLight.h"
#include "OtScene.h"
#include "OtPass.h"
#include "OtViewPort.h"


//
//	Constants
//

constexpr int shadowmapSize = 1024;


//
//	OtLightClass::castShadow
//

OtObject OtLightClass::castShadow() {
	castShadowFlag = true;
	return shared();
}


//
//	OtLightClass::toggleShadow
//

OtObject OtLightClass::toggleShadow(bool flag) {
	castShadowFlag = flag;
	return shared();
}


//
//	OtLightClass::update
//

void OtLightClass::update(OtRenderer& renderer) {
	// handle shadow (if required)
	if (castShadowFlag) {
		// create camera (if required)
		if (!shadowCamera) {
			shadowCamera = OtCameraClass::create();
		}

		// reserve shadow rendering view
		shadowPass.reserveRenderingSlot();
	}

	// note: derived classes must configure camera
}


//
//	OtLightClass::render
//

void OtLightClass::render(OtRenderer& renderer) {
	// only respond to lighting pass
	if (renderer.inLightingPass()) {
		// render shadowmap if required
		if (castShadowFlag) {
			// update framebuffer
			framebuffer.update(shadowmapSize, shadowmapSize);

			// run shadow pass
			OtRenderer shadowMapRenderer;
			shadowMapRenderer.copyLightProperties(renderer);
			shadowMapRenderer.runShadowPass(shadowPass, renderer.getScene(), shadowCamera, framebuffer);

			// add shadowmap settings to renderer
			glm::mat4 shadowmapMatrix = OtViewPortGetMatrix() * shadowCamera->getViewProjectionMatrix();
			renderer.setShadowMap(framebuffer, shadowmapMatrix);
		}
	}
}


//
//	OtLightClass::getMeta
//

OtType OtLightClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtTypeClass::create<OtLightClass>("Light", OtSceneObjectClass::getMeta());
	}

	return type;
}
