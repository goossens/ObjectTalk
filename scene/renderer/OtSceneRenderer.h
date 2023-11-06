//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
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

#include "OtTtlCache.h"

#include "OtBoxGeometry.h"
#include "OtFrameBuffer.h"
#include "OtFrustum.h"
#include "OtGbuffer.h"
#include "OtIndexBuffer.h"
#include "OtPass.h"
#include "OtSampler.h"
#include "OtShaderProgram.h"
#include "OtSphereGeometry.h"
#include "OtUniformMat4.h"
#include "OtUniformVec4.h"
#include "OtVertexBuffer.h"

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

	struct TerrainTile {
		TerrainTile(OtTerrainComponent* t, OtMaterial m, int _x, int _z, int l) : terrain(t), material(m), x(_x), z(_z), lod(l), northLod(1), eastLod(1), southLod(1), westLod(1) {}
		OtTerrainComponent* terrain;
		OtMaterial material;
		int x, z;
		int lod;
		int northLod, eastLod, southLod, westLod;
	};

	// render passes
	void renderPreProcessingPass(OtScene* scene, OtEntity selected);
	// void renderShadowPass(OtScene* scene);
	void renderGeometryPass(OtScene* scene);
	void renderTerrainPass(OtScene* scene);
	void renderBackgroundPass(OtScene* scene);
	void renderSkyPass(OtScene* scene);
	void renderLightingPass(OtScene* scene);
	void renderTransparentPass(OtScene* scene);
	void renderGridPass();
	void renderHighlightPass(OtScene* scene, OtEntity selected);
	void renderPostProcessingPass(OtScene* scene);

	// preprocess entities
	void preprocessSingleInstanceGeometry(OtScene* scene, OtEntity entity, bool selected);
	void preprocessMultipleInstanceGeometry(OtScene* scene, OtEntity entity, bool selected);
	void preprocessSingleInstanceModel(OtScene* scene, OtEntity entity, bool selected);
	void preprocessMultipleInstanceModel(OtScene* scene, OtEntity entity, bool selected);
	void preprocessTerrainTiles(OtScene* scene, OtEntity entity);

	// render entities
	void renderSky(OtPass& pass, OtSkyComponent& component);
	void renderSkyBox(OtPass& pass, OtSkyBoxComponent& component);
	void renderSkySphere(OtPass& pass, OtSkySphereComponent& component);
	void renderGeometry(OtPass& pass, OtScene* scene, OtEntity entity);
	void renderModel(OtPass& pass, OtScene* scene, OtEntity entity);
	void renderTerrainTile(OtPass& pass, TerrainTile& tile);
	void renderTransparentGeometry(OtPass& pass, OtScene* scene, OtEntity entity);
	void renderHighlight(OtPass& pass, OtScene* scene, OtEntity entity);
	void renderBloom(float bloomIntensity);

	// rendering tools
	void submitMaterialUniforms(OtMaterial material);
	void submitPbrUniforms(OtPbrMaterial material);
	void submitTerrainUniforms(OtTerrainMaterial material);
	void submitLightUniforms(OtScene* scene);
	void submitTerrainGeometry(TerrainTile& tile);

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

	// procedural terrain data
	OtTtlCache<size_t, OtVertexBuffer> terrainVertices;
	OtTtlCache<size_t, OtIndexBuffer> terrainIndices;
	std::vector<TerrainTile> terrainTiles;

	// uniforms
	OtUniformVec4 pbrMaterialUniforms{"u_pbrMaterial", 5};
	OtUniformVec4 terrainMaterialUniforms{"u_terrainMaterial", 6};
	OtUniformVec4 lightingUniforms{"u_lighting", 3};
	OtUniformVec4 skyUniforms{"u_sky", 3};
	OtUniformVec4 gridUniforms{"u_grid", 1};
	OtUniformVec4 outlineUniforms{"u_outline", 1};
	OtUniformVec4 bloomUniforms{"u_bloom", 1};
	OtUniformVec4 postProcessUniforms{"u_postProcess", 1};
	OtUniformMat4 inverseTransform{"u_inverseTransform", 1};

	// samplers
	OtSampler textureSampler0{"s_textureSampler0"};
	OtSampler textureSampler1{"s_textureSampler1"};
	OtSampler textureSampler2{"s_textureSampler2"};
	OtSampler textureSampler3{"s_textureSampler3"};

	OtSampler geometryAlbedoSampler{"s_geometryAlbedoTexture"};
	OtSampler geometryNormalSampler{"s_geometryNormalTexture"};
	OtSampler geometryMetallicRoughnessSampler{"s_geometryMetallicRoughnessTexture"};
	OtSampler geometryEmissiveSampler{"s_geometryEmissiveSampler"};
	OtSampler geometryAoSampler{"s_geometryAoTexture"};

	OtSampler lightingAlbedoSampler{"s_lightingAlbedoTexture"};
	OtSampler lightingPositionSampler{"s_lightingPositionTexture"};
	OtSampler lightingNormalSampler{"s_lightingNormalTexture"};
	OtSampler lightingPbrSampler{"s_lightingPbrTexture"};
	OtSampler lightingEmissiveSampler{"s_lightingEmissiveTexture"};
	OtSampler lightingDepthSampler{"s_lightingDepthTexture"};

	OtSampler selectedSampler{"s_selectedTexture"};

	OtSampler postProcessSampler{"s_postProcessTexture"};
	OtSampler bloomSampler{"s_bloomTexture"};

	// shader programs
	OtShaderProgram geometryPbrProgram{"OtGeometryPbrVS", "OtGeometryPbrFS"};
	OtShaderProgram geometryTerrainProgram{"OtGeometryTerrainVS", "OtGeometryTerrainFS"};
	OtShaderProgram geometryInstancingProgram{"OtGeometryVSI", "OtGeometryFS"};
	OtShaderProgram lightingProgram{"OtLightingVS", "OtLightingFS"};
	OtShaderProgram transparentProgram{"OtTransparentVS", "OtTransparentFS"};
	OtShaderProgram transparentInstancingProgram{"OtTransparentVSI", "OtTransparentFS"};
	OtShaderProgram gridProgram{"OtGridVS", "OtGridFS"};
	OtShaderProgram selectProgram{"OtSelectVS", "OtSelectFS"};
	OtShaderProgram outlineProgram{"OtOutlineVS", "OtOutlineFS"};
	OtShaderProgram skyProgram{"OtSkyVS", "OtSkyFS"};
	OtShaderProgram skyBoxProgram{"OtSkyboxVS", "OtSkyboxFS"};
	OtShaderProgram skySphereProgram{"OtSkySphereVS", "OtSkySphereFS"};
	OtShaderProgram bloomDownSampleProgram{"OtBloomDownSampleVS", "OtBloomDownSampleFS"};
	OtShaderProgram bloomUpSampleProgram{"OtBloomUpSampleVS", "OtBloomUpSampleFS"};
	OtShaderProgram postProcessProgram{"OtPostProcessVS", "OtPostProcessFS"};
};
