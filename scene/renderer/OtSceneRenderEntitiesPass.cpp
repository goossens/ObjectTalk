//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <algorithm>
#include <cstdint>
#include <cstring>
#include <vector>

#include "imgui.h"

#include "OtGpu.h"
#include "OtMaterial.h"

#include "OtMaterialComponent.h"
#include "OtSceneRenderEntitiesPass.h"


//
//	OtSceneRenderEntitiesPass::renderEntities
//

void OtSceneRenderEntitiesPass::renderEntities(OtSceneRendererContext& ctx) {
	// render all opaque entities
	if (isRenderingOpaque() && ctx.hasOpaqueEntities) {
		// render geometries
		if (ctx.hasOpaqueGeometries) {
			for (auto entity : ctx.opaqueGeometryEntities) {
				auto& grd = ctx.geometryRenderData[entity];

				if (grd.cameras[ctx.cameraID].visible) {
					renderOpaqueGeometry(ctx, grd);
				}
			}
		}

		// render models
		if (ctx.hasOpaqueModels) {
			for (auto entity : ctx.modelEntities) {
				auto& mrd = ctx.modelRenderData[entity];

				if (mrd.visible[ctx.cameraID]) {
					renderOpaqueModel(ctx, mrd);
				}
			}
		}

		// render terrain
		if (ctx.hasTerrainEntities) {
			ctx.scene->view<OtTerrainComponent>().each([&](auto entity, auto& terrain) {
				if (!ctx.renderingShadow || terrain.terrain->isCastingShadow()) {
					renderTerrain(ctx, entity, terrain);
				}
			});
		}

		// render grass
		if (ctx.hasGrassEntities) {
			ctx.scene->view<OtGrassComponent>().each([&](auto entity, auto& grass) {
				if (!ctx.renderingShadow || grass.grass->castShadow) {
					renderGrass(ctx, entity, grass);
				}
			});
		}
	}

	// render all transparent geometries
	if (isRenderingTransparent() && ctx.hasTransparentGeometries) {
		for (auto entity : ctx.transparentGeometryEntities) {
			auto& grd = ctx.geometryRenderData[entity];

			if (grd.cameras[ctx.cameraID].visible) {
				renderTransparentGeometry(ctx, grd);
			}
		}
	}
}


//
//	OtSceneRenderEntitiesPass::renderEntity
//

void OtSceneRenderEntitiesPass::renderEntity(OtSceneRendererContext& ctx, OtEntity entity) {
	// render geometry (if required)
	if (ctx.scene->hasComponent<OtGeometryComponent>(entity)) {
		auto& grd = ctx.geometryRenderData[entity];
		auto& camera = grd.cameras[ctx.cameraID];

		if (camera.visible) {
			auto& geometry = ctx.scene->getComponent<OtGeometryComponent>(entity);

			if (geometry.transparent) {
				renderTransparentGeometry(ctx, grd);

			} else {
				renderOpaqueGeometry(ctx, grd);
			}
		}
	}

	// render model (if required)
	if (ctx.scene->hasComponent<OtModelComponent>(entity)) {
		auto& mrd = ctx.modelRenderData[entity];

		if (mrd.visible[ctx.cameraID]) {
			renderOpaqueModel(ctx, mrd);
		}
	}

	// render terrain (if required)
	if (ctx.scene->hasComponent<OtTerrainComponent>(entity)) {
		auto& terrain = ctx.scene->getComponent<OtTerrainComponent>(entity);

		if (!ctx.renderingShadow || terrain.terrain->isCastingShadow()) {
			renderTerrain(ctx, entity, terrain);
		}
	}

	// render grass (if required)
	if (ctx.scene->hasComponent<OtGrassComponent>(entity)) {
		auto& grass = ctx.scene->getComponent<OtGrassComponent>(entity);

		if (!ctx.renderingShadow || grass.grass->castShadow) {
			renderGrass(ctx, entity, grass);
		}
	}
}


//
//	OtSceneRenderEntitiesPass::renderGeometryHelper
//

void OtSceneRenderEntitiesPass::renderGeometryHelper(
	OtSceneRendererContext& ctx,
	OtGeometryRenderData& grd,
	MaterialSubmission materialSubmission,
	OtRenderPipeline& cullingPipeline,
	OtRenderPipeline& noCullingPipeline,
	OtRenderPipeline& linesPipeline,
	OtRenderPipeline& instancedCullingPipeline,
	OtRenderPipeline& instancedNoCullingPipeline,
	OtRenderPipeline& instancedLinesPipeline) {

	// do we have instances?
	if (grd.instances) {
		// bind pipeline
		if (grd.component->wireframe) {
			ctx.pass->bindPipeline(instancedLinesPipeline);

		} else if (grd.component->cullBack) {
			ctx.pass->bindPipeline(instancedCullingPipeline);

		} else {
			ctx.pass->bindPipeline(instancedNoCullingPipeline);
		}

		// setup instance data
		ctx.pass->setInstanceData(grd.cameras[ctx.cameraID].idb);

		// set vertex uniforms
		struct Uniforms {
			glm::mat4 viewProjectionMatrix;

		} uniforms {
			ctx.camera.viewProjectionMatrix
		};

		ctx.pass->setVertexUniforms(0, &uniforms, sizeof(uniforms));

	} else {
		// bind pipeline
		if (grd.component->wireframe) {
			ctx.pass->bindPipeline(linesPipeline);

		} else if (grd.component->cullBack) {
			ctx.pass->bindPipeline(cullingPipeline);

		} else {
			ctx.pass->bindPipeline(noCullingPipeline);
		}

		// set vertex uniforms
		struct Uniforms {
			glm::mat4 viewProjectionMatrix;
			glm::mat4 modelMatrix;
		} uniforms {
			ctx.camera.viewProjectionMatrix,
			ctx.scene->getGlobalTransform(grd.entity)
		};

		ctx.pass->setVertexUniforms(0, &uniforms, sizeof(uniforms));
	}

	// submit material uniforms (if required)
	if (materialSubmission == MaterialSubmission::full) {
		setMaterialUniforms(ctx, materialUniformSlot, materialSamplerSlot, grd.entity);

	} else if (materialSubmission == MaterialSubmission::justAlbedo) {
		setAlbedoUniforms(ctx, albedoUniformSlot,albedoSamplerSlot, grd.entity);
	}

	// render geometry
	ctx.pass->render(grd.component->asset->getGeometry());
}


//
//	OtSceneRenderEntitiesPass::renderModelHelper
//

void OtSceneRenderEntitiesPass::renderModelHelper(
	OtSceneRendererContext& ctx,
	OtModelRenderData& mrd,
	MaterialSubmission materialSubmission,
	OtRenderPipeline& staticPipeline,
	OtRenderPipeline& animatedPipeline) {

	// process all render commands
	auto globalTransform = ctx.scene->getGlobalTransform(mrd.entity);
	auto renderList = mrd.model->getRenderList(globalTransform);

	for (auto& cmd : renderList) {
		// handle animations
		if (cmd.animation) {
			// bind pipeline
			ctx.pass->bindPipeline(animatedPipeline);

			// set vertex uniforms
			struct Uniforms {
				glm::mat4 viewProjectionMatrix;
				glm::mat4 models[64];
			} uniforms;

			uniforms.viewProjectionMatrix = ctx.camera.viewProjectionMatrix;
			std::memcpy(&uniforms.models, cmd.transforms.data(), std::min(cmd.transforms.size(), size_t(64)) * sizeof(glm::mat4));
			ctx.pass->setVertexUniforms(0, &uniforms, sizeof(uniforms));

			// bind animation data
			ctx.pass->setAnimationData(cmd.mesh->getBonesBuffer());

		} else {
			// bind pipeline
			ctx.pass->bindPipeline(staticPipeline);

			// set vertex uniforms
			struct Uniforms {
				glm::mat4 viewProjectionMatrix;
				glm::mat4 modelMatrix;
			} uniforms {
				ctx.camera.viewProjectionMatrix,
				cmd.transforms[0]
			};

			ctx.pass->setVertexUniforms(0, &uniforms, sizeof(uniforms));
		}

		// submit material uniforms (if required)
		if (materialSubmission == MaterialSubmission::full) {
			setMaterialUniforms(ctx, materialUniformSlot, materialSamplerSlot, cmd.material);

		} else if (materialSubmission == MaterialSubmission::justAlbedo) {
			setAlbedoUniforms(ctx, albedoUniformSlot, albedoSamplerSlot, cmd.material);
		}

		// render geometry
		ctx.pass->render(cmd.mesh->getVertexBuffer(), cmd.mesh->getIndexBuffer());
	}
}


//
//	OtSceneRenderEntitiesPass::renderTerrainHelper
//

void OtSceneRenderEntitiesPass::renderTerrainHelper(
	OtSceneRendererContext& ctx,
	OtTerrainComponent& component,
	bool setFragmentUniforms,
	OtRenderPipeline& cullingPipeline,
	OtRenderPipeline& linesPipeline) {

	// get terrain data
	auto& terrain = *component.terrain;
	OtTerrainHeights& heights = terrain.heights;
	OtTerrainMaterial& material = terrain.material;

	// setup pipeline
	ctx.pass->bindPipeline(terrain.isWireframe() ? linesPipeline : cullingPipeline);

	// set (per terrain) vertex uniforms
	struct TerrainVertexUniforms {
		glm::mat4 viewProjectionMatrix;
		float hScale;
		float heightMapSize;
	} terrainVertexUniforms {
		ctx.camera.viewProjectionMatrix,
		terrain.hScale,
		static_cast<float>(heights.heightmapSize)
	};

	ctx.pass->setVertexUniforms(0, &terrainVertexUniforms, sizeof(terrainVertexUniforms));
	ctx.pass->bindVertexSampler(0, ctx.normalmapSampler, heights.normalmap);

	// set fragment uniforms
	if (setFragmentUniforms) {
		struct FragmentUniforms {
			glm::vec4 region1Color;
			glm::vec4 region2Color;
			glm::vec4 region3Color;
			glm::vec4 region4Color;

			float hScale;
			float vScale;
			float vOffset;
			float heightMapSize;

			float region1TextureSize;
			float region2TextureSize;
			float region3TextureSize;
			float region4TextureSize;

			float region1TextureScale;
			float region2TextureScale;
			float region3TextureScale;
			float region4TextureScale;

			float region1Transition;
			float region2Transition;
			float region3Transition;

			float region1Overlap;
			float region2Overlap;
			float region3Overlap;
		} fragmentUniforms {
			glm::vec4(material.region1Color, static_cast<float>(material.region1Texture.isReady())),
			glm::vec4(material.region2Color, static_cast<float>(material.region2Texture.isReady())),
			glm::vec4(material.region3Color, static_cast<float>(material.region3Texture.isReady())),
			glm::vec4(material.region4Color, static_cast<float>(material.region4Texture.isReady())),

			terrain.hScale,
			terrain.vScale,
			terrain.vOffset,
			static_cast<float>(heights.heightmapSize),

			static_cast<float>(material.region1Texture.isReady() ? material.region1Texture->getTexture().getWidth() : 1),
			static_cast<float>(material.region2Texture.isReady() ? material.region2Texture->getTexture().getWidth() : 1),
			static_cast<float>(material.region3Texture.isReady() ? material.region3Texture->getTexture().getWidth() : 1),
			static_cast<float>(material.region4Texture.isReady() ? material.region4Texture->getTexture().getWidth() : 1),

			material.region1TextureScale,
			material.region2TextureScale,
			material.region3TextureScale,
			material.region4TextureScale,

			material.region1Transition,
			material.region2Transition,
			material.region3Transition,

			material.region1Overlap,
			material.region2Overlap,
			material.region3Overlap
		};

		ctx.pass->setFragmentUniforms(0, &fragmentUniforms, sizeof(fragmentUniforms));
	}

	// bind textures
	ctx.bindFragmentSampler(0, ctx.region1Sampler, material.region1Texture);
	ctx.bindFragmentSampler(1, ctx.region2Sampler, material.region2Texture);
	ctx.bindFragmentSampler(2, ctx.region3Sampler, material.region3Texture);
	ctx.bindFragmentSampler(3, ctx.region4Sampler, material.region4Texture);

	// process all the terrain meshes
	for (auto& mesh : terrain.getMeshes()) {
		// set (per mesh) vertex uniforms
		struct MeshVertexUniforms {
			glm::mat4 modelMatrix;
		} meshVertexUniforms {
			mesh.transform
		};

		ctx.pass->setVertexUniforms(1, &meshVertexUniforms, sizeof(meshVertexUniforms));

		// render terrain tile
		ctx.pass->render(mesh.tile.vertices, mesh.tile.triangles);
	}
}


//
//	OtSceneRenderEntitiesPass::renderGrassHelper
//

void OtSceneRenderEntitiesPass::renderGrassHelper(
	OtSceneRendererContext& ctx,
	OtEntity entity,
	OtGrassComponent& component,
	OtRenderPipeline& pipeline) {

	// set uniforms
	auto& grass = *component.grass;

	struct Uniforms {
		glm::mat4 viewProjectionMatrix;
		glm::mat4 modelMatrix;

		glm::vec4 baseColor;
		glm::vec4 tipColor;

		float patchWidth;
		float patchDepth;
		int32_t segments;
		int32_t blades;

		float bladeWidth;
		float bladeHeight;
		float BladePointiness;
		float BladeCurve;

		float time;
		float windDirection;
		float windStrength;

		float widthVariation;
		float heightVariation;
		float windVariation;
		float colorVariation;
	} uniforms {
		ctx.camera.viewProjectionMatrix,
		ctx.scene->getGlobalTransform(entity),

		glm::vec4(grass.baseColor, 1.0f),
		glm::vec4(grass.tipColor, 1.0f),

		grass.patchWidth,
		grass.patchDepth,
		static_cast<int32_t>(grass.bladeSegments),
		static_cast<int32_t>(grass.blades),

		grass.bladeWidth,
		grass.bladeHeight,
		grass.bladePointiness,
		grass.bladeCurve,

		static_cast<float>(ImGui::GetTime()),
		glm::radians(grass.windDirection),
		grass.windStrength,

		grass.widthVariation,
		grass.heightVariation,
		grass.windVariation,
		grass.colorVariation
	};

	ctx.pass->setVertexUniforms(0, &uniforms, sizeof(uniforms));

	ctx.pass->bindPipeline(pipeline);
	ctx.pass->setInstanceCount(grass.blades);
	ctx.pass->render(grass.getVertexBuffer(), grass.getIndexBuffer());
}


//
//	OtSceneRenderEntitiesPass::setMaterialUniforms
//

void OtSceneRenderEntitiesPass::setMaterialUniforms(
	OtSceneRendererContext& ctx,
	size_t uniformSlot,
	size_t samplerSlot,
	std::shared_ptr<OtMaterial> material) {

	// set uniforms
	struct Uniforms {
		glm::vec4 albedoColor;
		glm::vec3 emissiveColor;
		float textureScale;
		glm::vec2 textureOffset;
		float metallicFactor;
		float roughnessFactor;
		float aoFactor;
		uint32_t hasAlbedoTexture;
		uint32_t hasMetallicRoughnessTexture;
		uint32_t hasEmissiveTexture;
		uint32_t hasAoTexture;
		uint32_t hasNormalTexture;
	} uniforms {
		material->albedo,
		material->emissive,
		material->scale,
		material->offset,
		material->metallic,
		material->roughness,
		material->ao,
		static_cast<uint32_t>(material->albedoTexture.isReady()),
		static_cast<uint32_t>(material->metallicRoughnessTexture.isReady()),
		static_cast<uint32_t>(material->emissiveTexture.isReady()),
		static_cast<uint32_t>(material->aoTexture.isReady()),
		static_cast<uint32_t>(material->normalTexture.isReady())
	};

	ctx.pass->setFragmentUniforms(uniformSlot, &uniforms, sizeof(uniforms));

	// set textures
	ctx.bindFragmentSampler(samplerSlot++, ctx.albedoSampler, material->albedoTexture);
	ctx.bindFragmentSampler(samplerSlot++, ctx.metallicRoughnessSampler, material->metallicRoughnessTexture);
	ctx.bindFragmentSampler(samplerSlot++, ctx.emissiveSampler, material->emissiveTexture);
	ctx.bindFragmentSampler(samplerSlot++, ctx.aoSampler, material->aoTexture);
	ctx.bindFragmentSampler(samplerSlot++, ctx.normalSampler, material->normalTexture);
}


//
//	OtSceneRenderEntitiesPass::setMaterialUniforms
//

void OtSceneRenderEntitiesPass::setMaterialUniforms(OtSceneRendererContext& ctx, size_t uniformSlot, size_t samplerSlot, OtEntity entity) {
	setMaterialUniforms(
		ctx,
		uniformSlot,
		samplerSlot,
		ctx.scene->hasComponent<OtMaterialComponent>(entity)
			? ctx.scene->getComponent<OtMaterialComponent>(entity).material
			: std::make_shared<OtMaterial>());
}


//
//	OtSceneRenderEntitiesPass::setAlbedoUniforms
//

void OtSceneRenderEntitiesPass::setAlbedoUniforms(
	OtSceneRendererContext& ctx,
	size_t uniformSlot,
	size_t samplerSlot,
	std::shared_ptr<OtMaterial> material) {

	// set uniforms
	struct Uniforms {
		glm::vec4 albedoColor;
		glm::vec2 textureOffset;
		float textureScale;
		uint32_t hasAlbedoTexture;
	} uniforms {
		material->albedo,
		material->offset,
		material->scale,
		static_cast<uint32_t>(material->albedoTexture.isReady()),
	};

	ctx.pass->setFragmentUniforms(uniformSlot, &uniforms, sizeof(uniforms));

	// set textures
	ctx.bindFragmentSampler(samplerSlot, ctx.albedoSampler, material->albedoTexture);
}


//
//	OtSceneRenderEntitiesPass::setAlbedoUniforms
//

void OtSceneRenderEntitiesPass::setAlbedoUniforms(OtSceneRendererContext& ctx, size_t uniformSlot, size_t samplerSlot, OtEntity entity) {
	setAlbedoUniforms(
		ctx,
		uniformSlot,
		samplerSlot,
		ctx.scene->hasComponent<OtMaterialComponent>(entity)
			? ctx.scene->getComponent<OtMaterialComponent>(entity).material
			: std::make_shared<OtMaterial>());
}
