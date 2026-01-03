//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Waggressive-loop-optimizations"
#endif

#include "FastNoiseLite.h"

#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic pop
#endif


//
//	OtNoise
//

class OtNoise {
public:
	// constructors
	OtNoise() = default;
	OtNoise(size_t o, float f, float l, float p) : octaves(o), frequency(f), lacunarity(l), persistence(p) {}

	// get noise values
	float noise(float x, float y);
	float noise(float x, float y, float z);

	// get Fractional Brownian Motion (FBM) values
	float fbm(float x, float y);
	float fbm(float x, float y, float z);

	// configure FBM
	inline void setOctaves(size_t value) { octaves = value; stateDirty = true; }
	inline void setFrequency(float value) { frequency = value; stateDirty = true; }
	inline void setLacunarity(float value) { lacunarity = value; stateDirty = true; }
	inline void setPersistence(float value) { persistence = value; stateDirty = true; }

private:
	// Fractional Brownian Motion (FBM) properties
	size_t octaves = 4;
	float frequency = 1.0f;		// ("width") of the first octave of noise
	float lacunarity = 2.0f;	// specifies the frequency multiplier between successive octaves
	float persistence = 0.5f;	// is the loss of amplitude between successive octaves

	// noise generator state
	FastNoiseLite noiseState;
	FastNoiseLite fbmState;
	bool stateDirty = true;
	void updateState();
};
