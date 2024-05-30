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

#include "OtVsplit.h"


//
//	OtVsplitClass::init
//

void OtVsplitClass::init(size_t count, OtObject* parameters) {
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
			OtLogFatal("[Vsplit] constructor expects 0, 1 or 3 arguments (not {})", count);
	}
}


//
//	OtVsplitClass::render
//

void OtVsplitClass::render() {
	// get available space in window
	auto available = ImGui::GetContentRegionAvail();

	// determine widths
	float topHeight = available.y * split;
	float minHeight = available.y * minSplit;
	float maxHeight = available.y * maxSplit;

	// render top widget
	ImGui::PushID(this);
	ImGui::BeginChild("top", ImVec2(0.0f, topHeight), ImGuiChildFlags_Border);

	if (children.size() > 0) {
		children[0]->render();
	}

	ImGui::EndChild();

	// render splitter
	OtUiSplitterVertical(&topHeight, minHeight, maxHeight);
	split = topHeight / available.y;

	// render right widget
	ImGui::BeginChild("bottom", ImVec2(0.0f, 0.0f), ImGuiChildFlags_Border);

	if (children.size() > 1) {
		children[1]->render();
	}

	ImGui::EndChild();
	ImGui::PopID();
}


//
//	OtVsplitClass::getMeta
//

OtType OtVsplitClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtType::create<OtVsplitClass>("Vsplit", OtWidgetClass::getMeta());
		type->set("__init__", OtFunction::create(&OtVsplitClass::init));
		type->set("setSplit", OtFunction::create(&OtVsplitClass::setSplit));
		type->set("setSplitLimits", OtFunction::create(&OtVsplitClass::setSplitLimits));
	}

	return type;
}
