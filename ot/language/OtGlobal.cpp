//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
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
#include "OtWorker.h"

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
	set("null", OtObjectClass::create());
	set("true", OtBooleanClass::create(true));
	set("false", OtBooleanClass::create(false));
	set("pi", OtRealClass::create(std::numbers::pi));
	set("e", OtRealClass::create(std::numbers::e));

	// add default functions
	set("assert", OtFunctionClass::create(&OtGlobalClass::doAssert));
	set("range", OtFunctionClass::create(&OtGlobalClass::range));
	set("import", OtFunctionClass::create(&OtGlobalClass::import));
	set("print", OtFunctionClass::create(&OtGlobalClass::print));
	set("super", OtFunctionClass::create(&OtGlobalClass::super));
	set("members", OtFunctionClass::create(&OtGlobalClass::members));

	// add default classes
	set("Object", OtClassClass::create(OtObjectClass::getMeta()));

	set("Primitive", OtClassClass::create(OtPrimitiveClass::getMeta()));
	set("Boolean", OtClassClass::create(OtBooleanClass::getMeta()));
	set("Integer", OtClassClass::create(OtIntegerClass::getMeta()));
	set("Real", OtClassClass::create(OtRealClass::getMeta()));
	set("String", OtClassClass::create(OtStringClass::getMeta()));
	set("Function", OtClassClass::create(OtFunctionClass::getMeta()));

	set("Collection", OtClassClass::create(OtCollectionClass::getMeta()));
	set("Array", OtClassClass::create(OtArrayClass::getMeta()));
	set("Dict", OtClassClass::create(OtDictClass::getMeta()));
	set("Set", OtClassClass::create(OtSetClass::getMeta()));

	set("System", OtClassClass::create(OtSystemClass::getMeta()));
	set("Path", OtClassClass::create(OtPathClass::getMeta()));
	set("IO", OtClassClass::create(OtIOClass::getMeta()));
	set("OS", OtClassClass::create(OtOSClass::getMeta()));
	set("FS", OtClassClass::create(OtFSClass::getMeta()));
	set("Worker", OtClassClass::create(OtWorkerClass::getMeta()));

	set("Stream", OtClassClass::create(OtStreamClass::getMeta()));
	set("Cout", OtClassClass::create(OtCoutClass::getMeta()));
	set("Cerr", OtClassClass::create(OtCerrClass::getMeta()));

	// add default class instances
	set("io", OtIOClass::create());
	set("os", OtOSClass::create());
	set("fs", OtFSClass::create());

	set("cout", OtCoutClass::create());
}


//
//	OtGlobalClass::doAssert
//

void OtGlobalClass::doAssert(bool condition) {
	if (!condition) {
		OtExcept("Assertion error");
	}
}


//
//	OtGlobalClass::import
//

OtObject OtGlobalClass::import(const std::string name) {
	return OtModuleClass::import(name);
}


//
//	OtGlobalClass::range
//

OtObject OtGlobalClass::range(size_t count, OtObject* parameters) {
	long from;
	long to;
	long increment;

	if (count == 1) {
		from = 1;
		to = parameters[0]->operator long();
		increment = 1;

	} else if (count == 2) {
		from = parameters[0]->operator long();
		to = parameters[1]->operator long();
		increment = 1;

	} else if (count == 3) {
		from = parameters[0]->operator long();
		to = parameters[1]->operator long();
		increment = parameters[2]->operator long();

	}else {
		OtExcept("Range functions required 1, 2 or 3 parameters, not %ld", count);
	}

	if (increment == 0) {
		OtExcept("You can't create a range with an increment of 0");

	} else if (to > from && increment < 0) {
		OtExcept("Forward ranges must have positive increments");

	} else if (from > to && increment > 0) {
		OtExcept("Reverse ranges must have negative increments");
	}

	return OtRangeIteratorClass::create(from, to, increment);
}


//
//	OtGlobalClass::print
//

void OtGlobalClass::print(size_t count, OtObject* parameters) {
	for (size_t i = 0; i < count; i++) {
		OtCoutClass::instance()->write(parameters[i]->operator std::string());
	}

	OtCoutClass::instance()->write("\n");
}


//
//	OtGlobalClass::super
//

OtObject OtGlobalClass::super(size_t count, OtObject* parameters) {
	// sanity check
	if (count < 2) {
		OtExcept("Super requires at least 2 parameters (%ld given)", count);
	}

	// get member from super class
	OtType type = parameters[0]->getType()->getParent();
	std::string memberName = parameters[1]->operator std::string();
	OtObject member;

	while (type && !member) {
		member = type->get(memberName);
		type = type->getParent();
	}

	// sanity check
	if (!member) {
		OtExcept(
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
	OtArray array = OtArrayClass::create();
	OtMembers members;

	// special treatment for class objects
	if (object->isKindOf("Class")) {
		members = object->cast<OtClassClass>()->getClassType()->getMembers();

	} else {
		members = object->getMembers();
	}

	// process all members (if required)
	if (members) {
		for (auto& name : members->names()) {
			array->append(OtStringClass::create(name));
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
		type = OtTypeClass::create<OtGlobalClass>("Global", OtInternalClass::getMeta());
	}

	return type;
}


//
//	OtGlobalClass::create
//

OtGlobal OtGlobalClass::create() {
	OtGlobal module = std::make_shared<OtGlobalClass>();
	module->setType(getMeta());
	return module;
}
