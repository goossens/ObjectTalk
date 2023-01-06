//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
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
//	OtCoutClass::operator <<
//

OtObject OtCoutClass::operator << (OtObject object) {
	std::cout << object->operator std::string();
	return shared();
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
		type = OtTypeClass::create<OtCoutClass>(
			"Cout",
			OtStreamClass::getMeta(),
			[]() {
				return (OtObject) OtCoutClass::instance();
			});

		type->set("__lshift__", OtFunctionClass::create(&OtCoutClass::operator <<));
	}

	return type;
}


//
//	OtCoutClass::create
//

OtCout OtCoutClass::create() {
	return OtCoutClass::instance();
}
