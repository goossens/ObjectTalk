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
#include "OtPostProcessingComp.h"


//
//	OtPostProcessing
//

class OtPostProcessing : public OtFilter {
public:
	enum class Tonemap {
		reinhardSimple,
		reinhardExtended,
		filmic,
		acesFilmic,
		uncharted2,
		lottes,
		uchimura
	};

	static constexpr const char* tonemapTypes[] = {
		"Reinhard Simple",
		"Reinhard Extended",
		"Filmic",
		"Aces Filmic",
		"Uncharted 2",
		"Lottes",
		"Uchimura"
	};

	static constexpr size_t tonemapTypeCount = sizeof(tonemapTypes) / sizeof(*tonemapTypes);

	// set properties
	inline void setExposure(float value) { exposure = value; }
	inline void setContrast(float value) { contrast = value; }
	inline void setTonemap(Tonemap value) { tonemap = value; }

	// configure the compute pass
	void configurePass(OtComputePass& pass) override {
		// initialize pipeline (if required)
		if (!pipeline.isValid()) {
			pipeline.setShader(OtPostProcessingComp, sizeof(OtPostProcessingComp));
		}

		// set uniforms
		struct Uniforms {
			float exposure;
			float contrast;
			int32_t tonemap;
		} uniforms {
			exposure,
			contrast,
			static_cast<int32_t>(tonemap)
		};

		pass.addUniforms(&uniforms, sizeof(uniforms));
	}

private:
	// properties
	float exposure = 1.0f;
	float contrast = 1.0f;
	Tonemap tonemap = Tonemap::acesFilmic;
};
