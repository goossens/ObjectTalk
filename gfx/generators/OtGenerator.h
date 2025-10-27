//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "OtComputeProgram.h"
#include "OtFrameBuffer.h"


//
//	OtGenerator
//

class OtGenerator {
public:
	// destructor
	virtual inline ~OtGenerator() {}

	// run generator
	void render(OtFrameBuffer& destination);

private:
	// prepare generator pass
	virtual OtComputeProgram& preparePass() = 0;

	// number of GPU threads
	static constexpr int threadCount = 8;
};
