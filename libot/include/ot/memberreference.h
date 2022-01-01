//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	  Include files
//

#include "reference.h"


//
//	OtMemberReference
//

class OtMemberReferenceClass;
typedef std::shared_ptr<OtMemberReferenceClass> OtMemberReference;

class OtMemberReferenceClass : public OtReferenceClass {
public:
	// constructors
	OtMemberReferenceClass() = default;
	OtMemberReferenceClass(OtObject o, const std::string& m) : object(o), member(m) {}

	// debugging support
	std::string describe() { return object->getType()->getName() + " " + member; }

	// (de)reference functions
	OtObject deref();
	OtObject assign(OtObject value);

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtMemberReference create(OtObject object, const std::string& member);

private:
	OtObject object;
	std::string member;
};
