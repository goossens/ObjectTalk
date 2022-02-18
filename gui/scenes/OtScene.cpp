//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "bgfx/embedded_shader.h"
#include "glm/ext.hpp"

#include "OtException.h"

#include "OtFramework.h"
#include "OtScene.h"
#include "OtSceneObject.h"
#include "OtShadowShader.h"


//
//	Globals
//

static const bgfx::EmbeddedShader embeddedShaders[] = {
	BGFX_EMBEDDED_SHADER(OtShadowVS),
	BGFX_EMBEDDED_SHADER(OtShadowFS),
	BGFX_EMBEDDED_SHADER_END()
};


//
//	OtSceneClass::~OtSceneClass
//

OtSceneClass::~OtSceneClass() {
	// release resources
	if (bgfx::isValid(shadowmapFrameBuffer)) {
		bgfx::destroy(shadowmapFrameBuffer);
	}

	if (bgfx::isValid(shader)) {
		bgfx::destroy(shader);
	}
}


//
//	OtSceneClass::validateChild
//

void OtSceneClass::validateChild(OtComponent child) {
	if (!child->isKindOf("SceneObject")) {
		OtExcept("A [Scene] can only have [SceneObjects] as children, not [%s]", child->getType()->getName().c_str());
	}
}


//
//	OtSceneClass::setShadow
//

OtObject OtSceneClass::setShadow(bool flag) {
	shadow = flag;
	return shared();
}


//
//	OtSceneClass::update
//

void OtSceneClass::update(OtCamera camera, float x, float y, float w, float h) {
	// reset context state
	context.clear();
	context.setViewRect(x, y, w, h);
	context.setScene(cast<OtSceneClass>());
	context.setCamera(camera);

	// reserve shadow rendering view (if required)
	if (shadow) {
		shadowView = OtFrameworkClass::instance()->getNextViewID();
	}

	// update all children
	for (auto const& child : children) {
		if (child->isEnabled()) {
			child->cast<OtSceneObjectClass>()->update(&context);
		}
	}

	// don't keep circular references
	context.setScene(nullptr);
}


//
//	OtSceneClass::render
//

void OtSceneClass::render() {
	// reference ourselves in context
	context.setScene(cast<OtSceneClass>());

	// create shadow map (if required)
	if (shadow) {
		createShadowmap();
	}

	// setup scene and render it for real
	context.setView(OtFrameworkClass::instance()->getNextViewID());

	bgfx::setViewRect(
		context.getView(),
		context.getViewX(),
		context.getViewY(),
		context.getViewW(),
		context.getViewH());

	context.getCamera()->submit(&context);
	renderChildren(&context);

	// don't keep circular references
	context.setScene(nullptr);
}


//
//	OtSceneClass::renderChildren
//

void OtSceneClass::renderChildren(OtRenderingContext context) {
	for (auto const& child : children) {
		if (child->isEnabled()) {
			child->cast<OtSceneObjectClass>()->render(context);
		}
	}
}


//
//	OtSceneClass::createShadowmap
//

void OtSceneClass::createShadowmap() {
	// (re) create the shadowmap frame buffer (if required)
	auto aspectRatio = context.getViewAspectRatio();

	if (shadowmapAspectRation != aspectRatio) {
		if (bgfx::isValid(shadowmapFrameBuffer)) {
			bgfx::destroy(shadowmapFrameBuffer);
		}

		// create new shadowmap
		int w = 512;
		int h = w / aspectRatio;
		uint64_t flags = BGFX_TEXTURE_RT | BGFX_SAMPLER_COMPARE_LEQUAL;
		shadowmapTexture = bgfx::createTexture2D(w, h, false, 1, bgfx::TextureFormat::D16, flags);
		shadowmapFrameBuffer = bgfx::createFrameBuffer(1, &shadowmapTexture, true);
		shadowmapAspectRation = aspectRatio;
	}

	// create shader (if required)
	if (!bgfx::isValid(shader)) {
		bgfx::RendererType::Enum type = bgfx::getRendererType();

		shader = bgfx::createProgram(
			bgfx::createEmbeddedShader(embeddedShaders, type, "OtShadowVS"),
			bgfx::createEmbeddedShader(embeddedShaders, type, "OtShadowFS"),
			true);
	}

	// get a camera from the lights point of view
	shadowCamera = context.getCameraFromLightPosition();

	// create shadow map rendering context
	OtRenderingContextClass shadowMapContext;
	shadowMapContext.setView(shadowView);
	shadowMapContext.setViewRect(0, 0, 512, 512 / aspectRatio);
	shadowMapContext.setCamera(shadowCamera);
	shadowMapContext.setScene(cast<OtSceneClass>());

	// render shadow map
	bgfx::setViewClear(shadowView, BGFX_CLEAR_DEPTH);
	bgfx::setViewRect(shadowView, 0, 0, 512, 512 / aspectRatio);
	bgfx::setViewFrameBuffer(shadowView, shadowmapFrameBuffer);
	shadowCamera->submit(&shadowMapContext);

	// render all scene objects
	uint64_t state = BGFX_STATE_WRITE_Z | BGFX_STATE_DEPTH_TEST_LESS | BGFX_STATE_CULL_CCW | BGFX_STATE_MSAA;

	for (auto const& child : children) {
		if (child->isEnabled()) {
			child->cast<OtSceneObjectClass>()->renderShadow(shadowView, state, shader);
		}
	}

	// now set the matching matrix for the final rendering
	glm::mat4 shadowmapMatrix = shadowCamera->getViewProjectionMatrix();

	if (bgfx::getCaps()->originBottomLeft) {
		shadowmapMatrix = glm::scale(shadowmapMatrix, glm::vec3(1.0, -1.0, 1.0));
	}

	context.setShadow(shadowmapTexture, shadowmapMatrix);
}


//
//	OtSceneClass::getMeta
//

OtType OtSceneClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtTypeClass::create<OtSceneClass>("Scene", OtComponentClass::getMeta());
		type->set("setShadow", OtFunctionClass::create(&OtSceneClass::setShadow));
	}

	return type;
}


//
//	OtSceneClass::create
//

OtScene OtSceneClass::create() {
	OtScene scene = std::make_shared<OtSceneClass>();
	scene->setType(getMeta());
	return scene;
}
