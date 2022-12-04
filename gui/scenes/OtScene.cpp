//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "debugdraw.h"

#include "OtException.h"

#include "OtFramework.h"
#include "OtScene.h"
#include "OtSceneObject.h"


//
//	OtSceneClass::validateChild
//

void OtSceneClass::validateChild(OtComponent child) {
	if (!child->isKindOf("SceneObject")) {
		OtExcept("A [Scene] can only have [SceneObjects] as children, not [%s]", child->getType()->getName().c_str());
	}
}


//
//	OtSceneClass::update
//

void OtSceneClass::update(OtCamera camera, float x, float y, float w, float h) {
	// update the camera
	camera->update(w / h);

	// create a new rendering context
	context = new OtRenderingContextClass();
	context->setViewRect(x, y, w, h);
	context->setScene(cast<OtSceneClass>());
	context->setCamera(camera);

	// update all children
	for (auto const& child : children) {
		if (child->isEnabled()) {
			child->cast<OtSceneObjectClass>()->update(context);
		}
	}
}


//
//	OtSceneClass::render
//

void OtSceneClass::render() {
	// get our camera
	auto camera = context->getCamera();

	// setup debug drawing view
	bgfx::setViewRect(254, context->getViewX(), context->getViewY(), context->getViewW(), context->getViewH());
	camera->submit(254);

	DebugDrawEncoder debugDraw;
	context->setDebugDraw(&debugDraw);
	debugDraw.begin(254);
	debugDraw.setState(true, true, true);
	camera->render(&debugDraw);

	//debugDraw.drawAxis(0.0, 0.0, 0.0);
	//debugDraw->drawOrb(cameraTarget.x, cameraTarget.y, cameraTarget.z, 1);

	// reference ourselves in context
	context->setScene(cast<OtSceneClass>());

	// setup scene and render it for real
	context->setPass(OtRenderingContextClass::lightingPass);
	context->setView(OtFrameworkClass::instance()->getNextViewID());

	bgfx::setViewRect(
		context->getView(),
		context->getViewX(),
		context->getViewY(),
		context->getViewW(),
		context->getViewH());

	camera->submit(context->getView());
	renderChildren(context);

	// render debug drawing
	debugDraw.end();

	// delete rendering context
	delete context;
	context = nullptr;
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
//	OtSceneClass::getMeta
//

OtType OtSceneClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtTypeClass::create<OtSceneClass>("Scene", OtComponentClass::getMeta());
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
