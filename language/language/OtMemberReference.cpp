//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtMemberReference.h"


//
//	OtMemberReferenceClass::getMeta
//

OtType OtMemberReferenceClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtType::create<OtMemberReferenceClass>("MemberReference", OtReferenceClass::getMeta());
		type->set("__deref__", OtFunction::create(&OtMemberReferenceClass::deref));
		type->set("__assign__", OtFunction::create(&OtMemberReferenceClass::assign));
	}

	return type;
}
