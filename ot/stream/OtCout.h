//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <string>

#include "OtSingleton.h"
#include "OtStream.h"


//
//	OtCout
//

class OtCoutClass;
typedef std::shared_ptr<OtCoutClass> OtCout;
typedef std::function<void(const std::string&)> OtCoutOutputFunction;

class OtCoutClass : public OtStreamClass, public OtObjectSingleton<OtCoutClass> {
public:
	// output an object (will internally be converted to a String)
	OtObject operator << (OtObject object);

	// write strings
	void write(const char* string);
	void write(const std::string& string);

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtCout create();
};
