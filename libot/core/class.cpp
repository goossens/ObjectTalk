//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "ot/exception.h"
#include "ot/function.h"
#include "ot/class.h"
#include "ot/vm.h"


//
//	OtClassClass::instantiate
//

OtObject OtClassClass::instantiate(size_t count, OtObject* parameters) {
	// create new instance
	OtObject object = classType->construct();
	object->setType(classType);

	// run possible init function
	if (object->has("__init__")) {
		OtVM::redirectMemberFunction(object, "__init__", count);

	} else if (count) {
		OtExcept(
			"Class [%s] is missing [__init__] member function (called with %ld parameters)",
			classType->getName().c_str(),
			count);
	}

	return object;
}


//
//	OtClassClass::getMeta
//

OtType OtClassClass::getMeta() {
	static OtType type = nullptr;

	if (!type) {
		type = OtTypeClass::create<OtClassClass>("Class", OtInternalClass::getMeta());
		type->set("__call__", OtFunctionClass::create(&OtClassClass::instantiate));
		type->set("getName", OtFunctionClass::create(&OtClassClass::getName));
		type->set("hasParent", OtFunctionClass::create(&OtClassClass::hasParent));
		type->set("getParent", OtFunctionClass::create(&OtClassClass::getParent));
		type->set("subClass", OtFunctionClass::create(&OtClassClass::subClass));
		type->set("isKindOf", OtFunctionClass::create(&OtClassClass::isKindOf));
	}

	return type;
}


//
//	OtClassClass::create
//

OtClass OtClassClass::create(OtType type) {
	OtClass cls = std::make_shared<OtClassClass>(type);
	cls->setType(getMeta());
	return cls;
}
