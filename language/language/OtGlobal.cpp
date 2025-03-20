//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <iostream>

#include "OtLog.h"
#include "OtNumbers.h"

#include "OtGlobal.h"

#include "OtPrimitive.h"
#include "OtBoolean.h"
#include "OtInteger.h"
#include "OtReal.h"
#include "OtFunction.h"

#include "OtCollection.h"
#include "OtArray.h"
#include "OtDict.h"
#include "OtSet.h"

#include "OtSystem.h"
#include "OtPathObject.h"
#include "OtIO.h"
#include "OtOS.h"
#include "OtFS.h"

#include "OtStream.h"
#include "OtCout.h"
#include "OtCerr.h"

#include "OtClass.h"
#include "OtModule.h"

#include "OtRangeIterator.h"

#include "OtDebugger.h"


//
//	OtGlobalClass::OtGlobalClass
//

OtGlobalClass::OtGlobalClass() {
	// add default constants
	set("null", OtObject::create());
	set("true", OtBoolean::create(true));
	set("false", OtBoolean::create(false));
	set("pi", OtReal::create(std::numbers::pi));
	set("e", OtReal::create(std::numbers::e));

	// add default functions
	set("assert", OtFunction::create(&OtGlobalClass::doAssert));
	set("range", OtFunction::create(&OtGlobalClass::range));
	set("addImportPath", OtFunction::create(&OtGlobalClass::addImportPath));
	set("import", OtFunction::create(&OtGlobalClass::import));
	set("print", OtFunction::create(&OtGlobalClass::print));
	set("members", OtFunction::create(&OtGlobalClass::members));

	// add default classes
	set("Object", OtClass::create(OtObjectClass::getMeta()));

	set("Primitive", OtClass::create(OtPrimitiveClass::getMeta()));
	set("Boolean", OtClass::create(OtBooleanClass::getMeta()));
	set("Integer", OtClass::create(OtIntegerClass::getMeta()));
	set("Real", OtClass::create(OtRealClass::getMeta()));
	set("String", OtClass::create(OtStringClass::getMeta()));
	set("Function", OtClass::create(OtFunctionClass::getMeta()));

	set("Collection", OtClass::create(OtCollectionClass::getMeta()));
	set("Array", OtClass::create(OtArrayClass::getMeta()));
	set("Dict", OtClass::create(OtDictClass::getMeta()));
	set("Set", OtClass::create(OtSetClass::getMeta()));

	set("System", OtClass::create(OtSystemClass::getMeta()));
	set("Path", OtClass::create(OtPathObjectClass::getMeta()));
	set("IO", OtClass::create(OtIOClass::getMeta()));
	set("OS", OtClass::create(OtOSClass::getMeta()));
	set("FS", OtClass::create(OtFSClass::getMeta()));

	set("Stream", OtClass::create(OtStreamClass::getMeta()));
	set("Cout", OtClass::create(OtCoutClass::getMeta()));
	set("Cerr", OtClass::create(OtCerrClass::getMeta()));

	// add default class instances
	set("io", OtIO::create());
	set("os", OtOS::create());
	set("fs", OtFS::create());

	set("cout", OtCout::create());
	set("cerr", OtCerr::create());

	set("debug", OtDebugger::create());
}


//
//	OtGlobalClass::doAssert
//

void OtGlobalClass::doAssert(bool condition) {
	if (!condition) {
		OtLogError("Assertion error");
	}
}


//
//	OtGlobalClass::addImportPath
//

void OtGlobalClass::addImportPath(const std::string& path) {
	OtModuleClass::addPath(path);
}


//
//	OtGlobalClass::import
//

OtObject OtGlobalClass::import(const std::string& path) {
	return OtModuleClass::import(path);
}


//
//	OtGlobalClass::range
//

OtObject OtGlobalClass::range(size_t count, OtObject* parameters) {
	int64_t from = 0;
	int64_t to = 0;
	int64_t increment = 0;

	if (count == 1) {
		from = 0;
		to = parameters[0]->operator int64_t() - 1;
		increment = 1;

	} else if (count == 2) {
		from = parameters[0]->operator int64_t();
		to = parameters[1]->operator int64_t();
		increment = 1;

	} else if (count == 3) {
		from = parameters[0]->operator int64_t();
		to = parameters[1]->operator int64_t();
		increment = parameters[2]->operator int64_t();

	} else {
		OtLogError("Range function requires 1, 2 or 3 parameters, not {}", count);
	}

	if (increment == 0) {
		OtLogError("You can't create a range with an increment of 0");

	} else if (to > from && increment < 0) {
		OtLogError("Forward ranges must have positive increments");

	} else if (from > to && increment > 0) {
		OtLogError("Reverse ranges must have negative increments");
	}

	return OtRangeIterator::create(from, to, increment);
}


//
//	OtGlobalClass::print
//

void OtGlobalClass::print(size_t count, OtObject* parameters) {
	for (size_t i = 0; i < count; i++) {
		std::cout << parameters[i]->operator std::string();
	}

	std::cout << std::endl;
}


//
//	OtGlobalClass::members
//

OtObject OtGlobalClass::members(OtObject object) {
	OtArray array = OtArray::create();

	// special treatment for class objects
	if (object.isKindOf<OtClass>()) {
		OtClass(object)->getClassType()->eachMemberID([&](OtID id) {
			array->append(OtString::create(OtIdentifier::name(id)));
		});

	} else if (object->hasMembers()) {
		object->eachMemberID([&](OtID id) {
			array->append(OtString::create(OtIdentifier::name(id)));
		});
	}

	// return results
	return array;
}


//
//	OtGlobalClass::getMeta
//

OtType OtGlobalClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtType::create<OtGlobalClass>("Global", OtInternalClass::getMeta());
	}

	return type;
}
