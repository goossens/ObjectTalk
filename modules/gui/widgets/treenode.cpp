//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "ot/function.h"

#include "treenode.h"


//
//	OtTreeNodeClass::init
//

void OtTreeNodeClass::init(const std::string& l) {
	// save label
	label = l;
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
	int flags = ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_DefaultOpen;

	if (ImGui::TreeNodeEx(this, flags, "%s", label.c_str())) {
		for (auto& child : children) {
			child->render();
		}

		ImGui::TreePop();
	}
}


//
//	OtTreeNodeClass::getMeta
//

OtType OtTreeNodeClass::getMeta() {
	static OtType type = nullptr;

	if (!type) {
		type = OtTypeClass::create<OtTreeNodeClass>("TreeNode", OtWidgetClass::getMeta());
		type->set("__init__", OtFunctionClass::create(&OtTreeNodeClass::init));
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
