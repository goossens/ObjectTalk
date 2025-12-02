//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtNoise.h"


//
//	OtNoise::noise
//

float OtNoise::noise(float x, float y) {
	updateState();
	return noiseState.GetNoise(x, y);
}

float OtNoise::noise(float x, float y, float z) {
	updateState();
	return noiseState.GetNoise(x, y, z);
}


//
//	OtNoise::fbm
//

float OtNoise::fbm(float x, float y) {
	updateState();
	return fbmState.GetNoise(x, y);
}

float OtNoise::fbm(float x, float y, float z) {
	updateState();
	return fbmState.GetNoise(x, y, z);
}


//
//	OtNoise::updateState
//

void OtNoise::updateState() {
	if (stateDirty) {
		noiseState.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
		noiseState.SetFrequency(frequency);

		fbmState.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
		fbmState.SetFrequency(frequency);
		fbmState.SetFractalType(FastNoiseLite::FractalType_FBm);
		fbmState.SetFractalOctaves(static_cast<int>(octaves));
		fbmState.SetFractalLacunarity(lacunarity);
		fbmState.SetFractalGain(persistence);

		stateDirty = false;
	}
}
