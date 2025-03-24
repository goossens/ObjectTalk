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
//	OtThrow
//

class OtThrowClass;
using OtThrow = OtObjectPointer<OtThrowClass>;

class OtThrowClass : public OtInternalClass {
public:
	// throw the exception
	void call(const std::string& error);

	// get type definition
	static OtType getMeta();

protected:
	// constructor
	friend class OtObjectPointer<OtThrowClass>;
	OtThrowClass() = default;
};
