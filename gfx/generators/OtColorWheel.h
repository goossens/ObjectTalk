//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "OtColorWheelComp.h"
#include "OtGenerator.h"


//
//	OtColorWheel
//

class OtColorWheel : public OtGenerator {
public:
	// configure the compute pass
	void configurePass([[maybe_unused]] OtComputePass& pass) override {
		// initialize pipeline (if required)
		if (!pipeline.isValid()) {
			pipeline.setShader(OtColorWheelComp, sizeof(OtColorWheelComp));
		}
	}
};
