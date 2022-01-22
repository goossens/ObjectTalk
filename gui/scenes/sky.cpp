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

#include "sky.h"
#include "skyshader.h"


//
//	Globals
//

static const bgfx::EmbeddedShader embeddedShaders[] = {
	BGFX_EMBEDDED_SHADER(vs_sky),
	BGFX_EMBEDDED_SHADER(fs_sky),
	BGFX_EMBEDDED_SHADER_END()
};


//
//	OtSkyClass::OtSkyClass
//

OtSkyClass::OtSkyClass() {
	// create dome
	int widthSegments = 16;
	int heightSegments = 8;
	float widthDelta = std::numbers::pi * 2.0 / widthSegments;
	float heightDelta = (std::numbers::pi / 2.0 + 0.1) / heightSegments;

	// address each ring
	for (auto ring = 0; ring <= heightSegments; ring++) {
		auto theta = ring * heightDelta;
		auto r0 = std::sin(theta);
		auto y0 = std::cos(theta);

		// address each segment
		for (auto seg = 0; seg <= widthSegments; seg++) {
			auto phi = seg * widthDelta;
			auto x0 = r0 * -std::sin(phi);
			auto z0 = r0 * -std::cos(phi);

			// add vertex
			vertices.push_back(glm::vec3(x0, y0, z0));
		}
	}

	// add triangles
	for (auto ring = 0; ring < heightSegments; ring++) {
		for (auto seg = 0; seg < widthSegments; seg++) {
			auto a = ring * (widthSegments + 1) + seg;
			auto b = a + (widthSegments + 1);
			auto c = b + 1;
			auto d = a + 1;

			triangles.push_back(a);
			triangles.push_back(b);
			triangles.push_back(d);
			triangles.push_back(b);
			triangles.push_back(c);
			triangles.push_back(d);
		}
	}

	// create geometry
	bgfx::VertexLayout layout;

	layout.begin()
		.add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
		.end();

	vertexBuffer = bgfx::createVertexBuffer(bgfx::makeRef(vertices.data(), sizeof(glm::vec3) * vertices.size()), layout);
	indexBuffer = bgfx::createIndexBuffer(bgfx::makeRef(triangles.data(), sizeof(uint32_t) * triangles.size()), BGFX_BUFFER_INDEX32);

	// register uniforms
	skyUniform = bgfx::createUniform("u_sky", bgfx::UniformType::Vec4, 3);

	// initialize shader
	bgfx::RendererType::Enum type = bgfx::getRendererType();

	shader = bgfx::createProgram(
		bgfx::createEmbeddedShader(embeddedShaders, type, "vs_sky"),
		bgfx::createEmbeddedShader(embeddedShaders, type, "fs_sky"),
		true);
}


//
//	OtSkyClass::~OtSkyClass
//

OtSkyClass::~OtSkyClass() {
	// release resources
	bgfx::destroy(vertexBuffer);
	bgfx::destroy(indexBuffer);
	bgfx::destroy(skyUniform);
	bgfx::destroy(shader);
}


//
//	OtSkyClass::init
//

OtObject OtSkyClass::init(size_t count, OtObject* parameters) {
	switch (count) {
		case 3:
			setClouds(
				parameters[2]->operator float(),
				parameters[3]->operator float());

		case 1:
			setSun(parameters[0]);
			break;

		case 0:
			break;

		default:
			OtExcept("[Sky] constructor expects 0, 1 or 3 arguments (not %ld)", count);
	}

	return nullptr;
}


//
//	OtSkyClass::setSun(
//

OtObject OtSkyClass::setSun(OtObject object) {
	// ensure object is a material
	if (object->isKindOf("Sun")) {
		sun = object->cast<OtSunClass>();

	} else {
		OtExcept("Expected a [Sun] object, not a [%s]", object->getType()->getName().c_str());
	}

	return shared();
}


//
//	OtSkyClass::setClouds
//

OtObject OtSkyClass::setClouds(float cir, float cum) {
	cirrus = cir;
	cumulus = cum;
	return shared();
}


//
//	OtSkyClass::renderGUI
//

void OtSkyClass::renderGUI() {
	ImGui::Checkbox("Enabled", &enabled);

	ImGui::SliderFloat("Rayleigh Coefficient", &rayleighCoefficient, 0.5f, 6.0f);
	ImGui::SliderFloat("Mie Coefficient", &mieCoefficient, 1.0f, 10.0f);
	ImGui::SliderFloat("Mie Scattering", &mieScattering, 0.9f, 0.99f);

	ImGui::SliderFloat("Cirrus", &cirrus, 0.0f, 1.0f);
	ImGui::SliderFloat("Cumulus", &cumulus, 0.0f, 1.0f);
}


//
//	OtSkyClass::render
//

void OtSkyClass::render(OtRenderingContext* context) {
	// ensure sky is centered at camera position
	glm::mat4 transform = glm::translate(glm::mat4(1.0), context->camera->getPosition());
	bgfx::setTransform(glm::value_ptr(transform));

	// submit vertices and triangles
	bgfx::setVertexBuffer(0, vertexBuffer);
	bgfx::setIndexBuffer(indexBuffer);

	// set uniforms
	glm::vec4 uniforms[3];
	uniforms[0].x = 0.0;
	uniforms[0].y = cirrus;
	uniforms[0].z = cumulus;
	uniforms[1].x = rayleighCoefficient / 1000.0;
	uniforms[1].y = mieCoefficient / 1000.0;
	uniforms[1].z = mieScattering;

	if (sun) {
		uniforms[2] = glm::vec4(sun->getDirection(), 0.0);
	}

	bgfx::setUniform(skyUniform, uniforms, 3);

	// run shader
	bgfx::setState(BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A | BGFX_STATE_MSAA);
	bgfx::submit(context->view, shader);
}


//
//	OtSkyClass::getMeta
//

OtType OtSkyClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtTypeClass::create<OtSkyClass>("Sky", OtSceneObjectClass::getMeta());
		type->set("__init__", OtFunctionClass::create(&OtSkyClass::init));
		type->set("setSun", OtFunctionClass::create(&OtSkyClass::setSun));
		type->set("setClouds", OtFunctionClass::create(&OtSkyClass::setClouds));
	}

	return type;
}


//
//	OtSkyClass::create
//

OtSky OtSkyClass::create() {
	OtSky sky = std::make_shared<OtSkyClass>();
	sky->setType(getMeta());
	return sky;
}