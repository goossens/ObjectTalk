//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <chrono>

#include "glm/glm.hpp"

#include "OtException.h"

#include "OtInput.h"
#include "OtMath.h"

#include "OtAnimator.h"
#include "OtEntityObject.h"
#include "OtSceneModule.h"
#include "OtSceneRuntime.h"


//
//	OtSceneRuntime::setup
//

void OtSceneRuntime::setup(std::filesystem::path path) {
	// start the loader thread
	scenePath = path;
	loader = std::async(std::launch::async, &OtSceneRuntime::load, this);
}


//
//	OtSceneRuntime::load
//

bool OtSceneRuntime::load() {
	// create the scene and a renderer
	scene = std::make_unique<OtScene>();
	renderer = std::make_unique<OtSceneRenderer>();

	// load the scene and initialize the systems
	scene->load(scenePath);
	initializeScriptingSystem();
	initializeRenderingSystem();

	// we are now fully loaded
	ready = true;
	return true;
}


//
//	OtSceneRuntime::isReady
//

bool OtSceneRuntime::isReady() {
	if (ready) {
		return true;

	} else {
		using namespace std::chrono_literals;

		if (loader.wait_for(0ms) == std::future_status::ready) {
			auto result = loader.get();
			ready = true;
			return true;

		} else {
			return false;
		}
	}
}


//
//	OtSceneRuntime::render
//

int OtSceneRuntime::render(int width, int height) {
	// run all animations
	OtAnimator::instance()->update();

	// update all the scripts
	for (auto [entity, component] : scene->view<OtScriptComponent>().each()) {
		component.update();
	}

	// get camera information
	auto camera = scene->getComponent<OtCameraComponent>(activeCamera);
	glm::vec3 cameraPosition = scene->getComponent<OtTransformComponent>(activeCamera).translation;
	glm::mat4 camerViewMatrix = glm::inverse(scene->getGlobalTransform(activeCamera));
	glm::mat4 cameraProjectionMatrix = camera.getProjectionMatrix((float) width / (float) height);

	// render the scene and return the ID of the generated texture
	renderer->setCameraPosition(cameraPosition);
	renderer->setViewMatrix(camerViewMatrix);
	renderer->setProjectionMatrix(cameraProjectionMatrix);
	renderer->setSize(width, height);
	return renderer->render(scene.get());
}


//
//	OtSceneRuntime::terminate
//

void OtSceneRuntime::terminate() {
	// clear the scene and the renderer to release all resources
	scene = nullptr;
	renderer = nullptr;
}


//
//	OtSceneRuntime::initializeScriptingSystem
//

void OtSceneRuntime::initializeScriptingSystem() {
	// register modules
	OtInputRegister();
	OtMathRegister();
	OtSceneModuleRegister();

	// load and compile all the scripts
	for (auto [entity, component] : scene->view<OtScriptComponent>().each()) {
		component.load();
		OtEntityObject(component.instance)->linkToECS(scene.get(), entity);
	}

	// now initialize all the scripts
	for (auto [entity, component] : scene->view<OtScriptComponent>().each()) {
		component.create();
	}
}


//
//	OtSceneRuntime::initializeRenderingSystem
//

void OtSceneRuntime::initializeRenderingSystem() {
	// select default camera
	OtEntity firstCamera = OtEntityNull;
	OtEntity mainCamera = OtEntityNull;

	for (auto [entity, component] : scene->view<OtCameraComponent>().each()) {
		if (!scene->isValidEntity(firstCamera)) {
			firstCamera = entity;
		}

		if (!scene->isValidEntity(mainCamera) && component.mainCamera) {
			mainCamera = entity;
		}
	}

	if (scene->isValidEntity(mainCamera)) {
		activeCamera = mainCamera;

	} else if (scene->isValidEntity(firstCamera)) {
		activeCamera = firstCamera;

	} else {
		OtExcept("No camera found in scene at [%s]", scenePath.c_str());
	}

	cameraSelected = true;
}


//
//	OtSceneRuntime::initializeAssetSystem
//

void OtSceneRuntime::initializeAssetSystem() {
	// load all material assets
	for (auto [entity, component] : scene->view<OtMaterialComponent>().each()) {
		component.material->update();
	}
}
