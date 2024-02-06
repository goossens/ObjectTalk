//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <exception>
#include <fstream>
#include <sstream>

#include "glm/glm.hpp"
#include "nlohmann/json.hpp"

#include "OtException.h"
#include "OtLog.h"

#include "OtCubeMap.h"
#include "OtGpu.h"
#include "OtImage.h"
#include "OtPass.h"
#include "OtPathTools.h"
#include "OtVertex.h"


//
//	Globals
//

static float vertices[] = {
	// back face
	-1.0f, -1.0f, -1.0f,	// bottom-left
	 1.0f,  1.0f, -1.0f,	// top-right
	 1.0f, -1.0f, -1.0f,	// bottom-right
	 1.0f,  1.0f, -1.0f,	// top-right
	-1.0f, -1.0f, -1.0f,	// bottom-left
	-1.0f,  1.0f, -1.0f,	// top-left
	// front face
	-1.0f, -1.0f,  1.0f,	// bottom-left
	 1.0f, -1.0f,  1.0f,	// bottom-right
	 1.0f,  1.0f,  1.0f,	// top-right
	 1.0f,  1.0f,  1.0f,	// top-right
	-1.0f,  1.0f,  1.0f,	// top-left
	-1.0f, -1.0f,  1.0f,	// bottom-left
	// left face
	-1.0f,  1.0f,  1.0f,	// top-right
	-1.0f,  1.0f, -1.0f,	// top-left
	-1.0f, -1.0f, -1.0f,	// bottom-left
	-1.0f, -1.0f, -1.0f,	// bottom-left
	-1.0f, -1.0f,  1.0f,	// bottom-right
	-1.0f,  1.0f,  1.0f,	// top-right
	// right face
	 1.0f,  1.0f,  1.0f,	// top-left
	 1.0f, -1.0f, -1.0f,	// bottom-right
	 1.0f,  1.0f, -1.0f,	// top-right
	 1.0f, -1.0f, -1.0f,	// bottom-right
	 1.0f,  1.0f,  1.0f,	// top-left
	 1.0f, -1.0f,  1.0f,	// bottom-left
	// bottom face
	-1.0f, -1.0f, -1.0f,	// top-right
	 1.0f, -1.0f, -1.0f,	// top-left
	 1.0f, -1.0f,  1.0f,	// bottom-left
	 1.0f, -1.0f,  1.0f,	// bottom-left
	-1.0f, -1.0f,  1.0f,	// bottom-right
	-1.0f, -1.0f, -1.0f,	// top-right
	// top face
	-1.0f,  1.0f, -1.0f,	// top-left
	 1.0f,  1.0f , 1.0f,	// bottom-right
	 1.0f,  1.0f, -1.0f,	// top-right
	 1.0f,  1.0f,  1.0f,	// bottom-right
	-1.0f,  1.0f, -1.0f,	// top-left
	-1.0f,  1.0f,  1.0f,	// bottom-left
};

static constexpr size_t vertexCount = sizeof(vertices) / sizeof(*vertices) / 3;


//
//	OtCubeMap::load
//

void OtCubeMap::load(const std::string& path) {
	auto ext = OtPathGetExtension(path);

	if (ext == ".cubemap") {
		loadJSON(path);

	} else if (ext == ".hdr") {
		loadHdrImage(path);

	} else {
		loadCubemapImage(path);
	}
}


//
//	OtCubeMap::loadJSON
//

void OtCubeMap::loadJSON(const std::string& path) {
	// load cubemap definition from file
	std::stringstream buffer;

	try {
		std::ifstream stream(path.c_str());

		if (stream.fail()) {
			OtError("Can't read from file [%s]", path.c_str());
		}

		buffer << stream.rdbuf();
		stream.close();

	} catch (std::exception& e) {
		OtError("Can't read from file [%s], error: %s", path.c_str(), e.what());
	}

	// parse json
	auto basedir = OtPathGetParent(path);
	auto data = nlohmann::json::parse(buffer.str());

	auto negx = OtPathGetAbsolute(data, "negx", &basedir);
	auto negy = OtPathGetAbsolute(data, "negy", &basedir);
	auto negz = OtPathGetAbsolute(data, "negz", &basedir);
	auto posx = OtPathGetAbsolute(data, "posx", &basedir);
	auto posy = OtPathGetAbsolute(data, "posy", &basedir);
	auto posz = OtPathGetAbsolute(data, "posz", &basedir);

	if (negx.empty() || negy.empty() || negz.empty() || posx.empty() || posy.empty() || posz.empty()) {
		OtError("Incomplete CubeMap specification in [%s]", path.c_str());
	}

	// load first side
	OtImage image;
	image.load(posx, true, true);
	bimg::ImageContainer* container = image.getContainer();
	uint16_t imageSize = container->m_width;
	bimg::TextureFormat::Enum imageFormat = container->m_format;

	// create a new cubemap
	cubemap = bgfx::createTextureCube(imageSize, false, 1, bgfx::TextureFormat::Enum(imageFormat), BGFX_TEXTURE_NONE | BGFX_SAMPLER_NONE);

	// store first side
	const bgfx::Memory* mem = bgfx::copy(container->m_data, container->m_size);
	bgfx::updateTextureCube(cubemap.getHandle(), 0, 0, 0, 0, 0, imageSize, imageSize, mem);

	// load and store other sides
	image.load(negx, true, true);
	container = image.getContainer();

	if (container->m_width != imageSize || container->m_format != imageFormat) {
		OtError("Cubemap image (negx] does not have same size or format as others");
	}

	mem = bgfx::copy(container->m_data, container->m_size);
	bgfx::updateTextureCube(cubemap.getHandle(), 0, 1, 0, 0, 0, imageSize, imageSize, mem);

	image.load(posy, true, true);
	container = image.getContainer();

	if (container->m_width != imageSize || container->m_format != imageFormat) {
		OtError("Cubemap image (posy] does not have same size or format as others");
	}

	mem = bgfx::copy(container->m_data, container->m_size);
	bgfx::updateTextureCube(cubemap.getHandle(), 0, 2, 0, 0, 0, imageSize, imageSize, mem);

	image.load(negy, true, true);
	container = image.getContainer();

	if (container->m_width != imageSize || container->m_format != imageFormat) {
		OtError("Cubemap image (negy] does not have same size or format as others");
	}

	mem = bgfx::copy(container->m_data, container->m_size);
	bgfx::updateTextureCube(cubemap.getHandle(), 0, 3, 0, 0, 0, imageSize, imageSize, mem);

	image.load(posz, true, true);
	container = image.getContainer();

	if (container->m_width != imageSize || container->m_format != imageFormat) {
		OtError("Cubemap image (posz] does not have same size or format as others");
	}

	mem = bgfx::copy(container->m_data, container->m_size);
	bgfx::updateTextureCube(cubemap.getHandle(), 0, 4, 0, 0, 0, imageSize, imageSize, mem);

	image.load(negz, true, true);
	container = image.getContainer();

	if (container->m_width != imageSize || container->m_format != imageFormat) {
		OtError("Cubemap image (negz] does not have same size or format as others");
	}

	mem = bgfx::copy(container->m_data, container->m_size);
	bgfx::updateTextureCube(cubemap.getHandle(), 0, 5, 0, 0, 0, imageSize, imageSize, mem);
}


//
//	OtCubeMap::loadCubemapImage
//

void OtCubeMap::loadCubemapImage(const std::string& path) {
	// load image
	OtImage image;
	image.load(path, true, true);
	bimg::ImageContainer* container = image.getContainer();

	// is this a real cubemap?
	if (container->m_cubeMap) {
		cubemap = bgfx::createTextureCube(
			container->m_width,
			false,
			1,
			bgfx::TextureFormat::Enum(container->m_format),
			BGFX_TEXTURE_NONE | BGFX_SAMPLER_NONE,
			bgfx::copy(container->m_data, container->m_size));

	} else {
		OtError("Image is not a cubemap");
	}
}


//
//	OtCubeMap::loadHdrImage
//

void OtCubeMap::loadHdrImage(const std::string& path) {
	// load image
	OtImage image;
	image.load(path);
	bimg::ImageContainer* container = image.getContainer();

	// create a temporary texture
	tmpTexture = bgfx::createTexture2D(
		uint16_t(container->m_width),
		uint16_t(container->m_height),
		false,
		container->m_numLayers,
		bgfx::TextureFormat::Enum(container->m_format),
		BGFX_TEXTURE_NONE | BGFX_SAMPLER_NONE,
		bgfx::copy(container->m_data, container->m_size));

	// schedule a task to convert an equirectangular HDR image to a cubemap
	// we can't do that here as loading is done asynchronously in a seperate thread
	// the callback below will be called in the main rendering thread
	hdrConversionHandle = new uv_async_t;
	hdrConversionHandle->data = this;

	auto status = uv_async_init(uv_default_loop(), hdrConversionHandle, [](uv_async_t* handle){
		// run the HDR to cubemap conversion
		auto cubemap = (OtCubeMap*) handle->data;
		cubemap->renderCubemap();

		// cleanup
		uv_close((uv_handle_t*) cubemap->hdrConversionHandle, [](uv_handle_t* handle) {
			auto cubemap = (OtCubeMap*) handle->data;
			delete (uv_fs_event_t*) handle;
			cubemap->hdrConversionHandle = nullptr;
			cubemap->tmpTexture.clear();

			for (auto& framebuffer : cubemap->framebuffers) {
				framebuffer.clear();
			}
		});
	});

	UV_CHECK_ERROR("uv_async_init", status);

	status = uv_async_send(hdrConversionHandle);
	UV_CHECK_ERROR("uv_async_send", status);
}


//
//	OtCubeMap::renderCubemap
//

void OtCubeMap::renderCubemap() {
	// create an empty cubemap
	static constexpr uint16_t size = 512;

	cubemap = bgfx::createTextureCube(
		size,
		false,
		1,
		bgfx::TextureFormat::RGBA16F,
		BGFX_TEXTURE_RT,
		nullptr);

	// create a unity cube
	if (bgfx::getAvailTransientVertexBuffer(vertexCount, OtVertexPosUv::getLayout()) != vertexCount) {
		OtLogFatal("Internal error: insufficient transient buffer space");
	}

	bgfx::TransientVertexBuffer tvb;
	bgfx::allocTransientVertexBuffer(&tvb, vertexCount, OtVertexPos::getLayout());
	memcpy(tvb.data, vertices, sizeof(vertices));

	// define the projection matrix
	glm::mat4 projectionMatrix = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);

	// define the view transformations
	glm::mat4 viewMatrices[] = {
		glm::lookAt(glm::vec3(0.0f), glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
		glm::lookAt(glm::vec3(0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
#if OT_GPU_OPENGL
		glm::lookAt(glm::vec3(0.0f), glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec3(0.0f, 0.0f,  1.0f)),
		glm::lookAt(glm::vec3(0.0f), glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec3(0.0f, 0.0f, -1.0f)),
#else
		glm::lookAt(glm::vec3(0.0f), glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec3(0.0f, 0.0f, -1.0f)),
		glm::lookAt(glm::vec3(0.0f), glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec3(0.0f, 0.0f,  1.0f)),
#endif
		glm::lookAt(glm::vec3(0.0f), glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
		glm::lookAt(glm::vec3(0.0f), glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f))};

	// render all 6 sides of the cube
	for (int side = 0; side < 6; side++) {
		// setup framebuffer
		bgfx::Attachment attachment;
		attachment.init(cubemap.getHandle(), bgfx::Access::Write, side);
		framebuffers[side] = bgfx::createFrameBuffer(1, &attachment);

		// setup a rendering pass
		OtPass pass;
		pass.setClear(false, false);
		pass.setRectangle(0, 0, size, size);
		pass.setFrameBuffer(framebuffers[side].getHandle());
		pass.setTransform(viewMatrices[side], projectionMatrix);

		// setup sampler
		sampler.submit(0, tmpTexture.getHandle());

		// submit geometry
		bgfx::setVertexBuffer(0, &tvb);

		reprojectShader.setState(OtStateWriteRgb | OtStateWriteA | OtStateCullCcw);
		pass.runShaderProgram(reprojectShader);
	}
}
