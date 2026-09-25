//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "glm/glm.hpp" // IWYU pragma: keep

#include "OtException.h"
#include "OtLog.h"

#include "OtCamera.h"

#include "OtSceneViewer.h"


//
//	OtSceneViewer::render
//

void OtSceneViewer::render(OtScene* scene, const ImVec2& requestedSize) {
	// determine positions and size
	size = (requestedSize.x == 0.0f && requestedSize.y == 0.0f) ? ImGui::GetContentRegionAvail() : requestedSize;
	startPos = ImGui::GetCursorScreenPos();
	endPos = ImGui::GetCursorScreenPos();

	// render scene (or error message)
	if (error.size()) {
		renderCenteredText(error);

	} else {
		try {
			updateScene(scene);
			renderScene(scene);
			renderMessages(scene);

		} catch (const OtException& exception) {
			error = exception.what();
			renderCenteredText(error);
		}
	}

	ImGui::SetCursorScreenPos(endPos);
}


//
//	OtSceneViewer::updateScene
//

void OtSceneViewer::updateScene(OtScene* scene) {
	// on ly do this during first frame
	if (!initialized) {
		// setup default camera
		findActiveCamera(scene);

		// reset message timers on startup
		for (auto&& [entity, component] : scene->view<OtMessageComponent>().each()) {
			component.visibleRemaining = component.visibleTime;
			component.fadeRemaining = component.fadeTime;
		};

		initialized = true;
	}

	// update all scripts
	for (auto&& [entity, component] : scene->view<OtScriptComponent>().each()) {
		component.update(scene, entity);
	}

	// update all models
	for (auto&& [entity, component] : scene->view<OtModelComponent>().each()) {
		component.update();
	}

	// update (evaluate) all nodes
	scene->evaluateNodes();
}


//
//	OtSceneViewer::renderScene
//

void OtSceneViewer::renderScene(OtScene* scene) {
	// get camera information
	auto& component = scene->getComponent<OtCameraComponent>(activeCamera);
	glm::mat4 transform = scene->getGlobalTransform(activeCamera);
	glm::mat4 cameraViewMatrix = glm::inverse(transform);

	// create a camera
	OtCamera camera{
		static_cast<int>(size.x), static_cast<int>(size.y),
		component.nearPlane, component.farPlane, component.fov,
		cameraViewMatrix};

	// render the scene and show it on screen
	if (!renderer) {
		renderer = std::make_unique<OtSceneRenderer>();
	}

	ImGui::Image(renderer->render(camera, scene), size);
}


//
//	OtSceneViewer::renderMessages
//

void OtSceneViewer::renderMessages(OtScene* scene) {
	// render messages
	auto delta = ImGui::GetIO().DeltaTime;

	for (auto&& [entity, component] : scene->view<OtMessageComponent>().each()) {
		if (component.visibleRemaining > 0.0f || component.fadeRemaining > 0.0f) {
			auto alpha = 1.0f;

			if (component.visibleRemaining < 0.0f && component.fadeRemaining > 0.0f) {
				alpha = component.fadeRemaining / component.fadeTime;
			}

			renderCenteredText(component.message, alpha);

			if (component.visibleRemaining > 0.0f) {
				component.visibleRemaining -= delta;

			} else {
				component.fadeRemaining -= delta;
			}
		}
	}
}


//
//	OtSceneViewer::renderCenteredText
//

void OtSceneViewer::renderCenteredText(const std::string& text, float alpha) {
	auto backgroundColor = ImGui::GetColorU32(ImGuiCol_WindowBg, alpha);
	auto borderColor = ImGui::GetColorU32(ImGuiCol_Border, alpha);
	auto textColor = ImGui::GetColorU32(ImGuiCol_Text, alpha);
	auto padding = ImGui::GetStyle().WindowPadding;
	auto messageSize = ImGui::CalcTextSize(text.c_str());
	auto backgroundSize = messageSize + padding * 2.0f;

	auto drawList = ImGui::GetWindowDrawList();
	auto topLeft = startPos + ImVec2(size.x * 0.5f - backgroundSize.x * 0.5f, 10.0f);
	auto bottomRight = topLeft + backgroundSize;

	drawList->AddRectFilled(topLeft, bottomRight, backgroundColor);
	drawList->AddRect(topLeft, bottomRight, borderColor);
	topLeft += padding;

	OtText::splitIterator(text, '\n', [&](const std::string& line) {
		auto textString = line.c_str();
		auto textSize = ImGui::CalcTextSize(textString);
		drawList->AddText(topLeft + ImVec2((messageSize.x - textSize.x) * 0.5f, 0.0f), textColor, textString);
		topLeft += ImVec2(0.0f, textSize.y);
	});
}


//
//	OtSceneViewer::findActiveCamera
//

void OtSceneViewer::findActiveCamera(OtScene* scene) {
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
		OtLogError("No camera found in scene");
	}
}
