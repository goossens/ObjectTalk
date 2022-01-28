//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

# include <vector>

#include "OtMath.h"


//
//	OtPerlin
//

class OtPerlinClass;
typedef std::shared_ptr<OtPerlinClass> OtPerlin;

class OtPerlinClass : public OtMathClass {
public:
	// constructors
	OtPerlinClass() { generateSeedVector(); }
	OtPerlinClass(int seed) : seedValue(seed) { generateSeedVector(); }

	// initialize
	OtObject init(size_t count, OtObject* parameters);

	// use a new seed
	void seed(int seed) { seedValue = seed; generateSeedVector(); }

	// get perlin noise value
	float noise(float x, float y, float z);
	float octaveNoise(float x, float y, float z, int octaves, float persistence);

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtPerlin create();
	static OtPerlin create(int seed);

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
