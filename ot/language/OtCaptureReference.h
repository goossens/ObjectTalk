//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "OtReference.h"
#include "OtSelector.h"


//
//	OtCaptureReferenceClass
//

class OtCaptureReferenceClass;
using OtCaptureReference = OtObjectPointer<OtCaptureReferenceClass>;

class OtCaptureReferenceClass : public OtReferenceClass {
public:
	// constructors
	OtCaptureReferenceClass() = default;
	OtCaptureReferenceClass(size_t m) :  member(m) {}

	// debugging support
	std::string describe() override { return std::string(OtSelector::name(member)); }

	// (de)reference functions
	OtObject deref();
	OtObject assign(OtObject value);

	// get type definition
	static OtType getMeta();

private:
	size_t member;
};
