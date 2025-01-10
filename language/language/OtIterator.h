//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
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

class OtIteratorClass;
using OtIterator = OtObjectPointer<OtIteratorClass>;

class OtIteratorClass : public OtInternalClass {
public:
	// get type definition
	static OtType getMeta();
};
