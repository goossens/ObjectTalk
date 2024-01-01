//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <cstddef>


//
//	OtTransientIndexBuffer
//

class OtTransientIndexBuffer {
public:
	// submit to GPU
	void submit(void* data, size_t count);
};
