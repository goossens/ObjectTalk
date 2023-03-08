//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

# include <vector>


//
//	OtPerlin
//

class OtPerlin {
public:
	// constructors
	OtPerlin() { generateSeedVector(); }
	OtPerlin(int seed) : seedValue(seed) { generateSeedVector(); }

	// use a new seed
	void seed(int seed) { seedValue = seed; generateSeedVector(); }

	// get perlin noise value
	float noise(float x, float y, float z);
	float octaveNoise(float x, float y, float z, int octaves, float persistence);

private:
	// seed properties
	void generateSeedVector();
	int seedValue = 237;
	std::vector<int> p;

	// perlin support functions
	float fade(float t);
	float lerp(float t, float a, float b);
	float invlerp(float v, float a, float b);
	float grad(int hash, float x, float y, float z);
};
