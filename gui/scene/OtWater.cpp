//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
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
#include "OtWater.h"


//
//	Globals
//

static glm::vec3 waterVertices[] = {
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
//	OtWaterClass::preRender
//

void OtWaterClass::preRender(OtRenderer& renderer) {
	renderReflection(renderer);
	renderRefraction(renderer);
}


//
//	OtWaterClass::render
//

void OtWaterClass::render(OtRenderer& renderer) {
	// sanity check
	if (!normals.isValid()) {
		OtExcept("Normalmap not specified for [Water] object");
	}

	// water doesn't cause shadows
	// and we don't render while we're creating reflections and refractions
	if (renderer.inLightingPass()) {
		// ensure water is centered at camera position
		auto center = renderer.getCamera()->getPosition();
		glm::mat4 transform = glm::translate(glm::mat4(1.0), glm::vec3(center.x, 0.0, center.z));
		transform = glm::scale(transform, glm::vec3(size, 1.0, size));
		renderer.setTransform(transform);

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
		renderer.setDefaultState();
		renderer.runShader(shader);
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

	// run shadow pass
	OtRenderer reflectionRenderer;
	reflectionRenderer.copyLightProperties(renderer);
	reflectionRenderer.runReflectionPass(renderer.getScene(), reflectionCamera, reflectionFrameBuffer);
}


//
//	OtWaterClass::renderRefraction
//

void OtWaterClass::renderRefraction(OtRenderer& renderer) {
	// update framebuffer
	refractionFrameBuffer.update(renderer.getViewW() / 2.0, renderer.getViewH() / 2.0);
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
