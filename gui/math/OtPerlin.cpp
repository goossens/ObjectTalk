//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <algorithm>
#include <numeric>
#include <random>

#include "OtException.h"
#include "OtFunction.h"

#include "OtPerlin.h"


//
//	OtPerlin::generateSeedVector
//

void OtPerlin::generateSeedVector() {
	p.resize(256);
	std::iota(p.begin(), p.end(), 0);
	std::default_random_engine engine(seedValue);
	std::shuffle(p.begin(), p.end(), engine);
	p.insert(p.end(), p.begin(), p.end());
}


//
//	OtPerlin::noise
//

float OtPerlin::noise(float x, float y, float z) {
	int X = (int) floor(x) & 255;
	int Y = (int) floor(y) & 255;
	int Z = (int) floor(z) & 255;

	x -= floor(x);
	y -= floor(y);
	z -= floor(z);

	float u = fade(x);
	float v = fade(y);
	float w = fade(z);

	int A = p[X] + Y;
	int AA = p[A] + Z;
	int AB = p[A + 1] + Z;
	int B = p[X + 1] + Y;
	int BA = p[B] + Z;
	int BB = p[B + 1] + Z;

	float res = lerp(w,
		lerp(v,
			lerp(u, grad(p[AA], x, y, z), grad(p[BA], x - 1, y, z)),
			lerp(u, grad(p[AB], x, y - 1, z), grad(p[BB], x - 1, y - 1, z))),
		lerp(v,
			lerp(u, grad(p[AA + 1], x, y, z - 1), grad(p[BA + 1], x - 1, y, z - 1)),
			lerp(u, grad(p[AB + 1], x, y - 1, z - 1), grad(p[BB + 1], x - 1, y - 1, z - 1))));

	return (res + 1.0) / 2.0;
}


//
//	OtPerlin::fade
//

float OtPerlin::fade(float t) {
	return t * t * t * (t * (t * 6 - 15) + 10);
}


//
//	OtPerlin::lerp
//

float OtPerlin::lerp(float t, float a, float b) {
	return a + t * (b - a);
}


//
//	OtPerlin::grad
//

float OtPerlin::grad(int hash, float x, float y, float z) {
	int h = hash & 15;
	float u = h < 8 ? x : y, v = h < 4 ? y : h == 12 || h == 14 ? x : z;
	return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
}


//
//	OtPerlin::octaveNoise
//

float OtPerlin::octaveNoise(float x, float y, float z, int octaves, float persistence) {
	float total = 0.0;
	float frequency = 1.0;
	float amplitude = 1.0;
	float maxValue = 0.0;

	for (auto i = 0; i < octaves; i++) {
		total += noise(x * frequency, y * frequency, z * frequency) * amplitude;
		maxValue += amplitude;
		amplitude *= persistence;
		frequency *= 2.0;
	}

	return total / maxValue;
}
