//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "OtInternal.h"


//
//	OtInternal
//

class OtReferenceClass;
using OtReference = OtObjectPointer<OtReferenceClass>;

class OtReferenceClass : public OtInternalClass {
public:
	// get type definition
	static OtType getMeta();
};
