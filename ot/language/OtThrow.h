//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
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
typedef std::shared_ptr<OtThrowClass> OtThrow;

class OtThrowClass : public OtInternalClass {
public:
	// constructor
	OtThrowClass() = default;

	// throw the exception
	void call(const std::string& error);

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtThrow create();
};