//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "internal.h"


//
//	OtGlobal
//

class OtGlobalClass;
typedef std::shared_ptr<OtGlobalClass> OtGlobal;

class OtGlobalClass : public OtInternalClass {
public:
	// constructor (create a default ObjectTalk scope)
	OtGlobalClass();

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtGlobal create();

private:
	// check assertion
	static void doAssert(bool condition);

	// import module
	static OtObject import(const std::string name);

	// create a range iterator
	static OtObject range(size_t count, OtObject* parameters);

	// print to STDOUT
	static void print(size_t count, OtObject* parameters);
};
