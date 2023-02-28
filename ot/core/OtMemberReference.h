//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	  Include files
//

#include "OtReference.h"
#include "OtSelector.h"


//
//	OtMemberReference
//

class OtMemberReferenceClass;
typedef std::shared_ptr<OtMemberReferenceClass> OtMemberReference;

class OtMemberReferenceClass : public OtReferenceClass {
public:
	// constructors
	OtMemberReferenceClass() = default;
	OtMemberReferenceClass(OtObject o, size_t m) : object(o), member(m) {}

	// debugging support
	std::string describe() override { return OtSelector::name(member); }

	// (de)reference functions
	OtObject deref();
	OtObject assign(OtObject value);

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtMemberReference create(OtObject object, size_t member);

private:
	OtObject object;
	size_t member;
};
