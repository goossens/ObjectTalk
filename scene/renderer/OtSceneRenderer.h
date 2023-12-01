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

#include "glm/glm.hpp"

#include "OtBoxGeometry.h"
#include "OtFrameBuffer.h"
#include "OtFrustum.h"
#include "OtGbuffer.h"
#include "OtIndexBuffer.h"
#include "OtNormalMapper.h"
#include "OtPass.h"
#include "OtSampler.h"
#include "OtShaderProgram.h"
#include "OtSphereGeometry.h"
#include "OtTileableFbm.h"
#include "OtUniformMat4.h"
#include "OtUniformVec4.h"

#include "OtEntity.h"
#include "OtMaterials.h"
#include "OtScene.h"


//
//	OtSceneRenderer
//

class OtSceneRenderer {
public:
	// set the properties
	void setCameraPosition(const glm::vec3 pos) { cameraPosition = pos; }
	void setViewMatrix(const glm::mat4& view) { viewMatrix = view; viewProjectionMatrix = projectionMatrix * viewMatrix; }
	void setProjectionMatrix(const glm::mat4& proj) { projectionMatrix = proj; viewProjectionMatrix = projectionMatrix * viewMatrix; }
	void setSize(int w, int h) { width = w; height = h; }
	void setGridScale(float gs) { gridScale = gs; }

	// render the specified scene
	int render(OtScene* scene, OtEntity selected=OtEntityNull);

private:
	// private data types
	struct VisibleEntity {
		VisibleEntity(OtEntity e) : entity(e) {}
		OtEntity entity;
		bool model = false;
		bool transparent = false;
		bool instanced = false;
	};

	// render passes
	void renderPreProcessingPass(OtScene* scene, OtEntity selected);
	// void renderShadowPass(OtScene* scene);
	void renderDeferredGeometryPass(OtScene* scene);
	void renderDeferredTerrainPass(OtScene* scene);
	void renderBackgroundPass(OtScene* scene);
	void renderSkyPass(OtScene* scene);
	void renderDeferredLightingPass(OtScene* scene);
	void renderForwardGeometryPass(OtScene* scene);
	void renderGridPass();
	void renderHighlightPass(OtScene* scene, OtEntity selected);
	void renderPostProcessingPass(OtScene* scene);

	// preprocess entities
	void preprocessSingleInstanceGeometry(OtScene* scene, OtEntity entity, bool selected);
	void preprocessMultipleInstanceGeometry(OtScene* scene, OtEntity entity, bool selected);
	void preprocessSingleInstanceModel(OtScene* scene, OtEntity entity, bool selected);
	void preprocessMultipleInstanceModel(OtScene* scene, OtEntity entity, bool selected);
	void preprocessTerrain(OtScene* scene, OtEntity entity);

	// render entities
	void renderSky(OtPass& pass, OtSkyComponent& component);
	void renderSkyBox(OtPass& pass, OtSkyBoxComponent& component);
	void renderSkySphere(OtPass& pass, OtSkySphereComponent& component);
	void renderDeferredGeometry(OtPass& pass, OtScene* scene, OtEntity entity);
	void renderDeferredModel(OtPass& pass, OtScene* scene, OtEntity entity);
	void renderDeferredTerrain(OtPass& pass, OtTerrainComponent& component);
	void renderForwardGeometry(OtPass& pass, OtScene* scene, OtEntity entity);
	void renderHighlight(OtPass& pass, OtScene* scene, OtEntity entity);
	void renderBloom(float bloomIntensity);

	// rendering tools
	void submitSampler(OtSampler& sampler, int unit, OtAsset<OtTextureAsset>& texture);
	void submitMaterialUniforms(OtMaterial material);
	void submitPbrUniforms(OtPbrMaterial material);
	void submitLightUniforms(OtScene* scene);
	void submitTerrainUniforms(OtTerrain terrain);

	// camera information
	glm::vec3 cameraPosition;
	glm::mat4 viewMatrix;
	glm::mat4 projectionMatrix;
	glm::mat4 viewProjectionMatrix;
	OtFrustum frustum;

	// image dimensions
	int width;
	int height;

	// grid scale (0.0 means no grid)
	float gridScale = 0.0f;

	// framebuffers
	OtGbuffer gbuffer;
	OtFrameBuffer compositeBuffer{OtFrameBuffer::rgbaFloat16Texture, OtFrameBuffer::dFloatTexture, 1, true};
	OtFrameBuffer postProcessBuffer{OtFrameBuffer::rgba8Texture, OtFrameBuffer::noTexture};
	OtFrameBuffer selectedBuffer{OtFrameBuffer::r8Texture, OtFrameBuffer::noTexture};

	static constexpr int bloomDepth = 5;
	OtFrameBuffer bloomBuffer[bloomDepth];

	// standard geometries
	OtBoxGeometry unityBoxGeometry;
	OtSphereGeometry unitySphereGeometry;

	// visible entities
	std::vector<VisibleEntity> visibleEntities;
	bool hasOpaqueEntities = false;
	bool hasTransparentEntities = false;
	bool hasSkyEntities = false;
	bool hasTerrainEntities = false;
	bool renderEntityHighlight = false;

	// generators/filters
	OtTileableFbm tileableFbm;
	OtNormalMapper normalMapper;

	// uniforms
	OtUniformVec4 terrainUniforms{"u_terrain", 7};
	OtUniformVec4 pbrMaterialUniforms{"u_pbrMaterial", 5};
	OtUniformVec4 lightingUniforms{"u_lighting", 3};
	OtUniformVec4 skyUniforms{"u_sky", 3};
	OtUniformVec4 gridUniforms{"u_grid", 1};
	OtUniformVec4 outlineUniforms{"u_outline", 1};
	OtUniformVec4 bloomUniforms{"u_bloom", 1};
	OtUniformVec4 postProcessUniforms{"u_postProcess", 1};
	OtUniformMat4 inverseTransformUniforms{"u_inverseTransform", 1};

	// samplers
	OtSampler deferredGeometryAlbedoSampler{"s_deferredGeometryAlbedoTexture"};
	OtSampler deferredGeometryNormalSampler{"s_deferredGeometryNormalTexture"};
	OtSampler deferredGeometryMetallicRoughnessSampler{"s_deferredGeometryMetallicRoughnessTexture"};
	OtSampler deferredGeometryEmissiveSampler{"s_deferredGeometryEmissiveSampler"};
	OtSampler deferredGeometryAoSampler{"s_deferredGeometryAoTexture"};

	OtSampler deferredLightingAlbedoSampler{"s_deferredLightingAlbedoTexture", OtSampler::pointSampling | OtSampler::clampSampling};
	OtSampler deferredLightingPositionSampler{"s_deferredLightingPositionTexture", OtSampler::pointSampling | OtSampler::clampSampling};
	OtSampler deferredLightingNormalSampler{"s_deferredLightingNormalTexture", OtSampler::pointSampling | OtSampler::clampSampling};
	OtSampler deferredLightingPbrSampler{"s_deferredLightingPbrTexture", OtSampler::pointSampling | OtSampler::clampSampling};
	OtSampler deferredLightingEmissiveSampler{"s_deferredLightingEmissiveTexture", OtSampler::pointSampling | OtSampler::clampSampling};
	OtSampler deferredLightingDepthSampler{"s_deferredLightingDepthTexture", OtSampler::pointSampling | OtSampler::clampSampling};

	OtSampler region1Sampler{"s_region1Sampler"};
	OtSampler region2Sampler{"s_region2Sampler"};
	OtSampler region3Sampler{"s_region3Sampler"};
	OtSampler region4Sampler{"s_region4Sampler"};

	OtSampler normalmapSampler{"s_normalmapSampler"};
	OtSampler skySampler{"s_skySampler"};

	OtSampler selectedSampler{"s_selectedTexture", OtSampler::pointSampling | OtSampler::clampSampling};

	OtSampler postProcessSampler{"s_postProcessTexture", OtSampler::pointSampling | OtSampler::clampSampling};
	OtSampler bloomSampler{"s_bloomTexture", OtSampler::pointSampling | OtSampler::clampSampling};

	// shader programs
	OtShaderProgram deferredPbrProgram{"OtDeferredVS", "OtDeferredPbrFS"};
	OtShaderProgram deferredInstancingProgram{"OtDeferredInstancingVS", "OtDeferredPbrFS"};
	OtShaderProgram deferredTerrainProgram{"OtTerrainVS", "OtTerrainFS"};
	OtShaderProgram deferredLightingProgram{"OtDeferredLightingVS", "OtDeferredLightingFS"};
	OtShaderProgram forwardPbrProgram{"OtForwardVS", "OtForwardPbrFS"};
	OtShaderProgram forwardInstancingProgram{"OtForwardInstancingVS", "OtForwardPbrFS"};
	OtShaderProgram gridProgram{"OtGridVS", "OtGridFS"};
	OtShaderProgram selectProgram{"OtSelectVS", "OtSelectFS"};
	OtShaderProgram outlineProgram{"OtOutlineVS", "OtOutlineFS"};
	OtShaderProgram skyProgram{"OtSkyVS", "OtSkyFS"};
	OtShaderProgram skyBoxProgram{"OtSkyVS", "OtSkyBoxFS"};
	OtShaderProgram skySphereProgram{"OtSkyVS", "OtSkySphereFS"};
	OtShaderProgram bloomDownSampleProgram{"OtFilterVS", "OtBloomDownSampleFS"};
	OtShaderProgram bloomUpSampleProgram{"OtFilterVS", "OtBloomUpSampleFS"};
	OtShaderProgram postProcessProgram{"OtFilterVS", "OtPostProcessFS"};
};
