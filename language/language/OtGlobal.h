//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <string>

#include "OtInternal.h"


//
//	OtGlobal
//

class OtGlobalClass;
using OtGlobal = OtObjectPointer<OtGlobalClass>;

class OtGlobalClass : public OtInternalClass {
public:
	// constructor (create a default ObjectTalk scope)
	OtGlobalClass();

	// get type definition
	static OtType getMeta();

private:
	// check assertion
	static void doAssert(bool condition);

	// add a module import path
	static void addImportPath(const std::string& path);

	// import module
	static OtObject import(const std::string& path);

	// create a range iterator
	static OtObject range(size_t count, OtObject* parameters);

	// print to STDOUT
	static void print(size_t count, OtObject* parameters);

	// get list of objects member names
	static OtObject members(OtObject object);
};
