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
// OtClassReferenceClass
//

class OtClassReferenceClass;
typedef std::shared_ptr<OtClassReferenceClass> OtClassReference;

class OtClassReferenceClass : public OtReferenceClass {
public:
	// constructors
	OtClassReferenceClass() = default;
	OtClassReferenceClass(const std::string& cn, const std::string& mn) : className(cn), memberName(mn) {}

	// debugging support
	std::string describe() { return className + " " + memberName; }

	// (de)reference functions
	OtObject deref() {	return OtTypeClass::getRegistered(className)->get(memberName); }
	OtObject assign(OtObject value) { return OtTypeClass::getRegistered(className)->set(memberName, value); }

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtClassReference create(const std::string& className, const std::string& memberName);

private:
	std::string className;
	std::string memberName;
};
