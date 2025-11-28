//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <unordered_map>
#include <vector>

#include "glm/glm.hpp"

#include "OtAsset.h"
#include "OtCamera.h"
#include "OtCascadedShadowMap.h"
#include "OtGrass.h"
#include "OtImageBasedLighting.h"
#include "OtMaterial.h"
#include "OtRenderPass.h"
#include "OtTextureAsset.h"

#include "OtGeometryRenderData.h"
#include "OtImageBasedLighting.h"
#include "OtModelRenderData.h"
#include "OtSampler.h"
#include "OtScene.h"
#include "OtTerrain.h"


//
//	OtSceneRendererContext
//

class OtSceneRendererContext {
public:
	// initialize context
	void initialize(OtScene* s, OtCamera c);

	// camera information
	OtCamera camera;
	OtCamera reflectionCamera;

	size_t cameraID;
	static inline size_t getMainCameraID() { return 0; }
	static inline size_t getReflectionCameraID() { return 1; }
	static inline size_t getShadowCameraID(size_t camera) { return 2 + camera; }

	// scene to render
	OtScene* scene;

	// rendering pass
	OtRenderPass* pass;

	// image base lighting
	OtImageBasedLighting ibl;

	// shadows
	OtCascadedShadowMap csm;

	// rendering flags
	bool hasImageBasedLighting;
	bool hasDirectionalLighting;
	bool hasPointLighting;
	bool hasOpaqueEntities;
	bool hasOpaqueGeometries;
	bool hasOpaqueModels;
	bool hasTerrainEntities;
	bool hasSkyEntities;
	bool hasTransparentGeometries;
	bool hasWaterEntities;
	bool hasGrassEntities;
	bool hasParticlesEntities;
	bool renderingShadow;

	// visible entity lists and render information
	std::vector<OtEntity> geometryEntities;
	std::vector<OtEntity> modelEntities;

	OtEntity iblEntity = OtEntityNull;
	OtEntity waterEntity = OtEntityNull;

	std::vector<OtEntity> opaqueGeometryEntities;
	std::vector<OtEntity> transparentGeometryEntities;
	std::unordered_map<OtEntity, OtGeometryRenderData> geometryRenderData;
	std::unordered_map<OtEntity, OtModelRenderData> modelRenderData;

	// directional light information
	glm::vec3 directionalLightDirection;
	glm::vec3 directionalLightColor;
	float directionalLightAmbient;
	bool renderDirectionalLight;
	bool castShadow;

	// properties
	OtSampler iblBrdfLutSampler{OtSampler::Filter::linear, OtSampler::Addressing::repeat};
	OtSampler iblIrradianceMapSampler{OtSampler::Filter::linear, OtSampler::Addressing::repeat};
	OtSampler iblEnvironmentMapSampler{OtSampler::Filter::linear, OtSampler::Addressing::repeat};

	OtSampler shadowMap0Sampler{OtSampler::Filter::nearest, OtSampler::Addressing::clamp};
	OtSampler shadowMap1Sampler{OtSampler::Filter::nearest, OtSampler::Addressing::clamp};
	OtSampler shadowMap2Sampler{OtSampler::Filter::nearest, OtSampler::Addressing::clamp};
	OtSampler shadowMap3Sampler{OtSampler::Filter::nearest, OtSampler::Addressing::clamp};

	OtSampler albedoSampler{OtSampler::Filter::linear, OtSampler::Addressing::repeat};
	OtSampler normalSampler{OtSampler::Filter::linear, OtSampler::Addressing::repeat};
	OtSampler metallicRoughnessSampler{OtSampler::Filter::linear, OtSampler::Addressing::repeat};
	OtSampler emissiveSampler{OtSampler::Filter::linear, OtSampler::Addressing::repeat};
	OtSampler aoSampler{OtSampler::Filter::linear, OtSampler::Addressing::repeat};

	OtSampler lightingAlbedoSampler{OtSampler::Filter::nearest, OtSampler::Addressing::clamp};
	OtSampler lightingNormalSampler{OtSampler::Filter::nearest, OtSampler::Addressing::clamp};
	OtSampler lightingPbrSampler{OtSampler::Filter::nearest, OtSampler::Addressing::clamp};
	OtSampler lightingEmissiveSampler{OtSampler::Filter::nearest, OtSampler::Addressing::clamp};
	OtSampler lightingDepthSampler{OtSampler::Filter::nearest, OtSampler::Addressing::clamp};

	OtSampler cubemapSampler{OtSampler::Filter::linear, OtSampler::Addressing::clamp};

	OtSampler waterNormalmapSampler{OtSampler::Filter::linear, OtSampler::Addressing::repeat};
	OtSampler reflectionSampler{OtSampler::Filter::linear, OtSampler::Addressing::clamp};

	OtSampler normalmapSampler{OtSampler::Filter::linear, OtSampler::Addressing::repeat};
	OtSampler region1Sampler{OtSampler::Filter::linear, OtSampler::Addressing::repeat};
	OtSampler region2Sampler{OtSampler::Filter::linear, OtSampler::Addressing::repeat};
	OtSampler region3Sampler{OtSampler::Filter::linear, OtSampler::Addressing::repeat};
	OtSampler region4Sampler{OtSampler::Filter::linear, OtSampler::Addressing::repeat};

	OtSampler particlesSampler{OtSampler::Filter::linear, OtSampler::Addressing::repeat};
	OtSampler highlightSampler{OtSampler::Filter::nearest, OtSampler::Addressing::clamp};

	// support functions
	void setLightingUniforms(size_t uniformSlot, size_t samplerSlot);
	void setShadowUniforms(size_t uniformSlot, size_t samplerSlot);
	void bindFragmentSampler(size_t slot, OtSampler& sampler, OtAsset<OtTextureAsset>& texture);
};
