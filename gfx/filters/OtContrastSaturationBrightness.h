//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "OtFilter.h"
#include "OtShaders.h"


//
//	OtContrastSaturationBrightness
//

class OtContrastSaturationBrightness : public OtFilter {
public:
	// set properties
	inline void setContrast(float value) { contrast = value; }
	inline void setSaturation(float value) { saturation = value; }
	inline void setBrightness(float value) { brightness = value; }

	// configure the compute pass
	void configurePass(OtComputePass& pass) override {
		// initialize pipeline (if required)
		if (!pipeline.isValid()) {
			pipeline.setShader(OtContrastSaturationBrightnessComp, OtContrastSaturationBrightnessCompSize);
		}

		// set uniforms
		struct Uniforms {
			float contrast;
			float saturation;
			float brightness;
		} uniforms {
			contrast,
			saturation,
			brightness
		};

		pass.addUniforms(&uniforms, sizeof(uniforms));
	}

private:
	// properties
	float contrast = 1.0f;
	float saturation = 1.0f;
	float brightness = 1.0f;
};
