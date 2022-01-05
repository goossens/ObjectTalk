//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "ot/function.h"
#include "ot/numbers.h"

#include "application.h"
#include "color.h"
#include "scene.h"
#include "water.h"
#include "watershader.h"


//
//	Globals
//

static const bgfx::EmbeddedShader embeddedShaders[] = {
	BGFX_EMBEDDED_SHADER(vs_water),
	BGFX_EMBEDDED_SHADER(fs_water),
	BGFX_EMBEDDED_SHADER_END()
};

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
	// create geometry
	bgfx::VertexLayout layout;

	layout.begin()
		.add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
		.end();

	vertexBuffer = bgfx::createDynamicVertexBuffer(bgfx::makeRef(waterVertices, sizeof(waterVertices) * sizeof(glm::vec3)), layout);
	indexBuffer = bgfx::createIndexBuffer(bgfx::makeRef(waterIndices, sizeof(waterIndices) *  sizeof(uint32_t)), BGFX_BUFFER_INDEX32);

	// register uniforms
	materialUniform = bgfx::createUniform("u_material", bgfx::UniformType::Vec4, 5);
	waterUniform = bgfx::createUniform("u_water", bgfx::UniformType::Vec4, 2);
	normalsUniform = bgfx::createUniform("s_normals", bgfx::UniformType::Sampler);
	reflectionUniform = bgfx::createUniform("s_reflection", bgfx::UniformType::Sampler);
	refractionUniform = bgfx::createUniform("s_refraction", bgfx::UniformType::Sampler);

	// initialize shader
	bgfx::RendererType::Enum type = bgfx::getRendererType();

	shader = bgfx::createProgram(
		bgfx::createEmbeddedShader(embeddedShaders, type, "vs_water"),
		bgfx::createEmbeddedShader(embeddedShaders, type, "fs_water"),
		true);
}


//
//	OtWaterClass::~OtWaterClass
//

OtWaterClass::~OtWaterClass() {
	// release resources
	if (bgfx::isValid(reflectionFrameBuffer)) {
		bgfx::destroy(reflectionFrameBuffer);
	}

	if (bgfx::isValid(refractionFrameBuffer)) {
		bgfx::destroy(refractionFrameBuffer);
	}

	bgfx::destroy(vertexBuffer);
	bgfx::destroy(indexBuffer);
	bgfx::destroy(materialUniform);
	bgfx::destroy(waterUniform);
	bgfx::destroy(normalsUniform);
	bgfx::destroy(reflectionUniform);
	bgfx::destroy(refractionUniform);
	bgfx::destroy(shader);
}


//
//	OtWaterClass::updateFrameBuffers
//


void OtWaterClass::updateFrameBuffers(float viewAspect) {
	// release resources
	if (bgfx::isValid(reflectionFrameBuffer)) {
		bgfx::destroy(reflectionFrameBuffer);
	}

	if (bgfx::isValid(refractionFrameBuffer)) {
		bgfx::destroy(refractionFrameBuffer);
	}

	// determine size of frame buffer
	const uint64_t flags = BGFX_TEXTURE_RT | BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP;
	int w = 512;
	int h = w / viewAspect;

	// create new frame buffers
	reflectionTextures[0] = bgfx::createTexture2D(w, h, false, 1, bgfx::TextureFormat::RGBA16F, flags);
	reflectionTextures[1] = bgfx::createTexture2D(w, h, false, 1, bgfx::TextureFormat::D32F, flags);
	reflectionFrameBuffer = bgfx::createFrameBuffer(2, reflectionTextures, true);

	refractionTextures[0] = bgfx::createTexture2D(w, h, false, 1, bgfx::TextureFormat::RGBA16F, flags);
	refractionTextures[1] = bgfx::createTexture2D(w, h, false, 1, bgfx::TextureFormat::D32F, flags);
	refractionFrameBuffer = bgfx::createFrameBuffer(2, refractionTextures, true);
}


//
//	OtWaterClass::init
//

OtObject OtWaterClass::init(size_t count, OtObject* parameters) {
	switch (count) {
		case 0:
			break;

		default:
			OtExcept("[Water] constructor expects 0 arguments (not %ld)", count);
	}

	return nullptr;
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

OtObject OtWaterClass::setNormalMap(OtObject object) {
	// ensure object is a texture
	if (!object->isKindOf("Texture")) {
		OtExcept("Expected a [Texture] object, not a [%s]", object->getType()->getName().c_str());
	}

	normals = object->cast<OtTextureClass>();
	return shared();
}


// set the scale of the normals
OtObject OtWaterClass::setNormalScale(float s) {
	scale = s;
	return shared();
}


//
//	OtWaterClass::preRender
//

void OtWaterClass::preRender(OtRenderingContext* context) {
	// get new IDs for reflection and refraction rendering
	reflectionView = OtApplicationClass::instance()->getNextViewID();
	refractionView = OtApplicationClass::instance()->getNextViewID();

	// create/update frame buffers (if required)
	if (frameBufferAspect != context->viewAspect) {
		updateFrameBuffers(context->viewAspect);
		frameBufferAspect = context->viewAspect;
	}
}


//
//	OtWaterClass::render
//

void OtWaterClass::render(OtRenderingContext* context) {
	// sanity check
	if (!normals) {
		OtExcept("Normals map not specified for [Water] object");
	}

	// render thr three parts of water
	renderReflection(context);
	renderRefraction(context);
	renderWater(context);
}


//
//	OtWaterClass::renderReflection
//

void OtWaterClass::renderReflection(OtRenderingContext* context) {
	// create reflection camera
	glm::vec3 position = context->camera->getPosition();
	glm::vec3 target = context->camera->getTarget();

	position.y = -position.y;
	target.y = -target.y;

	OtCamera reflectionCamera = OtCameraClass::create(context->camera);
	reflectionCamera->setPositionVector(position);
	reflectionCamera->setTargetVector(target);

	// create reflection rendering context
	OtRenderingContext reflectionContext(reflectionView, context->viewAspect, context->scene, reflectionCamera);
	reflectionContext.reflection = true;

	// temporarily disable ourselves so we don't draw ourselves
	disable();

	// render reflection
	reflectionCamera->update(&reflectionContext);
	reflectionCamera->submit(&reflectionContext);

	bgfx::setViewClear(reflectionView, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH);
	bgfx::setViewRect(reflectionView, 0, 0, 512, 512 / context->viewAspect);
	bgfx::setViewFrameBuffer(reflectionView, reflectionFrameBuffer);
	context->scene->render(&reflectionContext);

	// reenable so we can be seen in the final rendering
	enable();
}


//
//	OtWaterClass::renderRefraction
//

void OtWaterClass::renderRefraction(OtRenderingContext* context) {

}


//
//	OtWaterClass::renderWater
//

void OtWaterClass::renderWater(OtRenderingContext* context) {
	// determine center
	glm::vec center = context->camera->getPosition();
	center.y = 0.0;

	// adjust water size
	for (auto c = 0; c < 4; c ++) {
		waterVertices[c] = templateVertices[c] * size + center;
	}

	// submit vertices and triangles
	bgfx::update(vertexBuffer, 0, bgfx::makeRef(waterVertices, sizeof(waterVertices) * sizeof(glm::vec3)));
	bgfx::setVertexBuffer(0, vertexBuffer);
	bgfx::setIndexBuffer(indexBuffer);

	// set normal map, reflection and refraction
	normals->submit(0, normalsUniform);
	bgfx::setTexture(1, reflectionUniform, reflectionTextures[0]);
	bgfx::setTexture(2, refractionUniform, refractionTextures[0]);

	// update time for water ripples
	time += OtApplicationClass::instance()->getLoopDuration() / 2500.0;

	// set uniforms
	context->submit();

	glm::vec4 materialUniforms[5];
	materialUniforms[0] = glm::vec4(1.0);
	materialUniforms[1] = glm::vec4(0.4, 0.4, 0.4, 1.0);
	materialUniforms[2] = glm::vec4(0.6, 0.6, 0.6, 1.0);
	materialUniforms[3] = glm::vec4(1.0);
	materialUniforms[4].x = shininess;
	bgfx::setUniform(materialUniform, &materialUniforms, 5);

	glm::vec4 waterUniforms[2];
	waterUniforms[0].x = time;
	waterUniforms[0].y = scale;
	waterUniforms[1] = glm::vec4(color, 1.0);
	bgfx::setUniform(waterUniform, waterUniforms, 2);

	// run shader
	bgfx::setState(BGFX_STATE_DEFAULT);
	bgfx::submit(context->view, shader);
}


//
//	OtWaterClass::renderGUI
//

void OtWaterClass::renderGUI() {
	ImGui::Checkbox("Enabled", &enabled);
	ImGui::SliderFloat("Texture Scale", &scale, 0.1f, 10.0f);
	ImGui::SliderFloat("Shininess", &shininess, 10.0f, 200.0f);
	ImGui::ColorEdit3("Color", glm::value_ptr(color));
}


//
//	OtWaterClass::getMeta
//

OtType OtWaterClass::getMeta() {
	static OtType type = nullptr;

	if (!type) {
		type = OtTypeClass::create<OtWaterClass>("Water", OtObject3dClass::getMeta());
		type->set("__init__", OtFunctionClass::create(&OtWaterClass::init));
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
