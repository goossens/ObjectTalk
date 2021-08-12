//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "ot/function.h"

#include "composite.h"


//
//	OtCompositeClass::add
//

OtObject OtCompositeClass::add(OtObject child) {
	// ensure object is a component
	if (child->isKindOf("Component")) {
		// ensure component is allowed as child
		auto component = child->cast<OtComponentClass>();
		validateChild(component);

		// add new child
		children.push_back(component);
		component->setParent(cast<OtComponentClass>());

	} else {
		OtExcept("Only an object derivered from [Component] can be added as a child, not a [%s]", child->getType()->getName().c_str());
	}

	return child;
}


//
//	OtCompositeClass::clear
//

void OtCompositeClass::clear() {
	// clear all children
	for (auto& child : children) {
		child->clear();
	}

	// let parent class do its thing
	OtComponentClass::clear();
}


//
//	OtCompositeClass::update
//

void OtCompositeClass::update() {
	// update all children
	for (auto const& child : children) {
		child->update();
	}
}


//
//	OtCompositeClass::render
//

void OtCompositeClass::render() {
	// render all children
	for (auto const& child : children) {
		if (child->isEnabled()) {
			child->render();
		}
	}
}


//
//	OtCompositeClass::getMeta
//

OtType OtCompositeClass::getMeta() {
	static OtType type = nullptr;

	if (!type) {
		type = OtTypeClass::create<OtCompositeClass>("Composite", OtComponentClass::getMeta());
		type->set("add", OtFunctionClass::create(&OtCompositeClass::add));
	}

	return type;
}


//
//	OtCompositeClass::create
//

OtComposite OtCompositeClass::create() {
	OtComposite composite = std::make_shared<OtCompositeClass>();
	composite->setType(getMeta());
	return composite;
}
