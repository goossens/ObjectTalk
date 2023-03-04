//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtFunction.h"
#include "OtCaptureReference.h"
#include "OtClosure.h"
#include "OtVM.h"


//
//	OtCaptureReferenceClass::deref
//

OtObject OtCaptureReferenceClass::deref() {
	OtClosure closure = OtVM::instance()->getStack()->getClosure();
	return closure->get(member);
}


//
//	OtCaptureReferenceClass::assign
//

OtObject OtCaptureReferenceClass::assign(OtObject value) {
	OtClosure closure = OtVM::instance()->getStack()->getClosure();
	return closure->set(member, value);
}


//
//	OtCaptureReferenceClass::getMeta
//

OtType OtCaptureReferenceClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtType::create<OtCaptureReferenceClass>("CaptureReference", OtReferenceClass::getMeta());
		type->set("__deref__", OtFunction::create(&OtCaptureReferenceClass::deref));
		type->set("__assign__", OtFunction::create(&OtCaptureReferenceClass::assign));
	}

	return type;
}
