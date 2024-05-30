//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "imgui.h"

#include "OtFunction.h"
#include "OtLog.h"

#include "OtUi.h"

#include "OtHsplit.h"


//
//	OtHsplitClass::init
//

void OtHsplitClass::init(size_t count, OtObject* parameters) {
	switch (count) {
		case 3:
			setSplitLimits(
				parameters[1]->operator float(),
				parameters[2]->operator float());

		case 1:
			setSplit(parameters[0]->operator float());

		case 0:
			break;

		default:
			OtLogFatal("[Hsplit] constructor expects 0, 1 or 3 arguments (not {})", count);
	}
}


//
//	OtHsplitClass::render
//

void OtHsplitClass::render() {
	// get available space in window
	auto available = ImGui::GetContentRegionAvail();

	// determine widths
	float leftWidth = available.x * split;
	float minWidth = available.x * minSplit;
	float maxWidth = available.x * maxSplit;

	// render left widget
	ImGui::PushID(this);
	ImGui::BeginChild("left", ImVec2(leftWidth, 0.0f), ImGuiChildFlags_Border);

	if (children.size() > 0) {
		children[0]->render();
	}

	ImGui::EndChild();

	// render splitter
	OtUiSplitterHorizontal(&leftWidth, minWidth, maxWidth);
	split = leftWidth / available.x;

	// render right widget
	ImGui::BeginChild("right", ImVec2(0.0f, 0.0f), ImGuiChildFlags_Border);

	if (children.size() > 1) {
		children[1]->render();
	}

	ImGui::EndChild();
	ImGui::PopID();
}


//
//	OtHsplitClass::getMeta
//

OtType OtHsplitClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtType::create<OtHsplitClass>("Hsplit", OtWidgetClass::getMeta());
		type->set("__init__", OtFunction::create(&OtHsplitClass::init));
		type->set("setSplit", OtFunction::create(&OtHsplitClass::setSplit));
		type->set("setSplitLimits", OtFunction::create(&OtHsplitClass::setSplitLimits));
	}

	return type;
}
