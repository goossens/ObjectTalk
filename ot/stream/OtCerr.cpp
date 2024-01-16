//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <iostream>

#include "OtCerr.h"
#include "OtFunction.h"


//
//	OtCerrClass::operator<<
//

OtObject OtCerrClass::operator<<(OtObject object) {
	std::cerr << object->operator std::string();
	return OtObject(this);
}


//
//	OtCerrClass::write
//

void OtCerrClass::write(const char* string) {
	std::cerr << string;
}


void OtCerrClass::write(const std::string& string) {
	std::cerr << string;
}


//
//	OtCerrClass::getMeta
//

OtType OtCerrClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtType::create<OtCerrClass>("Cerr", OtStreamClass::getMeta());
		type->set("__lshift__", OtFunction::create(&OtCerrClass::operator<<));
	}

	return type;
}
