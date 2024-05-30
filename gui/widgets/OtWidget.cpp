//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <algorithm>

#include "OtFunction.h"
#include "OtLog.h"

#include "OtWidget.h"


//
//	OtWidgetClass::add
//

OtObject OtWidgetClass::add(OtObject object) {
	// ensure we don't add too many children
	int maxChildren = getMaxChildren();

	if (maxChildren >= 0) {
		if (int(children.size()) == maxChildren) {
			OtLogFatal("Too many children for [{}], max [{}]", getType()->getName(), maxChildren);
		}
	}

	// ensure object is of the right class
	if (!object->isKindOf("Widget")) {
		OtLogFatal("Can't have a [{}] as a child of a [{}]", object->getType()->getName(), getType()->getName());
	}

	// cast the object to a widget
	OtWidget child = OtWidget(object);

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
//	OtWidgetClass::remove
//

OtObject OtWidgetClass::remove(OtObject object) {
	// delete child from list
	bool found = false;

	for (auto it = children.begin(); !found && it != children.end();) {
		if ((*it)->equal(object)) {
			OtWidget(object)->parent = nullptr;
			it = children.erase(it);
			found = true;

		} else {
			it++;
		}
	}

	return OtObject(this);
}


//
//	OtWidgetClass::clear
//

void OtWidgetClass::clear() {
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
//	OtWidgetClass::update
//

void OtWidgetClass::update() {
	// update all children
	for (auto& child : children) {
		if (child->isEnabled()) {
			OtWidget(child)->update();
		}
	}
}


//
//	OtWidgetClass::render
//

void OtWidgetClass::render() {
	// render all children
	for (auto& child : children) {
		if (child->isEnabled()) {
			OtWidget(child)->render();
		}
	}
}


//
//	OtWidgetClass::getMeta
//

OtType OtWidgetClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtType::create<OtWidgetClass>("Widget", OtGuiClass::getMeta());

		type->set("add", OtFunction::create(&OtWidgetClass::add));
		type->set("remove", OtFunction::create(&OtWidgetClass::remove));

		type->set("clear", OtFunction::create(&OtWidgetClass::clear));
		type->set("size", OtFunction::create(&OtWidgetClass::size));

		type->set("enable", OtFunction::create(&OtWidgetClass::enable));
		type->set("disable", OtFunction::create(&OtWidgetClass::disable));
		type->set("setEnabled", OtFunction::create(&OtWidgetClass::setEnabled));
		type->set("isEnabled", OtFunction::create(&OtWidgetClass::isEnabled));
	}

	return type;
}
