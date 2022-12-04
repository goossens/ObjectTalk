//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "glm/glm.hpp"
#include "glm/ext.hpp"
#include "imgui.h"

#include "OtFramework.h"
#include "OtLight.h"
#include "OtScene.h"


//
//	Globals
//

const static int shadowmapSize = 512;


//
//	OtLightClass::~OtLightClass
//

OtLightClass::~OtLightClass() {
	// release resources
	if (bgfx::isValid(shadowmapFrameBuffer)) {
		bgfx::destroy(shadowmapFrameBuffer);
	}
}


//
//	OtLightClass::castShadow
//

OtObject OtLightClass::castShadow() {
	// create camera (if required)
	if (!shadowCamera) {
		shadowCamera = OtCameraClass::create();
	}

	// create shadowmap frame buffer (if required)
	if (!bgfx::isValid(shadowmapFrameBuffer)) {
		shadowmapTexture = bgfx::createTexture2D(shadowmapSize, shadowmapSize, false, 1, bgfx::TextureFormat::D32F, BGFX_TEXTURE_RT);
		shadowmapFrameBuffer = bgfx::createFrameBuffer(1, &shadowmapTexture, true);
	}

	shadow = true;
	return shared();
}


//
//	OtLightClass::renderFrustum
//

OtObject OtLightClass::renderFrustum(bool flag) {
	// create camera (if required)
	if (!shadowCamera) {
		shadowCamera = OtCameraClass::create();
	}

	shadowCamera->renderFrustum(flag);
	return shared();
}


//
//	OtLightClass::update
//

void OtLightClass::update(OtRenderingContext context) {
	// handle shadow (if required)
	if (shadow) {
		// reserve shadow rendering view
		shadowView = OtFrameworkClass::instance()->getNextViewID();
	}
}


//
//	OtLightClass::render
//

void OtLightClass::render(OtRenderingContext context) {
	// only respond to lighting pass
	if (context->inLightingPass()) {
		// render shadowmap if required
		if (shadow) {
			// render shadow map
			bgfx::setViewClear(shadowView, BGFX_CLEAR_DEPTH);
			bgfx::setViewRect(shadowView, 0, 0, shadowmapSize, shadowmapSize);
			bgfx::setViewFrameBuffer(shadowView, shadowmapFrameBuffer);
			bgfx::touch(shadowView);

			// create shadowmap rendering context
			OtRenderingContextClass shadowMapContext;
			shadowMapContext.setPass(OtRenderingContextClass::shadowmapPass);
			shadowMapContext.setView(shadowView);
			shadowMapContext.setViewRect(0, 0, shadowmapSize, shadowmapSize);
			shadowMapContext.setCamera(shadowCamera);
			shadowMapContext.copyLightProperties(context);

			// render shadow
			shadowCamera->submit(shadowView);
			context->getScene()->renderChildren(&shadowMapContext);

			// determine crop matrix to ensure shadow matrix ends up with platform correct texture coordinates
			auto caps = bgfx::getCaps();
			float sy = caps->originBottomLeft ? 0.5f : -0.5f;
			float sz = caps->homogeneousDepth ? 0.5f :  1.0f;
			float tz = caps->homogeneousDepth ? 0.5f :  0.0f;

			glm::mat4 cropMatrix = glm::mat4(1.0);
			cropMatrix = glm::translate(cropMatrix, glm::vec3(0.5, 0.5, tz));
			cropMatrix = glm::scale(cropMatrix, glm::vec3(0.5, sy, sz));

			// add shadowmap matrix to rendering context
			glm::mat4 shadowmapMatrix = cropMatrix * shadowCamera->getViewProjectionMatrix();
			context->setShadowMap(shadowmapTexture, shadowmapMatrix);

			// render camera frustum (if required)
			shadowCamera->render(context->getDebugDraw());
		}
	}
}


//
//	OtLightClass::renderShadowCameraGUI
//

void OtLightClass::renderShadowCameraGUI() {
	if (ImGui::TreeNodeEx("Shadow Camera", ImGuiTreeNodeFlags_Framed)) {
		shadowCamera->renderGUI();

		float width = ImGui::GetContentRegionAvail().x;
		ImGui::Image((void*)(intptr_t) shadowmapTexture.idx, ImVec2(width, width));
		ImGui::TreePop();
	}
}


//
//	OtLightClass::getMeta
//

OtType OtLightClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtTypeClass::create<OtLightClass>("Light", OtSceneObjectClass::getMeta());
		type->set("renderFrustum", OtFunctionClass::create(&OtLightClass::renderFrustum));
	}

	return type;
}
