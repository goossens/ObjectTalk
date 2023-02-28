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
//	OtCaptureReferenceClass
//

class OtCaptureReferenceClass;
typedef std::shared_ptr<OtCaptureReferenceClass> OtCaptureReference;

class OtCaptureReferenceClass : public OtReferenceClass {
public:
	// constructors
	OtCaptureReferenceClass() = default;
	OtCaptureReferenceClass(size_t m) :  member(m) {}

	// debugging support
	std::string describe() override { return OtSelector::name(member); }

	// (de)reference functions
	OtObject deref();
	OtObject assign(OtObject value);

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtCaptureReference create(const size_t member);

private:
	size_t member;
};
