//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	OtNoise
//

class OtNoise {
public:
	// get perlin values
	float perlin(float x, float y);
	float perlin(float x, float y, float z);

	// get Fractional Brownian Motion (FBM) values
	float fbm(float x, float y);
	float fbm(float x, float y, float z);

private:
	// Fractional Brownian Motion (FBM) properties
	size_t octaves = 4;
	float startFrequency = 1.0f;	// ("width") of the first octave of noise
	float startAmplitude = 1.0f; 	// ("height") of the first octave of noise
	float lacunarity = 2.0f;		// specifies the frequency multiplier between successive octaves
	float persistence = 0.5f;		// is the loss of amplitude between successive octaves
};
