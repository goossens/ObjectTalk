//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
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
	ImGui::BeginChild("Console", ImVec2(0.0, 0.0), ImGuiChildFlags_Borders, ImGuiWindowFlags_HorizontalScrollbar);

	ImGuiListClipper clipper;
	clipper.Begin((int) lines.size());

	while (clipper.Step()) {
		for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++) {
			if (lines[i].type == Type::standardOut) {
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0, 1.0, 1.0, 1.0));

			} else if (lines[i].type == Type::standardError) {
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0, 0.0, 0.0, 1.0));

			} else if (lines[i].type == Type::help) {
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0, 0.7, 0.7, 1.0));

			} else if (lines[i].type == Type::input) {
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0, 0.7, 1.0, 1.0));

			} else if (lines[i].type == Type::success) {
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0, 1.0, 0.0, 1.0));

			} else if (lines[i].type == Type::debug) {
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.7, 0.7, 0.7, 1.0));

			} else if (lines[i].type == Type::info) {
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5, 0.8, 1.0, 1.0));

			} else if (lines[i].type == Type::warning) {
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0, 0.8, 0.0, 1.0));

			} else if (lines[i].type == Type::error) {
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0, 0.3, 0.0, 1.0));

			} else if (lines[i].type == Type::fatal) {
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

	// show context menu (if required)
	if (ImGui::BeginPopupContextWindow("Console")) {
		if (ImGui::MenuItem("Clear")) { clear(); }
		if (ImGui::MenuItem("Copy")) { copy(); }
		ImGui::EndPopup();
	}

	ImGui::EndChild();
}


//
//	OtConsole::clear
//

void OtConsole::clear() {
	lines.clear();
	lines.push_back(Line(Type::standardOut, ""));
}


//
//	OtConsole::copy
//

void OtConsole::copy() {
	std::string text;

	for (auto& line : lines) {
		text += line.text + "\n";
	}

	ImGui::SetClipboardText(text.c_str());
}


//
//	OtConsole::writeColored
//

void OtConsole::writeColored(Type type, const std::string& text) {
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
	writeColored(Type::standardOut, text);
}


//
//	OtConsole::writeError
//

void OtConsole::writeError(const std::string& text) {
	writeColored(Type::standardError, text);
}


//
//	OtConsole::writeHelp
//

void OtConsole::writeHelp(const std::string& text) {
	writeColored(Type::help, text);
}


//
//	OtConsole::writeInput
//

void OtConsole::writeInput(const std::string& text) {
	writeColored(Type::input, std::string("<< ") + text);
}


//
//	OtConsole::writeSuccess
//

void OtConsole::writeSuccess(const std::string& text) {
	writeColored(Type::success, text);
}


//
//	OtConsole::writeLog
//

void OtConsole::writeLog(OtLog::Type type, const std::string& text) {
	writeColored(static_cast<Type>(static_cast<int>(type) + 10), text);
}
