//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "glm/glm.hpp"

#include "OtException.h"

#include "OtSceneRuntime.h"


//
//	OtSceneRuntime::~OtSceneRuntime
//

OtSceneRuntime::~OtSceneRuntime() {
	if (loader.joinable()) {
		loader.join();
	}
}


//
//	OtSceneRuntime::setup
//

void OtSceneRuntime::setup(std::filesystem::path path) {
	// start the loader thread
	scenePath = path;
	loader = std::thread(&OtSceneRuntime::load, this);
}


//
//	OtSceneRuntime::load
//

void OtSceneRuntime::load() {
	// create the scene and a renderer
	scene = std::make_shared<OtScene>();
	renderer = std::make_shared<OtSceneRenderer>();

	// load the scene
	scene->load(scenePath);

	// we are now fully loaded
	loaded = true;
}


//
//	OtSceneRuntime::render
//

int OtSceneRuntime::render(int width, int height) {
	// select default camera if this is the first time through
	if (!cameraSelected) {
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
