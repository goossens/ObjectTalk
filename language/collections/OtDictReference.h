//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
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
using OtDictReference = OtObjectPointer<OtDictReferenceClass>;

class OtDictReferenceClass : public OtReferenceClass {
public:
	// debugging support
	inline std::string describe() override { return "[\"" + index + "\"]"; }

	// (de)reference functions
	inline OtObject deref() { return dict->getEntry(index); }
	inline OtObject assign(OtObject value) { dict->setEntry(index, value); return value; }

	// get type definition
	static OtType getMeta();

protected:
	// constructors
	friend class OtObjectPointer<OtDictReferenceClass>;
	OtDictReferenceClass() = default;
	OtDictReferenceClass(OtDict d, const std::string& i) : dict(d), index(i) {}

private:
	// data
	OtDict dict;
	std::string index;
};
