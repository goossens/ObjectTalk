//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "gui.h"


//
//	OtMath
//

class OtMathClass;
typedef std::shared_ptr<OtMathClass> OtMath;

class OtMathClass : public OtGuiClass {
public:
	// get type definition
	static OtType getMeta();
};
