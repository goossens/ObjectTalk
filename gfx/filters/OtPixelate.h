//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <cstdint>

#include "OtFilter.h"
#include "OtShaders.h"


//
//	OtPixelate
//

class OtPixelate : public OtFilter {
public:
	// set properties
	inline void setSize(int value) { size = value; }

	// configure the compute pass
	void configurePass(OtComputePass& pass) override {
		// initialize pipeline (if required)
		if (!pipeline.isValid()) {
			pipeline.setShader(OtPixelateComp, OtPixelateCompSize);
		}

		// set uniforms
		struct Uniforms {
			int32_t size;
		} uniforms {
			static_cast<int32_t>(size)
		};

		pass.addUniforms(&uniforms, sizeof(uniforms));
	}

private:
	// properties
	int size = 10;
};
