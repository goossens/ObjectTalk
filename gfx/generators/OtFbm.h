//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
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
	// set the properties
	inline void setFrequency(int f) { frequency = f; }
	inline void setLacunarity(int l) { lacunarity = l; }
	inline void setAmplitude(float a) { amplitude = a; }
	inline void setPersistence(float p) { persistence = p; }
	inline void setOctaves(int o) { octaves = o; }

private:
	// execute generator
	void execute(OtPass& pass, int w, int h) override;

	// properties
	int frequency = 10;
	int lacunarity = 2;
	float amplitude = 0.5f;
	float persistence = 0.5f;
	int octaves = 5;

	// shader resources
	OtShaderProgram program{"OtGeneratorVS", "OtFbmFS"};
	OtUniformVec4 uniform{"u_fbm", 2};
};
