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

#include "OtFunction.h"

#include "OtCamera.h"
#include "OtColor.h"
#include "OtFramework.h"
#include "OtLight.h"
#include "OtScene.h"
#include "OtShadowShader.h"


//
//	Globals
//

const static int shadowmapSize = 512;


//
//	OtLightClass::OtLightClass
//

OtLightClass::OtLightClass() {
	shadowCamera = OtCameraClass::create();
}


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
//	OtLightClass::setDirection
//

OtObject OtLightClass::setDirection(float px, float py, float pz) {
	direction = glm::vec3(px, py, pz);
	return shared();
}


//
//	OtLightClass::setDiffuse
//

OtObject OtLightClass::setDiffuse(const std::string& c) {
	diffuse = OtColorParseToVec3(c);
	return shared();
}


//
//	OtLightClass::setSpecular
//

OtObject OtLightClass::setSpecular(const std::string& c) {
	specular = OtColorParseToVec3(c);
	return shared();
}


//
//	OtLightClass::castShadow
//

OtObject OtLightClass::castShadow(float width, float near, float far, bool debug) {
	shadowCamera->setOrthographic(width, near, far);
	shadowCamera->setWidthLimits(width / 10.0, width * 10.0);
	shadowCamera->setNearFarLimits(near / 10.0, near * 10.0, far / 10.0, far * 10.0);
	shadowCamera->renderFrustum(debug);
	shadow = true;
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

		// update "light" camera
		auto target = context->getCamera()->getTarget();
		shadowCamera->setPositionVector(target + direction);
		shadowCamera->setTargetVector(target);
		shadowCamera->update();

		// create shadowmap frame buffer (if required)
		if (!bgfx::isValid(shadowmapFrameBuffer)) {
			shadowmapTexture = bgfx::createTexture2D(shadowmapSize, shadowmapSize, false, 1, bgfx::TextureFormat::D32F, BGFX_TEXTURE_RT);
			shadowmapFrameBuffer = bgfx::createFrameBuffer(1, &shadowmapTexture, true);
		}
	}

	// add light to context
	context->setLight(direction, diffuse, specular);
}


//
//	OtLightClass::render
//

void OtLightClass::render(OtRenderingContext context) {
	// only respond to the main rendering phase
	if (context->inMainPhase()) {
		// render shadowmap if required
		if (shadow) {
			// render shadow map
			bgfx::setViewClear(shadowView, BGFX_CLEAR_DEPTH);
			bgfx::setViewRect(shadowView, 0, 0, shadowmapSize, shadowmapSize);
			bgfx::setViewFrameBuffer(shadowView, shadowmapFrameBuffer);

			// create shadowmap rendering context
			OtRenderingContextClass shadowMapContext = *context;
			shadowMapContext.setPhase(OtRenderingContextClass::shadowmapPhase);
			shadowMapContext.setView(shadowView);
			shadowMapContext.setViewRect(0, 0, shadowmapSize, shadowmapSize);
			shadowMapContext.setCamera(shadowCamera);

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
		}

		// render camera frustum (if required)
		shadowCamera->render(context->getDebugDraw());
	}
}


//
//	OtLightClass::renderGUI
//

void OtLightClass::renderGUI() {
	ImGui::Checkbox("Enabled", &enabled);
	ImGui::Checkbox("Casts Shadow", &shadow);
	ImGui::SliderFloat3("Direction", glm::value_ptr(direction), -50.0f, 50.0f);
	ImGui::ColorEdit3("Diffuse", glm::value_ptr(diffuse));
	ImGui::ColorEdit3("Specular", glm::value_ptr(specular));

	if (shadow) {
		if (ImGui::TreeNodeEx("Shadow Camera", ImGuiTreeNodeFlags_Framed)) {
			shadowCamera->renderGUI();
			ImGui::TreePop();
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
		type->set("setDirection", OtFunctionClass::create(&OtLightClass::setDirection));
		type->set("setDiffuse", OtFunctionClass::create(&OtLightClass::setDiffuse));
		type->set("setSpecular", OtFunctionClass::create(&OtLightClass::setSpecular));

		type->set("castShadow", OtFunctionClass::create(&OtLightClass::castShadow));
	}

	return type;
}


//
//	OtLightClass::create
//

OtLight OtLightClass::create() {
	OtLight light = std::make_shared<OtLightClass>();
	light->setType(getMeta());
	return light;
}
