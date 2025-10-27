//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "glm/glm.hpp"

#include "OtColor.h"
#include "OtGenerator.h"
#include "OtUniformVec4.h"


//
//	OtCheckerBoard
//

class OtCheckerBoard : public OtGenerator {
public:
	// set the properties
	inline void setRepeat(float r) { repeat = r; }
	inline void setBlackColor(OtColor color) { blackColor = color; }
	inline void setWhiteColor(OtColor color) { whiteColor = color; }

private:
	// prepare generator pass
	OtComputeProgram& preparePass() override;

	// properties
	float repeat = 1.0f;
	OtColor blackColor{0.0f, 0.0f, 0.0f};
	OtColor whiteColor{1.0f, 1.0f, 1.0f};

	// shader resources
	OtComputeProgram program{"OtCheckerBoardCS"};
	OtUniformVec4 uniform{"u_checkerboard", 3};
};
