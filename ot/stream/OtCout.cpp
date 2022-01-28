//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <iostream>
#include <fstream>
#include <sstream>

#include "OtCout.h"
#include "OtFunction.h"


//
//	OtCoutClass::OtCoutClass
//

OtCoutClass::OtCoutClass() {
	// set default output function
	setOutputFunction([](const std::string& text) {
		std::cout << text;
	});
}


//
//	OtCoutClass::operator <<
//

OtObject OtCoutClass::operator << (OtObject object) {
	outputFunction(object->operator std::string());
	return shared();
}


//
//	OtCoutClass::write
//

void OtCoutClass::write(const char* string) {
	outputFunction(std::string(string));
}


void OtCoutClass::write(const std::string& string) {
	outputFunction(string);
}


//
//	OtCoutClass::setOutputFunction
//

void OtCoutClass::setOutputFunction(std::function<void(const std::string&)> function) {
	outputFunction = function;
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
