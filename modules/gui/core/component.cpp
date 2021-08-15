//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <algorithm>

#include "ot/function.h"

#include "component.h"


//
//	OtComponentClass::add
//

OtComponent OtComponentClass::add(OtObject object) {
	// ensure object is of the right class
	if (!object->isKindOf("Component")) {
		OtExcept("Can't have a [%s] as a child of a [Component]", object->getType()->getName().c_str());
	}

	// cast the object to the child type
	OtComponent child = object->cast<OtComponentClass>();

	// ensure new child is a valid type
	validateChild(child);

	// remove from previous parent if required
	if (child->parent) {
		child->parent->remove(object);
	}

	// set new parent
	child->parent = cast<OtComponentClass>();

	// add new child
	children.push_back(child);

	// return new child as the correct type
	return child;
}


//
//	OtComponentClass::remove
//

void OtComponentClass::remove(OtObject object) {
	// ensure object is of the right class
	if (!object->isKindOf("Component")) {
		OtExcept("Can't have a [%s] as a child of a [Component]", object->getType()->getName().c_str());
	}

	// cast the object to the child type
	OtComponent child = object->cast<OtComponentClass>();

	// detach from this object
	child->parent = nullptr;

	// delete child from list
	bool found = false;

	for (auto it = children.begin(); !found && it != children.end(); it++) {
		if ((*it)->equal(object)) {
			children.erase(it);
			found = true;
		}
	}
}


//
//	OtComponentClass::validateChild
//

void OtComponentClass::validateChild(OtComponent child) {
	OtExcept("A [%s] can't have children", getType()->getName().c_str());
}


//
//	OtComponentClass::clear
//

void OtComponentClass::clear() {
	// empty children first
	for (auto& child : children) {
		child->clear();
	}

	// remove all children
	while (children.size()) {
		remove(children[0]);
	}
}


//
//	OtComponentClass::getMeta
//

OtType OtComponentClass::getMeta() {
	static OtType type = nullptr;

	if (!type) {
		type = OtTypeClass::create<OtComponentClass>("Component", OtGuiClass::getMeta());

		type->set("add", OtFunctionClass::create(&OtComponentClass::add));
		type->set("remove", OtFunctionClass::create(&OtComponentClass::remove));

		type->set("enable", OtFunctionClass::create(&OtComponentClass::enable));
		type->set("disable", OtFunctionClass::create(&OtComponentClass::disable));
		type->set("setEnabled", OtFunctionClass::create(&OtComponentClass::setEnabled));
		type->set("isEnabled", OtFunctionClass::create(&OtComponentClass::isEnabled));
	}

	return type;
}


//
//	OtComponentClass::create
//

OtComponent OtComponentClass::create() {
	OtComponent component = std::make_shared<OtComponentClass>();
	component->setType(getMeta());
	return component;
}
