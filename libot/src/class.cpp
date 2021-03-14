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


//
//	OtClassClass::operator ()
//

OtObject OtClassClass::operator () (OtContext context, size_t count, OtObject* parameters) {
	// create new instance
	OtObject object = classType->construct();
	object->setType(classType);

	// run possible init function
	if (object->has("__init__")) {
		OtObject pars[count + 1];
		pars[0] = object;

		for (size_t c = 0; c < count; c++) {
			pars[c + 1] = parameters[c];
		}

		object->get("__init__")->operator ()(context, count + 1, pars);
	}

	return object;
}


//
//	OtClassClass::getMeta
//

OtType OtClassClass::getMeta() {
	static OtType type = nullptr;

	if (!type) {
		type = OtTypeClass::create<OtClassClass>("Class", OtContextClass::getMeta());
		type->set("__call__", OtFunctionClass::create(&OtClassClass::operator ()));
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
