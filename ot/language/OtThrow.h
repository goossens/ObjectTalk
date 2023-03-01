//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "OtInternal.h"


//
//	OtThrow
//

class OtThrowClass;
using OtThrow = OtObjectPointer<OtThrowClass>;

class OtThrowClass : public OtInternalClass {
public:
	// constructor
	OtThrowClass() = default;

	// throw the exception
	void call(const std::string& error);

	// get type definition
	static OtType getMeta();
};
