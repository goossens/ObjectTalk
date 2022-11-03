//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "imgui.h"

#include "OtException.h"
#include "OtNumbers.h"
#include "OtString.h"

#include "OtFramework.h"

#include "OtConsole.h"


//
//	OtConsoleClass::OtConsoleClass
//

OtConsoleClass::OtConsoleClass() {
	// send welcome message
	lines.push_back(Line(Normal, ""));

	writeHelp(
		"Welcome to the ObjectTalk Integrated Development Environment (IDE)\n"
		"Right click here or type 'help' below for options\n"
	);
}


//
//	OtConsoleClass::render
//

void OtConsoleClass::render() {
	// calculate size and position for editor window
	OtFramework framework = OtFrameworkClass::instance();
	int width = framework->getWidth();
	int height = framework->getHeight();
	ImVec2 size = ImVec2(width * 0.6, height * 0.6);

	ImVec2 pos = ImVec2(
		width * 0.05 + OtRandom(width * 0.9 - size.x),
		height * 0.05 + OtRandom(height * 0.9 - size.y));

	// create the window
	ImGui::Begin("Console");
	ImGui::SetWindowSize(size, ImGuiCond_FirstUseEver);
	ImGui::SetWindowPos(pos, ImGuiCond_FirstUseEver);
	ImGui::PushID(this);

	float footerHeight = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
	ImGui::BeginChild("scrolling", ImVec2(0, -footerHeight), false, ImGuiWindowFlags_HorizontalScrollbar);

	if (ImGui::BeginPopupContextWindow()) {
		if (ImGui::MenuItem("Help")) { help(); }
		if (ImGui::MenuItem("Clear")) { clear(); }
		ImGui::EndPopup();
	}

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
	ImGui::Separator();

	// command line input
	ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
	ImGuiInputTextFlags input_text_flags = ImGuiInputTextFlags_EnterReturnsTrue;
	bool refocus = false;

	if (ImGui::InputText("##Input", inputBuffer, IM_ARRAYSIZE(inputBuffer), input_text_flags)) {
		OT_DEBUG(inputBuffer);
		inputBuffer[0] = 0;
		refocus = true;
	}

	ImGui::SetItemDefaultFocus();

	if (refocus) {
		ImGui::SetKeyboardFocusHere(-1);
	}

	ImGui::PopID();
	ImGui::End();
}


//
//	OtConsoleClass::help
//

void OtConsoleClass::help() {
}


//
//	OtConsoleClass::clear
//

void OtConsoleClass::clear() {
	lines.clear();
	lines.push_back(Line(Normal, ""));
}


//
//	OtConsoleClass::writeColored
//

void OtConsoleClass::writeColored(LineType type, const std::string& text) {
	std::string part;

	for (char const &c: text) {
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
//	OtConsoleClass::write
//

void OtConsoleClass::write(const std::string& text) {
	writeColored(Normal, text);
}


//
//	OtConsoleClass::writeHelp
//

void OtConsoleClass::writeHelp(const std::string& text) {
	writeColored(Help, text);
}


//
//	OtConsoleClass::writeInput
//

void OtConsoleClass::writeInput(const std::string& text) {
	writeColored(Input, std::string("<< ") + text);
}


//
//	OtConsoleClass::writeError
//

void OtConsoleClass::writeError(const std::string& text) {
	writeColored(Error, text);
}


//
//	OtConsoleClass::getMeta
//

OtType OtConsoleClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtTypeClass::create<OtConsoleClass>("Console", OtIdeClass::getMeta());
	}

	return type;
}


//
//	OtConsoleClass::create
//

OtConsole OtConsoleClass::create() {
	OtConsole console = std::make_shared<OtConsoleClass>();
	console->setType(getMeta());
	return console;
}
