//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <vector>

#include "OtController.h"
#include "OtGui.h"
#include "OtEasing.h"
#include "OtPerlin.h"


//
//	OtNoiseMapClass
//

class OtNoiseMapClass;
using OtNoiseMap = OtObjectPointer<OtNoiseMapClass>;

class OtNoiseMapClass : public OtGuiClass {
public:
	// initialize
	void init(size_t count, OtObject* parameters);

	// provide a new seed for the perlin noise generator
	OtObject setSeed(int seed);

	// set number of perlin octaves
	OtObject setOctaves(int octaves);

	// set perlin persistence
	OtObject setPersistence(float persistence);

	// set the noisemap scaling factors
	OtObject setScaleXY(float xy);

	// set the noisemap scaling factors
	OtObject setScaleZ(float z);

	// set vertical noisemap offset (after scaling)
	OtObject setOffsetZ(float z);

	// set the noisemap easing function
	OtObject setEasing(int easing);

	// get minimum noise value
	float getMinNoise();

	// get maximum noise value
	float getMaxNoise();

	// get noise value at location
	float getNoise(float x, float y);

	// create a noise array
	void getNoiseArray(float* output, size_t width, size_t height, float x, float y, bool normalize=false);

	// GUI to change properties
	void renderGUI();

	// get type definition
	static OtType getMeta();

private:
	// generate noisemap from perlin noise
	OtPerlin perlin;
	int currentSeed = 237;
	int octaves = 8;
	float persistence = 0.5;

	// noisemap properties
	float scaleXY = 200.0;
	float scaleZ = 1.0;
	float offsetZ = 0.0;

	int easing = 0;
	OtEasingFunction easingFunction = OtEasingGetFunction(0);
};


//
//	Controller widget
//

OT_CONTROLLER(NoiseMap)
