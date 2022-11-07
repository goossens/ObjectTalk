//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	  Include files
//

#include "OtReference.h"


//
//	OtCaptureReferenceClass
//

class OtCaptureReferenceClass;
typedef std::shared_ptr<OtCaptureReferenceClass> OtCaptureReference;

class OtCaptureReferenceClass : public OtReferenceClass {
public:
	// constructors
	OtCaptureReferenceClass() = default;
	OtCaptureReferenceClass(const std::string& n) :  name(n) {}

	// debugging support
	std::string describe() override { return name; }

	// (de)reference functions
	OtObject deref();
	OtObject assign(OtObject value);

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtCaptureReference create(const std::string& name);

private:
	std::string name;
};
