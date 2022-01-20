//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <string>

#include "singleton.h"
#include "system.h"


//
//	OtIO
//

class OtIOClass;
typedef std::shared_ptr<OtIOClass> OtIO;

class OtIOClass : public OtSystemClass, public OtObjectSingleton<OtIOClass> {
public:
	// print objects to STDOUT
	void print(size_t count, OtObject* parameters);

	// print objects to STDOUT with end of line
	void println(size_t count, OtObject* parameters);

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
