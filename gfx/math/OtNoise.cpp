//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <algorithm>
#include <cstdint>
#include <numeric>

#include "OtNoise.h"


//
// Inspired by https://github.com/SRombauts/SimplexNoise/tree/master
//


//
//	Globals
//

static uint8_t perm[256] = {
	151, 160, 137, 91, 90, 15, 	131, 13, 201, 95, 96, 53, 194, 233, 7, 225,
	140, 36, 103, 30, 69, 142, 8, 99, 37, 240, 21, 10, 23, 190, 6, 148, 247,
	120, 234, 75, 0, 26, 197, 62, 94, 252, 219, 203, 117, 35, 11, 32, 57, 177,
	33, 88, 237, 149, 56, 87, 174, 20, 125, 136, 171, 168, 68, 175, 74, 165,
	71, 134, 139, 48, 27, 166, 77, 146, 158, 231, 83, 111, 229, 122, 60, 211,
	133, 230, 220, 105, 92, 41, 55, 46, 245, 40, 244, 102, 143, 54, 65, 25,
	63, 161, 1, 216, 80, 73, 209, 76, 132, 187, 208, 89, 18, 169, 200, 196,
	135, 130, 116, 188, 159, 86, 164, 100, 109, 198, 173, 186, 3, 64, 52, 217,
	226, 250, 124, 123, 5, 202, 38, 147, 118, 126, 255, 82, 85, 212, 207, 206,
	59, 227, 47, 16, 58, 17, 182, 189, 28, 42, 223, 183, 170, 213, 119, 248,
	152, 2, 44, 154, 163, 70, 221, 153, 101, 155, 167, 43, 172, 9, 129, 22, 39,
	253, 19, 98, 108, 110, 79, 113, 224, 232, 178, 185, 112, 104, 218, 246, 97,
	228, 251, 34, 242, 193, 238, 210, 144, 12, 191, 179, 162, 241, 81, 51, 145,
	235, 249, 14, 239, 107, 49, 192, 214, 31, 181, 199, 106, 157, 184, 84, 204,
	176, 115, 121, 50, 45, 127, 4, 150, 254, 138, 236, 205, 93, 222, 114, 67,
	29, 24, 72, 243, 141, 128, 195, 78, 66, 215, 61, 156, 180
};


//
//	hash
//

static inline uint8_t hash(int32_t i) {
	return perm[static_cast<uint8_t>(i)];
}


//
//	fastfloor
//

static inline int32_t fastfloor(float fp) {
	int32_t i = static_cast<int32_t>(fp);
	return (fp < i) ? (i - 1) : (i);
}


//
//	grad
//

static inline float grad(int32_t hash, float x, float y) {
	int32_t h = hash & 0x3F;
	float u = h < 4 ? x : y;
	float v = h < 4 ? y : x;
	return ((h & 1) ? -u : u) + ((h & 2) ? -2.0f * v : 2.0f * v);
}

static float grad(int32_t hash, float x, float y, float z) {
    int h = hash & 15;
    float u = h < 8 ? x : y;
    float v = h < 4 ? y : h == 12 || h == 14 ? x : z;
    return ((h & 1) ? -u : u) + ((h & 2) ? -v : v);
}


//
//	OtNoise::perlin
//

float OtNoise::perlin(float x, float y) {
	// noise contributions from the three corners
	float n0, n1, n2;

	// skewing/unskewing factors for 2D
	static constexpr float F2 = 0.366025403f;  // F2 = (sqrt(3) - 1) / 2
	static float G2 = 0.211324865f;  // G2 = (3 - sqrt(3)) / 6   = F2 / (1 + 2 * K)

	// skew the input space to determine which simplex cell we're in
	float s = (x + y) * F2;
	float xs = x + s;
	float ys = y + s;
	int32_t i = fastfloor(xs);
	int32_t j = fastfloor(ys);

	// unskew the cell origin back to (x,y) space
	float t = static_cast<float>(i + j) * G2;
	float X0 = i - t;
	float Y0 = j - t;
	float x0 = x - X0;
	float y0 = y - Y0;

	// determine which simplex (an equilateral triangle) we are in.
	int32_t i1, j1;

	if (x0 > y0) {
		// lower triangle, XY order: (0,0)->(1,0)->(1,1)
		i1 = 1;
		j1 = 0;

	} else {
		// upper triangle, YX order: (0,0)->(0,1)->(1,1)
		i1 = 0;
		j1 = 1;
	}

	// offsets for middle corner in (x,y) unskewed coords
	float x1 = x0 - i1 + G2;
	float y1 = y0 - j1 + G2;

	// offsets for last corner in (x,y) unskewed coords
	float x2 = x0 - 1.0f + 2.0f * G2;
	float y2 = y0 - 1.0f + 2.0f * G2;

	// work out the hashed gradient indices of the three simplex corners
	int gi0 = hash(i + hash(j));
	int gi1 = hash(i + i1 + hash(j + j1));
	int gi2 = hash(i + 1 + hash(j + 1));

	// calculate the contribution from the first corner

	float t0 = 0.5f - x0* x0 - y0 * y0;

	if (t0 < 0.0f) {
		n0 = 0.0f;

	} else {
		t0 *= t0;
		n0 = t0 * t0 * grad(gi0, x0, y0);
	}

	// calculate the contribution from the second corner
	float t1 = 0.5f - x1*x1 - y1*y1;

	if (t1 < 0.0f) {
		n1 = 0.0f;

	} else {
		t1 *= t1;
		n1 = t1 * t1 * grad(gi1, x1, y1);
	}

	// calculate the contribution from the third corner

	float t2 = 0.5f - x2*x2 - y2*y2;

	if (t2 < 0.0f) {
		n2 = 0.0f;

	} else {
		t2 *= t2;
		n2 = t2 * t2 * grad(gi2, x2, y2);
	}

	// add contributions from each corner to get the final noise value.
	// the result is scaled to return values in the interval [-1.0 ,1.0].
	return 45.23065f * (n0 + n1 + n2);
}


//
//	OtNoise::perlin
//

float OtNoise::perlin(float x, float y, float z) {
	// noise contributions from the four corners
	float n0, n1, n2, n3;

	// skewing/unskewing factors for 3D
	static float F3 = 1.0f / 3.0f;
	static float G3 = 1.0f / 6.0f;

	// Skew the input space to determine which simplex cell we're in
	float s = (x + y + z) * F3;
	int i = fastfloor(x + s);
	int j = fastfloor(y + s);
	int k = fastfloor(z + s);
	float t = (i + j + k) * G3;

	// Uunskew the cell origin back to (x,y,z) space
	float X0 = i - t;
	float Y0 = j - t;
	float Z0 = k - t;

	// the x,y,z distances from the cell origin
	float x0 = x - X0;
	float y0 = y - Y0;
	float z0 = z - Z0;

	// determine which simplex (slightly irregular tetrahedron) we are in
	int i1, j1, k1;
	int i2, j2, k2;

	if (x0 >= y0) {
		if (y0 >= z0) {
			// X Y Z order
			i1 = 1; j1 = 0; k1 = 0; i2 = 1; j2 = 1; k2 = 0;

		} else if (x0 >= z0) {
			// X Z Y order
			i1 = 1; j1 = 0; k1 = 0; i2 = 1; j2 = 0; k2 = 1;

		} else {
			// Z X Y order
			i1 = 0; j1 = 0; k1 = 1; i2 = 1; j2 = 0; k2 = 1;
		}

	} else {
		if (y0 < z0) {
			// Z Y X order
			i1 = 0; j1 = 0; k1 = 1; i2 = 0; j2 = 1; k2 = 1;

		} else if (x0 < z0) {
			// Y Z X order
			i1 = 0; j1 = 1; k1 = 0; i2 = 0; j2 = 1; k2 = 1;

		} else {
			// Y X Z order
			i1 = 0; j1 = 1; k1 = 0; i2 = 1; j2 = 1; k2 = 0;
		}
	}

	// offsets for second corner in (x,y,z) coords
	float x1 = x0 - i1 + G3;
	float y1 = y0 - j1 + G3;
	float z1 = z0 - k1 + G3;

	// offsets for third corner in (x,y,z) coords
	float x2 = x0 - i2 + 2.0f * G3;
	float y2 = y0 - j2 + 2.0f * G3;
	float z2 = z0 - k2 + 2.0f * G3;

	// offsets for last corner in (x,y,z) coords
	float x3 = x0 - 1.0f + 3.0f * G3;
	float y3 = y0 - 1.0f + 3.0f * G3;
	float z3 = z0 - 1.0f + 3.0f * G3;

	// work out the hashed gradient indices of the four simplex corners
	int gi0 = hash(i + hash(j + hash(k)));
	int gi1 = hash(i + i1 + hash(j + j1 + hash(k + k1)));
	int gi2 = hash(i + i2 + hash(j + j2 + hash(k + k2)));
	int gi3 = hash(i + 1 + hash(j + 1 + hash(k + 1)));

	// calculate the contribution from the four corners
	float t0 = 0.6f - x0 * x0 - y0 * y0 - z0 * z0;

	if (t0 < 0) {
		n0 = 0.0;

	} else {
		t0 *= t0;
		n0 = t0 * t0 * grad(gi0, x0, y0, z0);
	}

	float t1 = 0.6f - x1 * x1 - y1 * y1 - z1 * z1;

	if (t1 < 0) {
		n1 = 0.0;

	} else {
		t1 *= t1;
		n1 = t1 * t1 * grad(gi1, x1, y1, z1);
	}

	float t2 = 0.6f - x2 * x2 - y2 * y2 - z2 * z2;

	if (t2 < 0) {
		n2 = 0.0;

	} else {
		t2 *= t2;
		n2 = t2 * t2 * grad(gi2, x2, y2, z2);
	}

	float t3 = 0.6f - x3 * x3 - y3 * y3 - z3 * z3;

	if (t3 < 0) {
		n3 = 0.0;

	} else {
		t3 *= t3;
		n3 = t3 * t3 * grad(gi3, x3, y3, z3);
	}

	// add contributions from each corner to get the final noise value.
	// the result is scaled to stay just inside [-1.0, 1.0]
	return 32.0f * (n0 + n1 + n2 + n3);
}


//
//	OtNoise::fbm
//

float OtNoise::fbm(float x, float y) {
	float frequency = startFrequency;
	float amplitude = startAmplitude;
	float total = 0.0f;
	float denom = 0.0f;

	for (size_t i = 0; i < octaves; i++) {
		total += perlin(x * frequency, y * frequency) * amplitude;
		denom += amplitude;
		frequency *= lacunarity;
		amplitude *= persistence;
	}

	return total / denom;
}

float OtNoise::fbm(float x, float y, float z) {
	float frequency = startFrequency;
	float amplitude = startAmplitude;
	float total = 0.0f;
	float denom = 0.0f;

	for (size_t i = 0; i < octaves; i++) {
		total += perlin(x * frequency, y * frequency, z * frequency) * amplitude;
		denom += amplitude;
		frequency *= lacunarity;
		amplitude *= persistence;
	}

	return total / denom;
}
