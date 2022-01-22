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

#include "ot/cerr.h"
#include "ot/function.h"


//
//	OtCerrClass::OtCerrClass
//

OtCerrClass::OtCerrClass() {
	// set default output function
	setOutputFunction([](const std::string& text) {
		std::cerr << text;
	});
}


//
//	OtCerrClass::operator <<
//

OtObject OtCerrClass::operator << (OtObject object) {
	outputFunction(object->operator std::string());
	return shared();
}


//
//	OtCerrClass::write
//

void OtCerrClass::write(const char* string) {
	outputFunction(std::string(string));
}


void OtCerrClass::write(const std::string& string) {
	outputFunction(string);
}


//
//	OtCerrClass::setOutputFunction
//

void OtCerrClass::setOutputFunction(std::function<void(const std::string&)> function) {
	outputFunction = function;
}


//
//	OtCerrClass::getMeta
//

OtType OtCerrClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtTypeClass::create<OtCerrClass>(
			"Cerr",
			OtStreamClass::getMeta(),
			[]() {
				return (OtObject) OtCerrClass::instance();
			});

		type->set("__lshift__", OtFunctionClass::create(&OtCerrClass::operator <<));
	}

	return type;
}


//
//	OtCerrClass::create
//

OtCerr OtCerrClass::create() {
	return OtCerrClass::instance();
}
