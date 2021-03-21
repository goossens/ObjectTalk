//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <uv.h>

#include "ot/exception.h"


//
//	Error checking
//

#define UV_CHECK_ERROR(action, status) if (status < 0) OT_EXCEPT("Libuv error in %s: %s", action, uv_strerror(status))
