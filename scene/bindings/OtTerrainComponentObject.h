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

#define declareTerrainProperty(T, N)														\
	inline void set##N(T value) { component().terrain->set##N(value); }						\
	inline T get##N() { return component().terrain->get##N(); }

#define declareHeightsProperty(T, N)														\
	inline void set##N(T value) { component().terrain->getHeights().set##N(value); }		\
	inline T get##N() { return component().terrain->getHeights().get##N(); }

#define declareHeightsVectorProperty(D, N)													\
	inline void set##N(size_t count, OtObject* parameters) {								\
		component().terrain->getHeights().set##N(toVec##D(count, parameters));				\
	}																						\
																							\
	inline glm::vec##D get##N() { return component().terrain->getHeights().get##N(); }

#define declareHeightsMaterialProperty(T, P, N)												\
	inline void set##N(T value) { component().terrain->getMaterial().P = value; }			\
	inline T get##N() { return component().terrain->getMaterial().P; }

#define declareMaterialAssetProperty(P, N)													\
	inline void set##N(const std::string& value) { 											\
		component().terrain->getMaterial().P = value;										\
	}																						\
																							\
	inline std::string get##N() { return component().terrain->getMaterial().P.getPath(); }

#define declareHeightsMaterialVectorProperty(D, P, N)										\
	inline void set##N(size_t count, OtObject* parameters) {								\
		component().terrain->getMaterial().P = toVec##D(count, parameters);					\
	}																						\
																							\
	inline glm::vec##D get##N() { return component().terrain->getMaterial().P; }


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

	declareHeightsProperty(int, MapSize)
	declareHeightsProperty(float, MapDensity)
	declareHeightsProperty(float, NormalStrength)

	declareHeightsProperty(int, NoiseFrequency)
	declareHeightsProperty(int, NoiseLacunarity)
	declareHeightsProperty(float, NoiseAmplitude)
	declareHeightsProperty(float, NoisePersistence)
	declareHeightsProperty(int, NoiseOctaves)

	declareHeightsVectorProperty(4, ErosionRounding)
	declareHeightsVectorProperty(4, ErosionOnset)
	declareHeightsVectorProperty(2, ErosionAssumedSlope)
	declareHeightsProperty(float, ErosionScale)
	declareHeightsProperty(float, ErosionStrength)
	declareHeightsProperty(float, ErosionGullyWeight)
	declareHeightsProperty(float, ErosionDetail)
	declareHeightsProperty(float, ErosionCellScale)
	declareHeightsProperty(float, ErosionGain)
	declareHeightsProperty(float, ErosionLacunarity)
	declareHeightsProperty(float, ErosionNormalization)
	declareHeightsProperty(int, ErosionOctaves)

	declareHeightsMaterialVectorProperty(3, region1Color, Region1Color)
	declareHeightsMaterialVectorProperty(3, region2Color, Region2Color)
	declareHeightsMaterialVectorProperty(3, region3Color, Region3Color)
	declareHeightsMaterialVectorProperty(3, region4Color, Region4Color)

	declareMaterialAssetProperty(region1Texture, Region1Texture)
	declareMaterialAssetProperty(region2Texture, Region2Texture)
	declareMaterialAssetProperty(region3Texture, Region3Texture)
	declareMaterialAssetProperty(region4Texture, Region4Texture)

	declareHeightsMaterialProperty(float, region1TextureScale, Region1TextureScale)
	declareHeightsMaterialProperty(float, region2TextureScale, Region2TextureScale)
	declareHeightsMaterialProperty(float, region3TextureScale, Region3TextureScale)
	declareHeightsMaterialProperty(float, region4TextureScale, Region4TextureScale)

	declareHeightsMaterialProperty(float, region1Transition, Region1Transition)
	declareHeightsMaterialProperty(float, region2Transition, Region2Transition)
	declareHeightsMaterialProperty(float, region3Transition, Region3Transition)

	declareHeightsMaterialProperty(float, region1Overlap, Region1Overlap)
	declareHeightsMaterialProperty(float, region2Overlap, Region2Overlap)
	declareHeightsMaterialProperty(float, region3Overlap, Region3Overlap)

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

			metaProperty(Terrain, MapSize)
			metaProperty(Terrain, MapDensity)
			metaProperty(Terrain, NormalStrength)

			metaProperty(Terrain, NoiseFrequency)
			metaProperty(Terrain, NoiseLacunarity)
			metaProperty(Terrain, NoiseAmplitude)
			metaProperty(Terrain, NoisePersistence)
			metaProperty(Terrain, NoiseOctaves)

			metaProperty(Terrain, ErosionRounding)
			metaProperty(Terrain, ErosionOnset)
			metaProperty(Terrain, ErosionAssumedSlope)
			metaProperty(Terrain, ErosionScale)
			metaProperty(Terrain, ErosionStrength)
			metaProperty(Terrain, ErosionGullyWeight)
			metaProperty(Terrain, ErosionDetail)
			metaProperty(Terrain, ErosionCellScale)
			metaProperty(Terrain, ErosionGain)
			metaProperty(Terrain, ErosionLacunarity)
			metaProperty(Terrain, ErosionNormalization)
			metaProperty(Terrain, ErosionOctaves)

			metaProperty(Terrain, Region1Color)
			metaProperty(Terrain, Region2Color)
			metaProperty(Terrain, Region3Color)
			metaProperty(Terrain, Region4Color)

			metaProperty(Terrain, Region1Texture)
			metaProperty(Terrain, Region2Texture)
			metaProperty(Terrain, Region3Texture)
			metaProperty(Terrain, Region4Texture)

			metaProperty(Terrain, Region1TextureScale)
			metaProperty(Terrain, Region2TextureScale)
			metaProperty(Terrain, Region3TextureScale)
			metaProperty(Terrain, Region4TextureScale)

			metaProperty(Terrain, Region1Transition)
			metaProperty(Terrain, Region2Transition)
			metaProperty(Terrain, Region3Transition)

			metaProperty(Terrain, Region1Overlap)
			metaProperty(Terrain, Region2Overlap)
			metaProperty(Terrain, Region3Overlap)
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
