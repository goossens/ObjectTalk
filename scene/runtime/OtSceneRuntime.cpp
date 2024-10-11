//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "glm/glm.hpp"

#include "OtLog.h"

#include "OtAssetManager.h"

#include "OtAnimator.h"
#include "OtEntityObject.h"
#include "OtSceneRuntime.h"


//
//	OtSceneRuntime::setup
//

void OtSceneRuntime::setup(std::string path) {
	// start loading the scene and create a renderer
	sceneAsset = path;
	renderer = std::make_unique<OtSceneRenderer>();
}


//
//	OtSceneRuntime::isReady
//

bool OtSceneRuntime::isReady() {
	// are we past initialization?
	if (ready) {
		return true;

	// are we still loading assets?
	} else if (OtAssetManager::isLoading()) {
		return false;

	// just finished loading our assets so we can now complete the setup
	} else {
		initializeScriptingSystem();
		initializeRenderingSystem();
		ready = true;
		return true;
	}
}


//
//	OtSceneRuntime::render
//

int OtSceneRuntime::render(int width, int height) {
	// run all animations
	OtAnimator::update();

	// get our scene
	auto scene = sceneAsset->getScene();

	// update all the scripts
	for (auto&& [entity, component] : scene->view<OtScriptComponent>().each()) {
		component.update();
	}

	// update (evaluate) all the nodes
	scene->evaluateNodes();

	// get camera information
	auto& component = scene->getComponent<OtCameraComponent>(activeCamera);
	glm::mat4 transform = scene->getGlobalTransform(activeCamera);
	glm::vec3 cameraPosition = glm::vec3(transform[3]);
	glm::mat4 cameraViewMatrix = glm::inverse(transform);

	OtCamera camera{
		width, height,
		component.nearPlane, component.farPlane, component.fov,
		cameraPosition, cameraViewMatrix};

	// render the scene and return the ID of the generated texture
	return renderer->render(camera, scene);
}


//
//	OtSceneRuntime::terminate
//

void OtSceneRuntime::terminate() {
	// clear the scene and the renderer to release all resources
	sceneAsset.clear();
	renderer = nullptr;
}


//
//	OtSceneRuntime::initializeScriptingSystem
//

void OtSceneRuntime::initializeScriptingSystem() {
	// access the scene
	auto scene = sceneAsset->getScene();

	// load and compile all the scripts
	for (auto&& [entity, component] : scene->view<OtScriptComponent>().each()) {
		component.process();
		OtEntityObject(component.instance)->linkToECS(scene, entity);
	}

	// now initialize all the scripts
	for (auto&& [entity, component] : scene->view<OtScriptComponent>().each()) {
		component.create();
	}
}


//
//	OtSceneRuntime::initializeRenderingSystem
//

void OtSceneRuntime::initializeRenderingSystem() {
	// access the scene
	auto scene = sceneAsset->getScene();

	// select default camera
	OtEntity firstCamera = OtEntityNull;
	OtEntity mainCamera = OtEntityNull;

	for (auto&& [entity, component] : scene->view<OtCameraComponent>().each()) {
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
		OtLogFatal("No camera found in scene at [{}]", sceneAsset.getPath());
	}
}
