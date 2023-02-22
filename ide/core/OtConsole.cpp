//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
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
	ImGui::BeginChild("Console", ImVec2(0.0, 0.0), true, ImGuiWindowFlags_HorizontalScrollbar);

	ImGuiListClipper clipper;
	clipper.Begin((int) lines.size());

	while (clipper.Step()) {
		for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++) {
			if (lines[i].type) {
				if (lines[i].type == LineType::Help) {
					ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0, 0.7, 0.7, 1.0));

				} else if (lines[i].type == LineType::Input) {
					ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0, 0.7, 1.0, 1.0));

				} else if (lines[i].type == LineType::Error) {
					ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0, 0.0, 0.0, 1.0));
				}
			}

			ImGui::TextUnformatted(lines[i].text.c_str());

			if (lines[i].type) {
				 ImGui::PopStyleColor();
			}
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
	lines.push_back(Line(Normal, ""));
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
	writeColored(Normal, text);
}


//
//	OtConsole::writeHelp
//

void OtConsole::writeHelp(const std::string& text) {
	writeColored(Help, text);
}


//
//	OtConsole::writeInput
//

void OtConsole::writeInput(const std::string& text) {
	writeColored(Input, std::string("<< ") + text);
}

//
//	OtConsole::writeError
//

void OtConsole::writeError(const std::string& text) {
	writeColored(Error, text);
}
