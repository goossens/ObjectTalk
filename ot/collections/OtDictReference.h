//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <string>

#include "OtDict.h"
#include "OtReference.h"


//
//	OtDictReferenceClass
//

class OtDictReferenceClass;
typedef std::shared_ptr<OtDictReferenceClass> OtDictReference;

class OtDictReferenceClass : public OtReferenceClass {
public:
	// constructors
	OtDictReferenceClass() = default;
	OtDictReferenceClass(OtDict d, const std::string& i) : dict(d), index(i) {}

	// debugging support
	std::string describe() override { return "[\"" + index + "\"]"; }

	// (de)reference functions
	OtObject deref() { return dict->getEntry(index); }
	OtObject assign(OtObject value) { dict->setEntry(index, value); return value; }

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtDictReference create(OtDict a, const std::string& i);

private:
	OtDict dict;
	std::string index;
};
