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
#include "OtSampler.h"
#include "OtShaderProgram.h"
#include "OtTexture.h"


//
//	OtAlphaOver
//

class OtAlphaOver : public OtFilter {
public:
	// set properties
	inline void setOverlay(OtTexture overlay) { overlayTexture = overlay; }

private:
	// execute filter
	void execute(OtPass& pass) override;

	// GPU assets
	OtTexture overlayTexture;
	OtSampler overlaySampler{"s_overlay", OtSampler::pointSampling | OtSampler::clampSampling};
	OtShaderProgram program = OtShaderProgram("OtFilterVS", "OtAlphaOverFS");
};
