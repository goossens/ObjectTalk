//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <cstddef>


//
//	OtInstanceDataBuffer
//

class OtInstanceDataBuffer {
public:
	// submit to GPU
	void submit(void* data, size_t count, size_t stride);
};
