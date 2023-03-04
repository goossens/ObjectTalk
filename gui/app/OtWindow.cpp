//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "imgui.h"

#include "OtFunction.h"

#include "OtWindow.h"


//
//	OtWindowClass::validateChild
//

void OtWindowClass::validateChild(OtNode child) {
	if (!child->isKindOf("Widget")) {
		OtExcept("A [Window] can only have [Widget] subclasses as children, not [%s]", child->getType()->getName().c_str());
	}
}


//
//	OtWindowClass::render
//

void OtWindowClass::render() {
	// position window
	ImGui::SetNextWindowSize(ImVec2(width, height), ImGuiCond_FirstUseEver);

	if (ImGui::Begin(title.c_str(), nullptr, flags)) {
		// render all children
		OtAppObjectClass::render();
	}

	ImGui::End();
}


//
//	OtWindowClass::getMeta
//

OtType OtWindowClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtType::create<OtWindowClass>("Window", OtAppObjectClass::getMeta());
		type->set("__init__", OtFunction::create(&OtWindowClass::init));
		type->set("setTitle", OtFunction::create(&OtWindowClass::setTitle));
		type->set("getTitle", OtFunction::create(&OtWindowClass::getTitle));
		type->set("setSize", OtFunction::create(&OtWindowClass::setSize));
		type->set("getWidth", OtFunction::create(&OtWindowClass::getWidth));
		type->set("getHeight", OtFunction::create(&OtWindowClass::getHeight));
		type->set("setFlags", OtFunction::create(&OtWindowClass::setFlags));
		type->set("getFlags", OtFunction::create(&OtWindowClass::getFlags));
	}

	return type;
}
