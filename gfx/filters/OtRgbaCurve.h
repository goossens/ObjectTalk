//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <array>
#include <cstdint>

#include "glm/glm.hpp"

#include "OtColor.h"
#include "OtFilter.h"
#include "OtShaders.h"


//
//	OtRgbaCurve
//

class OtRgbaCurve : public OtFilter {
public:
	// curve modes
	enum class Mode {
		rgb,
		red,
		green,
		blue,
		alpha
	};

	// constructor
	OtRgbaCurve() {
		// initialize default LUT
		for (int i = 0; i < 256; i++) {
			lut[i] = static_cast<float>(i) / 255.0f;
		}
	}

	// set properties
	inline void setMode(Mode value) { mode = value; }
	inline void setLUT(std::array<float, 256>& value) { lut = value; lutDirty = true; }
	inline void setBlackLevel(OtColor value) { blackLevel = value; }
	inline void setWhiteLevel(OtColor value) { whiteLevel = value; }

	// configure the compute pass
	void configurePass(OtComputePass& pass) override {
		// initialize pipeline (if required)
		if (!pipeline.isValid()) {
			pipeline.setShader(OtRgbaCurveComp, OtRgbaCurveCompSize);
		}

		// set uniforms
		struct Uniforms {
			glm::vec4 blackLevel;
			glm::vec4 whiteLevel;
			glm::vec4 lut[256];
			int32_t mode;
		} uniforms {
			blackLevel,
			whiteLevel,
			{},
			static_cast<int32_t>(mode)
		};

		for (size_t i = 0; i < 256; i++) {
			uniforms.lut[i].r = lut[i];
		}

		pass.addUniforms(&uniforms, sizeof(uniforms));
	}

private:
	// properties
	Mode mode = Mode::rgb;
	std::array<float, 256> lut;
	OtColor blackLevel{0.0f, 0.0f, 0.0f};
	OtColor whiteLevel{1.0f, 1.0f, 1.0f};

	// work variables
	bool lutDirty = true;
};
