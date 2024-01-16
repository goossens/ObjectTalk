//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <iostream>

#include "OtCout.h"
#include "OtFunction.h"


//
//	OtCoutClass::operator<<
//

OtObject OtCoutClass::operator<<(OtObject object) {
	std::cout << object->operator std::string();
	return OtObject(this);
}


//
//	OtCoutClass::write
//

void OtCoutClass::write(const char* string) {
	std::cout << string;
}


void OtCoutClass::write(const std::string& string) {
	std::cout << string;
}


//
//	OtCoutClass::getMeta
//

OtType OtCoutClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtType::create<OtCoutClass>("Cout", OtStreamClass::getMeta());
		type->set("__lshift__", OtFunction::create(&OtCoutClass::operator<<));
	}

	return type;
}
