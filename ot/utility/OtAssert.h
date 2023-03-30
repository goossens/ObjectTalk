//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	  Include files
//

#include "OtFormat.h"
#include "OtLog.h"


//
//	OtAssert
//

#define OtAssert(assertion) if (!(assertion)) OtLogFatal(OtFormat("Assertion error: %s", #assertion))
