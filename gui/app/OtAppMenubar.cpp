//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "imgui.h"

#include "OtException.h"

#include "OtAppMenubar.h"


//
//	OtAppMenubarClass::validateChild
//

void OtAppMenubarClass::validateChild(OtNode child) {
	if (!child->isKindOf("Menu")) {
		OtExcept("An [AppMenubar] can only have [Menu]s as children, not [%s]", child->getType()->getName().c_str());
	}
}


//
//	OtAppMenubarClass::render
//

void OtAppMenubarClass::render() {
	if (ImGui::BeginMainMenuBar()) {
		height = ImGui::GetWindowSize().y;
		OtAppObjectClass::render();
		ImGui::EndMainMenuBar();
	}
}


//
//	OtAppMenubarClass::getMeta
//

OtType OtAppMenubarClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtTypeClass::create<OtAppMenubarClass>("AppMenubar", OtAppObjectClass::getMeta());
	}

	return type;
}
