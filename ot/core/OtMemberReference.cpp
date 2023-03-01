//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtBoundFunction.h"
#include "OtFunction.h"
#include "OtMemberReference.h"


//
//	OtMemberReferenceClass::deref
//

OtObject OtMemberReferenceClass::deref() {
	// get the member
	OtObject result = object->get(member);

	// never create bound functions for Module or Class members
	if (object->isKindOf("Module")) { // || object->isKindOf("Class")) {
		return result;

	// create bound function if required
	} else if (result && (result->isKindOf("Function") || result->isKindOf("ByteCodeFunction"))) {
		return OtBoundFunction::create(object, result);

	// it's just a member variable
	} else {
		return result;
	}
}


//
//	OtMemberReferenceClass::assign
//

OtObject OtMemberReferenceClass::assign(OtObject value) {
	return object->set(member, value);
}


//
//	OtMemberReferenceClass::getMeta
//

OtType OtMemberReferenceClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtTypeClass::create<OtMemberReferenceClass>("MemberReference", OtReferenceClass::getMeta());
		type->set("__deref__", OtFunction::create(&OtMemberReferenceClass::deref));
		type->set("__assign__", OtFunction::create(&OtMemberReferenceClass::assign));
	}

	return type;
}
