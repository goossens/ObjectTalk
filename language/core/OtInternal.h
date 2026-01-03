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
//	OtInternal
//

class OtInternalClass;
using OtInternal = OtObjectPointer<OtInternalClass>;

class OtInternalClass : public OtObjectClass {
public:
	// get type definition
	static OtType getMeta();

protected:
	// constructor
	friend class OtObjectPointer<OtInternalClass>;
	OtInternalClass() = default;
};
