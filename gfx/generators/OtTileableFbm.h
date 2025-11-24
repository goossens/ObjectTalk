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

#include "OtTileableFbmComp.h"
#include "OtGenerator.h"


//
//	OtTileableFbm (Fractional Brownian Motion)
//

class OtTileableFbm : public OtGenerator {
public:
	// set the properties
	inline void setFrequency(int f) { frequency = f; }
	inline void setLacunarity(int l) { lacunarity = l; }
	inline void setAmplitude(float a) { amplitude = a; }
	inline void setPersistence(float p) { persistence = p; }
	inline void setOctaves(int o) { octaves = o; }

	// configure the compute pass
	void configurePass(OtComputePass& pass) override {
		// initialize pipeline (if required)
		if (!pipeline.isValid()) {
			pipeline.setShader(OtTileableFbmComp, sizeof(OtTileableFbmComp));
		}

		// set uniforms
		struct Uniforms {
			int32_t frequency;
			int32_t lacunarity;
			float amplitude;
			float persistence;
			int32_t octaves;
		} uniforms{
			static_cast<int32_t>(frequency),
			static_cast<int32_t>(lacunarity),
			amplitude,
			persistence,
			static_cast<int32_t>(octaves)
		};


		pass.addUniforms(&uniforms, sizeof(uniforms));
	}

private:
	// properties
	int frequency = 10;
	int lacunarity = 2;
	float amplitude = 0.5f;
	float persistence = 0.5f;
	int octaves = 5;
};
