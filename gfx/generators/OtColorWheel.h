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

#include "OtGenerator.h"
#include "OtShaderProgram.h"
#include "OtUniformVec4.h"


//
//	OtColorWheel
//

class OtColorWheel : public OtGenerator {private:
	// execute generator
	void execute(OtPass& pass) override;

	// shader resources
	OtShaderProgram program{"OtGeneratorVS", "OtColorWheelFS"};
};
