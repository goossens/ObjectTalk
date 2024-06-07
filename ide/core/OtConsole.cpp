//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "imgui.h"

#include "OtConsole.h"


//
//	OtConsole::OtConsole
//

OtConsole::OtConsole() {
	clear();
}


//
//	OtConsole::render
//

void OtConsole::render() {
	// create the window
	ImGui::BeginChild("Console", ImVec2(0.0, 0.0), ImGuiChildFlags_Border, ImGuiWindowFlags_HorizontalScrollbar);

	ImGuiListClipper clipper;
	clipper.Begin((int) lines.size());

	while (clipper.Step()) {
		for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++) {
			if (lines[i].type == LineType::standardOut) {
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0, 1.0, 1.0, 1.0));

			} else if (lines[i].type == LineType::standardError) {
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0, 0.0, 0.0, 1.0));

			} else if (lines[i].type == LineType::help) {
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0, 0.7, 0.7, 1.0));

			} else if (lines[i].type == LineType::input) {
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0, 0.7, 1.0, 1.0));

			} else if (lines[i].type == LineType::success) {
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0, 1.0, 0.0, 1.0));

			} else if (lines[i].type == LineType::debug) {
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.7, 0.7, 0.7, 1.0));

			} else if (lines[i].type == LineType::info) {
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5, 0.8, 1.0, 1.0));

			} else if (lines[i].type == LineType::warning) {
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0, 0.8, 0.0, 1.0));

			} else if (lines[i].type == LineType::error) {
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0, 0.3, 0.0, 1.0));

			} else if (lines[i].type == LineType::fatal) {
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0, 0.3, 0.0, 1.0));

			} else {
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0, 0.0, 1.0, 1.0));
			}

			ImGui::TextUnformatted(lines[i].text.c_str());
			ImGui::PopStyleColor();
		}
	}

	if (scrollBottom || ImGui::GetScrollY() >= ImGui::GetScrollMaxY()) {
		scrollBottom = false;
		ImGui::SetScrollHereY(1.0);
	}

	ImGui::EndChild();
}


//
//	OtConsole::clear
//

void OtConsole::clear() {
	lines.clear();
	lines.push_back(Line(standardOut, ""));
}


//
//	OtConsole::writeColored
//

void OtConsole::writeColored(LineType type, const std::string& text) {
	std::string part;

	for (auto& c : text) {
		if (c == '\n') {
			lines.back().type = type;
			lines.back().text += part;
			part.clear();

			lines.push_back(Line(type, ""));

		} else {
			part += c;
		}
	}

	if (part.size()) {
		lines.back().type = type;
		lines.back().text += part;
	}
}


//
//	OtConsole::write
//

void OtConsole::write(const std::string& text) {
	writeColored(standardOut, text);
}


//
//	OtConsole::writeError
//

void OtConsole::writeError(const std::string& text) {
	writeColored(standardError, text);
}


//
//	OtConsole::writeHelp
//

void OtConsole::writeHelp(const std::string& text) {
	writeColored(help, text);
}


//
//	OtConsole::writeInput
//

void OtConsole::writeInput(const std::string& text) {
	writeColored(input, std::string("<< ") + text);
}


//
//	OtConsole::writeSuccess
//

void OtConsole::writeSuccess(const std::string& text) {
	writeColored(success, text);
}


//
//	OtConsole::writeLog
//

void OtConsole::writeLog(int type, const std::string& text) {
	writeColored(LineType(type + 10), text);
}
