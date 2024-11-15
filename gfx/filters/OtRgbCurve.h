//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
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
//	OtRgbCurve
//

class OtRgbCurve : public OtFilter {
public:
	// curve types
	enum {
		rgbCurve,
		redCurve,
		greenCurve,
		blueCurve,
		alphaCurve
	};

	// set properties
	inline void setCurve(int c) { curve = c; }
	inline void setLUT(OtTexture lut) { lutTexture = lut; }
	inline void setBlackLevel(OtColor color) { blackLevel = color; }
	inline void setWhiteLevel(OtColor color) { whiteLevel = color; }

private:
	// execute filter
	void execute(OtPass& pass) override;

	// properties
	int curve = rgbCurve;
	OtColor blackLevel{0.0f, 0.0f, 0.0f};
	OtColor whiteLevel{1.0f, 1.0f, 1.0f};

	// GPU assets
	OtUniformVec4 uniform = OtUniformVec4("u_rbgcurves", 3);
	OtTexture lutTexture;
	OtSampler lutSampler{"s_lut", OtTexture::linearSampling | OtTexture::clampSampling};
	OtShaderProgram program = OtShaderProgram("OtFilterVS", "OtRgbCurvesFS");
};
