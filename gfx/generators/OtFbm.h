//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "OtGenerator.h"
#include "OtShaderProgram.h"
#include "OtUniformVec4.h"


//
//	OtFbm (Fractional Brownian Motion)
//

class OtFbm : public OtGenerator {
public:
	// noise types
	enum class NoiseType {
		simplex,
		perlin,
		gradient,
		worley
	};

	static constexpr const char* noiseTypes[] = {
		"Simplex Noise",
		"Perlin Noise",
		"Gradient Noise",
		"Worley Noise"
	};

	static constexpr size_t noiseTypeCount = sizeof(noiseTypes) / sizeof(*noiseTypes);

	// set the properties
	inline void setFrequency(int f) { frequency = f; }
	inline void setLacunarity(float l) { lacunarity = l; }
	inline void setAmplitude(float a) { amplitude = a; }
	inline void setPersistence(float p) { persistence = p; }
	inline void setOctaves(int o) { octaves = o; }
	inline void setNoiseType(NoiseType nt) { noiseType = nt; }

private:
	// execute generator
	void execute(OtPass& pass) override;

	// properties
	int frequency = 1;
	float lacunarity = 2.0f;
	float amplitude = 0.5f;
	float persistence = 0.5f;
	int octaves = 5;
	NoiseType noiseType = NoiseType::simplex;

	// shader resources
	OtShaderProgram program{"OtGeneratorVS", "OtFbmFS"};
	OtUniformVec4 uniform{"u_fbm", 2};
};
