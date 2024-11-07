//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <string>

#include "OtStream.h"


//
//	OtCout
//

class OtCoutClass;
using OtCout = OtObjectPointer<OtCoutClass>;

class OtCoutClass : public OtStreamClass {
public:
	// output an object (will internally be converted to a String first)
	OtObject operator<<(OtObject object);

	// write strings
	void write(const char* string);
	void write(const std::string& string);

	// get type definition
	static OtType getMeta();
};
