//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "ot/object.h"


//
//	OtGui
//

class OtGuiClass;
typedef std::shared_ptr<OtGuiClass> OtGui;

class OtGuiClass : public OtObjectClass {
public:
	// get type definition
	static OtType getMeta();
};
