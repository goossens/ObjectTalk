//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "ot/object.h"


//
//	OtIdeClass
//

class OtIdeClass;
typedef std::shared_ptr<OtIdeClass> OtIde;

class OtIdeClass : public OtObjectClass {
public:
	// get type definition
	static OtType getMeta();
};
