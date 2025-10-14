//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "OtMaterial.h"
#include "OtPass.h"

#include "OtSceneRendererContext.h"
#include "OtSceneRenderPass.h"
#include "OtTerrain.h"


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
	virtual OtShaderProgram* getGrassProgram() = 0;

	virtual uint64_t getNormalState() = 0;
	virtual uint64_t getCullBackState() = 0;
	virtual uint64_t getWireframeState() = 0;

	struct Scope {
		OtEntity entity;
		bool isTransparent;
		bool isTerrain;
		bool isGrass;
		std::shared_ptr<OtMaterial> material;
		std::shared_ptr<OtTerrain> terrain;
		OtGrassComponent* grass;
	};

	virtual void submitUniforms(OtSceneRendererContext& /* ctx */, Scope& /* scope */) {}

protected:
	// utility methods for subclasses
	void renderEntities(OtSceneRendererContext& ctx, OtPass& pass);
	void renderEntity(OtSceneRendererContext& ctx, OtPass& pass, OtEntity entity);

private:
	// private methods to better structure rendering pipeline
	void renderOpaqueGeometry(OtSceneRendererContext& ctx, OtEntity entity, OtGeometryComponent& component);
	void renderOpaqueModel(OtSceneRendererContext& ctx, OtEntity entity, OtModelComponent& component);
	void renderTerrain(OtSceneRendererContext& ctx, OtEntity entity, OtTerrainComponent& component);
	void renderGrass(OtSceneRendererContext& ctx, OtEntity entity, OtGrassComponent& component);
	void renderTransparentGeometry(OtSceneRendererContext& ctx, OtEntity entity, OtGeometryComponent& component);
};
