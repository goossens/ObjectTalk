//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtClass.h"
#include "OtException.h"
#include "OtFunction.h"
#include "OtVM.h"


//
//	OtClassClass::OtClassClass
//

OtClassClass::OtClassClass(const std::string& name) {
	classType = OtType::create(name);
}


//
//	OtClassClass::setParent
//

void OtClassClass::setParent(OtObject object) {
	object->expectKindOf("Class");
	classType->setParent(OtClass(object)->classType);
}


//
//	OtClassClass::instantiate
//

OtObject OtClassClass::instantiate(size_t count, OtObject* parameters) {
	// create new instance
	OtObject object = classType->allocate();
	object->setType(classType);

	// run possible init function
	if (object->hasByName("__init__")) {
		OtVM::instance()->redirectMemberFunction(object, "__init__", count);

	} else if (count) {
		OtError(
			"Class [{}] is missing [__init__] member function (called with {} parameters)",
			classType->getName().c_str(),
			count);
	}

	return object;
}


//
//	OtClassClass::has
//

bool OtClassClass::has(size_t symbol) {
	if (OtInternalClass::has(symbol)) {
		return true;

	} else {
		return classType->has(symbol);
	}
}


//
//	OtClassClass::get
//

OtObject& OtClassClass::get(size_t symbol) {
	if (OtInternalClass::has(symbol)) {
		return OtInternalClass::get(symbol);

	} else {
		return classType->get(symbol);
	}
}


//
//	OtClassClass::getSuper
//

 OtObject OtClassClass::getSuper(size_t symbol) {
	for (auto t = classType ? classType->getParent() : classType; t; t = t->getParent()) {
		if (t->has(symbol)) {
			return t->get(symbol);
		}
	}

	auto name = OtSymbolizer::name(symbol);
	OtError("Unknown member [{}] in superclass of [{}]", name, type->getName());

	// we will never get here because of the exception but a return statement keeps the compiler happy
	return members->get(symbol);
 }


//
//	OtClassClass::getMeta
//

OtType OtClassClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtType::create<OtClassClass>("Class", OtInternalClass::getMeta());
		type->set("__call__", OtFunction::create(&OtClassClass::instantiate));
		type->set("getName", OtFunction::create(&OtClassClass::getName));
		type->set("setParent", OtFunction::create(&OtClassClass::setParent));
		type->set("hasParent", OtFunction::create(&OtClassClass::hasParent));
		type->set("getParent", OtFunction::create(&OtClassClass::getParent));
		type->set("isKindOf", OtFunction::create(&OtClassClass::isKindOf));
	}

	return type;
}
