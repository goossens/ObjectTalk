//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "OtLog.h"


//
//	OtAssert
//

#if OT_DEBUG

#define OtAssert(assertion) if (!(assertion)) OtLogFatal("Assertion error: {}", #assertion)

#else

#define OtAssert(assertion) ((void)0)

#endif