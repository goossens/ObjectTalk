//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtSceneRenderer.h"


//
//	OtSceneRenderer::renderDeferredTerrainPass
//

void OtSceneRenderer::renderDeferredTerrainPass(OtScene* scene) {
	// setup pass
	OtPass pass;
	pass.setRectangle(0, 0, width, height);
	pass.setFrameBuffer(gbuffer);
	pass.setTransform(viewMatrix, projectionMatrix);

	// render each terrain entity
	for (auto [entity, component] : scene->view<OtTerrainComponent>().each()) {
		renderDeferredTerrain(pass, component);
	}
}


//
//	OtSceneRenderer::renderDeferredTerrain
//

void OtSceneRenderer::renderDeferredTerrain(OtPass& pass, OtTerrainComponent& terrain) {
	// process all the terrain meshes
	for (auto& mesh : terrain.terrain->getMeshes(frustum, cameraPosition)) {
		// submit the geometry
		mesh.tile.vertices.submit();

		// submit the terrain uniforms
		submitTerrainUniforms(terrain.terrain);

		if (terrain.terrain->isWireframe()) {
			mesh.tile.lines.submit();

		} else {
			mesh.tile.triangles.submit();
		}

		// set the program state
		if (terrain.terrain->isWireframe()) {
			deferredTerrainProgram.setState(
				OtStateWriteRgb |
				OtStateWriteA |
				OtStateWriteZ |
				OtStateDepthTestLess |
				OtStateLines);

		} else {
			deferredTerrainProgram.setState(
				OtStateWriteRgb |
				OtStateWriteA |
				OtStateWriteZ |
				OtStateDepthTestLess |
				OtStateCullCw);
		}

		// set the transform
		deferredTerrainProgram.setTransform(mesh.transform);

		// run the program
		pass.runShaderProgram(deferredTerrainProgram);
	}
}


//
//	OtSceneRenderer::submitTerrainUniforms
//

void OtSceneRenderer::submitTerrainUniforms(OtTerrain terrain) {
	// get access to key terrain information
	OtTerrainHeights& heights = terrain->heights;
	OtTerrainMaterial& material = terrain->material;

	// set the uniform values
	glm::vec4* uniforms = terrainUniforms.getValues();

	uniforms[0] = glm::vec4(
		terrain->hScale,
		terrain->vScale,
		terrain->vOffset,
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
	submitSampler(region1Sampler, 1, material.region1Texture);
	submitSampler(region2Sampler, 2, material.region2Texture);
	submitSampler(region3Sampler, 3, material.region3Texture);
	submitSampler(region4Sampler, 4, material.region4Texture);
}
