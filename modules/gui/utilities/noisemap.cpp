//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <algorithm>
#include <limits>
#include <random>

#include "bimg/bimg.h"

#include "ot/function.h"

#include "noisemap.h"


//
//	OtNoiseMapClass::OtNoiseMapClass
//

OtNoiseMapClass::OtNoiseMapClass() {
	seed(237);
	generate();
}


//
//	OtNoiseMapClass::~OtNoiseMapClass
//

OtNoiseMapClass::~OtNoiseMapClass() {
	delete [] noisemap;
}


//
//	OtNoiseMapClass::init
//

OtObject OtNoiseMapClass::init(size_t count, OtObject* parameters) {
	bool changed = false;

	switch (count) {
		case 3:
			setScale(parameters[2]->operator float());
			changed = true;
			break;

		case 1:
			setSize(parameters[0]->operator int(), parameters[1]->operator int());
			changed = true;
			break;

		case 0:
			break;

		default:
			OtExcept("[NoiseMap] constructor expects 0, 1 or 3 arguments (not %ld)", count);
	}

	if (changed) {
		generate();
	}

	return nullptr;
}


//
//	OtNoiseMapClass::seed
//

void OtNoiseMapClass::seed(int seed) {
	currentSeed = seed;

	p.resize(256);
	std::iota(p.begin(), p.end(), 0);
	std::default_random_engine engine(seed);
	std::shuffle(p.begin(), p.end(), engine);
	p.insert(p.end(), p.begin(), p.end());
	generate();
}


//
//	OtNoiseMapClass::setSize
//

OtObject OtNoiseMapClass::setSize(size_t w, size_t h) {
	if (w != width || h != height) {
		width = w;
		height = h;
		generate();
	}

	return shared();
}


//
//	OtNoiseMapClass::setScale
//

OtObject OtNoiseMapClass::setScale(float s) {
	if (s != scale) {
		scale = s;
		generate();
	}

	return shared();
}


// set noisemap offset
OtObject OtNoiseMapClass::setOffset(int x, int y) {
	if (x != offsetX || y != offsetY) {
		offsetX = x;
		offsetY = y;
		generate();
	}

	return shared();
}


//
//	OtNoiseMapClass::generate
//

void OtNoiseMapClass::generate() {
	// create new noisemap
	if (noisemap) {
		delete [] noisemap;
	}

	noisemap = new float[width * height];

	// track min/max value so we can normalize later
	float minNoiseHeight = std::numeric_limits<float>::max();
	float maxNoiseHeight = std::numeric_limits<float>::min();

	// fill noisemap
	for (auto x = 0; x < width; x++) {
		for (auto y = 0; y < height; y++) {
			float sampleX = x / scale + offsetX;
			float sampleY = y / scale + offsetY;
			float noiseHeight = noise(sampleX, sampleY, 0.5) * 2.0 - 1.0;

			noisemap[y * width + x] = noiseHeight;
			minNoiseHeight = std::min(minNoiseHeight, noiseHeight);
			maxNoiseHeight = std::max(maxNoiseHeight, noiseHeight);
		}
	}

	// normalize noisemap
	for (auto c = 0; c < width * height; c++) {
		noisemap[c] = invlerp(noisemap[c], minNoiseHeight, maxNoiseHeight);
	}

	// notify objects that depend on us
	notify();
}


//
//	OtNoiseMapClass::noise
//

float OtNoiseMapClass::noise(float x, float y, float z) {
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

	float res = lerp(w, lerp(v, lerp(u, grad(p[AA], x, y, z), grad(p[BA], x-1, y, z)), lerp(u, grad(p[AB], x, y-1, z), grad(p[BB], x-1, y-1, z))),	lerp(v, lerp(u, grad(p[AA+1], x, y, z-1), grad(p[BA+1], x-1, y, z-1)), lerp(u, grad(p[AB+1], x, y-1, z-1),	grad(p[BB+1], x-1, y-1, z-1))));
	return (res + 1.0) / 2.0;
}


//
//	OtNoiseMapClass::fad
//

float OtNoiseMapClass::fade(float t) {
	return t * t * t * (t * (t * 6 - 15) + 10);
}


//
//	OtNoiseMapClass::lerp
//

float OtNoiseMapClass::lerp(float t, float a, float b) {
	return a + t * (b - a);
}


//
//	OtNoiseMapClass::invlerp
//

float OtNoiseMapClass::invlerp(float v, float a, float b) {
	return (v - a) / (b - a);
}


//
//	OtNoiseMapClass::grad
//

float OtNoiseMapClass::grad(int hash, float x, float y, float z) {
	int h = hash & 15;
	float u = h < 8 ? x : y, v = h < 4 ? y : h == 12 || h == 14 ? x : z;
	return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
}


//
//	OtAmbientClass::renderGUI
//

void OtNoiseMapClass::renderGUI() {
	bool changed = false;

	if (ImGui::SliderInt("Width", &width, 16, 1024)) {
		changed = true;
	}

	if (ImGui::SliderInt("Height", &height, 16, 1024)) {
		changed = true;
	}

	if (ImGui::SliderInt("Seed", &currentSeed, 1, 1000)) {
		seed(currentSeed);
	}

	if (ImGui::SliderFloat("Scale", &scale, 2.0f, 100.0f)) {
		changed = true;
	}

	if (ImGui::SliderFloat("Offset X", &offsetX, -10.0f, 10.0f)) {
		changed = true;
	}

	if (ImGui::SliderFloat("Offset Y", &offsetY, -10.0f, 10.0f)) {
		changed = true;
	}

	if (changed) {
		generate();
	}
}


//
//	OtNoiseMapClass::getMeta
//

OtType OtNoiseMapClass::getMeta() {
	static OtType type = nullptr;

	if (!type) {
		type = OtTypeClass::create<OtNoiseMapClass>("NoiseMap", OtGuiClass::getMeta());
		type->set("__init__", OtFunctionClass::create(&OtNoiseMapClass::init));
		type->set("setSize", OtFunctionClass::create(&OtNoiseMapClass::setSize));
		type->set("setScale", OtFunctionClass::create(&OtNoiseMapClass::setScale));
		type->set("setOffset", OtFunctionClass::create(&OtNoiseMapClass::setOffset));
	}

	return type;
}


//
//	OtNoiseMapClass::create
//

OtNoiseMap OtNoiseMapClass::create() {
	OtNoiseMap noisemap = std::make_shared<OtNoiseMapClass>();
	noisemap->setType(getMeta());
	return noisemap;
}
