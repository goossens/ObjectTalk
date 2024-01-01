//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <iostream>

#include "OtException.h"
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
#include "OtPath.h"
#include "OtIO.h"
#include "OtOS.h"
#include "OtFS.h"

#include "OtStream.h"
#include "OtCout.h"
#include "OtCerr.h"

#include "OtClass.h"
#include "OtModule.h"

#include "OtRangeIterator.h"

#include "OtVM.h"


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
	set("import", OtFunction::create(&OtGlobalClass::import));
	set("print", OtFunction::create(&OtGlobalClass::print));
	set("super", OtFunction::create(&OtGlobalClass::super));
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
	set("Path", OtClass::create(OtPathClass::getMeta()));
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
}


//
//	OtGlobalClass::doAssert
//

void OtGlobalClass::doAssert(bool condition) {
	if (!condition) {
		OtError("Assertion error");
	}
}


//
//	OtGlobalClass::import
//

OtObject OtGlobalClass::import(const std::filesystem::path& path) {
	return OtModuleClass::import(path.string());
}


//
//	OtGlobalClass::range
//

OtObject OtGlobalClass::range(size_t count, OtObject* parameters) {
	int64_t from;
	int64_t to;
	int64_t increment;

	if (count == 1) {
		from = 1;
		to = parameters[0]->operator int64_t();
		increment = 1;

	} else if (count == 2) {
		from = parameters[0]->operator int64_t();
		to = parameters[1]->operator int64_t();
		increment = 1;

	} else if (count == 3) {
		from = parameters[0]->operator int64_t();
		to = parameters[1]->operator int64_t();
		increment = parameters[2]->operator int64_t();

	}else {
		OtError("Range functions required 1, 2 or 3 parameters, not %ld", count);
	}

	if (increment == 0) {
		OtError("You can't create a range with an increment of 0");

	} else if (to > from && increment < 0) {
		OtError("Forward ranges must have positive increments");

	} else if (from > to && increment > 0) {
		OtError("Reverse ranges must have negative increments");
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
//	OtGlobalClass::super
//

OtObject OtGlobalClass::super(size_t count, OtObject* parameters) {
	// sanity check
	if (count < 2) {
		OtError("Super requires at least 2 parameters (%ld given)", count);
	}

	// get member from super class
	OtType type = parameters[0]->getType()->getParent();
	std::string memberName = parameters[1]->operator std::string();
	size_t selector = OtSelector::create(memberName);
	OtObject member;

	while (type && !member) {
		member = type->get(selector);
		type = type->getParent();
	}

	// sanity check
	if (!member) {
		OtError(
			"Can't find member function [%s] in super class of [%s]",
			memberName.c_str(),
			parameters[0]->getType()->getName().c_str());
	}

	// call member function
	return OtVM::instance()->callMemberFunction(parameters[0], member, count - 2, &(parameters[2]));
}


//
//	OtGlobalClass::members
//

OtObject OtGlobalClass::members(OtObject object) {
	OtArray array = OtArray::create();
	std::vector<std::string_view> names;

	// special treatment for class objects
	if (object->isKindOf("Class")) {
		OtClass(object)->getClassType()->getMemberNames(names);

		for (auto& name : names) {
			array->append(OtString::create(name));
		}

	} else if (object->hasMembers()) {
		object->getMemberNames(names);

		for (auto& name : names) {
			array->append(OtString::create(name));
		}
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
