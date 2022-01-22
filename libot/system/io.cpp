//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <fstream>
#include <sstream>

#include "ot/source.h"

#include "ot/string.h"
#include "ot/function.h"

#include "ot/bytecode.h"
#include "ot/compiler.h"
#include "ot/vm.h"

#include "ot/io.h"


//
//	OtIOClass::readJSON
//

OtObject OtIOClass::readJSON(const std::string& name) {
	std::stringstream buffer;

	try {
		std::ifstream stream(name.c_str());

		if (stream.fail()) {
			OtExcept("Can't read from file [%s]", name.c_str());
		}

		buffer << stream.rdbuf();
		stream.close();

	} catch (std::exception& e) {
		OtExcept("Can't read from file [%s], error: %s", name.c_str(), e.what());
	}

	OtCompiler compiler;
	OtSource source = OtSourceClass::create(name, buffer.str());
	OtByteCode bytecode = compiler.compileExpression(source);

	return OtVM::instance()->execute(bytecode);
}


//
//	OtIOClass::writeJSON
//

void OtIOClass::writeJSON(const std::string& name, OtObject object) {
	try {
		std::ofstream stream(name.c_str());

		if (stream.fail()) {
			OtExcept("Can't write to file [%s]", name.c_str());
		}

		stream << object->json();
		stream.close();

	} catch (std::exception& e) {
		OtExcept("Can't write to file [%s], error: %s", name.c_str(), e.what());
	}
}


//
//	OtIOClass::readText
//

OtObject OtIOClass::readText(const std::string& name) {
	std::stringstream buffer;

	try {
		std::ifstream stream(name.c_str());

		if (stream.fail()) {
			OtExcept("Can't read from file [%s]", name.c_str());
		}

		buffer << stream.rdbuf();
		stream.close();

	} catch (std::exception& e) {
		OtExcept("Can't read from file [%s], error: %s", name.c_str(), e.what());
	}

	return OtStringClass::create(buffer.str());
}


//
//	OtIOClass::writeText
//

void OtIOClass::writeText(const std::string& name, OtObject object) {
	try {
		std::ofstream stream(name.c_str());

		if (stream.fail()) {
			OtExcept("Can't write to file [%s]", name.c_str());
		}

		stream << object->operator std::string();
		stream.close();

	} catch (std::exception& e) {
		OtExcept("Can't write to file [%s], error: %s", name.c_str(), e.what());
	}
}


//
//	OtIOClass::getMeta
//

OtType OtIOClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtTypeClass::create<OtIOClass>(
			"IO",
			OtSystemClass::getMeta(),
			[]() {
				return (OtObject) OtIOClass::instance();
			});

		type->set("readJSON", OtFunctionClass::create(&OtIOClass::readJSON));
		type->set("writeJSON", OtFunctionClass::create(&OtIOClass::writeJSON));
		type->set("readText", OtFunctionClass::create(&OtIOClass::readText));
		type->set("writeText", OtFunctionClass::create(&OtIOClass::writeText));
	}

	return type;
}


//
//	OtIOClass::create
//

OtIO OtIOClass::create() {
	return OtIOClass::instance();
}
