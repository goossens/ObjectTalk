//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "OtReference.h"
#include "OtIdentifier.h"


//
//	OtCaptureReferenceClass
//

class OtCaptureReferenceClass;
using OtCaptureReference = OtObjectPointer<OtCaptureReferenceClass>;

class OtCaptureReferenceClass : public OtReferenceClass {
public:
	// constructors
	OtCaptureReferenceClass() = default;
	OtCaptureReferenceClass(OtID m) :  member(m) {}

	// debugging support
	inline std::string describe() override { return std::string(OtIdentifier::name(member)); }

	// (de)reference functions
	OtObject deref();
	OtObject assign(OtObject value);

	// get type definition
	static OtType getMeta();

private:
	OtID member;
};
