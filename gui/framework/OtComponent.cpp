//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <algorithm>

#include "OtFunction.h"

#include "OtComponent.h"


//
//	OtComponentClass::add
//

OtObject OtComponentClass::add(OtObject object) {
	// ensure object is of the right class
	if (!object->isKindOf("Component")) {
		OtExcept("Can't have a [%s] as a child of a [%s]",
			object->getType()->getName().c_str(),
			getType()->getName().c_str());
	}

	// cast the object to the child type
	OtComponent child = object->cast<OtComponentClass>();

	// ensure new child is a valid type
	validateChild(child);

	// remove from previous parent if required
	if (child->parent.lock()) {
		child->parent.lock()->remove(object);
	}

	// set new parent
	child->parent = cast<OtComponentClass>();

	// add new child
	children.push_back(child);
	return shared();
}


//
//	OtComponentClass::remove
//

OtObject OtComponentClass::remove(OtObject object) {
	// ensure object is of the right class
	if (!object->isKindOf("Component")) {
		OtExcept("Can't have a [%s] as a child of a [Component]", object->getType()->getName().c_str());
	}

	// delete child from list
	bool found = false;

	// use remove_if when we move the C++20
	for (auto it = children.begin(); !found && it != children.end();) {
		if ((*it)->equal(object)) {
			object->cast<OtComponentClass>()->parent.reset();
			it = children.erase(it);
			found = true;

		} else {
			it++;
		}
	}

	return shared();
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
	static OtType type;

	if (!type) {
		type = OtTypeClass::create<OtComponentClass>("Component", OtGuiClass::getMeta());

		type->set("add", OtFunctionClass::create(&OtComponentClass::add));
		type->set("remove", OtFunctionClass::create(&OtComponentClass::remove));

		type->set("clear", OtFunctionClass::create(&OtComponentClass::clear));
		type->set("size", OtFunctionClass::create(&OtComponentClass::size));

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
