//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "OtComponentObject.h"
#include "OtTerrainComponent.h"


//
//	Helper macros
//

#define declareTerrainProperty(T, N)												\
	inline void set##N(T value) { component().terrain->set##N(value); }				\
	inline T get##N() { return component().terrain->get##N(); }

//
//	OtTerrainComponentObjectClass
//

class OtTerrainComponentObjectClass;
using OtTerrainComponentObject = OtObjectPointer<OtTerrainComponentObjectClass>;

class OtTerrainComponentObjectClass : public OtComponentObjectClass {
public:
	// access the properties
	declareTerrainProperty(int, TileSize)
	declareTerrainProperty(int, LOD)
	declareTerrainProperty(float, HorizontalScale)
	declareTerrainProperty(float, VerticalScale)
	declareTerrainProperty(float, VerticalOffset)
	declareTerrainProperty(bool, CastShadow)
	declareTerrainProperty(bool, Wireframe)

	// get type definition
	static inline OtType getMeta() {
		static OtType type = nullptr;

		if (!type) {
			type = OtType::create<OtTerrainComponentObjectClass>("TerrainComponent", OtComponentObjectClass::getMeta());

			metaProperty(Terrain, TileSize)
			metaProperty(Terrain, LOD)
			metaProperty(Terrain, HorizontalScale)
			metaProperty(Terrain, VerticalScale)
			metaProperty(Terrain, VerticalOffset)
			metaProperty(Terrain, CastShadow)
			metaProperty(Terrain, Wireframe)
		}

		return type;
	}

protected:
	// constructor
	friend class OtObjectPointer<OtTerrainComponentObjectClass>;
	OtTerrainComponentObjectClass() = default;
	OtTerrainComponentObjectClass(OtScene* scene, OtEntity entity) { setEcsReferences(scene, entity); }

private:
	// get component reference
	inline OtTerrainComponent& component() { return OtComponentObjectClass::component<OtTerrainComponent>(scene, entity); }
};
