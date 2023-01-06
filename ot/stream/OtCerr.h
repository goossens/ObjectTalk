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
//	OtCerr
//

class OtCerrClass;
typedef std::shared_ptr<OtCerrClass> OtCerr;
typedef std::function<void(const std::string&)> OtCerrOutputFunction;

class OtCerrClass : public OtStreamClass, public OtObjectSingleton<OtCerrClass> {
public:
	// output an object (will internally be converted to a String)
	OtObject operator << (OtObject object);

	// write strings
	void write(const char* string);
	void write(const std::string& string);

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtCerr create();
};
