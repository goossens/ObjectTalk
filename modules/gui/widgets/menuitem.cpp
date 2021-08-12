//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "ot/function.h"

#include "menuitem.h"


//
//	OtMenuItemClass::render
//

void OtMenuItemClass::render() {
	if (ImGui::MenuItem(title.c_str(), shortcut.c_str(), selected, enabled)) {

	}
}


//
//	OtMenuItemClass::getMeta
//

OtType OtMenuItemClass::getMeta() {
	static OtType type = nullptr;

	if (!type) {
		type = OtTypeClass::create<OtMenuItemClass>("MenuItem", OtWidgetClass::getMeta());
		type->set("__init__", OtFunctionClass::create(&OtMenuItemClass::init));

		type->set("getTitle", OtFunctionClass::create(&OtMenuItemClass::getTitle));
		type->set("setTitle", OtFunctionClass::create(&OtMenuItemClass::setTitle));

		type->set("getShortcut", OtFunctionClass::create(&OtMenuItemClass::getShortcut));
		type->set("setShortcut", OtFunctionClass::create(&OtMenuItemClass::setShortcut));

		type->set("select", OtFunctionClass::create(&OtMenuItemClass::select));
		type->set("unselect", OtFunctionClass::create(&OtMenuItemClass::unselect));
		type->set("isSelected", OtFunctionClass::create(&OtMenuItemClass::isSelected));

		type->set("enable", OtFunctionClass::create(&OtMenuItemClass::enable));
		type->set("disable", OtFunctionClass::create(&OtMenuItemClass::disable));
		type->set("isEnabled", OtFunctionClass::create(&OtMenuItemClass::isEnabled));
	}

	return type;
}


//
//	OtMenuItemClass::create
//

OtMenuItem OtMenuItemClass::create() {
	OtMenuItem menuitem = std::make_shared<OtMenuItemClass>();
	menuitem->setType(getMeta());
	return menuitem;
}
