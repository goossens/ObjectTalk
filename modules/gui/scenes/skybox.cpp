//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "ot/function.h"

#include "image.h"
#include "skybox.h"
#include "skyboxshader.h"


//
//	Globals
//

static const bgfx::EmbeddedShader embeddedShaders[] = {
	BGFX_EMBEDDED_SHADER(vs_skybox),
	BGFX_EMBEDDED_SHADER(fs_skybox),
	BGFX_EMBEDDED_SHADER_END()
};

static glm::vec3 skyboxVertices[] = {
	{ -1.0f,  1.0f, -1.0f },
	{ -1.0f, -1.0f, -1.0f },
	{  1.0f, -1.0f, -1.0f },
	{  1.0f, -1.0f, -1.0f },
	{  1.0f,  1.0f, -1.0f },
	{ -1.0f,  1.0f, -1.0f },

	{ -1.0f, -1.0f,  1.0f },
	{ -1.0f, -1.0f, -1.0f },
	{ -1.0f,  1.0f, -1.0f },
	{ -1.0f,  1.0f, -1.0f },
	{ -1.0f,  1.0f,  1.0f },
	{ -1.0f, -1.0f,  1.0f },

	{  1.0f, -1.0f, -1.0f },
	{  1.0f, -1.0f,  1.0f },
	{  1.0f,  1.0f,  1.0f },
	{  1.0f,  1.0f,  1.0f },
	{  1.0f,  1.0f, -1.0f },
	{  1.0f, -1.0f, -1.0f },

	{ -1.0f, -1.0f,  1.0f },
	{ -1.0f,  1.0f,  1.0f },
	{  1.0f,  1.0f,  1.0f },
	{  1.0f,  1.0f,  1.0f },
	{  1.0f, -1.0f,  1.0f },
	{ -1.0f, -1.0f,  1.0f },

	{ -1.0f,  1.0f, -1.0f },
	{  1.0f,  1.0f, -1.0f },
	{  1.0f,  1.0f,  1.0f },
	{  1.0f,  1.0f,  1.0f },
	{ -1.0f,  1.0f,  1.0f },
	{ -1.0f,  1.0f, -1.0f },

	{ -1.0f, -1.0f, -1.0f },
	{ -1.0f, -1.0f,  1.0f },
	{  1.0f, -1.0f, -1.0f },
	{  1.0f, -1.0f, -1.0f },
	{ -1.0f, -1.0f,  1.0f },
	{  1.0f, -1.0f,  1.0f }
};

static uint32_t skyboxIndices[] = {
	0, 1, 2,
	3, 4, 5,

	6, 7, 8,
	9, 10, 11,

	12, 13, 14,
	15, 16, 17,

	18, 19, 20,
	21, 22, 23,

	24, 25, 26,
	27, 28, 29,

	30, 31, 32,
	33, 34, 35
};


//
//	OtSkyboxClass::OtSkyboxClass
//

OtSkyboxClass::OtSkyboxClass() {
	// create geometry
	bgfx::VertexLayout layout;

	layout.begin()
		.add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
		.end();

	vertexBuffer = bgfx::createVertexBuffer(bgfx::makeRef(skyboxVertices, sizeof(skyboxVertices) * sizeof(glm::vec3)), layout);
	indexBuffer = bgfx::createIndexBuffer(bgfx::makeRef(skyboxIndices, sizeof(skyboxIndices) *	sizeof(uint32_t)), BGFX_BUFFER_INDEX32);

	// register uniform
	cubemapUniform = bgfx::createUniform("s_cubemap", bgfx::UniformType::Sampler);

	// initialize shader
	bgfx::RendererType::Enum type = bgfx::getRendererType();

	shader = bgfx::createProgram(
		bgfx::createEmbeddedShader(embeddedShaders, type, "vs_skybox"),
		bgfx::createEmbeddedShader(embeddedShaders, type, "fs_skybox"),
		true);
}


//
//	OtSkyboxClass::~OtSkyboxClass
//

OtSkyboxClass::~OtSkyboxClass() {
	// release resources
	bgfx::destroy(vertexBuffer);
	bgfx::destroy(indexBuffer);
	bgfx::destroy(cubemapUniform);

	clearCubemap();

	bgfx::destroy(shader);
}


//
//	OtSkyboxClass::init
//

OtObject OtSkyboxClass::init(size_t count, OtObject* parameters) {
	if (count == 6) {
		setCubemap(
			parameters[0]->operator std::string(),
			parameters[1]->operator std::string(),
			parameters[2]->operator std::string(),
			parameters[3]->operator std::string(),
			parameters[4]->operator std::string(),
			parameters[5]->operator std::string());

	} else if (count != 0) {
		OtExcept("[Skybox] constructor expects 0 or 6 arguments (not %ld)", count);
	}

	return nullptr;
}


//
//	OtSkyboxClass::clearCubemap
//

void OtSkyboxClass::clearCubemap() {
	if (bgfx::isValid(cubemap)) {
		bgfx::destroy(cubemap);
		cubemap = BGFX_INVALID_HANDLE;
	}

	if (posxImage) {
		bimg::imageFree(posxImage);
		posxImage = nullptr;
	}

	if (negxImage) {
		bimg::imageFree(negxImage);
		negxImage = nullptr;
	}

	if (posyImage) {
		bimg::imageFree(posyImage);
		posyImage = nullptr;
	}

	if (negyImage) {
		bimg::imageFree(negyImage);
		negyImage = nullptr;
	}

	if (poszImage) {
		bimg::imageFree(poszImage);
		poszImage = nullptr;
	}

	if (negzImage) {
		bimg::imageFree(negzImage);
		negzImage = nullptr;
	}
}


//
//	OtSkyboxClass::setCubemap
//

OtObject OtSkyboxClass::setCubemap(const std::string& posx, const std::string& negx, const std::string& posy, const std::string& negy, const std::string& posz, const std::string& negz) {
	// clear previous cubemap
	clearCubemap();

	// load first side
	bimg::ImageContainer* image = OtLoadImage(posx, true, true);
	uint16_t imageSize = image->m_width;
	bimg::TextureFormat::Enum imageFormat = image->m_format;

	// create a new cubemap
	cubemap = bgfx::createTextureCube(imageSize, false, 1, bgfx::TextureFormat::Enum(imageFormat), BGFX_TEXTURE_NONE | BGFX_SAMPLER_NONE);

	// store first side
	const bgfx::Memory* mem = bgfx::makeRef(image->m_data, image->m_size);
	bgfx::updateTextureCube(cubemap, 0, 0, 0, 0, 0, imageSize, imageSize, mem);

	// load and store other sides
	image = OtLoadImage(negx, true, true);

	if (image->m_width != imageSize || image->m_format != imageFormat) {
		bimg::imageFree(image);
		OtExcept("Cubemap image (negx] does not have same size or format as others");
	}

	mem = bgfx::makeRef(image->m_data, image->m_size);
	bgfx::updateTextureCube(cubemap, 0, 1, 0, 0, 0, imageSize, imageSize, mem);

	image = OtLoadImage(posy, true, true);

	if (image->m_width != imageSize || image->m_format != imageFormat) {
		bimg::imageFree(image);
		OtExcept("Cubemap image (posy] does not have same size or format as others");
	}

	mem = bgfx::makeRef(image->m_data, image->m_size);
	bgfx::updateTextureCube(cubemap, 0, 2, 0, 0, 0, imageSize, imageSize, mem);

	image = OtLoadImage(negy, true, true);

	if (image->m_width != imageSize || image->m_format != imageFormat) {
		bimg::imageFree(image);
		OtExcept("Cubemap image (negy] does not have same size or format as others");
	}

	mem = bgfx::makeRef(image->m_data, image->m_size);
	bgfx::updateTextureCube(cubemap, 0, 3, 0, 0, 0, imageSize, imageSize, mem);

	image = OtLoadImage(posz, true, true);

	if (image->m_width != imageSize || image->m_format != imageFormat) {
		bimg::imageFree(image);
		OtExcept("Cubemap image (posz] does not have same size or format as others");
	}

	mem = bgfx::makeRef(image->m_data, image->m_size);
	bgfx::updateTextureCube(cubemap, 0, 4, 0, 0, 0, imageSize, imageSize, mem);

	image = OtLoadImage(negz, true, true);

	if (image->m_width != imageSize || image->m_format != imageFormat) {
		bimg::imageFree(image);
		OtExcept("Cubemap image (negz] does not have same size or format as others");
	}

	mem = bgfx::makeRef(image->m_data, image->m_size);
	bgfx::updateTextureCube(cubemap, 0, 5, 0, 0, 0, imageSize, imageSize, mem);

	return shared();
}


//
//	OtSkyboxClass::render
//

void OtSkyboxClass::render(OtRenderingContext* context) {
	// sanity check
	if (!bgfx::isValid(cubemap)) {
		OtExcept("[cubemap] missing for [skybox]");
	}

	// submit cubemap
	bgfx::setTexture(0, cubemapUniform, cubemap);

	// submit vertices and triangles
	bgfx::setVertexBuffer(0, vertexBuffer);
	bgfx::setIndexBuffer(indexBuffer);

	// run shader
	bgfx::setState(BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A | BGFX_STATE_MSAA);
	bgfx::submit(context->view, shader);
}


//
//	OtSkyboxClass::getMeta
//

OtType OtSkyboxClass::getMeta() {
	static OtType type = nullptr;

	if (!type) {
		type = OtTypeClass::create<OtSkyboxClass>("Skybox", OtSceneObjectClass::getMeta());
		type->set("__init__", OtFunctionClass::create(&OtSkyboxClass::init));
		type->set("setCubemap", OtFunctionClass::create(&OtSkyboxClass::setCubemap));
	}

	return type;
}


//
//	OtSkyboxClass::create
//

OtSkybox OtSkyboxClass::create() {
	OtSkybox skybox = std::make_shared<OtSkyboxClass>();
	skybox->setType(getMeta());
	return skybox;
}
