//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "ot/function.h"

#include "theme.h"
#include "screen.h"
#include "panel.h"


//
//	OtPanelClass::validateChild
//

void OtPanelClass::validateChild(OtComponent child) {
	if (!child->isKindOf("Widget")) {
		OtExcept("A [Panel] can only have [Widget]s as children, not [%s]", child->getType()->getName().c_str());
	}
}


//
//	OtPanelClass::render
//

void OtPanelClass::render() {
	size_t offset = 0;

	// take menubar into account if we have one
	if (parent) {
		offset = parent->cast<OtScreenClass>()->getMenubarHeight();
	}

	// screen height
	float sw = OtTheme::width;
	float sh = OtTheme::height - offset;

	// determine dimensions
	float vx = x < 0 ? sw + x * sw / 100.0 : x * sw / 100.0;
	float vy = offset + (y < 0 ? sh + y * sh / 100.0 : y * sh / 100.0);
	float vw = w * sw / 100.0;
	float vh = h * sh / 100.0;

	// position panel
	ImGui::SetNextWindowPos(ImVec2(vx, vy), ImGuiCond_Always);
	ImGui::SetNextWindowSize(ImVec2(vw, vh), ImGuiCond_Always);

	// determine flags
	int flags = ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoCollapse;

	if (ImGui::Begin("panel", nullptr, flags)) {
		// render all children
		for (auto const& child : children) {
			child->render();
		}
	}

	ImGui::End();
}


//
//	OtPanelClass::getMeta
//

OtType OtPanelClass::getMeta() {
	static OtType type = nullptr;

	if (!type) {
		type = OtTypeClass::create<OtPanelClass>("Panel", OtWidgetClass::getMeta());
		type->set("__init__", OtFunctionClass::create(&OtPanelClass::init));
	}

	return type;
}


//
//	OtPanelClass::create
//

OtPanel OtPanelClass::create() {
	OtPanel panel = std::make_shared<OtPanelClass>();
	panel->setType(getMeta());
	return panel;
}
