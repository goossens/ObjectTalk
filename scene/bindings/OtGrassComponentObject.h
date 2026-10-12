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
#include "OtGrassComponent.h"


//
//	Helper macros
//

#define declareGrassProperty(T, P, N)										\
	inline void set##N(T value) { component().grass->P = value; }			\
	inline T get##N() { return component().grass->P; }

#define declareGrassVectorProperty(D, P, N)									\
	inline void set##N(size_t count, OtObject* parameters) {				\
		component().grass->P = toVec##D(count, parameters);					\
	}																		\
																			\
	inline glm::vec##D get##N() { return component().grass->P; }


//
//	OtGrassComponentObjectClass
//

class OtGrassComponentObjectClass;
using OtGrassComponentObject = OtObjectPointer<OtGrassComponentObjectClass>;

class OtGrassComponentObjectClass : public OtComponentObjectClass {
public:
	// access the properties
	declareGrassProperty(float, patchWidth, PatchWidth)
	declareGrassProperty(float, patchDepth, PatchDepth)
	declareGrassProperty(int, blades, Blades)

	declareGrassProperty(int, bladeSegments, BladeSegments)
	declareGrassProperty(float, bladeWidth, BladeWidth)
	declareGrassProperty(float, bladeHeight, BladeHeight)
	declareGrassProperty(float, bladePointiness, BladePointiness)
	declareGrassProperty(float, bladeCurve, BladeCurve)

	declareGrassProperty(float, windDirection, WindDirection)
	declareGrassProperty(float, windStrength, WindStrength)

	declareGrassVectorProperty(3, baseColor, BaseColor)
	declareGrassVectorProperty(3, tipColor, TipColor)

	declareGrassProperty(float, widthVariation, WidthVariation)
	declareGrassProperty(float, heightVariation, HeightVariation)
	declareGrassProperty(float, windVariation, WindVariation)
	declareGrassProperty(float, colorVariation, ColorVariation)

	declareGrassProperty(bool, castShadow, CastShadow)

	// get type definition
	static inline OtType getMeta() {
		static OtType type = nullptr;

		if (!type) {
			type = OtType::create<OtGrassComponentObjectClass>("GrassComponent", OtComponentObjectClass::getMeta());
			metaProperty(Grass, PatchWidth)
			metaProperty(Grass, PatchDepth)
			metaProperty(Grass, Blades)

			metaProperty(Grass, BladeSegments)
			metaProperty(Grass, BladeWidth)
			metaProperty(Grass, BladeHeight)
			metaProperty(Grass, BladePointiness)
			metaProperty(Grass, BladeCurve)

			metaProperty(Grass, WindDirection)
			metaProperty(Grass, WindStrength)

			metaProperty(Grass, BaseColor)
			metaProperty(Grass, TipColor)

			metaProperty(Grass, WidthVariation)
			metaProperty(Grass, HeightVariation)
			metaProperty(Grass, WindVariation)
			metaProperty(Grass, ColorVariation)

			metaProperty(Grass, CastShadow)
		}

		return type;
	}

protected:
	// constructor
	friend class OtObjectPointer<OtGrassComponentObjectClass>;
	OtGrassComponentObjectClass() = default;
	OtGrassComponentObjectClass(OtScene* scene, OtEntity entity) { setEcsReferences(scene, entity); }

private:
	// get component reference
	inline OtGrassComponent& component() { return OtComponentObjectClass::component<OtGrassComponent>(scene, entity); }
};
