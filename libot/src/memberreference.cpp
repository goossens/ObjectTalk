//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "ot/function.h"
#include "ot/boundfunction.h"
#include "ot/memberreference.h"


//
//	OtMemberReferenceClass::deref
//

OtObject OtMemberReferenceClass::deref() {
	OtObject result = object->get(member);

	// never create bound functions for Model or Class members
	if (object->isKindOf("Module")) { // || object->isKindOf("Class")) {
		return result;

	// create bound function if required
	} else if (result && (result->isKindOf("Function") || result->isKindOf("CodeFunction"))) {
		return OtBoundFunctionClass::create(object, result);

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
	static OtType type = nullptr;

	if (!type) {
		type = OtTypeClass::create<OtMemberReferenceClass>("MemberReference", OtReferenceClass::getMeta());
		type->set("__deref__", OtFunctionClass::create(&OtMemberReferenceClass::deref));
		type->set("__assign__", OtFunctionClass::create(&OtMemberReferenceClass::assign));
	}

	return type;
}


//
//	OtMemberReferenceClass::create
//

OtMemberReference OtMemberReferenceClass::create(OtObject o, const std::string& m) {
	OtMemberReference member = std::make_shared<OtMemberReferenceClass>(o, m);
	member->setType(getMeta());
	return member;
}
