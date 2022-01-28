//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtFunction.h"
#include "OtStack.h"
#include "OtStackReference.h"
#include "OtVM.h"


//
//	OtStackReferenceClass::deref
//

OtObject OtStackReferenceClass::deref() {
	return OtVM::instance()->getStack()->getFrameItem(OtStackItem(0, slot));
}


//
//	OtStackReferenceClass::assign
//

OtObject OtStackReferenceClass::assign(OtObject value) {
	OtVM::instance()->getStack()->setFrameItem(OtStackItem(0, slot), value);
	return value;
}


//
//	OtStackReferenceClass::getMeta
//

OtType OtStackReferenceClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtTypeClass::create<OtStackReferenceClass>("StackReference", OtReferenceClass::getMeta());
		type->set("__deref__", OtFunctionClass::create(&OtStackReferenceClass::deref));
		type->set("__assign__", OtFunctionClass::create(&OtStackReferenceClass::assign));
	}

	return type;
}


//
//	OtStackReferenceClass::create
//

OtStackReference OtStackReferenceClass::create(const std::string& name, size_t slot) {
	OtStackReference reference = std::make_shared<OtStackReferenceClass>(name, slot);
	reference->setType(getMeta());
	return reference;
}