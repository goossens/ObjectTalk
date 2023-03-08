//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "imgui.h"

#include "OtFramework.h"
#include "OtGpu.h"
#include "OtUi.h"

#include "OtSceneApp.h"


//
//	OtSceneApp::run
//

void OtSceneApp::run(std::filesystem::path path) {
	// remember the path
	scenePath = path;

	// run the framework;
	OtFrameworkClass::instance()->run(this);
}


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
	if (isLoaded()) {
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

	ImVec2 center = ImGui::GetMainViewport()->GetCenter();
	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5, 0.5));

	ImGui::Begin(
		"SplashScreen",
		nullptr,
		ImGuiWindowFlags_NoDecoration |
			ImGuiWindowFlags_AlwaysAutoResize |
			ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoBringToFrontOnFocus |
			ImGuiWindowFlags_NoInputs);

	ImGui::Image((void*)(intptr_t) logo->getTextureIndex(), ImVec2(logo->getWidth(), logo->getHeight()));

	OtUiCenteredText("Loading scene...");
	OtUiCenteredText("");

	ImGui::End();
}


//
//	OtSceneApp::renderViewPort
//

void OtSceneApp::renderViewPort() {
	// create the window
	ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
	ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);

	ImGui::Begin(
		"Scene",
		nullptr,
		ImGuiWindowFlags_NoDecoration |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoBringToFrontOnFocus);

	// render the scene
	auto size = ImGui::GetContentRegionAvail();
	auto textureIndex = render(size.x, size.y);

	// show it on the screen
	if (OtGpuHasOriginBottomLeft()) {
		ImGui::Image((void*)(intptr_t) textureIndex, size, ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f));

	} else {
		ImGui::Image((void*)(intptr_t) textureIndex, size);
	}

	ImGui::End();
	ImGui::PopStyleVar();
}
