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

#include "glm/glm.hpp"

#include "OtColor.h"
#include "OtGenerator.h"
#include "OtShaders.h"


//
//	OtFlood
//

class OtFlood : public OtGenerator {
public:
	// set the properties
	inline void setColor(OtColor c) { color = c; }

	// configure the compute pass
	void configurePass(OtComputePass& pass) override {
		// initialize pipeline (if required)
		if (!pipeline.isValid()) {
			pipeline.setShader(OtFloodComp, OtFloodCompSize);
		}

		// set uniforms
		struct Uniforms {
			glm::vec4 color;
		} uniforms{
			color};

		pass.addUniforms(&uniforms, sizeof(uniforms));
	}

private:
	// properties
	OtColor color{0.0f};
};
