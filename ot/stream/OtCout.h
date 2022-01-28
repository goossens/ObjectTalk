//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <functional>
#include <string>

#include "OtSingleton.h"
#include "OtStream.h"


//
//	OtCout
//

class OtCoutClass;
typedef std::shared_ptr<OtCoutClass> OtCout;

class OtCoutClass : public OtStreamClass, public OtObjectSingleton<OtCoutClass> {
public:
	// constructor
	OtCoutClass();

	// output an object (will internally be converted to a String)
	OtObject operator << (OtObject object);

	// write strings
	void write(const char* string);
	void write(const std::string& string);

	// set output function
	void setOutputFunction(std::function<void(const std::string&)> function);

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtCout create();

private:
	// the output function currently in use
	std::function<void(const std::string&)> outputFunction;
};
