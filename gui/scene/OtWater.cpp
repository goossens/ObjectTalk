//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "glm/glm.hpp"
#include "glm/ext.hpp"
#include "imgui.h"

#include "OtFunction.h"
#include "OtNumbers.h"

#include "OtColor.h"
#include "OtFramework.h"
#include "OtGpu.h"
#include "OtScene.h"
#include "OtVertex.h"
#include "OtPass.h"
#include "OtWater.h"


//
//	Globals
//

static glm::vec3 templateVertices[] = {
	{ -1.0f, 0.0f, -1.0f },
	{ -1.0f, 0.0f,	1.0f },
	{  1.0f, 0.0f,	1.0f },
	{  1.0f, 0.0f, -1.0f }
};

static uint32_t waterIndices[] = {
	0, 1, 3,
	1, 2, 3
};


//
//	OtWaterClass::OtWaterClass
//

OtWaterClass::OtWaterClass() {
	vertexBuffer.set(waterVertices, sizeof(waterVertices), OtVertexPos::getLayout());
	indexBuffer.set(waterIndices, sizeof(waterIndices));
}


//
//	OtWaterClass::setSize
//

OtObject OtWaterClass::setSize(int s) {
	size = s;
	return shared();
}


//
//	OtWaterClass::setNormalMap
//

OtObject OtWaterClass::setNormalMap(const std::string& normalsName) {
	if (normals.isValid()) {
		OtExcept("Normal map already set for [Water]");
	}

	normals.loadFromFile(normalsName);
	return shared();
}


//
//	OtWaterClass::setNormalScale
//

OtObject OtWaterClass::setNormalScale(float s) {
	scale = s;
	return shared();
}


//
//	OtWaterClass::update
//

void OtWaterClass::update(OtRenderer& renderer) {
	// update vertices if camera has changed
	auto camera = renderer.getCamera();

	if (camera->hasChanged()) {
		// determine center
		glm::vec center = camera->getPosition();
		center.y = 0.0;

		// adjust vertices
		for (auto c = 0; c < 4; c ++) {
			waterVertices[c] = templateVertices[c] * size + center;
		}

		// tell GPU about it
		vertexBuffer.update(waterVertices, sizeof(waterVertices));
	}

	// reserve views for reflection and refraction
	reflectionPass.reserveRenderingSlot();
	refractionPass.reserveRenderingSlot();
}


//
//	OtWaterClass::render
//

void OtWaterClass::render(OtRenderer& renderer) {
	// water doesn't cause shadows
	// and we don't render while we're creating reflections and refractions
	if (renderer.inLightingPass()) {
		// render the water's parts
		renderReflection(renderer);
		renderRefraction(renderer);
		renderWater(renderer);
	}
}


//
//	OtWaterClass::renderReflection
//

void OtWaterClass::renderReflection(OtRenderer& renderer) {
	// update framebuffer
	reflectionFrameBuffer.update(renderer.getViewW() / 2.0, renderer.getViewH() / 2.0);

	// create reflection camera
	auto camera = renderer.getCamera();
	glm::vec3 position = camera->getPosition();
	glm::vec3 target = camera->getTarget();

	position.y = -position.y;
	target.y = -target.y;

	OtCamera reflectionCamera = OtCameraClass::create(camera);
	reflectionCamera->setPositionVector(position);
	reflectionCamera->setTargetVector(target);
	reflectionCamera->update(renderer);

	// run shadow pass
	OtRenderer reflectionRenderer;
	reflectionRenderer.copyLightProperties(renderer);
	reflectionRenderer.runReflectionPass(reflectionPass, renderer.getScene(), reflectionCamera, reflectionFrameBuffer);
}


//
//	OtWaterClass::renderRefraction
//

void OtWaterClass::renderRefraction(OtRenderer& renderer) {
	// update framebuffer
	refractionFrameBuffer.update(renderer.getViewW() / 2.0, renderer.getViewH() / 2.0);
}


//
//	OtWaterClass::renderWater
//

void OtWaterClass::renderWater(OtRenderer& renderer) {
	// sanity check
	if (!normals.isValid()) {
		OtExcept("Normals map not specified for [Water] object");
	}

	// submit vertices and triangles
	vertexBuffer.submit();
	indexBuffer.submit();

	// set normal map, reflection and refraction
	normalsSampler.submit(1, normals);
	reflectionFrameBuffer.bindColorTexture(reflectionSampler, 2);
	refractionFrameBuffer.bindColorTexture(refractionSampler, 3);

	// update time for water ripples
	time += OtFrameworkClass::instance()->getLoopDuration() / 2500.0;

	// set uniforms
	renderer.submit(receivesShadow());
	waterUniform.set(0, glm::vec4(time, scale, OtGpuHasOriginBottomLeft() ? -1.0 : 1.0, shininess));
	waterUniform.set(1, glm::vec4(color, 1.0));
	waterUniform.submit();

	// run shader
	renderer.setState(BGFX_STATE_DEFAULT);
	renderer.runShader(shader);
}


//
//	OtWaterClass::renderGUI
//

void OtWaterClass::renderGUI() {
	ImGui::Checkbox("Enabled", &enabled);
	ImGui::Checkbox("Receives Shadow", &receiveShadowFlag);
	ImGui::SliderFloat("Texture Scale", &scale, 0.1f, 10.0f);
	ImGui::SliderFloat("Shininess", &shininess, 10.0f, 200.0f);
	ImGui::ColorEdit3("Color", glm::value_ptr(color));
}


//
//	OtWaterClass::getMeta
//

OtType OtWaterClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtTypeClass::create<OtWaterClass>("Water", OtObject3dClass::getMeta());
		type->set("setSize", OtFunctionClass::create(&OtWaterClass::setSize));
		type->set("setNormalMap", OtFunctionClass::create(&OtWaterClass::setNormalMap));
		type->set("setNormalScale", OtFunctionClass::create(&OtWaterClass::setNormalScale));
	}

	return type;
}


//
//	OtWaterClass::create
//

OtWater OtWaterClass::create() {
	OtWater water = std::make_shared<OtWaterClass>();
	water->setType(getMeta());
	return water;
}
