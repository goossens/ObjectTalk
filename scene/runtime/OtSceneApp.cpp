//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "imgui.h"

#include "OtAssetManager.h"
#include "OtUi.h"

#include "OtSceneApp.h"


//
//	OtSceneApp::onSetup
//

void OtSceneApp::onSetup() {
	// load scene
	scene.load(scenePath);
}


//
//	OtSceneApp::onRender
//

void OtSceneApp::onRender() {
	// are we loaded yet?
	if (ready) {
		// yes, render viewport
		renderViewPort();

	} else {
		// no, show the splash screen for now
		renderSplashScreen();
		ready = !OtAssetManager::isLoading();
	}
}


//
//	OtSceneApp::onTerminate
//

void OtSceneApp::onTerminate() {
	// release all resources
	viewer.clear();
	scene.clear();
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
			ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoBringToFrontOnFocus |
			ImGuiWindowFlags_NoInputs);

	ImGui::Image(logo->getTextureID(), logo->getSize());
	OtUi::centerTextInWindow("Loading scene...");
	OtUi::centerTextInWindow("");

	ImGui::End();
}


//
//	OtSceneApp::renderViewPort
//

void OtSceneApp::renderViewPort() {
	// create the window
	ImGui::SetNextWindowPos(ImVec2());
	ImGui::SetNextWindowSize(ImGui::GetMainViewport()->Size);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::Begin("Scene", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoResize);

	// render the scene
	viewer.render(&scene);

	// cleanup
	ImGui::End();
	ImGui::PopStyleVar();
}
