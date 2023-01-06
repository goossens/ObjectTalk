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
//	OtConsoleClass::OtConsoleClass
//

OtConsoleClass::OtConsoleClass() {
	clear();
}


//
//	OtConsoleClass::update
//

void OtConsoleClass::update() {
	// process console events
	while (events.size()) {
		Event event = events.pop();

		switch (event.type) {
			case Event::stdoutEvent:
				write(event.text);
				break;

			case Event::stderrEvent:
				writeError(event.text);
				break;
		}
	}
}


//
//	OtConsoleClass::render
//

void OtConsoleClass::render() {
	// create the window
	ImGui::BeginChild("Console", ImVec2(0.0, 0.0), true, ImGuiWindowFlags_HorizontalScrollbar);

	ImGuiListClipper clipper;
	clipper.Begin((int) lines.size());

	while (clipper.Step()) {
		for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++) {
			if (lines[i].type) {
				if (lines[i].type == LineType::Error) {
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
//	OtConsoleClass::write
//

void OtConsoleClass::write(const std::string& text) {
	writeColored(Normal, text);
}


//
//	OtConsoleClass::writeError
//

void OtConsoleClass::writeError(const std::string& text) {
	writeColored(Error, text);
}


//
//	OtConsoleClass::create
//

OtConsole OtConsoleClass::create() {
	return OtConsoleClass::instance();
}
