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

#include "OtSky.h"
#include "OtVertex.h"


//
//	OtSkyClass::OtSkyClass
//

OtSkyClass::OtSkyClass() {
	// create our dome
	createSkyDome();
}


//
//	OtSkyClass::init
//

void OtSkyClass::init(size_t count, OtObject* parameters) {
	switch (count) {
		case 4:
			setRadius(parameters[3]->operator float());

		case 3:
			setClouds(
				parameters[1]->operator float(),
				parameters[2]->operator float());

		case 1:
			setSun(parameters[0]);
			break;

		case 0:
			break;

		default:
			OtExcept("[Sky] constructor expects 0, 1, 3 or 4 arguments (not %ld)", count);
	}
}


//
//	OtSkyClass::setSun(
//

OtObject OtSkyClass::setSun(OtObject object) {
	object->expectKindOf("Sun");
	sun = object->cast<OtSunClass>();
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
//	OtSkyClass::setRadius
//

OtObject OtSkyClass::setRadius(float r) {
	radius = r;
	createSkyDome();
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

void OtSkyClass::render(OtRenderer& renderer) {
	// no sun, no sky
	if (!(sun && !sun->isEnabled())) {
		// ensure sky is centered at camera position
		glm::mat4 transform = glm::translate(glm::mat4(1.0), renderer.getCamera()->getPosition());
		renderer.setTransform(transform);

		// submit vertices and triangles
		vertexBuffer.submit();
		indexBuffer.submit();

		// set uniforms
		uniform.set(0, glm::vec4(0.0, cirrus, cumulus, 0.0));
		uniform.set(1, glm::vec4(rayleighCoefficient / 1000.0, mieCoefficient / 1000.0, mieScattering, 0.0));
		uniform.set(2, sun ? glm::vec4(sun->getDirectionToSun(), 0.0) : glm::vec4(0.0));
		uniform.submit();

		// run shader
		renderer.setState(BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A | BGFX_STATE_MSAA);
		renderer.runShader(shader);
	}
}


//
//	OtSkyClass::createSkyDome
//

void OtSkyClass::createSkyDome() {
	// create dome
	vertices.clear();
	triangles.clear();

	int widthSegments = 16;
	int heightSegments = 8;
	float widthDelta = std::numbers::pi * 2.0 / widthSegments;
	float heightDelta = (std::numbers::pi / 2.0 + 0.1) / heightSegments;

	// address each ring
	for (auto ring = 0; ring <= heightSegments; ring++) {
		auto theta = ring * heightDelta;
		auto r0 = radius * std::sin(theta);
		auto y0 = radius * std::cos(theta);

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

	// create/update buffers
	vertexBuffer.set(vertices.data(), vertices.size(), OtVertexPos::getLayout());
	indexBuffer.set(triangles.data(), triangles.size());
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
		type->set("setRadius", OtFunctionClass::create(&OtSkyClass::setRadius));
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
