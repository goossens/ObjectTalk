//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "ot/function.h"
#include "ot/class.h"
#include "ot/vm.h"


//
//	OtClassClass::operator()
//

OtObject OtClassClass::operator()(size_t count, OtObject* parameters) {
	// create new instance
	OtObject object = classType->construct();
	object->setType(classType);

	// run possible init function
	if (object->has("__init__")) {
		// open a new stack frame
		OtVM::stack->openFrame(count + 1);

		// get a new stack pointer
		auto sp = OtVM::stack->sp(count + 1);

		// ugly patch
		*sp = object;

		// execute __init__ with one more parameter (this)
		object->get("__init__")->operator()(count + 1, sp);

		// close the stack frame
		OtVM::stack->closeFrame();
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
		type->set("__call__", OtFunctionClass::create(&OtClassClass::operator()));
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
