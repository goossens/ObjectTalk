//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "OtWaterComponent.h"
#include "OtComponentObject.h"


//
//	OtWaterComponentObjectClass
//

class OtWaterComponentObjectClass;
using OtWaterComponentObject = OtObjectPointer<OtWaterComponentObjectClass>;

class OtWaterComponentObjectClass : public OtComponentObjectClass {
public:
	// access properties
	declareProperty(float, level, Level)
	declareProperty(float, distance, Distance)
	declareProperty(bool, useRefractance, UseRefractance)
	declareAssetProperty(dudv, DUDV)
	declareAssetProperty(normals, Normals)
	declareProperty(float, scale, Scale)
	declareProperty(float, speed, Speed)
	declareProperty(float, randomFactor, RandomFactor)
	declareProperty(float, waveStrength, WaveStrength)

	declareProperty(float, metallic, Metallic)
	declareProperty(float, roughness, Roughness)
	declareProperty(float, ao, Ao)

	declareProperty(float, reflectivity, Reflectivity)
	declareProperty(float, depthFactor, DepthFactor)

	// get type definition
	static inline OtType getMeta()  {
		static OtType type = nullptr;

		if (!type) {
			type = OtType::create<OtWaterComponentObjectClass>("WaterComponent", OtComponentObjectClass::getMeta());
			metaProperty(Water, Level)
			metaProperty(Water, Distance)
			metaProperty(Water, UseRefractance)
			metaProperty(Water, DUDV)
			metaProperty(Water, Normals)
			metaProperty(Water, Scale)
			metaProperty(Water, Speed)
			metaProperty(Water, RandomFactor)
			metaProperty(Water, WaveStrength)

			metaProperty(Water, Metallic)
			metaProperty(Water, Roughness)
			metaProperty(Water, Ao)

			metaProperty(Water, Reflectivity)
			metaProperty(Water, DepthFactor)
		}

		return type;
	}

protected:
	// constructor
	friend class OtObjectPointer<OtWaterComponentObjectClass>;
	OtWaterComponentObjectClass() = default;
	OtWaterComponentObjectClass(OtScene* scene, OtEntity entity) { setEcsReferences(scene, entity); }

private:
	// get component reference
	inline OtWaterComponent& component() { return OtComponentObjectClass::component<OtWaterComponent>(scene, entity); }
};
