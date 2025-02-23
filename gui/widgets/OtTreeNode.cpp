//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
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

#include "OtTreeNode.h"


//
//	OtTreeNodeClass::init
//

void OtTreeNodeClass::init(size_t count, OtObject* parameters) {
	switch (count) {
		case 2:
			setOpen(parameters[1]->operator bool());
			[[fallthrough]];

		case 1:
			setLabel(parameters[0]->operator std::string());
			[[fallthrough]];

		case 0:
			break;

		default:
			OtLogFatal("[TextField] constructor expects up to 2 arguments (not {})", count);
	}
}


//
//	OtTreeNodeClass::render
//

void OtTreeNodeClass::render() {
	ImGui::PushID(this);
	int flags =
		ImGuiTreeNodeFlags_Framed |
		ImGuiTreeNodeFlags_FramePadding;

	if (openFlag) {
		flags |= ImGuiTreeNodeFlags_DefaultOpen;
	}

	if (ImGui::TreeNodeEx("treenode", flags, "%s", label.c_str())) {
		renderChildren();
		ImGui::TreePop();
		openFlag = true;

	} else {
		openFlag = false;
	}

	ImGui::PopID();
}


//
//	OtTreeNodeClass::getMeta
//

OtType OtTreeNodeClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtType::create<OtTreeNodeClass>("TreeNode", OtWidgetClass::getMeta());
		type->set("__init__", OtFunction::create(&OtTreeNodeClass::init));
		type->set("setLabel", OtFunction::create(&OtTreeNodeClass::setLabel));
		type->set("setOpen", OtFunction::create(&OtTreeNodeClass::setOpen));
		type->set("open", OtFunction::create(&OtTreeNodeClass::open));
		type->set("close", OtFunction::create(&OtTreeNodeClass::close));
		type->set("isOpen", OtFunction::create(&OtTreeNodeClass::isOpen));
	}

	return type;
}
