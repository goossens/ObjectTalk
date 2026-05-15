//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtClass.h"
#include "OtFunction.h"
#include "OtModule.h"
#include "OtObject.h"

#include "OtMathModule.h"

#include "OtWorldGenerator.h"
#include "OtHeightMap.h"


//
//	OtWorldGeneratorClass
//

class OtHeightMapClass;
using OtHeightMapObject = OtObjectPointer<OtHeightMapClass>;

class OtHeightMapClass : public OtObjectClass {
public:
	// update dimensions
	inline void update(int w, int h, bool clear) { heightmap.update(w, h, clear); }

	// reset heightmap
	inline void clear() { heightmap.clear(); }

	// load/save heightmap
	inline void load(const std::string& path) { heightmap.load(path); }
	inline void save(const std::string& path) { heightmap.save(path); }

	// access height/normal data
	inline void setHeight(int x, int y, float value) { heightmap.setHeight(x, y, value); }
	inline float getHeight(int x, int y) { return heightmap.getHeight(x, y); }
	inline float sampleHeight(float x, float y) { return heightmap.sampleHeight(x, y); }
	inline glm::vec3 sampleNormal(float x, float y) { return heightmap.sampleNormal(x, y); }

	// get heightmap limits
	inline float getMinHeight() { return heightmap.getMinHeight(); }
	inline float getMaxHeight() { return heightmap.getMaxHeight(); }

	// get type definition
	static OtType getMeta() {
		static OtType type;

		if (!type) {
			type = OtType::create<OtHeightMapClass>("HeightMap", OtObjectClass::getMeta());
			type->set("update", OtFunction::create(&OtHeightMapClass::update));
			type->set("clear", OtFunction::create(&OtHeightMapClass::clear));

			type->set("load", OtFunction::create(&OtHeightMapClass::load));
			type->set("save", OtFunction::create(&OtHeightMapClass::save));

			type->set("setHeight", OtFunction::create(&OtHeightMapClass::setHeight));
			type->set("getHeight", OtFunction::create(&OtHeightMapClass::getHeight));

			type->set("sampleHeight", OtFunction::create(&OtHeightMapClass::sampleHeight));
			type->set("sampleNormal", OtFunction::create(&OtHeightMapClass::sampleNormal));

			type->set("getMinHeight", OtFunction::create(&OtHeightMapClass::getMinHeight));
			type->set("getMinHeight", OtFunction::create(&OtHeightMapClass::getMaxHeight));
		}

		return type;
	}

private:
	friend class OtWorldGeneratorClass;
	OtHeightMap heightmap;
};


//
//	OtWorldGeneratorClass
//

class OtWorldGeneratorClass;
using OtWorldGeneratorObject = OtObjectPointer<OtWorldGeneratorClass>;

class OtWorldGeneratorClass : public OtObjectClass {
public:
	// methods
	inline void load(const std::string filename) { world.load(filename); }
	inline void save(const std::string filename) { world.save(filename); }
	inline void clear() { world.clear(); }

	// set properties
	inline OtObject setSize(int value) { world.setSize(value); return OtWorldGeneratorObject(this); }
	inline OtObject setSeed(int value) { world.setSeed(value); return OtWorldGeneratorObject(this); }
	inline OtObject setRuggedness(float value) { world.setRuggedness(value); return OtWorldGeneratorObject(this); }

	// generate world
	inline void generate() { world.generate(); }

	// generate a heightmap
	inline OtObject generateHeightMap(int size) {
		auto heightmap = OtHeightMapObject::create();
		world.generateHeightMap(heightmap->heightmap, size);
		return heightmap;
	}

	// get type definition
	static OtType getMeta() {
		static OtType type;

		if (!type) {
			type = OtType::create<OtWorldGeneratorClass>("World", OtObjectClass::getMeta());

			type->set("load", OtFunction::create(&OtWorldGeneratorClass::load));
			type->set("save", OtFunction::create(&OtWorldGeneratorClass::save));
			type->set("clear", OtFunction::create(&OtWorldGeneratorClass::clear));

			type->set("setSize", OtFunction::create(&OtWorldGeneratorClass::setSize));
			type->set("setSeed", OtFunction::create(&OtWorldGeneratorClass::setSeed));
			type->set("setRuggedness", OtFunction::create(&OtWorldGeneratorClass::setRuggedness));

			type->set("generate", OtFunction::create(&OtWorldGeneratorClass::generate));
			type->set("generateHeightMap", OtFunction::create(&OtWorldGeneratorClass::generateHeightMap));
		}

		return type;
	}

private:
	OtWorldGenerator world;
};


//
//	Module registration
//

static OtModuleRegistration registration{"world", [](OtModule module) {
	// register classes
	module->set("Generator", OtClass::create(OtWorldGeneratorClass::getMeta()));
	module->set("HeightMap", OtClass::create(OtHeightMapClass::getMeta()));
}};
