//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtByteCode.h"
#include "OtCompiler.h"
#include "OtFunction.h"
#include "OtIO.h"
#include "OtLog.h"
#include "OtSource.h"
#include "OtString.h"
#include "OtText.h"
#include "OtVM.h"


//
//	OtIOClass::readJSON
//

OtObject OtIOClass::readJSON(const std::string& name) {
	std::string text;
	OtText::load(name, text);

	OtCompiler compiler;
	OtSource source = OtSourceClass::create(name, text);
	OtByteCode bytecode = compiler.compileExpression(source);

	return OtVM::execute(bytecode);
}


//
//	OtIOClass::writeJSON
//

void OtIOClass::writeJSON(const std::string& name, OtObject object) {
	OtText::save(name, object->json());
}


//
//	OtIOClass::readText
//

OtObject OtIOClass::readText(const std::string& name) {
	std::string text;
	OtText::load(name, text);
	return OtString::create(text);
}


//
//	OtIOClass::writeText
//

void OtIOClass::writeText(const std::string& name, OtObject object) {
	OtText::save(name, object->operator std::string());
}


//
//	OtIOClass::getMeta
//

OtType OtIOClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtType::create<OtIOClass>("IO", OtSystemClass::getMeta());
		type->set("readJSON", OtFunction::create(&OtIOClass::readJSON));
		type->set("writeJSON", OtFunction::create(&OtIOClass::writeJSON));
		type->set("readText", OtFunction::create(&OtIOClass::readText));
		type->set("writeText", OtFunction::create(&OtIOClass::writeText));
	}

	return type;
}
