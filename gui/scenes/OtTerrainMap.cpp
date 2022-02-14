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

#include "OtTerrainMap.h"


//
//	OtTerrainMapClass::OtTerrainMapClass
//

OtTerrainMapClass::OtTerrainMapClass() {
	perlin = OtPerlinClass::create(currentSeed);
}


//
//	OtTerrainMapClass::init
//

OtObject OtTerrainMapClass::init(size_t count, OtObject* parameters) {
	if (count == 1) {
		setSeed(parameters[0]->operator int());

	} else if (count != 0) {
		OtExcept("[TerrainMap] constructor expects 0 or 1 arguments (not %ld)", count);
	}

	return nullptr;
}


//
//	OtTerrainMapClass::setSeed
//

OtObject OtTerrainMapClass::setSeed(int seed) {
	currentSeed = seed;
	perlin->seed(seed);
	propertiesChanged();
	return shared();
}


//
//	OtTerrainMapClass::setOctaves
//

OtObject OtTerrainMapClass::setOctaves(int o) {
	octaves = o;
	propertiesChanged();
	return shared();
}


//
//	OtTerrainMapClass::setPersistence
//

OtObject OtTerrainMapClass::setPersistence(float p) {
	persistence = p;
	propertiesChanged();
	return shared();
}


//
//	OtTerrainMapClass::setEasing
//

OtObject OtTerrainMapClass::setEasing(int e) {
	easing = e;
	easingFunction = OtEasingGetFunction(easing);
	propertiesChanged();
	return shared();
}


//
//	OtTerrainMapClass::setScaleXY
//

OtObject OtTerrainMapClass::setScaleXY(float xy) {
	scaleXY = xy;
	propertiesChanged();
	return shared();
}


//
//	OtTerrainMapClass::setScaleZ
//

OtObject OtTerrainMapClass::setScaleZ(float z) {
	scaleZ = z;
	propertiesChanged();
	return shared();
}


//
//	OtTerrainMapClass::setOffsetZ
//

OtObject OtTerrainMapClass::setOffsetZ(float z) {
	offsetZ = z;
	propertiesChanged();
	return shared();
}


//
//	OtTerrainMapClass::getMinHeight
//

float OtTerrainMapClass::getMinHeight() {
	return offsetZ;
}


//
//	OtTerrainMapClass::getMaxHeight
//

float OtTerrainMapClass::getMaxHeight() {
	return scaleZ + offsetZ;
}


//
//	OtTerrainMapClass::getHeight
//

float OtTerrainMapClass::getHeight(int32_t x, int32_t y) {
	// clear cache if dirty
	if (cacheDirty) {
		cache.clear();
		cacheDirty = false;
	}

	// statistics
	requests++;

	// check cache first
	uint64_t h = hash(x, y);

	if (cache.has(h)) {
		cachehits++;
		return cache.get(h);

	} else {
		float height = easingFunction(perlin->octaveNoise(
			(float) x / scaleXY, (float) y / scaleXY, 0.5,
			octaves, persistence)) * scaleZ + offsetZ;

		cache.set(h, height);
		return height;
	}
}


//
//	OtTerrainMapClass::getNormal
//

glm::vec3 OtTerrainMapClass::getNormal(int32_t x, int32_t y) {
	auto h1 = getHeight(x, y);
	auto h2 = getHeight(x + 1, y);
	auto h3 = getHeight(x, y - 1);
	return glm::normalize(glm::vec3(h1 - h2, 1.0, h1 - h3));
}


//
//	OtTerrainMapClass::renderGUI
//

void OtTerrainMapClass::renderGUI() {
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
		propertiesChanged();
	}

	if (ImGui::TreeNodeEx("Debug", ImGuiTreeNodeFlags_Framed)) {
		ImGui::Text("Cache size: %ld", cache.getNumberOfEntries());
		ImGui::Text("Cache hits: %.1f%%", (double) cachehits / (double) requests * 100.0);
		ImGui::TreePop();
	}
}


//
//	OtTerrainMapClass::propertiesChanged
//

void OtTerrainMapClass::propertiesChanged() {
	// mark the cache as dirty and notify subscribers
	cacheDirty = true;
	notify();
}


//
//	OtTerrainMapClass::getMeta
//

OtType OtTerrainMapClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtTypeClass::create<OtTerrainMapClass>("TerrainMap", OtGuiClass::getMeta());
		type->set("__init__", OtFunctionClass::create(&OtTerrainMapClass::init));

		type->set("setSeed", OtFunctionClass::create(&OtTerrainMapClass::setSeed));
		type->set("setOctaves", OtFunctionClass::create(&OtTerrainMapClass::setOctaves));
		type->set("setPersistence", OtFunctionClass::create(&OtTerrainMapClass::setPersistence));
		type->set("setEasing", OtFunctionClass::create(&OtTerrainMapClass::setEasing));
		type->set("setScaleXY", OtFunctionClass::create(&OtTerrainMapClass::setScaleXY));
		type->set("setScaleZ", OtFunctionClass::create(&OtTerrainMapClass::setScaleZ));
		type->set("setOffsetZ", OtFunctionClass::create(&OtTerrainMapClass::setOffsetZ));

		type->set("getHeight", OtFunctionClass::create(&OtTerrainMapClass::getHeight));
	}

	return type;
}


//
//	OtTerrainMapClass::create
//

OtTerrainMap OtTerrainMapClass::create() {
	OtTerrainMap terrainmap = std::make_shared<OtTerrainMapClass>();
	terrainmap->setType(getMeta());
	return terrainmap;
}
