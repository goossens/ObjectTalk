//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <string>

#include "OtSystem.h"


//
//	OtIO
//

class OtIOClass;
using OtIO = OtObjectPointer<OtIOClass>;

class OtIOClass : public OtSystemClass {
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

protected:
	// constructor
	friend class OtObjectPointer<OtIOClass>;
	OtIOClass() = default;
};
