//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "ot/function.h"
#include "ot/vm.h"

#include "application.h"
#include "menuitem.h"


//
//	OtMenuItemClass::init
//

OtObject OtMenuItemClass::init(size_t count, OtObject* parameters) {
	// set attributes
	if (count) {
		switch (count) {
			case 3:
				setCallback(parameters[2]);

			case 2:
				setShortcut(parameters[1]->operator std::string());

			case 1:
				setTitle(parameters[0]->operator std::string());
				break;

			default:
				OtExcept("Too many parameters [%ld] for [MenuItem] constructor (max 3)", count);
		}
	}

	return nullptr;
}


//
//	OtMenuItemClass::setShortcut
//

OtObject OtMenuItemClass::setShortcut(const std::string& s) {
	// sanity check
	if (s.size() == 0) {
		shortcut.clear();

	} else if (s.size() == 1) {
		// determine shortcut keycodes
		char ch = s[0];
		int keycode;

		if (ch >= '0' && ch <= '9') {
			keycode = GLFW_KEY_0 + ch - '0';

		} else if (ch >= 'a' && ch <= 'z') {
			keycode = GLFW_KEY_A + ch - 'a';

		} else if (ch >= 'A' && ch <= 'Z') {
			keycode = GLFW_KEY_A + ch - 'A';

		} else {
			OtExcept("Invalid shortcut for MenuItem [%s]", s.c_str());
		}

#if __APPLE__
		int modifier = GLFW_MOD_SUPER;
		shortcut = "Cmd-" + s;
#else
		int modifier = GLFW_MOD_CONTROL;
		shortcut = "Ctrl-" + s;
#endif

		// register shortcut
		OtApplicationClass::instance()->addShortcut(modifier, keycode, [this] (){
			if (this->callback) {
				OtVM::instance()->callMemberFunction(this->callback, "__call__");
			}
		});

	} else {
		OtExcept("MenuItem shortcut should be one character, not [%s]", s.c_str());
	}

	return shared();
}


//
//	OtMenuItemClass::render
//

void OtMenuItemClass::render() {
	if (ImGui::MenuItem(title.c_str(), shortcut.c_str(), selected, enabled)) {
		if (callback) {
			OtVM::instance()->callMemberFunction(callback, "__call__");
		}
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

		type->set("getCallback", OtFunctionClass::create(&OtMenuItemClass::getCallback));
		type->set("setCallback", OtFunctionClass::create(&OtMenuItemClass::setCallback));

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
