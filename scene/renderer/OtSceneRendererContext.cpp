//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtSceneRendererContext.h"


//
//	OtSceneRendererContext::initialize
//

void OtSceneRendererContext::initialize(OtCamera c, OtScene* s, OtImageBasedLighting* i, OtCascadedShadowMap* sm) {
	// store provided properties
	camera = c;
	scene = s;
	ibl = i;
	csm = sm;

	// reset state
	hasInstances = false;
	visibleInstances.clear();

	hasImageBasedLighting = false;
	hasDirectionalLighting = false;
	hasPointLighting = false;
	hasOpaqueEntities = false;
	hasOpaqueGeometries = false;
	hasOpaqueModels = false;
	hasTerrainEntities = false;
	hasSkyEntities = false;
	hasTransparentEntities = false;
	hasWaterEntities = false;
	hasGrassEntities = false;
	hasParticlesEntities = false;

	renderDirectionalLight = false;
	castShadow = false;

	iblEntity = OtEntityNull;
	waterEntity = OtEntityNull;

	directionalLightDirection = glm::vec3(0.0f);
	directionalLightColor = glm::vec3(0.0f);
	directionalLightAmbient = 0.0f;
	renderDirectionalLight = false;
	castShadow = false;
	renderingShadow = false;

	// check entities, collect data and set flags
	scene->eachEntityBreadthFirst([&](OtEntity entity) {
		// process image based lighting
		if (scene->hasComponent<OtIblComponent>(entity)) {
			hasImageBasedLighting |= scene->getComponent<OtIblComponent>(entity).cubemap.isReady();
			iblEntity = entity;
		}

		// process directional light
		if (scene->hasComponent<OtDirectionalLightComponent>(entity)) {
			auto& light = scene->getComponent<OtDirectionalLightComponent>(entity);

			hasDirectionalLighting = true;
			directionalLightDirection = light.getDirectionToLight();
			directionalLightColor = light.color;
			directionalLightAmbient = light.ambient;
			renderDirectionalLight = true;
			castShadow = light.castShadow;
		}

		// process procedural sky
		if (scene->hasComponent<OtSkyComponent>(entity)) {
			auto& sky = scene->getComponent<OtSkyComponent>(entity);

			hasDirectionalLighting = true;
			directionalLightDirection = sky.getDirectionToSun();
			directionalLightColor = glm::vec3(0.2f + std::clamp(sky.elevation / 10.0f, 0.0f, 0.8f));
			directionalLightAmbient = std::clamp((sky.elevation + 6.0f) / 200.0f, 0.0f, 0.2f);
			renderDirectionalLight = false;
			castShadow = sky.castShadow;
		}

		// process point lighting
		if (scene->hasComponent<OtPointLightComponent>(entity)) {
			hasPointLighting = true;
		}

		// process all geometry entities
		if (scene->hasComponent<OtGeometryComponent>(entity)) {
			bool transparent = scene->getComponent<OtGeometryComponent>(entity).transparent;
			hasOpaqueEntities |= !transparent;
			hasOpaqueGeometries |= !transparent;
			hasTransparentEntities |= transparent;
		}

		// process all model entities
		if (scene->hasComponent<OtModelComponent>(entity)) {
			hasOpaqueEntities = true;
			hasOpaqueModels = true;
		}

		// process all terrain entities
		if (scene->hasComponent<OtTerrainComponent>(entity)) {
			hasOpaqueEntities = true;
			hasTerrainEntities = true;
		}

		// process all sky entities
		if (scene->hasComponent<OtSkyComponent>(entity) || scene->hasComponent<OtSkyBoxComponent>(entity)) {
			hasSkyEntities = true;
		}

		// process all water entities
		if (scene->hasComponent<OtWaterComponent>(entity)) {
			hasTransparentEntities = true;
			hasWaterEntities = true;
			waterEntity = entity;
		}

		// process all grass entities
		if (scene->hasComponent<OtGrassComponent>(entity)) {
			hasOpaqueEntities = true;
			hasGrassEntities = true;
		}

		// process all particle entities
		if (scene->hasComponent<OtParticlesComponent>(entity)) {
			hasParticlesEntities = true;
		}
	});
}


//
//	OtSceneRendererContext::submitLightingUniforms
//

void OtSceneRendererContext::submitLightingUniforms() {
	// build and submit the light uniforms
	glm::vec4* uniforms = lightingUniforms.getValues();
	uniforms[0] = glm::vec4(camera.position, float(hasDirectionalLighting));
	uniforms[1] = glm::vec4(directionalLightDirection, 0.0f);
	uniforms[2] = glm::vec4(directionalLightColor, directionalLightAmbient);
	uniforms[3] = glm::vec4(float(hasImageBasedLighting), float(hasImageBasedLighting ? ibl->maxEnvLevel : 0), 0.0f, 0.0f);
	lightingUniforms.submit();

	// submit the IBL samplers
	if (hasImageBasedLighting) {
		iblBrdfLutSampler.submit(5, ibl->iblBrdfLut);
		iblIrradianceMapSampler.submit(6, ibl->iblIrradianceMap);
		iblEnvironmentMapSampler.submit(7, ibl->iblEnvironmentMap);

	} else {
		iblBrdfLutSampler.submitDummyTexture(5);
		iblIrradianceMapSampler.submitDummyCubeMap(6);
		iblEnvironmentMapSampler.submitDummyCubeMap(7);
	}
}


//
//	OtSceneRendererContext::submitShadowUniforms
//

void OtSceneRendererContext::submitShadowUniforms() {
	// build and submit the shadow uniforms
	shadowUniforms.setValue(0, float(castShadow), 1.0f / csm->getSize(), 0.0f, 0.0f);
	shadowUniforms.setValue(1, csm->getDistance(0), csm->getDistance(1), csm->getDistance(2), csm->getDistance(3)),
	shadowUniforms.submit();

	shadowViewProjUniform.setValue(0, csm->getCamera(0).viewProjectionMatrix);
	shadowViewProjUniform.setValue(1, csm->getCamera(1).viewProjectionMatrix);
	shadowViewProjUniform.setValue(2, csm->getCamera(2).viewProjectionMatrix);
	shadowViewProjUniform.setValue(3, csm->getCamera(3).viewProjectionMatrix);
	shadowViewProjUniform.submit();

	viewUniform.set(0, camera.viewMatrix);
	viewUniform.submit();

	// submit the shadow samplers
	csm->getFrameBuffer(0).bindDepthTexture(shadowMap0Sampler, 8);
	csm->getFrameBuffer(1).bindDepthTexture(shadowMap1Sampler, 9);
	csm->getFrameBuffer(2).bindDepthTexture(shadowMap2Sampler, 10);
	csm->getFrameBuffer(3).bindDepthTexture(shadowMap3Sampler, 11);
}


//
//	OtSceneRendererContext::submitMaterialUniforms
//

void OtSceneRendererContext::submitMaterialUniforms(OtMaterial& material) {
	// set the uniform values
	glm::vec4* uniforms = materialUniforms.getValues();
	uniforms[0] = material.albedo;

	uniforms[1] = glm::vec4(
		material.metallic,
		material.roughness,
		material.ao,
		0.0f);

	uniforms[2] = glm::vec4(
		material.emissive,
		0.0f);

	uniforms[3] = glm::vec4(
		material.offset,
		material.scale,
		material.albedoTexture.isReady());

	uniforms[4] = glm::vec4(
		material.metallicRoughnessTexture.isReady(),
		material.emissiveTexture.isReady(),
		material.aoTexture.isReady(),
		material.normalTexture.isReady());

	// submit the uniforms
	materialUniforms.submit();

	// submit all material textures (or dummies if they are not set (yet))
	submitTextureSampler(albedoSampler, 0, material.albedoTexture);
	submitTextureSampler(metallicRoughnessSampler, 1, material.metallicRoughnessTexture);
	submitTextureSampler(emissiveSampler, 2, material.emissiveTexture);
	submitTextureSampler(aoSampler, 3, material.aoTexture);
	submitTextureSampler(normalSampler, 4, material.normalTexture);
}


//
//	OtSceneRendererContext::submitTerrainUniforms
//

void OtSceneRendererContext::submitTerrainUniforms(OtTerrain& terrain) {
	// get access to key terrain information
	OtTerrainHeights& heights = terrain.heights;
	OtTerrainMaterial& material = terrain.material;

	// set the uniform values
	glm::vec4* uniforms = terrainUniforms.getValues();

	uniforms[0] = glm::vec4(
		terrain.hScale,
		terrain.vScale,
		terrain.vOffset,
		float(heights.heightmapSize));

	uniforms[1] = glm::vec4(
		material.region1Texture.isReady() ? material.region1Texture->getTexture().getWidth() : 1,
		material.region2Texture.isReady() ? material.region2Texture->getTexture().getWidth() : 1,
		material.region3Texture.isReady() ? material.region3Texture->getTexture().getWidth() : 1,
		material.region4Texture.isReady() ? material.region4Texture->getTexture().getWidth() : 1);

	uniforms[2] = glm::vec4(
		material.region1TextureScale,
		material.region2TextureScale,
		material.region3TextureScale,
		material.region4TextureScale);

	uniforms[3] = glm::vec4(
		material.region1Transition,
		material.region2Transition,
		material.region3Transition,
		0.0f);

	uniforms[4] = glm::vec4(
		material.region1Overlap,
		material.region2Overlap,
		material.region3Overlap,
		0.0f);

	uniforms[5] = glm::vec4(material.region1Color, material.region1Texture.isReady());
	uniforms[6] = glm::vec4(material.region2Color, material.region2Texture.isReady());
	uniforms[7] = glm::vec4(material.region3Color, material.region3Texture.isReady());
	uniforms[8] = glm::vec4(material.region4Color, material.region4Texture.isReady());

	// submit the uniforms
	terrainUniforms.submit();

	// bind the normalmap texture (which includes the heightmap in the alpha/w component)
	heights.normalmap.bindColorTexture(normalmapSampler, 0);

	// submit all material textures (or dummies if they are not set)
	submitTextureSampler(region1Sampler, 1, material.region1Texture);
	submitTextureSampler(region2Sampler, 2, material.region2Texture);
	submitTextureSampler(region3Sampler, 3, material.region3Texture);
	submitTextureSampler(region4Sampler, 4, material.region4Texture);
}


//
//	OtSceneRendererContext::submitGrassUniforms
//

void OtSceneRendererContext::submitGrassUniforms(OtGrass& grass) {
	grassUniforms.setValue(0, grass.patchWidth, grass.patchDepth, static_cast<float>(grass.bladeSegments), static_cast<float>(grass.blades));
	grassUniforms.setValue(1, grass.bladeWidth, grass.bladeHeight, grass.bladePointiness, grass.bladeCurve);
	grassUniforms.setValue(2, static_cast<float>(ImGui::GetTime()), glm::radians(grass.windDirection), grass.windStrength, 0.0f);
	grassUniforms.setValue(3, grass.widthVariation, grass.heightVariation, grass.windVariation, grass.colorVariation);
	grassUniforms.setValue(4, grass.baseColor, 0.0f);
	grassUniforms.setValue(5, grass.tipColor, 0.0f);
	grassUniforms.submit();
}


//
//	OtSceneRendererContext::submitTextureSampler
//

void OtSceneRendererContext::submitTextureSampler(OtSampler& sampler, int unit, OtAsset<OtTextureAsset>& texture) {
	if (texture.isReady()) {
		sampler.submit(unit, texture->getTexture());

	} else {
		sampler.submitDummyTexture(unit);
	}
}
