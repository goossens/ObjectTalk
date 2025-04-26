//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "imgui.h"

#include "OtGpu.h"
#include "OtUi.h"

#include "OtSceneApp.h"


//
//	OtSceneApp::onSetup
//

void OtSceneApp::onSetup() {
	// pass to runtime
	setup(scenePath);
}


//
//	OtSceneApp::onRender
//

void OtSceneApp::onRender() {
	// are we loaded yet?
	if (isReady()) {
		// yes, render viewport
		renderViewPort();

	} else {
		// no, show the splash screen for now
		renderSplashScreen();
	}
}


//
//	OtSceneApp::onTerminate
//

void OtSceneApp::onTerminate() {
	// pass to runtime
	terminate();

	// release resources
	logo = nullptr;
}


//
//	OtSceneApp::renderSplashScreen
//

void OtSceneApp::renderSplashScreen() {
	// load logo (if required)
	if (!logo) {
		logo = std::make_shared<OtLogo>();
	}

	// render splash screen
	ImVec2 center = ImGui::GetMainViewport()->GetCenter();
	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

	ImGui::Begin(
		"SplashScreen",
		nullptr,
		ImGuiWindowFlags_NoDecoration |
			ImGuiWindowFlags_AlwaysAutoResize |
			ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoBringToFrontOnFocus |
			ImGuiWindowFlags_NoInputs);

	ImGui::Image(
		(ImTextureID)(intptr_t) logo->getTextureIndex(),
		ImVec2(static_cast<float>(logo->getWidth()), static_cast<float>(logo->getHeight())));

	OtUi::centeredText("Loading scene...");
	OtUi::centeredText("");

	ImGui::End();
}


//
//	OtSceneApp::renderViewPort
//

void OtSceneApp::renderViewPort() {
	// create the window
	ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
	ImGui::SetNextWindowSize(ImGui::GetMainViewport()->Size);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);

	ImGui::Begin(
		"Scene",
		nullptr,
		ImGuiWindowFlags_NoDecoration |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoBringToFrontOnFocus);

	// render the scene
	auto size = ImGui::GetContentRegionAvail();
	auto textureIndex = render(static_cast<int>(size.x), static_cast<int>(size.y));

	// show it on the screen
	if (OtGpuHasOriginBottomLeft()) {
		ImGui::Image((ImTextureID)(intptr_t) textureIndex, size, ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f));

	} else {
		ImGui::Image((ImTextureID)(intptr_t) textureIndex, size);
	}

	ImGui::End();
	ImGui::PopStyleVar();

	// render messages
	messages();
}
