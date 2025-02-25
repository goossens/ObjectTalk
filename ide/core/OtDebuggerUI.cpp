//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "imgui.h"

#include "OtWorkspace.h"


//
//	OtWorkspace::renderDebugger
//

void OtWorkspace::renderDebugger() {
	// create debugger window
	ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
	ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);

	ImGui::Begin(
		"Debugger",
		nullptr,
		ImGuiWindowFlags_NoDecoration |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoBringToFrontOnFocus);

	// render the debugger control bar
	if (ImGui::Button("\u25a0 Stop", ImVec2(60.0f, 0.0f))) {
		subprocess.kill(SIGINT);
	}

	ImGui::SameLine();

	if (ImGui::Button("\u25b6 Run", ImVec2(60.0f, 0.0f))) {
	}

	ImGui::SameLine();

	if (ImGui::Button("\u25d5 Step", ImVec2(60.0f, 0.0f))) {
	}

	ImGui::SameLine();

	if (ImGui::Button("\u25bc Into", ImVec2(60.0f, 0.0f))) {
	}

	ImGui::SameLine();

	if (ImGui::Button("\u25b2 Out", ImVec2(60.0f, 0.0f))) {
	}

	ImGui::SameLine();
	std::string title = fmt::format("Runnning [{}]...", currentRunnable);
	ImGui::TextColored(ImVec4(0.0f, 1.0f, 1.0f, 1.0f), "%s", title.c_str());

	// render the console
	console.render();

	ImGui::End();
	ImGui::PopStyleVar();
}