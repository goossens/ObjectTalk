//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "OtFilter.h"
#include "OtShader.h"
#include "OtUniform.h"


//
//	OtBlitClass
//

class OtBlitClass;
typedef std::shared_ptr<OtBlitClass> OtBlit;

class OtBlitClass : public OtFilterClass {
public:
	// set properties
	void setIntensity(float i) { intensity = i; }
	void setAlpha(float a) { alpha = a; }

	// execute filter
	void execute(OtPass& pass, int w, int h) override;

private:
	// blur properties
	float intensity = -1.0;
	float alpha = -1.0;

	// GPU assets
	OtUniform uniform = OtUniform("u_blit", 1);
	OtShader shader = OtShader("OtBlitVS", "OtBlitFS");
};
