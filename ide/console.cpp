//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "ot/numbers.h"
#include "ot/text.h"

#include "application.h"

#include "console.h"


//
//	OtConsoleClass::OtConsoleClass
//

OtConsoleClass::OtConsoleClass() {
	writeHelp(
		"Welcome to the ObjectTalk Integrated Development Environment (IDE)\n"
		"Right click here or type 'help' below for options"
	);

	writeInput("i did it");
	writeError("that was a mistake");
}


//
//	OtConsoleClass::render
//

void OtConsoleClass::render() {
	// calculate size and position for editor window
	OtApplication application = OtApplicationClass::instance();
	int width = application->getWidth();
	int height = application->getHeight();
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
	ImGui::BeginChild("ScrollingRegion", ImVec2(0, -footerHeight), false, ImGuiWindowFlags_HorizontalScrollbar);

	if (ImGui::BeginPopupContextWindow()) {
		if (ImGui::MenuItem("Help")) { help(); }
		if (ImGui::MenuItem("Clear")) { clear(); }
		ImGui::MenuItem("Auto scroll", "", &autoscroll);
		ImGui::EndPopup();
	}

	ImGuiListClipper clipper;
	clipper.Begin(lines.size());

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

	if (autoscroll) {
		ImGui::SetScrollHereY(1.0f);
	}

	ImGui::EndChild();
	ImGui::Separator();

	// command line input
	ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
	ImGuiInputTextFlags input_text_flags = ImGuiInputTextFlags_EnterReturnsTrue;
	bool refocus = false;
	char InputBuf[256];

	if (ImGui::InputText("##Input", InputBuf, IM_ARRAYSIZE(InputBuf), input_text_flags)) {
		OT_DEBUG(InputBuf);
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
}


//
//	OtConsoleClass::write
//

void OtConsoleClass::write(const std::string& text) {

}


//
//	OtConsoleClass::writeColored
//

void OtConsoleClass::writeColored(LineType type, const std::string& text) {
	std::vector<std::string> parts;
	OtText::split(text, parts,'\n');

	for (auto& part : parts) {
		lines.push_back(Line(type, part));
	}
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
	static OtType type = nullptr;

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
