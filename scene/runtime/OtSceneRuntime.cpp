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
	scene = std::make_shared<OtScene>();
	renderer = std::make_shared<OtSceneRenderer>();

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
		}
	}
}


//
//	OtSceneRuntime::render
//

int OtSceneRuntime::render(int width, int height) {
	// update all the scripts
	for (auto [entity, component] : scene->view<OtScriptComponent>().each()) {
		component.update();
	}

	// get camera information
	auto camera = scene->getComponent<OtCameraComponent>(activeCamera);
	glm::mat4 cameraProjectionMatrix = camera.getProjectionMatrix((float) width / (float) height);
	glm::mat4 camerViewMatrix = glm::inverse(scene->getGlobalTransform(activeCamera));
	glm::vec3 cameraPosition = glm::vec3(camerViewMatrix[3]);

	// render the scene and return the ID of the generated texture
	renderer->setCameraPosition(cameraPosition);
	renderer->setViewMatrix(camerViewMatrix);
	renderer->setProjectionMatrix(cameraProjectionMatrix);
	renderer->setSize(width, height);
	return renderer->render(scene);
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
	// load and compile all the scripts
	for (auto [entity, component] : scene->view<OtScriptComponent>().each()) {
		component.load();
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
