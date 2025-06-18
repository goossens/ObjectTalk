//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "OtPass.h"

#include "OtSceneRendererContext.h"
#include "OtSceneRenderPass.h"


//
//	OtSceneRenderEntitiesPass
//

class OtSceneRenderEntitiesPass : public OtSceneRenderPass {
protected:
	// methods that must be overriden by subclasses (when required)
	virtual bool isRenderingOpaque() = 0;
	virtual bool isRenderingTransparent() = 0;

	virtual OtShaderProgram* getOpaqueProgram() = 0;
	virtual OtShaderProgram* getInstancedOpaqueProgram() = 0;
	virtual OtShaderProgram* getAnimatedOpaqueProgram() = 0;
	virtual OtShaderProgram* getTransparentProgram() = 0;
	virtual OtShaderProgram* getInstancedTransparentProgram() = 0;
	virtual OtShaderProgram* getTerrainProgram() = 0;

	virtual uint64_t getNormalState() = 0;
	virtual uint64_t getCullBackState() = 0;
	virtual uint64_t getWireframeState() = 0;

	struct Scope {
		OtEntity entity;
		bool isTransparent;
		bool isTerrain;
		std::shared_ptr<OtMaterial> material;
		std::shared_ptr<OtTerrain> terrain;
	};

	virtual void submitUniforms(OtSceneRendererContext& /* ctx */, Scope& /* scope */) {}

protected:
	// utility methods for subclasses
	void renderEntities(OtSceneRendererContext& ctx, OtPass& pass);
	void renderEntity(OtSceneRendererContext& ctx, OtPass& pass, OtEntity entity);

private:
	// private methods to better structure rendering pipeline
	void renderOpaqueGeometry(OtSceneRendererContext& ctx, OtEntity entity, OtGeometryComponent& geometry);
	void renderOpaqueModel(OtSceneRendererContext& ctx, OtEntity entity, OtModelComponent& model);
	void renderTerrain(OtSceneRendererContext& ctx, OtEntity entity, OtTerrainComponent& terrain);
	void renderTransparentGeometry(OtSceneRendererContext& ctx, OtEntity entity, OtGeometryComponent& geometry);

	// private properties
	OtUniformVec4 clipUniforms{"u_clip", 1};
	OtUniformVec4 lightingUniforms{"u_lighting", 4};
	OtUniformVec4 albedoUniforms{"u_albedo", 2};
	OtUniformVec4 materialUniforms{"u_material", 5};
	OtUniformVec4 terrainUniforms{"u_terrain", 9};

	OtSampler iblBrdfLutSampler{"s_iblBrdfLut"};
	OtSampler iblIrradianceMapSampler{"s_iblIrradianceMap"};
	OtSampler iblEnvironmentMapSampler{"s_iblEnvironmentMap"};

	OtSampler albedoSampler{"s_albedoTexture"};
	OtSampler normalSampler{"s_normalTexture"};
	OtSampler metallicRoughnessSampler{"s_metallicRoughnessTexture"};
	OtSampler emissiveSampler{"s_emissiveTexture"};
	OtSampler aoSampler{"s_aoTexture"};

	OtSampler region1Sampler{"s_region1Texture"};
	OtSampler region2Sampler{"s_region2Texture"};
	OtSampler region3Sampler{"s_region3Texture"};
	OtSampler region4Sampler{"s_region4Texture"};
	OtSampler normalmapSampler{"s_normalmapTexture"};
};
