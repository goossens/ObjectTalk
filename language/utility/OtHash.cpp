//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtHash.h"


//
//	Utility functions
//

static inline uint32_t hashBitRotate(uint32_t x, uint32_t k) {
  return (x << k) | (x >> (32 - k));
}

static inline void hashBitMix(uint32_t& a, uint32_t& b, uint32_t& c) {
	a -= c;
	a ^= hashBitRotate(c, 4);
	c += b;
	b -= a;
	b ^= hashBitRotate(a, 6);
	a += c;
	c -= b;
	c ^= hashBitRotate(b, 8);
	b += a;
	a -= c;
	a ^= hashBitRotate(c, 16);
	c += b;
	b -= a;
	b ^= hashBitRotate(a, 19);
	a += c;
	c -= b;
	c ^= hashBitRotate(b, 4);
	b += a;
}

static inline void hashBitFinal(uint32_t& a, uint32_t& b, uint32_t& c) {
	c ^= b;
	c -= hashBitRotate(b, 14);
	a ^= c;
	a -= hashBitRotate(c, 11);
	b ^= a;
	b -= hashBitRotate(a, 25);
	c ^= b;
	c -= hashBitRotate(b, 16);
	a ^= c;
	a -= hashBitRotate(c, 4);
	b ^= a;
	b -= hashBitRotate(a, 14);
	c ^= b;
	c -= hashBitRotate(b, 24);
}

static inline uint32_t hash(uint32_t kx) {
	uint32_t a, b, c;
	a = b = c = 0xdeadbeef + (1 << 2) + 13;

	a += kx;
	hashBitFinal(a, b, c);

	return c;
}

static inline uint32_t hash(uint32_t kx, uint32_t ky) {
	uint32_t a, b, c;
	a = b = c = 0xdeadbeef + (2 << 2) + 13;

	b += ky;
	a += kx;
	hashBitFinal(a, b, c);

	return c;
}

static inline uint32_t hash(uint32_t kx, uint32_t ky, uint32_t kz) {
	uint32_t a, b, c;
	a = b = c = 0xdeadbeef + (3 << 2) + 13;

	c += kz;
	b += ky;
	a += kx;
	hashBitFinal(a, b, c);

	return c;
}

static inline uint32_t hash(uint32_t kx, uint32_t ky, uint32_t kz, uint32_t kw) {
	uint32_t a, b, c;
	a = b = c = 0xdeadbeef + (4 << 2) + 13;

	a += kx;
	b += ky;
	c += kz;
	hashBitMix(a, b, c);

	a += kw;
	hashBitFinal(a, b, c);

	return c;
}

static inline float uint32ToFloat(uint32_t k) {
	return float(k) / float(0xFFFFFFFFu);
}


//
//	OtHash::toFloat
//

float OtHash::toFloat(uint32_t x) {
	return uint32ToFloat(hash(x));
}

float OtHash::toFloat(uint32_t x, uint32_t y) {
	return uint32ToFloat(hash(x, y));
}

float OtHash::toFloat(uint32_t x, uint32_t y, uint32_t z) {
	return uint32ToFloat(hash(x, y, z));
}

float OtHash::toFloat(uint32_t x, uint32_t y, uint32_t z, uint32_t w) {
	return uint32ToFloat(hash(x, y, z, w));
}