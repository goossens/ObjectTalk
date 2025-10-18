//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "glm/glm.hpp"
#include "imgui.h"

#include "OtLog.h"
#include "OtText.h"

#include "OtAssetManager.h"

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
		initializeMessagingSystem();
		ready = true;
		return true;
	}
}


//
//	OtSceneRuntime::render
//

ImTextureID OtSceneRuntime::render(int width, int height) {
	// get our scene
	auto scene = sceneAsset->getScene();

	// update all the scripts
	for (auto&& [entity, component] : scene->view<OtScriptComponent>().each()) {
		component.update();
	}

	// update all the models
	for (auto&& [entity, component] : scene->view<OtModelComponent>().each()) {
		component.update();
	}

	// update (evaluate) all the nodes
	scene->evaluateNodes();

	// get camera information
	auto& component = scene->getComponent<OtCameraComponent>(activeCamera);
	glm::mat4 transform = scene->getGlobalTransform(activeCamera);
	glm::mat4 cameraViewMatrix = glm::inverse(transform);

	// create a camera
	OtCamera camera{
		width, height,
		component.nearPlane, component.farPlane, component.fov,
		cameraViewMatrix};

	// render the scene and return the ID of the generated texture
	return renderer->render(camera, scene);
}


//
//	OtSceneRuntime::messages
//

void OtSceneRuntime::messages() {
	// render messages
	auto delta = ImGui::GetIO().DeltaTime;

	for (auto&& [entity, component] : sceneAsset->getScene()->view<OtMessageComponent>().each()) {
		if (component.visibleRemaining >= 0.0f || component.fadeRemaining >= 0.0f) {
			if (component.visibleRemaining < 0.0f && component.fadeRemaining > 0.0f) {
				ImGui::PushStyleVar(ImGuiStyleVar_Alpha, component.fadeRemaining / component.fadeTime);
			}

			ImGui::SetNextWindowPos(ImVec2(ImGui::GetMainViewport()->GetCenter().x, 10.0f), ImGuiCond_Always, ImVec2(0.5f, 0.0f));

			ImGui::Begin(
				"Message",
				nullptr,
				ImGuiWindowFlags_NoDecoration |
					ImGuiWindowFlags_AlwaysAutoResize |
					ImGuiWindowFlags_NoFocusOnAppearing |
					ImGuiWindowFlags_NoNav);

			OtText::splitIterator(component.message, '\n', [](const std::string& line) {
				OtUi::centeredText(line.c_str());
			});

			ImGui::End();

			if (component.visibleRemaining < 0.0f && component.fadeRemaining > 0.0f) {
				ImGui::PopStyleVar();
			}

			if (component.visibleRemaining > 0.0f) {
				component.visibleRemaining -= delta;

			} else if (component.visibleRemaining < 0.0f) {
				component.fadeRemaining -= delta;
			}
		}
	}
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
		OtLogError("No camera found in scene at [{}]", sceneAsset.getPath());
	}
}


//
//	OtSceneRuntime::initializeMessagingSystem
//

void OtSceneRuntime::initializeMessagingSystem() {
	for (auto&& [entity, component] : sceneAsset->getScene()->view<OtMessageComponent>().each()) {
		component.visibleRemaining = component.visibleTime;
		component.fadeRemaining = component.fadeTime;
	};
}
