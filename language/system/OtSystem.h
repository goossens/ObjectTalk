//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "OtObject.h"


//
//	OtSystem
//

class OtSystemClass;
using OtSystem = OtObjectPointer<OtSystemClass>;

class OtSystemClass : public OtObjectClass {
public:
	// get type definition
	static OtType getMeta();

protected:
	// constructor
	friend class OtObjectPointer<OtSystemClass>;
	OtSystemClass() = default;
};
