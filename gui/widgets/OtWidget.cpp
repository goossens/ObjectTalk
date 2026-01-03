//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <algorithm>

#include "OtFunction.h"
#include "OtLog.h"
#include "OtVM.h"

#include "OtWidget.h"


//
//	OtWidgetClass::add
//

OtObject OtWidgetClass::add(OtObject object) {
	// ensure we don't add too many children
	int maxChildren = getMaxChildren();

	if (maxChildren >= 0) {
		if (int(children.size()) == maxChildren) {
			OtLogError("Too many children for [{}], max [{}]", getTypeName(), maxChildren);
		}
	}

	// ensure object is of the right class
	if (!object.isKindOf<OtWidgetClass>()) {
		OtLogError("Can't have a [{}] as a child of a [{}]", object.getTypeName(), getTypeName());
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
	return OtWidget(this);
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

	return OtWidget(this);
}


//
//	OtWidgetClass::clear
//

void OtWidgetClass::clear() {
	// empty children first
	for (auto& child : children) {
		child->clear();
		child->parent = nullptr;
	}

	// remove all children
	children.clear();
	unsetAll();
}


//
//	OtWidgetClass::renderChildren
//

void OtWidgetClass::renderChildren() {
	// traverse all children
	for (auto& child : children) {
		// ensure child is enabled
		if (child->isEnabled()) {
			renderChild(child);
		}
	}
}


//
//	OtWidgetClass::renderChild
//

void OtWidgetClass::renderChild(OtWidget child) {
	// call subclass member function (if we have one)
	if (child->hasByName("update")) {
		OtObject object{child};
		OtVM::callMemberFunction(object, "update");
	}

	// render child
	child->render();
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
