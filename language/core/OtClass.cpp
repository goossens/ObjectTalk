//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtClass.h"
#include "OtFunction.h"
#include "OtLog.h"
#include "OtVM.h"


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
	OtID initID = OtIdentifier::create("__init__");

	if (object->has(initID)) {
		OtVM::callMemberFunction(object, initID, count, parameters);

	} else if (count) {
		OtLogError(
			"Class [{}] is missing [__init__] member function (called with {} parameters)",
			classType->getName().c_str(),
			count);
	}

	return object;
}


//
//	OtClassClass::getSuper
//

 OtObject OtClassClass::getSuper(OtID id) {
	for (auto t = classType ? classType->getParent() : classType; t; t = t->getParent()) {
		if (t->has(id)) {
			return t->get(id);
		}
	}

	auto name = OtIdentifier::name(id);
	OtLogError("Unknown member [{}] in superclass of [{}]", name, type->getName());

	// we will never get here because of the exception but a return statement keeps the compiler happy
	return members->get(id);
 }


//
//	OtClassClass::getMeta
//

OtType OtClassClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtType::create<OtClassClass>("Class", OtInternalClass::getMeta());
		type->set("__call__", OtFunction::create(&OtClassClass::instantiate));
		type->set("__parent__", OtFunction::create(&OtClassClass::setParent));
		type->set("getName", OtFunction::create(&OtClassClass::getName));
		type->set("hasParent", OtFunction::create(&OtClassClass::hasParent));
		type->set("getParent", OtFunction::create(&OtClassClass::getParent));
		type->set("isKindOf", OtFunction::create(&OtClassClass::isKindOf));
	}

	return type;
}
