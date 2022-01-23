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
//	OtClassClass::OtClassClass
//

OtClassClass::OtClassClass(const std::string& name) {
	classType = OtTypeClass::create(name);
}


//
//	OtClassClass::setParent
//

void OtClassClass::setParent(OtObject object) {
	if (object->isKindOf("Class")) {
		classType->setParent(object->cast<OtClassClass>()->classType);

	} else {
		OtExcept("Expected a [Class] instance, not a [%s]", object->getType()->getName().c_str());
	}
}


//
//	OtClassClass::instantiate
//

OtObject OtClassClass::instantiate(size_t count, OtObject* parameters) {
	// create new instance
	OtObject object = classType->allocate();
	object->setType(classType);

	// run possible init function
	if (object->has("__init__")) {
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

bool OtClassClass::has(const std::string& name) {
	if (OtInternalClass::has(name)) {
		return true;

	} else {
		return classType->has(name);
	}
}


//
//	OtClassClass::get
//

OtObject OtClassClass::get(const std::string& name) {
	if (OtInternalClass::has(name)) {
		return OtInternalClass::get(name);

	} else {
		return classType->get(name);
	}
}


//
//	OtClassClass::getMeta
//

OtType OtClassClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtTypeClass::create<OtClassClass>("Class", OtInternalClass::getMeta());
		type->set("__call__", OtFunctionClass::create(&OtClassClass::instantiate));
		type->set("getName", OtFunctionClass::create(&OtClassClass::getName));
		type->set("setParent", OtFunctionClass::create(&OtClassClass::setParent));
		type->set("hasParent", OtFunctionClass::create(&OtClassClass::hasParent));
		type->set("getParent", OtFunctionClass::create(&OtClassClass::getParent));
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

OtClass OtClassClass::create(const std::string& name) {
	OtClass cls = std::make_shared<OtClassClass>(name);
	cls->setType(getMeta());
	return cls;
}
