//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "bx/file.h"
#include "bgfx/embedded_shader.h"

#include "ot/function.h"

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

static uint16_t skyboxIndices[] = {
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
	indexBuffer = bgfx::createIndexBuffer(bgfx::makeRef(skyboxIndices, sizeof(skyboxIndices) *  sizeof(uint16_t)));

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
	bgfx::destroy(shader);
}


//
//	OtSkyboxClass::init
//

OtObject OtSkyboxClass::init(size_t count, OtObject* parameters) {
	if (count == 1) {
		setCubemap(parameters[0]->operator std::string());

	} else if (count != 0) {
		OtExcept("[Skybox] constructor expects 0 or 1 arguments (not %ld)", count);
	}

	return nullptr;
}


//
//	OtSkyboxClass::setCubemap
//

OtObject OtSkyboxClass::setCubemap(const std::string& file) {
	// load named texture
	static bx::DefaultAllocator allocator;
	static bx::FileReader reader;

	if (!bx::open(&reader, file.c_str())) {
		OtExcept("Can't open texture [%s]", file.c_str());
	}

	uint32_t size = (uint32_t) bx::getSize(&reader);
	void* data = BX_ALLOC(&allocator, size);
	bx::read(&reader, data, size);
	bx::close(&reader);

	image = bimg::imageParse(&allocator, data, size);

	if (!image)  {
		OtExcept("Can't process cubemap in [%s]", file.c_str());
	}

	if (!image->m_cubeMap) {
		OtExcept("Image provide [%s] provide the [skybox] is not a cubemap", file.c_str());
	}

	const bgfx::Memory* mem = bgfx::makeRef(image->m_data, image->m_size);
	BX_FREE(&allocator, data);

	if (bgfx::isTextureValid(0, false, image->m_numLayers, bgfx::TextureFormat::Enum(image->m_format), BGFX_TEXTURE_NONE | BGFX_SAMPLER_NONE)) {
		cubemap = bgfx::createTextureCube(
			uint16_t (image->m_width),
			1 < image->m_numMips,
			image->m_numLayers,
			bgfx::TextureFormat::Enum(image->m_format),
			BGFX_TEXTURE_NONE | BGFX_SAMPLER_NONE,
			mem);

	} else {
		OtExcept("Invalid cubemap format in [%s]", file.c_str());
	}

	return shared();
}


//
//	OtSkyboxClass::render
//

void OtSkyboxClass::render(int view, OtCamera camera, glm::mat4 parentTransform) {
	// sanity check
	if (!image) {
		OtExcept("[cubemap] missing for [skybox]");
	}

	//submit cubemap
	bgfx::setTexture(0, cubemapUniform, cubemap);

	// submit vertices and triangles
	bgfx::setVertexBuffer(0, vertexBuffer);
	bgfx::setIndexBuffer(indexBuffer);

	// run shader
	bgfx::setState(BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A | BGFX_STATE_MSAA);
	bgfx::submit(view, shader);
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
