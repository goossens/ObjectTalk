//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "OtColor.h"
#include "OtFilter.h"
#include "OtSampler.h"
#include "OtShaderProgram.h"
#include "OtTexture.h"
#include "OtUniformVec4.h"


//
//	OtRgbaCurve
//

class OtRgbaCurve : public OtFilter {
public:
	// curve types
	enum class Mode {
		rgb,
		red,
		green,
		blue,
		alpha
	};

	// set properties
	inline void setMode(Mode m) { mode = m; }
	inline void setLUT(OtTexture lut) { lutTexture = lut; }
	inline void setBlackLevel(OtColor color) { blackLevel = color; }
	inline void setWhiteLevel(OtColor color) { whiteLevel = color; }

private:
	// execute filter
	void execute(OtPass& pass) override;

	// properties
	Mode mode = Mode::rgb;
	OtColor blackLevel{0.0f, 0.0f, 0.0f};
	OtColor whiteLevel{1.0f, 1.0f, 1.0f};

	// GPU assets
	OtUniformVec4 uniform = OtUniformVec4("u_rbgcurves", 3);
	OtTexture lutTexture;
	OtSampler lutSampler{"s_lut", OtTexture::linearSampling | OtTexture::clampSampling};
	OtShaderProgram program = OtShaderProgram("OtFilterVS", "OtRgbaCurvesFS");
};
