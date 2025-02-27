//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
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
	return OtVM::getStack()->getFrameItem(slot);
}


//
//	OtStackReferenceClass::assign
//

OtObject OtStackReferenceClass::assign(OtObject value) {
	OtVM::getStack()->setFrameItem(slot, value);
	return value;
}


//
//	OtStackReferenceClass::getMeta
//

OtType OtStackReferenceClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtType::create<OtStackReferenceClass>("StackReference", OtReferenceClass::getMeta());
		type->set("__deref__", OtFunction::create(&OtStackReferenceClass::deref));
		type->set("__assign__", OtFunction::create(&OtStackReferenceClass::assign));
	}

	return type;
}
