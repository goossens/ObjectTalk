//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "imgui.h"

#include "OtUi.h"

#include "OtPanel.h"


//
//	OtPanelClass::render
//

void OtPanelClass::render() {
	// render all children
	ImGui::PushID(this);
	renderChildren();
	ImGui::PopID();
}


//
//	OtPanelClass::getMeta
//

OtType OtPanelClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtType::create<OtPanelClass>("Panel", OtWidgetClass::getMeta());
	}

	return type;
}
