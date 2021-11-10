//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

# include <vector>

#include "controller.h"
#include "gui.h"


//
//	OtNoiseMapClass
//

class OtNoiseMapClass;
typedef std::shared_ptr<OtNoiseMapClass> OtNoiseMap;

class OtNoiseMapClass : public OtGuiClass {
public:
	// constructor/destructor
	OtNoiseMapClass();
	~OtNoiseMapClass();

	// initialize
	OtObject init(size_t count, OtObject* parameters);

	// generate a new permutation vector based on the value of seed
	void seed(int seed);

	// set noisemap size
	OtObject setSize(size_t width, size_t height);

	// set the noisemap scale factor
	OtObject setScale(float scale);

	// set noisemap offset
	OtObject setOffset(int x, int y);

	// get noisemap data
	size_t getWidth() { return width; }
	size_t getHeight() { return height; }
	float* getNoise() { return noisemap; }

	// GUI to change properties
	void renderGUI();

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtNoiseMap create();

private:
	// generate noisemap from perlin noise
	void generate();

	// permutation vector
	std::vector<int> p;
	int currentSeed;

	// perlin support functions
	float noise(float x, float y, float z);
	float fade(float t);
	float lerp(float t, float a, float b);
	float invlerp(float v, float a, float b);
	float grad(int hash, float x, float y, float z);

	// noisemap properties
	int width = 256;
	int height = 256;

	float scale = 50.0;
	float offsetX = 0.0;
	float offsetY = 0.0;

	float* noisemap = nullptr;
};


//
//	Controller widget
//

OT_CONTROLLER(NoiseMap)
