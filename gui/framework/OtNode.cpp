//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <algorithm>

#include "OtFunction.h"

#include "OtNode.h"


//
//	OtNodeClass::add
//

OtObject OtNodeClass::add(OtObject object) {
	// ensure object is of the right class
	if (!object->isKindOf("Node")) {
		OtExcept("Can't have a [%s] as a child of a [%s]",
			object->getType()->getName().c_str(),
			getType()->getName().c_str());
	}

	// cast the object to a node
	OtNode child = OtNode(object);

	// ensure new child is a valid type
	validateChild(child);

	// remove from previous parent if required
	if (child->hasParent()) {
		child->parent->remove(object);
	}

	// set new parent
	child->parent = this;

	// add new child
	children.push_back(child);
	return OtObject(this);
}


//
//	OtNodeClass::remove
//

OtObject OtNodeClass::remove(OtObject object) {
	// ensure object is of the right class
	if (!object->isKindOf("Node")) {
		OtExcept("Can't have a [%s] as a child of a [Node]", object->getType()->getName().c_str());
	}

	// delete child from list
	bool found = false;

	for (auto it = children.begin(); !found && it != children.end();) {
		if ((*it)->equal(object)) {
			OtNode(object)->parent = nullptr;
			it = children.erase(it);
			found = true;

		} else {
			it++;
		}
	}

	return OtObject(this);
}


//
//	OtNodeClass::validateChild
//

void OtNodeClass::validateChild(OtNode child) {
	OtExcept("A [%s] can't have children", getType()->getName().c_str());
}


//
//	OtNodeClass::clear
//

void OtNodeClass::clear() {
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
//	OtNodeClass::getMeta
//

OtType OtNodeClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtType::create<OtNodeClass>("Node", OtGuiClass::getMeta());

		type->set("add", OtFunction::create(&OtNodeClass::add));
		type->set("remove", OtFunction::create(&OtNodeClass::remove));

		type->set("clear", OtFunction::create(&OtNodeClass::clear));
		type->set("size", OtFunction::create(&OtNodeClass::size));

		type->set("enable", OtFunction::create(&OtNodeClass::enable));
		type->set("disable", OtFunction::create(&OtNodeClass::disable));
		type->set("setEnabled", OtFunction::create(&OtNodeClass::setEnabled));
		type->set("isEnabled", OtFunction::create(&OtNodeClass::isEnabled));
	}

	return type;
}
