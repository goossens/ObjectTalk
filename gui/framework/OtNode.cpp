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
	OtNode child = object->cast<OtNodeClass>();

	// ensure new child is a valid type
	validateChild(child);

	// remove from previous parent if required
	if (child->hasParent()) {
		child->parent.lock()->remove(object);
	}

	// set new parent
	child->parent = cast<OtNodeClass>();

	// add new child
	children.push_back(child);
	return shared();
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
			object->cast<OtNodeClass>()->parent.reset();
			it = children.erase(it);
			found = true;

		} else {
			it++;
		}
	}

	return shared();
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
		type = OtTypeClass::create<OtNodeClass>("Node", OtGuiClass::getMeta());

		type->set("add", OtFunctionClass::create(&OtNodeClass::add));
		type->set("remove", OtFunctionClass::create(&OtNodeClass::remove));

		type->set("clear", OtFunctionClass::create(&OtNodeClass::clear));
		type->set("size", OtFunctionClass::create(&OtNodeClass::size));

		type->set("enable", OtFunctionClass::create(&OtNodeClass::enable));
		type->set("disable", OtFunctionClass::create(&OtNodeClass::disable));
		type->set("setEnabled", OtFunctionClass::create(&OtNodeClass::setEnabled));
		type->set("isEnabled", OtFunctionClass::create(&OtNodeClass::isEnabled));
	}

	return type;
}


//
//	OtNodeClass::create
//

OtNode OtNodeClass::create() {
	OtNode node = std::make_shared<OtNodeClass>();
	node->setType(getMeta());
	return node;
}
