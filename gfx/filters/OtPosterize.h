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
//	OtPosterize
//

class OtPosterize : public OtFilter {
public:
	// set properties
	inline void setLevels(int value) { levels = value; }

	// configure the compute pass
	void configurePass(OtComputePass& pass) override {
		// initialize pipeline (if required)
		if (!pipeline.isValid()) {
			pipeline.setShader(OtPosterizeComp, OtPosterizeCompSize);
		}

		// set uniforms
		struct Uniforms {
			int32_t levels;
		} uniforms {
			static_cast<int32_t>(levels)
		};

		pass.addUniforms(&uniforms, sizeof(uniforms));
	}

private:
	// properties
	int levels = 10;
};
