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
#include "OtSharpenComp.h"


//
//	OtSharpen
//

class OtSharpen : public OtFilter {
public:
	// set properties
	inline void setStrength(float value) { strength = value; }

	// configure the compute pass
	void configurePass(OtComputePass& pass) override {
		// initialize pipeline (if required)
		if (!pipeline.isValid()) {
			pipeline.setShader(OtSharpenComp, sizeof(OtSharpenComp));
		}

		// set uniforms
		struct Uniforms {
			glm::vec2 texelSize;
			int32_t strength;
		} uniforms {
			sourceTexelSize,
			static_cast<int32_t>(strength)
		};

		pass.addUniforms(&uniforms, sizeof(uniforms));
	}

private:
	// properties
	float strength = 100.0f;
};
