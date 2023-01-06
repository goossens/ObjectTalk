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

#include "OtTreeNode.h"


//
//	OtTreeNodeClass::init
//

void OtTreeNodeClass::init(size_t count, OtObject* parameters) {
	// set attributes
	if (count) {
		switch (count) {
			case 2:
				setOpen(parameters[1]->operator bool());

			case 1:
				setTitle(parameters[0]->operator std::string());
				break;

			default:
			OtExcept("A [TreeNode] constructor can only have 1 or 2 parameters, not [%ld]", count);
		}
	}
}


//
//	OtTreeNodeClass::validateChild
//

void OtTreeNodeClass::validateChild(OtComponent child) {
	if (!child->isKindOf("Widget")) {
		OtExcept("A [TreeNode] can only have [Widget]s as children, not [%s]", child->getType()->getName().c_str());
	}
}


//
//	OtTreeNodeClass::render
//

void OtTreeNodeClass::render() {
	int flags = ImGuiTreeNodeFlags_Framed;

	if (open) {
		flags |= ImGuiTreeNodeFlags_DefaultOpen;
	}

	if (ImGui::TreeNodeEx(this, flags, "%s", title.c_str())) {
		OtWidgetClass::render();
		ImGui::TreePop();
		open = true;

	} else {
		open = false;
	}
}


//
//	OtTreeNodeClass::getMeta
//

OtType OtTreeNodeClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtTypeClass::create<OtTreeNodeClass>("TreeNode", OtWidgetClass::getMeta());
		type->set("__init__", OtFunctionClass::create(&OtTreeNodeClass::init));

		type->set("getTitle", OtFunctionClass::create(&OtTreeNodeClass::getTitle));
		type->set("setTitle", OtFunctionClass::create(&OtTreeNodeClass::setTitle));

		type->set("getOpen", OtFunctionClass::create(&OtTreeNodeClass::getOpen));
		type->set("setOpen", OtFunctionClass::create(&OtTreeNodeClass::setOpen));
	}

	return type;
}


//
//	OtTreeNodeClass::create
//

OtTreeNode OtTreeNodeClass::create() {
	OtTreeNode treenode = std::make_shared<OtTreeNodeClass>();
	treenode->setType(getMeta());
	return treenode;
}
