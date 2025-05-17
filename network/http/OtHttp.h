//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "OtObject.h"


//
//	OtHttp
//

class OtHttpClass;
using OtHttp = OtObjectPointer<OtHttpClass>;

class OtHttpClass : public OtObjectClass {
public:
	// get type definition
	static OtType getMeta();

protected:
	// constructor
	friend class OtObjectPointer<OtHttpClass>;
	OtHttpClass() = default;
};
