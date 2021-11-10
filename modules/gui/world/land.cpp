//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "bgfx/embedded_shader.h"

#include "imgui.h"

#include "ot/function.h"

#include "land.h"
#include "landshader.h"


//
//	Globals
//

static const bgfx::EmbeddedShader embeddedShaders[] = {
	BGFX_EMBEDDED_SHADER(vs_land),
	BGFX_EMBEDDED_SHADER(fs_land),
	BGFX_EMBEDDED_SHADER_END()
};


//
//	OtLandClass::OtLandClass
//

OtLandClass::OtLandClass() {
	// register landUniforms
	materialUniform = bgfx::createUniform("u_material", bgfx::UniformType::Vec4, 5);
	landUniform = bgfx::createUniform("u_land", bgfx::UniformType::Vec4, 9);
	textureUniform1 = bgfx::createUniform("s_texture_1", bgfx::UniformType::Sampler);
	textureUniform2 = bgfx::createUniform("s_texture_2", bgfx::UniformType::Sampler);
	textureUniform3 = bgfx::createUniform("s_texture_3", bgfx::UniformType::Sampler);
	textureUniform4 = bgfx::createUniform("s_texture_4", bgfx::UniformType::Sampler);

	// initialize shader
	bgfx::RendererType::Enum type = bgfx::getRendererType();

	shader = bgfx::createProgram(
		bgfx::createEmbeddedShader(embeddedShaders, type, "vs_land"),
		bgfx::createEmbeddedShader(embeddedShaders, type, "fs_land"),
		true);

	// create a dummy texture
	dummyTexture = OtTextureClass::create();
}


//
//	OtLandClass::~OtLandClass
//

OtLandClass::~OtLandClass() {
	// release resources
	if (bgfx::isValid(textureUniform1)) {
		bgfx::destroy(textureUniform1);
	}

	if (bgfx::isValid(textureUniform2)) {
		bgfx::destroy(textureUniform2);
	}

	if (bgfx::isValid(textureUniform3)) {
		bgfx::destroy(textureUniform3);
	}

	if (bgfx::isValid(textureUniform4)) {
		bgfx::destroy(textureUniform4);
	}

	if (bgfx::isValid(vertexBuffer)) {
		bgfx::destroy(vertexBuffer);
	}

	if (bgfx::isValid(indexBuffer)) {
		bgfx::destroy(indexBuffer);
	}

	bgfx::destroy(materialUniform);
	bgfx::destroy(landUniform);
	bgfx::destroy(shader);
}


//
//	OtLandClass::init
//

OtObject OtLandClass::init(size_t count, OtObject* parameters) {
	switch (count) {
		case 3:
			setHeightMap(parameters[0]);

		case 2:
			setLevelOfDetail(parameters[0]->operator int());
			break;

		case 1:
			setSize(parameters[0]->operator float());
			break;

		case 0:
			break;

		default:
			OtExcept("[Land] constructor expects up to 3 arguments (not %ld)", count);
	}

	return nullptr;
}


//
//	OtLandClass::setSize
//

OtObject OtLandClass::setSize(float s) {
	if (s != size) {
		size = s;
		dirty = true;
	}

	return shared();
}


//
//	OtLandClass::setLevelOfDetail
//

OtObject OtLandClass::setLevelOfDetail(int lod) {
	if (lod != levelOfDetail) {
		levelOfDetail = std::clamp(lod, 0, 6);
		dirty = true;
	}

	return shared();
}


//
//	OtLandClass::setHeightMap
//

OtObject OtLandClass::setHeightMap(OtObject object) {
	// ensure object is a heightmap
	if (object->isKindOf("HeightMap")) {
		heightmap = object->cast<OtHeightMapClass>();

	} else {
		OtExcept("Expected a [HeightMap] object, not a [%s]", object->getType()->getName().c_str());
	}

	heightmap->attach([this]() {
		dirty = true;
	});

	dirty = true;
	return shared();
}


//
//	OtLandClass::setRegion1Texture
//

OtObject OtLandClass::setRegion1Texture(OtObject object) {
	// ensure object is a texture
	if (object->isKindOf("Texture")) {
		textureRegion1 = object->cast<OtTextureClass>();

	} else {
		OtExcept("Expected a [Texture] object, not a [%s]", object->getType()->getName().c_str());
	}

	return shared();
}


//
//	OtLandClass::setRegion2Texture
//

OtObject OtLandClass::setRegion2Texture(OtObject object) {
	// ensure object is a texture
	if (object->isKindOf("Texture")) {
		textureRegion2 = object->cast<OtTextureClass>();

	} else {
		OtExcept("Expected a [Texture] object, not a [%s]", object->getType()->getName().c_str());
	}

	return shared();
}


//
//	OtLandClass::setRegion3Texture
//

OtObject OtLandClass::setRegion3Texture(OtObject object) {
	// ensure object is a texture
	if (object->isKindOf("Texture")) {
		textureRegion3 = object->cast<OtTextureClass>();

	} else {
		OtExcept("Expected a [Texture] object, not a [%s]", object->getType()->getName().c_str());
	}

	return shared();
}


//
//	OtLandClass::setRegion4Texture
//

OtObject OtLandClass::setRegion4Texture(OtObject object) {
	// ensure object is a texture
	if (object->isKindOf("Texture")) {
		textureRegion4 = object->cast<OtTextureClass>();

	} else {
		OtExcept("Expected a [Texture] object, not a [%s]", object->getType()->getName().c_str());
	}

	return shared();
}


//
//	OtLandClass::renderGUI
//

void OtLandClass::renderGUI() {
	if (ImGui::SliderInt("LOD", &levelOfDetail, 0, 6)) {
		dirty = true;
	}
}


//
//	OtLandClass::generate
//

void OtLandClass::generate() {
	// clear arrays
	vertices.clear();
	triangles.clear();

	// add vertices
	auto half = size / 2.0;
	auto increment = levelOfDetail == 6 ? 1 : (6 - levelOfDetail) * 2;
	auto segments = 240 / increment;
	auto segments1 = segments + 1;
	auto segmentSize = size / (float) segments;

	for (auto iy = 0; iy < segments1; iy++) {
		auto y = iy * segmentSize - half;

		for (auto ix = 0; ix < segments1; ix++) {
			auto x = ix * segmentSize - half;
			auto u = (float) ix / (float) segments;
			auto v = (float) iy / (float) segments;

			if (heightmap) {
				vertices.push_back(OtLandVertex(
					glm::vec3(x, heightmap->getHeight(u, v), y),
					heightmap->getNormal(u, v)));

			} else {
				vertices.push_back(OtLandVertex(
					glm::vec3(x, 0.0, y),
					glm::vec3(0.0, 1.0, 0.0)));
			}
		}
	}

	// add triangles
	for (auto iy = 0; iy < segments; iy++) {
		for (auto ix = 0; ix < segments; ix ++) {
			auto a = ix + segments1 * iy;
			auto b = ix + segments1 * (iy + 1);
			auto c = (ix + 1) + segments1 * (iy + 1);
			auto d = (ix + 1) + segments1 * iy;

			triangles.push_back(a);
			triangles.push_back(b);
			triangles.push_back(d);

			triangles.push_back(b);
			triangles.push_back(c);
			triangles.push_back(d);
		}
	}

	// release resources (if required)
	if (bgfx::isValid(vertexBuffer)) {
		bgfx::destroy(vertexBuffer);
	}

	if (bgfx::isValid(indexBuffer)) {
		bgfx::destroy(indexBuffer);
	}

	// create buffers
	bgfx::VertexLayout layout;

	layout.begin()
		.add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
		.add(bgfx::Attrib::Normal, 3, bgfx::AttribType::Float)
		.end();

	vertexBuffer = bgfx::createVertexBuffer(bgfx::makeRef(vertices.data(), sizeof(OtLandVertex) * vertices.size()), layout);
	indexBuffer = bgfx::createIndexBuffer(bgfx::makeRef(triangles.data(), sizeof(uint32_t) * triangles.size()), BGFX_BUFFER_INDEX32);

	// reset flag
	dirty = false;
}


//
//	OtLandClass::render
//

void OtLandClass::render(int view, OtCamera camera, glm::mat4 parentTransform) {
	// generate mesh if required
	if (dirty) {
		generate();
	}

	// submit vertices and triangles
	bgfx::setVertexBuffer(0, vertexBuffer);
	bgfx::setIndexBuffer(indexBuffer);

	// set landUniforms
	glm::vec4 materialUniforms[5];
	materialUniforms[0] = glm::vec4(1.0);
	materialUniforms[1] = glm::vec4(0.4, 0.4, 0.4, 1.0);
	materialUniforms[2] = glm::vec4(0.6, 0.6, 0.6, 1.0);
	materialUniforms[3] = glm::vec4(0.5, 0.5, 0.5, 1.0);
	materialUniforms[4].x = 10.0;
	bgfx::setUniform(materialUniform, &materialUniforms, 5);

	glm::vec4 landUniforms[9];
	landUniforms[0].x = 1.0 / size * 4.0;

	landUniforms[1].x = region1Min;
	landUniforms[1].y = region1Max;
	landUniforms[1].z = textureRegion1 != nullptr;
	landUniforms[2] = glm::vec4(region1Color, 1.0);

	landUniforms[3].x = region2Min;
	landUniforms[3].y = region2Max;
	landUniforms[3].z = textureRegion2 != nullptr;
	landUniforms[4] = glm::vec4(region2Color, 1.0);

	landUniforms[5].x = region3Min;
	landUniforms[5].y = region3Max;
	landUniforms[5].z = textureRegion3 != nullptr;
	landUniforms[6] = glm::vec4(region3Color, 1.0);

	landUniforms[7].x = region4Min;
	landUniforms[7].y = region4Max;
	landUniforms[7].z = textureRegion4 != nullptr;
	landUniforms[8] = glm::vec4(region4Color, 1.0);

	bgfx::setUniform(landUniform, landUniforms, 9);

	// set textures
	(textureRegion1 ? textureRegion1 : dummyTexture)->submit(0, textureUniform1);
	(textureRegion2 ? textureRegion2 : dummyTexture)->submit(1, textureUniform2);
	(textureRegion3 ? textureRegion3 : dummyTexture)->submit(2, textureUniform3);
	(textureRegion4 ? textureRegion4 : dummyTexture)->submit(3, textureUniform4);

	// run shader
	bgfx::setState(BGFX_STATE_DEFAULT);
	bgfx::submit(view, shader);
}


//
//	OtLandClass::getMeta
//

OtType OtLandClass::getMeta() {
	static OtType type = nullptr;

	if (!type) {
		type = OtTypeClass::create<OtLandClass>("Land", OtObject3dClass::getMeta());
		type->set("__init__", OtFunctionClass::create(&OtLandClass::init));
		type->set("setSize", OtFunctionClass::create(&OtLandClass::setSize));
		type->set("setLevelOfDetail", OtFunctionClass::create(&OtLandClass::setLevelOfDetail));
		type->set("setHeightMap", OtFunctionClass::create(&OtLandClass::setHeightMap));

		type->set("setRegion1Limits", OtFunctionClass::create(&OtLandClass::setRegion1Limits));
		type->set("setRegion1Color", OtFunctionClass::create(&OtLandClass::setRegion1Color));
		type->set("setRegion1Texture", OtFunctionClass::create(&OtLandClass::setRegion1Texture));

		type->set("setRegion2Limits", OtFunctionClass::create(&OtLandClass::setRegion2Limits));
		type->set("setRegion2Color", OtFunctionClass::create(&OtLandClass::setRegion2Color));
		type->set("setRegion2Texture", OtFunctionClass::create(&OtLandClass::setRegion2Texture));

		type->set("setRegion3Limits", OtFunctionClass::create(&OtLandClass::setRegion3Limits));
		type->set("setRegion3Color", OtFunctionClass::create(&OtLandClass::setRegion3Color));
		type->set("setRegion3Texture", OtFunctionClass::create(&OtLandClass::setRegion3Texture));

		type->set("setRegion4Limits", OtFunctionClass::create(&OtLandClass::setRegion4Limits));
		type->set("setRegion4Color", OtFunctionClass::create(&OtLandClass::setRegion4Color));
		type->set("setRegion4Texture", OtFunctionClass::create(&OtLandClass::setRegion4Texture));
	}

	return type;
}


//
//	OtLandClass::create
//

OtLand OtLandClass::create() {
	OtLand land = std::make_shared<OtLandClass>();
	land->setType(getMeta());
	return land;
}
