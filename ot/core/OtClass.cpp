//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
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
		OtExcept(
			"Class [%s] is missing [__init__] member function (called with %ld parameters)",
			classType->getName().c_str(),
			count);
	}

	return object;
}


//
//	OtClassClass::has
//

bool OtClassClass::has(size_t selector) {
	if (OtInternalClass::has(selector)) {
		return true;

	} else {
		return classType->has(selector);
	}
}


//
//	OtClassClass::get
//

OtObject& OtClassClass::get(size_t selector) {
	if (OtInternalClass::has(selector)) {
		return OtInternalClass::get(selector);

	} else {
		return classType->get(selector);
	}
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
