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

#include "OtUi.h"

#include "OtPanel.h"


//
//	OtPanelClass::render
//

void OtPanelClass::render() {
	// render all children
	ImGui::PushID(this);
	ImGui::BeginChild("panel");
	ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.6f);

	for (auto& child : children) {
		if (child->isEnabled()) {
			OtWidget(child)->render();
		}
	}

	ImGui::PopItemWidth();
	ImGui::EndChild();
	ImGui::PopID();
}


//
//	OtPanelClass::getMeta
//

OtType OtPanelClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtType::create<OtPanelClass>("Panel", OtWidgetClass::getMeta());
//		type->set("setSplit", OtFunction::create(&OtPanelClass::setSplit));
	}

	return type;
}