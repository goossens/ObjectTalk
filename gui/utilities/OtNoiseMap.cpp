//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <algorithm>
#include <limits>
#include <random>

#include "glm/glm.hpp"
#include "imgui.h"

#include "OtFunction.h"

#include "OtNoiseMap.h"


//
//	OtNoiseMapClass::OtNoiseMapClass
//

OtNoiseMapClass::OtNoiseMapClass() {
	perlin = OtPerlinClass::create(currentSeed);
}


//
//	OtNoiseMapClass::init
//

OtObject OtNoiseMapClass::init(size_t count, OtObject* parameters) {
	if (count == 1) {
		setSeed(parameters[0]->operator int());

	} else if (count != 0) {
		OtExcept("[NoiseMap] constructor expects 0 or 1 arguments (not %ld)", count);
	}

	return nullptr;
}


//
//	OtNoiseMapClass::setSeed
//

OtObject OtNoiseMapClass::setSeed(int seed) {
	currentSeed = seed;
	perlin->seed(seed);
	notify();
	return shared();
}


//
//	OtNoiseMapClass::setOctaves
//

OtObject OtNoiseMapClass::setOctaves(int o) {
	octaves = o;
	notify();
	return shared();
}


//
//	OtNoiseMapClass::setPersistence
//

OtObject OtNoiseMapClass::setPersistence(float p) {
	persistence = p;
	notify();
	return shared();
}


//
//	OtNoiseMapClass::setEasing
//

OtObject OtNoiseMapClass::setEasing(int e) {
	easing = e;
	easingFunction = OtEasingGetFunction(easing);
	notify();
	return shared();
}


//
//	OtNoiseMapClass::setScaleXY
//

OtObject OtNoiseMapClass::setScaleXY(float xy) {
	scaleXY = xy;
	notify();
	return shared();
}


//
//	OtNoiseMapClass::setScaleZ
//

OtObject OtNoiseMapClass::setScaleZ(float z) {
	scaleZ = z;
	notify();
	return shared();
}


//
//	OtNoiseMapClass::setOffsetZ
//

OtObject OtNoiseMapClass::setOffsetZ(float z) {
	offsetZ = z;
	notify();
	return shared();
}


//
//	OtNoiseMapClass::getMinNoise
//

float OtNoiseMapClass::getMinNoise() {
	return offsetZ;
}


//
//	OtNoiseMapClass::getMaxNoise
//

float OtNoiseMapClass::getMaxNoise() {
	return scaleZ + offsetZ;
}


//
//	OtNoiseMapClass::getNoise
//

float OtNoiseMapClass::getNoise(float x, float y) {
	return easingFunction(perlin->octaveNoise(
		x / scaleXY, y / scaleXY, 0.5,
		octaves, persistence)) * scaleZ + offsetZ;
}


//
//	OtNoiseMapClass::getNoiseArray
//

void OtNoiseMapClass::getNoiseArray(float* output, size_t width, size_t height, float x, float y, bool normalize) {
	// fill noisemap
	float* ptr = output;

	for (auto iy = 0; iy < height; iy++) {
		for (auto ix = 0; ix < width; ix++) {
			*ptr++ = easingFunction(perlin->octaveNoise(
				(x + ix) / scaleXY, (y + iy) / scaleXY, 0.5,
				octaves, persistence));
		}
	}

	// normalize noisemap
	if (normalize) {
		// determine limites
		float minNoiseHeight = std::numeric_limits<float>::max();
		float maxNoiseHeight = std::numeric_limits<float>::lowest();
		ptr = output;

		for (auto c = 0; c < width * height; c++, ptr++) {
			minNoiseHeight = std::min(minNoiseHeight, *ptr);
			maxNoiseHeight = std::max(maxNoiseHeight, *ptr);
		}

		// normalize values between 0.0 and 1.0
		ptr = output;

		for (auto c = 0; c < width * height; c++, ptr++) {
			*ptr = (*ptr - minNoiseHeight) / (maxNoiseHeight - minNoiseHeight);
		}
	}

	// apply scale and offset
	ptr = output;

	for (auto c = 0; c < width * height; c++, ptr++) {
		*ptr = *ptr * scaleZ + offsetZ;
	}
}


//
//	OtAmbientClass::renderGUI
//

void OtNoiseMapClass::renderGUI() {
	bool changed = false;

	if (ImGui::SliderInt("Seed", &currentSeed, 1, 1000)) {
		setSeed(currentSeed);
	}

	if (ImGui::SliderInt("Octaves", &octaves, 1, 12)) {
		changed = true;
	}

	if (ImGui::SliderFloat("Persistence", &persistence, 0.2f, 0.7f)) {
		changed = true;
	}

	if (ImGui::Easing("Easing", &easing)) {
		setEasing(easing);
		changed = true;
	}

	if (ImGui::SliderFloat("Scale XY", &scaleXY, 1.0f, 200.0f)) {
		changed = true;
	}

	if (ImGui::SliderFloat("Scale Z", &scaleZ, 1.0f, 400.0f)) {
		changed = true;
	}

	if (ImGui::SliderFloat("Offset Z", &offsetZ, -200.0f, 200.0f)) {
		changed = true;
	}

	if (changed) {
		notify();
	}
}


//
//	OtNoiseMapClass::getMeta
//

OtType OtNoiseMapClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtTypeClass::create<OtNoiseMapClass>("NoiseMap", OtGuiClass::getMeta());
		type->set("__init__", OtFunctionClass::create(&OtNoiseMapClass::init));
		type->set("setSeed", OtFunctionClass::create(&OtNoiseMapClass::setSeed));
		type->set("setOctaves", OtFunctionClass::create(&OtNoiseMapClass::setOctaves));
		type->set("setPersistence", OtFunctionClass::create(&OtNoiseMapClass::setPersistence));
		type->set("setEasing", OtFunctionClass::create(&OtNoiseMapClass::setEasing));
		type->set("setScaleXY", OtFunctionClass::create(&OtNoiseMapClass::setScaleXY));
		type->set("setScaleZ", OtFunctionClass::create(&OtNoiseMapClass::setScaleZ));
		type->set("setOffsetZ", OtFunctionClass::create(&OtNoiseMapClass::setOffsetZ));
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
