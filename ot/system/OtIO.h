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
#include "OtSystem.h"


//
//	OtIO
//

class OtIOClass;
typedef std::shared_ptr<OtIOClass> OtIO;

class OtIOClass : public OtSystemClass, public OtObjectSingleton<OtIOClass> {
public:
	// read a JSON file
	OtObject readJSON(const std::string& name);

	// write a JSON file
	void writeJSON(const std::string& name, OtObject object);

	// read a text file
	OtObject readText(const std::string& name);

	// write a text file
	void writeText(const std::string& name, OtObject object);

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtIO create();
};
